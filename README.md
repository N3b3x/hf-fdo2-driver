# hf-fdo2-driver

Hardware-agnostic **C++17** UART client for **PyroScience Unified Protocol (PSUP)**
firmware generation **4.x**, as used on optical oxygen OEM modules (for example
**FDO2-G2**, **FD-OEM-O2**, **PICO-O2**). The code is header-only: link it from
CMake as `hf::fdo2` or use the ESP-IDF component wrapper under
`examples/esp32/components/hf_fdo2`.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

## Features

- CRTP `fdo2::UartInterface<Derived>` — zero virtual calls; bring your own UART.
- `fdo2::Driver<UartT>` — `#VERS`, `#IDNR`, `#PWUP` / `#PDWN`, `MEA` with decoded
  oxygen partial pressure, %O₂, temperatures, humidity/pressure when enabled by
  the sensor mask.
- No heap allocation in the driver paths; suitable for FreeRTOS / bare metal.
- ESP32-S3 **UART1** example (`GPIO17` / `GPIO18`, `115200` 8N1) under `examples/esp32/`.

## Quick start (CMake consumer)

```cmake
add_subdirectory(/path/to/hf-fdo2-driver)
target_link_libraries(your_target PRIVATE hf::fdo2)
```

```cpp
#include "fdo2.hpp"

struct MyUart : fdo2::UartInterface<MyUart> {
  void write(const uint8_t* d, size_t n);
  size_t read(uint8_t* o, size_t m, uint32_t t);
  void flush_rx();
};

MyUart uart;
fdo2::Driver<MyUart> dev(uart);
auto v = dev.ReadVersion();
auto m = dev.TriggerMeasurement(1, fdo2::kDefaultMeaSensorMask);
```

## Documentation

- **Protocol overview:** [`docs/protocol.md`](docs/protocol.md)  
- **Doxygen:** configure `_config/Doxyfile`, then run `doxygen` from `_config/`.  
- **ESP32 workflow:** [`examples/esp32/README.md`](examples/esp32/README.md)

## Official references

PyroScience GmbH publishes datasheets and the PSUP reference manual for your
exact firmware range. Always use those documents for calibration (`CHI` /
`CLO`), broadcast mode, Modbus bridges, and safety limits.

## License

MIT — see [`LICENSE`](LICENSE).
