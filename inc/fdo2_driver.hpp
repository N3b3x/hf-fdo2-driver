/**
 * @file fdo2_driver.hpp
 * @brief PyroScience Unified Protocol (PSUP) UART client for optical oxygen modules.
 *
 * @details This driver targets firmware-generation 4.x devices that speak PSUP over
 *          a 3.3 V UART (8 data bits, 1 stop bit, no parity). Typical products
 *          include FDO2-G2, FD-OEM-O2, and PICO-O2; always confirm baud rate and
 *          wiring against the module datasheet.
 *
 *          Public entry points are allocation-free and suitable for bare-metal /
 *          FreeRTOS use. Serialize all calls if multiple tasks share one UART.
 *
 * @copyright Copyright (c) 2026 HardFOC. All rights reserved.
 */
#pragma once

#include "fdo2_types.hpp"
#include "fdo2_uart_interface.hpp"

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace fdo2 {
namespace detail {

inline bool AppendCmd(char* buf, std::size_t cap, std::string_view cmd) noexcept {
    if (cmd.size() + 2U > cap) {
        return false;
    }
    std::memcpy(buf, cmd.data(), cmd.size());
    buf[cmd.size()]     = '\r';
    buf[cmd.size() + 1U] = '\0';
    return true;
}

template <typename UartT>
inline DriverError ReadAsciiLine(UartT& uart, char* buf, std::size_t cap,
                                 uint32_t timeout_ms) noexcept {
    if (cap < 2) {
        return DriverError::InvalidParameter;
    }
    uint8_t tmp[512];
    const std::size_t max_rx = sizeof(tmp) - 1U;
    const std::size_t n      = uart.read(tmp, max_rx, timeout_ms);
    if (n == 0U) {
        return DriverError::Timeout;
    }
    std::size_t pos = 0;
    for (std::size_t i = 0; i < n; ++i) {
        const char c = static_cast<char>(tmp[i]);
        if (c == '\r') {
            break;
        }
        if (c == '\n') {
            continue;
        }
        if (pos + 1 >= cap) {
            return DriverError::BufferOverflow;
        }
        buf[pos++] = c;
    }
    buf[pos] = '\0';
    return DriverError::None;
}

inline const char* SkipWs(const char* p) noexcept {
    while (*p != '\0' && std::isspace(static_cast<unsigned char>(*p)) != 0) {
        ++p;
    }
    return p;
}

inline DriverError Tokenize(const char* line, char* store, std::size_t store_cap,
                             const char* tokens[], std::size_t max_tokens,
                             std::size_t* out_count) noexcept {
    *out_count = 0;
    const char* p = SkipWs(line);
    if (*p == '\0') {
        return DriverError::ProtocolError;
    }
    std::size_t bi = 0;
    while (*p != '\0' && *out_count < max_tokens) {
        if (bi + 1U >= store_cap) {
            return DriverError::BufferOverflow;
        }
        const std::size_t start = bi;
        tokens[*out_count]      = store + start;
        while (*p != '\0' && std::isspace(static_cast<unsigned char>(*p)) == 0) {
            if (bi + 1U >= store_cap) {
                return DriverError::BufferOverflow;
            }
            store[bi++] = *p++;
        }
        if (bi + 1U >= store_cap) {
            return DriverError::BufferOverflow;
        }
        store[bi++] = '\0';
        ++(*out_count);
        p = SkipWs(p);
    }
    if (*p != '\0') {
        return DriverError::ProtocolError;
    }
    return DriverError::None;
}

inline int32_t ParseI32(const char* s, bool* ok) noexcept {
    char* end = nullptr;
    const long v = std::strtol(s, &end, 10);
    if (ok != nullptr) {
        *ok = (end != s) && (*end == '\0');
    }
    constexpr long kMin = -2147483648L;
    constexpr long kMax = 2147483647L;
    if (v < kMin || v > kMax) {
        if (ok != nullptr) {
            *ok = false;
        }
        return 0;
    }
    return static_cast<int32_t>(v);
}

inline uint64_t ParseU64(const char* s, bool* ok) noexcept {
    char* end = nullptr;
    const unsigned long long v = std::strtoull(s, &end, 10);
    if (ok != nullptr) {
        *ok = (end != s) && (*end == '\0');
    }
    return static_cast<uint64_t>(v);
}

}  // namespace detail

/**
 * @brief PSUP client bound to one UART adapter.
 * @tparam UartT Concrete type inheriting `UartInterface<UartT>`.
 */
template <typename UartT>
class Driver {
public:
    explicit Driver(UartT& uart) noexcept : uart_(uart) {}

    void SetLineTimeoutMs(uint32_t ms) noexcept { line_timeout_ms_ = ms; }
    void SetMeaTimeoutMs(uint32_t ms) noexcept { mea_timeout_ms_ = ms; }

    uint32_t GetLineTimeoutMs() const noexcept { return line_timeout_ms_; }
    uint32_t GetMeaTimeoutMs() const noexcept { return mea_timeout_ms_; }

    DriverResult<VersionInfo> ReadVersion() noexcept {
        char tx[16];
        if (!detail::AppendCmd(tx, sizeof(tx), "#VERS")) {
            return DriverResult<VersionInfo>::failure(DriverError::InvalidParameter);
        }
        char line[256];
        const auto err = TransactLine(tx, line, sizeof(line));
        if (err != DriverError::None) {
            return DriverResult<VersionInfo>::failure(err);
        }
        char tstore[192];
        const char* tok[16];
        std::size_t nt = 0;
        if (detail::Tokenize(line, tstore, sizeof(tstore), tok, 16, &nt) != DriverError::None ||
            nt < 7) {
            return DriverResult<VersionInfo>::failure(DriverError::ProtocolError);
        }
        if (std::strncmp(tok[0], "#VERS", 5) != 0) {
            if (std::strncmp(tok[0], "#ERRO", 5) == 0) {
                return DriverResult<VersionInfo>::failure(DriverError::DeviceError);
            }
            return DriverResult<VersionInfo>::failure(DriverError::ProtocolError);
        }
        VersionInfo v{};
        bool ok = true;
        v.device_id        = detail::ParseI32(tok[1], &ok);
        if (!ok) {
            return DriverResult<VersionInfo>::failure(DriverError::ProtocolError);
        }
        v.num_channels     = detail::ParseI32(tok[2], &ok);
        v.firmware_version = detail::ParseI32(tok[3], &ok);
        v.sensor_types     = detail::ParseI32(tok[4], &ok);
        v.build_number     = detail::ParseI32(tok[5], &ok);
        v.features         = detail::ParseI32(tok[6], &ok);
        if (!ok) {
            return DriverResult<VersionInfo>::failure(DriverError::ProtocolError);
        }
        return DriverResult<VersionInfo>::success(v);
    }

    DriverResult<uint64_t> ReadUniqueId() noexcept {
        char tx[16];
        if (!detail::AppendCmd(tx, sizeof(tx), "#IDNR")) {
            return DriverResult<uint64_t>::failure(DriverError::InvalidParameter);
        }
        char line[128];
        const auto err = TransactLine(tx, line, sizeof(line));
        if (err != DriverError::None) {
            return DriverResult<uint64_t>::failure(err);
        }
        char tstore[160];
        const char* tok[8];
        std::size_t nt = 0;
        if (detail::Tokenize(line, tstore, sizeof(tstore), tok, 8, &nt) != DriverError::None ||
            nt < 2) {
            return DriverResult<uint64_t>::failure(DriverError::ProtocolError);
        }
        if (std::strncmp(tok[0], "#IDNR", 5) != 0) {
            if (std::strncmp(tok[0], "#ERRO", 5) == 0) {
                return DriverResult<uint64_t>::failure(DriverError::DeviceError);
            }
            return DriverResult<uint64_t>::failure(DriverError::ProtocolError);
        }
        bool ok = false;
        const uint64_t id = detail::ParseU64(tok[1], &ok);
        if (!ok) {
            return DriverResult<uint64_t>::failure(DriverError::ProtocolError);
        }
        return DriverResult<uint64_t>::success(id);
    }

    DriverResult<void> PowerUpSensors() noexcept { return SimpleDeviceCommand("#PWUP"); }
    DriverResult<void> PowerDownSensors() noexcept { return SimpleDeviceCommand("#PDWN"); }

    DriverResult<Measurement> TriggerMeasurement(uint8_t channel = 1,
                                                 uint32_t sensor_mask = kDefaultMeaSensorMask,
                                                 uint32_t timeout_ms = 0U) noexcept {
        if (channel == 0U) {
            return DriverResult<Measurement>::failure(DriverError::InvalidParameter);
        }
        char tx[48];
        const int n = std::snprintf(tx, sizeof(tx), "MEA %u %lu\r", static_cast<unsigned>(channel),
                                    static_cast<unsigned long>(sensor_mask));
        if (n <= 0 || static_cast<std::size_t>(n) >= sizeof(tx)) {
            return DriverResult<Measurement>::failure(DriverError::InvalidParameter);
        }
        char line[640];
        const uint32_t tmo = (timeout_ms != 0U) ? timeout_ms : mea_timeout_ms_;
        const auto err     = TransactLine(tx, line, sizeof(line), tmo);
        if (err != DriverError::None) {
            return DriverResult<Measurement>::failure(err);
        }
        char tstore[512];
        const char* tok[32];
        std::size_t nt = 0;
        if (detail::Tokenize(line, tstore, sizeof(tstore), tok, 32, &nt) != DriverError::None ||
            nt < 3U + 18U) {
            return DriverResult<Measurement>::failure(DriverError::ProtocolError);
        }
        if (std::strncmp(tok[0], "MEA", 3) != 0) {
            if (std::strncmp(tok[0], "#ERRO", 5) == 0) {
                return DriverResult<Measurement>::failure(DriverError::DeviceError);
            }
            return DriverResult<Measurement>::failure(DriverError::ProtocolError);
        }
        MeasurementRaw raw{};
        for (int i = 0; i < 18; ++i) {
            bool ok = false;
            raw.reg[static_cast<std::size_t>(i)] = detail::ParseI32(tok[3U + static_cast<std::size_t>(i)], &ok);
            if (!ok) {
                return DriverResult<Measurement>::failure(DriverError::ProtocolError);
            }
        }
        return DriverResult<Measurement>::success(DecodeMeasurement(raw));
    }

private:
    DriverResult<void> SimpleDeviceCommand(std::string_view cmd) noexcept {
        char tx[24];
        if (!detail::AppendCmd(tx, sizeof(tx), cmd)) {
            return DriverResult<void>::failure(DriverError::InvalidParameter);
        }
        char line[64];
        const auto err = TransactLine(tx, line, sizeof(line));
        if (err != DriverError::None) {
            return DriverResult<void>::failure(err);
        }
        char tstore[64];
        const char* tok[4];
        std::size_t nt = 0;
        if (detail::Tokenize(line, tstore, sizeof(tstore), tok, 4, &nt) != DriverError::None ||
            nt < 1) {
            return DriverResult<void>::failure(DriverError::ProtocolError);
        }
        if (std::strncmp(tok[0], cmd.data(), cmd.size()) != 0) {
            if (std::strncmp(tok[0], "#ERRO", 5) == 0) {
                return DriverResult<void>::failure(DriverError::DeviceError);
            }
            return DriverResult<void>::failure(DriverError::ProtocolError);
        }
        return DriverResult<void>::success();
    }

    DriverError TransactLine(const char* tx, char* line, std::size_t line_cap,
                             uint32_t timeout_ms = 0U) noexcept {
        uart_.flush_rx();
        uart_.write(reinterpret_cast<const uint8_t*>(tx), std::strlen(tx));
        const uint32_t tmo = (timeout_ms != 0U) ? timeout_ms : line_timeout_ms_;
        return detail::ReadAsciiLine(uart_, line, line_cap, tmo);
    }

    UartT&   uart_;
    uint32_t line_timeout_ms_{250};
    uint32_t mea_timeout_ms_{3000};
};

}  // namespace fdo2
