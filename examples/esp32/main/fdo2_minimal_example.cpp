// =============================================================================
// HF-FDO2 — minimal ESP-IDF example (PyroScience PSUP over UART)
// =============================================================================
// Reads #VERS once, then loops with MEA (default sensor mask 47).
// Default wiring: UART1 TX=GPIO17, RX=GPIO18, 115200 8N1 (matches many
// PyroScience USB-UART adapters labeled Pyro_115200). Use the module datasheet
// to confirm baud rate and pinout for your assembly.
// =============================================================================

#include "fdo2.hpp"

#include "driver/uart.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace {

constexpr const char* TAG       = "FDO2Min";
constexpr uart_port_t kPort     = UART_NUM_1;
constexpr int         kTxPin  = 17;
constexpr int         kRxPin  = 18;
constexpr int         kRxBuf    = 2048;
constexpr int         kTxBuf    = 0;
constexpr uint32_t    kBaud     = 115200;

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
    ESP_LOGI(TAG, "HF-FDO2 minimal example (ESP32-S3 UART%d, %lu baud)", static_cast<int>(kPort),
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
    dev.SetLineTimeoutMs(400);
    dev.SetMeaTimeoutMs(4000);

    const auto ver = dev.ReadVersion();
    if (!ver.ok()) {
        ESP_LOGE(TAG, "#VERS failed: %s — check wiring, baud, and 3.3 V levels",
                 fdo2::ToString(ver.error).data());
    } else {
        ESP_LOGI(TAG, "device_id=%ld channels=%ld fw=%ld build=%ld", static_cast<long>(ver.value.device_id),
                 static_cast<long>(ver.value.num_channels), static_cast<long>(ver.value.firmware_version),
                 static_cast<long>(ver.value.build_number));
    }

    const auto uid = dev.ReadUniqueId();
    if (uid.ok()) {
        ESP_LOGI(TAG, "unique_id=%llu", static_cast<unsigned long long>(uid.value));
    }

    while (true) {
        const auto m = dev.TriggerMeasurement(1, fdo2::kDefaultMeaSensorMask, 0);
        if (!m.ok()) {
            ESP_LOGW(TAG, "MEA failed: %s", fdo2::ToString(m.error).data());
        } else {
            ESP_LOGI(TAG, "O2=%.3f %%  pO2=%.3f hPa  T_sample=%.3f C  status=0x%lx", m.value.percent_o2,
                     m.value.mbar, m.value.temp_sample_c, static_cast<unsigned long>(m.value.status_raw));
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
