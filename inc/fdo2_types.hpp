/**
 * @file fdo2_types.hpp
 * @brief Result types, decoded measurement fields, and PSUP scaling helpers.
 *
 * @details Integer fields follow PyroScience Unified Protocol (PSUP) firmware 4.x
 *          **Results** registers: values are signed 32-bit integers in fixed-point
 *          units of 0.001 (see PyroScience reference manual §2.9). When the
 *          device enables the “1000×Oxygen” option, oxygen-related registers are
 *          scaled by an additional factor of 1000 inside the module; consult
 *          `Results.status` bit 6 in the official documentation.
 *
 * @copyright Copyright (c) 2026 HardFOC. All rights reserved.
 */
#pragma once

#include <array>
#include <cstdint>
#include <string_view>

namespace fdo2 {

/// Driver-level error codes (stable for logging / telemetry).
enum class DriverError : uint8_t {
    None = 0,
    InvalidParameter,
    Timeout,
    BufferOverflow,
    ProtocolError,
    DeviceError,
};

constexpr std::string_view ToString(DriverError e) noexcept {
    switch (e) {
        case DriverError::None:              return "None";
        case DriverError::InvalidParameter: return "InvalidParameter";
        case DriverError::Timeout:          return "Timeout";
        case DriverError::BufferOverflow:   return "BufferOverflow";
        case DriverError::ProtocolError:    return "ProtocolError";
        case DriverError::DeviceError:      return "DeviceError";
    }
    return "?";
}

template <typename T>
struct DriverResult {
    T           value{};
    DriverError error{DriverError::None};

    constexpr bool ok() const noexcept { return error == DriverError::None; }
    constexpr explicit operator bool() const noexcept { return ok(); }

    static constexpr DriverResult success(T v) noexcept { return {v, DriverError::None}; }
    static constexpr DriverResult failure(DriverError e) noexcept { return {T{}, e}; }
};

template <>
struct DriverResult<void> {
    DriverError error{DriverError::None};
    constexpr bool ok() const noexcept { return error == DriverError::None; }
    constexpr explicit operator bool() const noexcept { return ok(); }
    static constexpr DriverResult success() noexcept { return {DriverError::None}; }
    static constexpr DriverResult failure(DriverError e) noexcept { return {e}; }
};

/// Fields returned by `#VERS` (device information).
struct VersionInfo {
    int32_t device_id{};       ///< PyroScience device type id (D).
    int32_t num_channels{};    ///< Optical channels (N).
    int32_t firmware_version{};///< e.g. 410 → firmware 4.10 (R).
    int32_t sensor_types{};    ///< Bit field S.
    int32_t build_number{};    ///< Firmware build B.
    int32_t features{};        ///< Feature bit field F.
};

/// Raw PSUP Results registers R0..R17 from `MEA` (see documentation §2.9).
struct MeasurementRaw {
    std::array<int32_t, 18> reg{};
};

/// Decoded engineering view (scaled by 0.001 per PSUP).
struct Measurement {
    int32_t  status_raw{};
    double   dphi_deg{};
    double   umolar{};
    double   mbar{};
    double   air_sat_percent{};
    double   temp_sample_c{};
    double   temp_case_c{};
    double   signal_intensity_mv{};
    double   ambient_light_mv{};
    double   pressure_mbar{};
    double   humidity_percent_rh{};
    double   resistor_temp_ohm{};
    double   percent_o2{};
    double   temp_optical_c{};
    double   ph{};
    double   r_value{};
    MeasurementRaw raw{};
};

/// PSUP `MEA` default sensor mask: optical + sample temp + pressure + humidity + case.
constexpr uint32_t kDefaultMeaSensorMask = 47U;

constexpr double ScaleMilli(int32_t v) noexcept {
    return static_cast<double>(v) * 0.001;
}

/// PSUP uses -300000 raw as “invalid / NaN” for a result slot.
constexpr bool IsInvalidRegisterValue(int32_t v) noexcept { return v == -300000; }

namespace status_bits {

constexpr int32_t kWarnAutoAmp      = 1 << 0;
constexpr int32_t kWarnLowSignal    = 1 << 1;
constexpr int32_t kErrDetectorSat   = 1 << 2;
constexpr int32_t kWarnRefLow       = 1 << 3;
constexpr int32_t kErrRefHigh       = 1 << 4;
constexpr int32_t kErrSampleTemp    = 1 << 5;
constexpr int32_t kWarn1000xO2      = 1 << 6;
constexpr int32_t kWarnHighHumidity = 1 << 7;
constexpr int32_t kErrCaseTemp      = 1 << 8;
constexpr int32_t kErrPressure      = 1 << 9;
constexpr int32_t kErrHumidity      = 1 << 10;

}  // namespace status_bits

inline Measurement DecodeMeasurement(const MeasurementRaw& m) noexcept {
    Measurement out{};
    out.raw = m;
    const auto& r = m.reg;
    out.status_raw = r[0];
    out.dphi_deg            = ScaleMilli(r[1]);
    out.umolar              = ScaleMilli(r[2]);
    out.mbar                = ScaleMilli(r[3]);
    out.air_sat_percent     = ScaleMilli(r[4]);
    out.temp_sample_c       = ScaleMilli(r[5]);
    out.temp_case_c         = ScaleMilli(r[6]);
    out.signal_intensity_mv = ScaleMilli(r[7]);
    out.ambient_light_mv    = ScaleMilli(r[8]);
    out.pressure_mbar       = ScaleMilli(r[9]);
    out.humidity_percent_rh = ScaleMilli(r[10]);
    out.resistor_temp_ohm   = ScaleMilli(r[11]);
    out.percent_o2          = ScaleMilli(r[12]);
    out.temp_optical_c      = ScaleMilli(r[13]);
    out.ph                  = ScaleMilli(r[14]);
    out.r_value             = ScaleMilli(r[15]);
    return out;
}

}  // namespace fdo2
