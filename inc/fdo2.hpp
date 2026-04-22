/**
 * @file fdo2.hpp
 * @brief Umbrella header for the HF-FDO2 / PyroScience PSUP UART driver.
 *
 * @details Include this header to pull in the UART transport interface, typed
 *          results, and `fdo2::Driver<UartT>`. See `fdo2_driver.hpp` for API
 *          documentation and `examples/esp32/` for a ready-made ESP-IDF UART
 *          adapter.
 *
 * @copyright Copyright (c) 2026 HardFOC. All rights reserved.
 */
#pragma once

#include "fdo2_uart_interface.hpp"
#include "fdo2_types.hpp"
#include "fdo2_driver.hpp"
