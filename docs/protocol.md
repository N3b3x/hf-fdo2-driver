# FDO2-G2 UART protocol (data sheet alignment)

This driver follows **PyroScience FDO2-G2**, document version **v5 (03/2026)**,
**§4 Communication Interface**.

## Electrical / framing

- **3.0 V UART levels** (max 3.3 V on IO).
- **8N1**, no handshake.
- **Default baud after power-up: 19200** (also 1200 … 115200 are programmable via `#BAUD`).
- Allow **~1 s** after power-up before sending commands.
- Each command from host ends with **`\\r`** (0x0D), or **`\\r\\n`** (§4.1).
- Normal responses end with **`\\r`** only. If **CRC** is enabled (`#CRCE 1`), the
  line ends with ` : <decimal CRC16>` before `\\r`; the driver strips that suffix
  before parsing.

## Implemented commands

| Command | Response shape | Driver API |
|--------|------------------|------------|
| `#VERS` | `#VERS D N R S` | `ReadVersion()` |
| `#IDNR` | `#IDNR N` (64-bit decimal) | `ReadUniqueId()` |
| `#MOXY` | `#MOXY O T S` | `MeasureMoxy()` |
| `#MRAW` | `#MRAW O T S D I A P H` | `MeasureMraw()` |
| `#LOGO` | `#LOGO` | `FlashLogo()` |

## Numeric decoding (same as data sheet tables)

- **O** — pO₂ in **10⁻³ hPa** → hPa = `O * 1e-3`.
- **T** — temperature in **m°C** → °C = `T * 1e-3`.
- **S** — **unsigned 32-bit** status; see data sheet §4.3 bit definitions. Normal
  operation: **S = 0 or 1** only; other values mean warnings/errors — host must
  gate oxygen use accordingly.
- **#MRAW** extras: **D** m°, **I** and **A** in µV → mV ×10⁻³, **P** in µbar → mbar ×10⁻⁶,
  **H** in m%RH → %RH ×10⁻³.

## Volume % O₂

When pressure at the sensing membrane matches the **vent / back-side** pressure
used for **P** in `#MRAW`, the data sheet gives:

`%O₂ = 100 × pO₂ [hPa] / pAir [hPa]` with **mbar ≈ hPa** for the ratio.

Use `VolumePercentO2(p_o2_hpa, pressure_mbar)` in `fdo2_types.hpp`.

## Errors

`#ERRO E` with signed **E** (Table 1, §4.4). On `DriverError::DeviceError`, read
`Driver::LastDeviceErrorCode()`.

## Not implemented here

Commands that **write flash** (`#CALO`, `#CAHI`, `#CRCE`, `#SETM`, `#BCST`, `#BAUD`,
`#WRUM`, …) are intentionally omitted from the minimal API until a safe HAL wrapper
exists — they are rate-limited by **~20 000** flash cycles total (data sheet warning).
