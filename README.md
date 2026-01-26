# Doom on ESP32 (Archival)

> **Status**: This project is kept for archival purposes. It faces runtime errors on real hardware and is not currently functional.

---

## Overview

This is an ESP32 port of the classic **Doom** engine based on PrBoom. It was originally intended to run on ESP32 with an ILI9341 SPI LCD and SD card, but due to persistent runtime issues on actual hardware, it is preserved here as a reference.

---

## Features (Intended)

- ILI9341 320x240 SPI LCD display
- SD card for IWAD storage
- ESP32 compatibility (originally targeting boards with PSRAM)
- Software rendering path

---

## Known Issues

- **Runtime failures on real hardware**: The application crashes or fails to initialize reliably on actual ESP32 boards.
- **SD card mounting**: Unstable SD card initialization and file access.
- **Memory allocation**: May not work reliably on boards without PSRAM.

---

## Hardware Requirements (Intended)

- ESP32 (PSRAM recommended in original design)
- ILI9341 SPI LCD (320x240)
- MicroSD card (FAT16/FAT32)
- Breadboard and jumper wires

---

## Wiring (Reference)

| ESP32 Pin | LCD Pin | SD Card Pin |
| --------- | ------- | ----------- |
| GPIO23    | MOSI    | MOSI        |
| GPIO19    | MISO    | MISO        |
| GPIO18    | SCLK    | SCLK        |
| GPIO21    | CS      | —           |
| GPIO13    | —       | CS          |
| GPIO2     | DC      | —           |
| GPIO4     | RST     | —           |
| GPIO5     | BL      | —           |
| 3.3V      | VCC     | VCC         |
| GND       | GND     | GND         |

---

## Build Instructions (For Reference)

### Prerequisites

- ESP-IDF v5.x
- CMake
- ESP32 toolchain

### Steps

```bash
git clone <this-repo>
cd doom-esp-with-arduino
idf.py menuconfig  # Configure pins if needed
idf.py build flash monitor
```

---

## Project Structure

- `main/`: ESP32 application entry point
- `components/prboom-esp32-compat/`: ESP32-specific compatibility layer
- `components/prboom/`: PrBoom game engine
- `partitions.csv`: Custom partition table

---

## License

This project is licensed under the GNU General Public License v2.0. See [LICENSE](LICENSE) for details.

---

## Contributing

Due to the archival status, this project is not actively accepting contributions. However, if you want to
make the project better .. feel free to fork it and submit a pull request.

---

## Acknowledgments

- PrBoom team for the original engine
- ESP-IDF developers
- Doom community

---

## Disclaimer

This software is provided **as-is** for archival and educational purposes. It may not run correctly on current hardware. Use at your own responsibility.
