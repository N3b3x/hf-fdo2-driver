# PyroScience Unified Protocol (PSUP) — integration notes

This repository implements a **small, host-side subset** of PSUP firmware
generation **4.x** over UART:

- `#VERS` — device type, channel count, firmware revision  
- `#IDNR` — 64-bit unique id  
- `#PWUP` / `#PDWN` — sensor power rails  
- `MEA C S` — trigger acquisition and return the 18 signed **Results** registers

Messages are **ASCII**, tokens separated by spaces, each frame terminated with
**carriage return** (`\r`, `0x0D`).

## Authoritative documentation

PyroScience publishes the full register map, calibration flow, status bits, and
optional CRC rules. Start from their OEM / developer downloads (search for
“PyroScience Unified Protocol” PDF for your firmware band).

## Engineering scaling

Unless the module enables the internal “1000×Oxygen” option, Results fields
are communicated as **signed 32-bit integers in fixed-point with factor 0.001**
(e.g. `20980` → 20.980 %O₂). The driver exposes both raw registers and scaled
`double` fields in `fdo2::Measurement`.

## Invalid samples

The reference manual defines sentinel raw values (`-300000`) for invalid
numeric slots; use `fdo2::IsInvalidRegisterValue()` when consuming raw registers.
