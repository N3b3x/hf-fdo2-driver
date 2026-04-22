// =============================================================================
// HF-FDO2-G2 — sensor bring-up demo (ESP32-S3)
// =============================================================================
// Same default pins as minimal: UART1 TX=GPIO47, RX=GPIO21 @ 19200 8N1.
// - One-time #VERS / #IDNR
// - Fast loop: #MOXY every 200 ms for live pO₂ / T / status (control-friendly)
// - Slow tick: #MRAW every 3 s for pressure, RH, dphi, intensities + %O₂
// =============================================================================

#include "fdo2.hpp"
#include "hf_fdo2_esp_uart.hpp"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace {

constexpr const char* TAG = "FDO2Demo";

using Uart = hf_fdo2_examples::Fdo2EspIdfUart<UART_NUM_1, 47, 21>;

void LogMoxyLine(const char* prefix, const fdo2::MoxyReading& r) {
    const bool ok = fdo2::MoxyStatusOkForOxygen(r.status_u32);
    ESP_LOGI(TAG,
             "%s #MOXY pO2=%.3f hPa T=%.3f C O2_status_ok=%d status=0x%lx", prefix, r.p_o2_hpa, r.temp_c,
             static_cast<int>(ok), static_cast<unsigned long>(r.status_u32));
}

void LogMrawLine(const char* prefix, const fdo2::MrawReading& r) {
    const double pct = fdo2::VolumePercentO2(r.p_o2_hpa, r.pressure_mbar);
    const bool ok = fdo2::MoxyStatusOkForOxygen(r.status_u32);
    ESP_LOGI(TAG,
             "%s #MRAW pO2=%.3f hPa T=%.3f C P=%.3f mbar %%O2~%.2f O2_ok=%d st=0x%lx dphi=%.3f RH=%.2f%%", prefix,
             r.p_o2_hpa, r.temp_c, r.pressure_mbar, pct, static_cast<int>(ok),
             static_cast<unsigned long>(r.status_u32), r.dphi_deg, r.rh_in_housing_pct);
}

}  // namespace

extern "C" void app_main(void) {
    ESP_LOGI(TAG, "FDO2-G2 sensor demo — UART%d TX=%d RX=%d @ %lu baud", static_cast<int>(UART_NUM_1), 47, 21,
             static_cast<unsigned long>(fdo2::kFdo2G2DefaultBaud));

    ESP_ERROR_CHECK(Uart::Install());

    Uart uart;
    fdo2::Driver<Uart> dev(uart);
    dev.SetMeasureTimeoutMs(400);
    dev.SetLineTimeoutMs(120);

    uart.delay_ms_impl(fdo2::kFdo2G2PowerUpSettleMs);

    if (const auto ver = dev.ReadVersion(); ver.ok()) {
        ESP_LOGI(TAG, "Connected: D=%ld fw=%.2f", static_cast<long>(ver.value.device_id),
                 fdo2::FirmwareRevisionToDouble(ver.value.firmware_revision));
    } else {
        ESP_LOGE(TAG, "#VERS failed: %s err=%ld", fdo2::ToString(ver.error).data(),
                 static_cast<long>(dev.LastDeviceErrorCode()));
    }
    if (const auto uid = dev.ReadUniqueId(); uid.ok()) {
        ESP_LOGI(TAG, "UID=%llu", static_cast<unsigned long long>(uid.value));
    }

    TickType_t last_mraw = xTaskGetTickCount();
    while (true) {
        const TickType_t now = xTaskGetTickCount();

        if (const auto mx = dev.MeasureMoxy(0); mx.ok()) {
            LogMoxyLine("fast", mx.value);
        } else {
            ESP_LOGW(TAG, "#MOXY: %s (device err=%ld)", fdo2::ToString(mx.error).data(),
                     static_cast<long>(dev.LastDeviceErrorCode()));
        }

        if ((now - last_mraw) >= pdMS_TO_TICKS(3000)) {
            last_mraw = now;
            if (const auto mr = dev.MeasureMraw(0); mr.ok()) {
                LogMrawLine("slow", mr.value);
            } else {
                ESP_LOGW(TAG, "#MRAW: %s (device err=%ld)", fdo2::ToString(mr.error).data(),
                         static_cast<long>(dev.LastDeviceErrorCode()));
            }
        }

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
