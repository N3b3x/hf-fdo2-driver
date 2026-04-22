// =============================================================================
// HF-FDO2-G2 — minimal ESP-IDF example (data sheet v5 §4 UART)
// =============================================================================
// Default wiring (ESP32-S3): UART1, TX=GPIO47, RX=GPIO21, **19200** 8N1.
// Waits ~1.1 s after boot before commands (module power-up window, §4).
// #VERS / #IDNR once, then #MRAW at 1 Hz (pO₂, T, P, %O₂ helper, status).
// =============================================================================

#include "fdo2.hpp"
#include "hf_fdo2_esp_uart.hpp"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace {

constexpr const char* TAG = "FDO2Min";

// UART_NUM_1 keeps UART0 free for USB-serial JTAG console on many devkits.
using Uart = hf_fdo2_examples::Fdo2EspIdfUart<UART_NUM_1, 47, 21>;

}  // namespace

extern "C" void app_main(void) {
    ESP_LOGI(TAG, "HF-FDO2-G2 minimal (UART%d, TX=%d RX=%d, %lu baud 8N1)", static_cast<int>(UART_NUM_1),
             47, 21, static_cast<unsigned long>(fdo2::kFdo2G2DefaultBaud));

    ESP_ERROR_CHECK(Uart::Install());

    Uart uart;
    fdo2::Driver<Uart> dev(uart);
    dev.SetMeasureTimeoutMs(300);

    uart.delay_ms_impl(fdo2::kFdo2G2PowerUpSettleMs);

    const auto ver = dev.ReadVersion();
    if (!ver.ok()) {
        ESP_LOGE(TAG, "#VERS failed: %s (device err=%ld)", fdo2::ToString(ver.error).data(),
                 static_cast<long>(dev.LastDeviceErrorCode()));
    } else {
        ESP_LOGI(TAG, "D=%ld N=%ld fw=%.2f S=%ld", static_cast<long>(ver.value.device_id),
                 static_cast<long>(ver.value.num_channels), fdo2::FirmwareRevisionToDouble(ver.value.firmware_revision),
                 static_cast<long>(ver.value.sensor_types));
    }

    const auto uid = dev.ReadUniqueId();
    if (uid.ok()) {
        ESP_LOGI(TAG, "unique_id=%llu", static_cast<unsigned long long>(uid.value));
    }

    while (true) {
        const auto m = dev.MeasureMraw(0);
        if (!m.ok()) {
            ESP_LOGW(TAG, "#MRAW failed: %s (device err=%ld)", fdo2::ToString(m.error).data(),
                     static_cast<long>(dev.LastDeviceErrorCode()));
        } else {
            const double pct = fdo2::VolumePercentO2(m.value.p_o2_hpa, m.value.pressure_mbar);
            const bool o2_ok = fdo2::MoxyStatusOkForOxygen(m.value.status_u32);
            ESP_LOGI(TAG,
                     "pO2=%.3f hPa  T=%.3f C  P=%.3f mbar  %%O2~%.2f  O2_status_ok=%d  raw_status=0x%lx  "
                     "dphi=%.3fdeg  I=%.1fmV  A=%.1fmV  RH=%.2f%%",
                     m.value.p_o2_hpa, m.value.temp_c, m.value.pressure_mbar, pct, static_cast<int>(o2_ok),
                     static_cast<unsigned long>(m.value.status_u32), m.value.dphi_deg,
                     m.value.signal_intensity_mv, m.value.ambient_light_mv, m.value.rh_in_housing_pct);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
