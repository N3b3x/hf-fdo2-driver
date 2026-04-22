---
layout: default
title: "Hardware setup"
nav_order: 4
parent: "Documentation"
permalink: /docs/hardware-setup/
---

# Hardware setup

## Logic levels

The FDO2-G2 UART is **3.0 V** signalling with **3.3 V absolute maximum** on those
pins (data sheet §2 / §4). Your MCU UART must **not** drive 5 V TTL into the
sensor. Use level shifters or a 3.3 V-native host (for example ESP32-S3).

## UART parameters

| Parameter | Default | Notes |
|-----------|---------|--------|
| Baud | **19200** | Factory default after every power cycle |
| Data | 8 bits | |
| Parity | None | |
| Stop | 1 bit | |
| Flow control | None | |

Higher baud is possible only after a successful `#BAUD` command (not implemented
in this driver build).

## Boot delay

The module needs on the order of **one second** after power-up before it accepts
commands. The ESP32 example waits `kFdo2G2PowerUpSettleMs` (~1100 ms) before `#VERS`.

## Connector

The data sheet references **Molex 560020-0420** for the module harness. Route
TX/RX/GND only after confirming pinout for your cable revision.

## Fast versus verbose reads

- **`#MOXY`** — smallest response; ideal for **high-rate control loops**.
- **`#MRAW`** — adds pressure, humidity, dphi, and intensities for diagnostics and
  for the documented **%O₂** ratio when **P** tracks vent-side pressure.

**Next:** [CMake integration →](cmake_integration.md)
