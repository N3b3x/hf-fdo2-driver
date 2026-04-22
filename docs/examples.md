---
layout: default
title: "Examples"
nav_order: 7
parent: "Documentation"
permalink: /docs/examples/
---

# Examples

## ESP32-S3 (`examples/esp32`)

1. Initialize the tooling submodule:

   ```bash
   cd examples/esp32
   git submodule update --init --recursive
   ```

2. Build (default app `fdo2_minimal_example`, ESP-IDF `release/v5.5`, target `esp32s3`):

   ```bash
   ./scripts/build_app.sh fdo2_minimal_example Debug
   ```

3. Flash and monitor:

   ```bash
   ./scripts/flash_app.sh flash_monitor fdo2_minimal_example Debug
   ```

The minimal example uses **UART1**, **GPIO17** / **GPIO18**, **19200 baud**, waits
for the module boot window, then prints `#VERS`, `#IDNR`, and periodic `#MRAW`
with decoded engineering units.

See [`examples/esp32/README.md`](https://github.com/N3b3x/hf-FDO2-driver/blob/main/examples/esp32/README.md) for matrix and override notes.

**Next:** [Troubleshooting →](troubleshooting.md)
