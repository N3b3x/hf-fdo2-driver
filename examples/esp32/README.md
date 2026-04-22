# ESP32 examples — HF-FDO2 (PyroScience PSUP)

These projects demonstrate the header-only driver against a **FDO2-G2** (or
compatible UART command set per data sheet v5 §4) using **ESP-IDF** on ESP32-S3.
Default link is **19200 baud** after power-up.

## Prerequisites

- ESP-IDF **v5.4+** (CI uses `release/v5.5`)
- Python 3 with PyYAML (for `scripts/generate_matrix.py`)

## One-time setup

From `examples/esp32/`:

```bash
git submodule update --init --recursive
```

This pulls `scripts/` (`hf-espidf-project-tools`) with `build_app.sh`,
`flash_app.sh`, and the CI matrix generator.

## Build / flash

```bash
cd examples/esp32
./scripts/build_app.sh fdo2_minimal_example Debug
./scripts/flash_app.sh fdo2_minimal_example Debug
```

Override UART pins or baud in `main/fdo2_minimal_example.cpp` to match your
carrier board and the module datasheet.

## Protocol reference

Official PSUP documentation is published by PyroScience (firmware 4.x), e.g.
“PyroScience Unified Protocol” reference manuals on their OEM downloads page.
The driver maps `MEA` result integers per §2.9 Results registers.
