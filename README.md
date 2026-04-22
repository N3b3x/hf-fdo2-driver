# hf-fdo2-driver

Hardware-agnostic **C++17** UART client for the **PyroScience FDO2-G2** optical
oxygen sensor (**data sheet v5, §4**): `#VERS`, `#IDNR`, `#MOXY`, `#MRAW`, `#LOGO`,
with scaling and status bits matching the published command tables. Default UART
baud is **19200** after power-up. The code is header-only: link it from CMake as
`hf::fdo2` or use the ESP-IDF component wrapper under `examples/esp32/components/hf_fdo2`.

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

## Features

- CRTP `fdo2::UartInterface<Derived>` — zero virtual calls; bring your own UART.
- `fdo2::Driver<UartT>` — `#VERS`, `#IDNR`, `#MOXY` (fast pO₂ + T + status), `#MRAW`
  (adds dphi, intensities, vent-side pressure, internal RH), `#LOGO`; optional
  **CRC suffix** stripping; `#ERRO` code via `LastDeviceErrorCode()`.
- No heap allocation in the driver paths; suitable for FreeRTOS / bare metal.
- ESP32-S3 **UART1** example (`GPIO17` / `GPIO18`, **`19200` 8N1**, ~1.1 s boot delay)
  under `examples/esp32/`.

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
auto m = dev.MeasureMraw();  // or dev.MeasureMoxy() for smallest frame
```

## Documentation

- **Protocol overview:** [`docs/protocol.md`](docs/protocol.md)  
- **Doxygen:** configure `_config/Doxyfile`, then run `doxygen` from `_config/`.  
- **ESP32 workflow:** [`examples/esp32/README.md`](examples/esp32/README.md)

## Official references

Use the **FDO2-G2 data sheet** (and firmware release notes) for accuracy, timing,
calibration (`#CALO` / `#CAHI`), `#SETM` modes, broadcast `#BCST`, and flash-lifetime
limits. Other PyroScience instruments may use different UART command sets.

## License

MIT — see [`LICENSE`](LICENSE).
