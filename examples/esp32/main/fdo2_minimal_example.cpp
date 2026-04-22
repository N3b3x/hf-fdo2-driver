// =============================================================================
// HF-FDO2-G2 — minimal ESP-IDF example (data sheet v5 §4 UART)
// =============================================================================
// Default: UART1 TX=GPIO17, RX=GPIO18, **19200** 8N1 (factory default per §4).
// Waits ~1.1 s after power-up before commands (data sheet: ~1 s module boot).
// Uses #VERS / #IDNR once, then #MRAW at 1 Hz for pO2, T, pressure, %O₂ helper.
// =============================================================================

#include "fdo2.hpp"

#include "driver/uart.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace {

constexpr const char* TAG       = "FDO2Min";
constexpr uart_port_t kPort     = UART_NUM_1;
constexpr int         kTxPin    = 17;
constexpr int         kRxPin    = 18;
constexpr int         kRxBuf    = 2048;
constexpr int         kTxBuf    = 0;
constexpr uint32_t    kBaud     = fdo2::kFdo2G2DefaultBaud;

class EspUartAdapter : public fdo2::UartInterface<EspUartAdapter> {
public:
    void write(const std::uint8_t* data, std::size_t len) noexcept {
        if (len > 0) {
            (void)uart_write_bytes(kPort, data, static_cast<int>(len));
        }
    }
    std::size_t read(std::uint8_t* out, std::size_t max, std::uint32_t timeout_ms) noexcept {
        const int n = uart_read_bytes(kPort, out, static_cast<int>(max), pdMS_TO_TICKS(timeout_ms));
        return (n < 0) ? 0U : static_cast<std::size_t>(n);
    }
    void flush_rx() noexcept { uart_flush_input(kPort); }
    void delay_ms_impl(std::uint32_t ms) noexcept { vTaskDelay(pdMS_TO_TICKS(ms)); }
};

}  // namespace

extern "C" void app_main(void) {
    ESP_LOGI(TAG, "HF-FDO2-G2 minimal (UART%d, %lu baud 8N1)", static_cast<int>(kPort),
             static_cast<unsigned long>(kBaud));

    uart_config_t uart_cfg = {};
    uart_cfg.baud_rate           = static_cast<int>(kBaud);
    uart_cfg.data_bits           = UART_DATA_8_BITS;
    uart_cfg.parity              = UART_PARITY_DISABLE;
    uart_cfg.stop_bits           = UART_STOP_BITS_1;
    uart_cfg.flow_ctrl           = UART_HW_FLOWCTRL_DISABLE;
    uart_cfg.rx_flow_ctrl_thresh = 0;
    uart_cfg.source_clk          = UART_SCLK_DEFAULT;
    ESP_ERROR_CHECK(uart_driver_install(kPort, kRxBuf, kTxBuf, 0, nullptr, 0));
    ESP_ERROR_CHECK(uart_param_config(kPort, &uart_cfg));
    ESP_ERROR_CHECK(uart_set_pin(kPort, kTxPin, kRxPin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    EspUartAdapter uart;
    fdo2::Driver<EspUartAdapter> dev(uart);
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
            const double pct =
                fdo2::VolumePercentO2(m.value.p_o2_hpa, m.value.pressure_mbar);
            ESP_LOGI(TAG,
                     "pO2=%.3f hPa  T=%.3f C  P=%.3f mbar  %%O2~%.2f (membrane≈vent)  status=0x%lx  "
                     "dphi=%.3fdeg  I=%.1fmV  A=%.1fmV  RH=%.2f%%",
                     m.value.p_o2_hpa, m.value.temp_c, m.value.pressure_mbar, pct,
                     static_cast<unsigned long>(m.value.status_u32), m.value.dphi_deg,
                     m.value.signal_intensity_mv, m.value.ambient_light_mv, m.value.rh_in_housing_pct);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
