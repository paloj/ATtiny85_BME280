# ATtiny85 + BME280 Logger Project

## Project Overview
This project is a compact environmental data logger using an **ATtiny85** microcontroller and a **BME280** sensor to measure temperature, humidity, and pressure. The firmware is optimized for low memory usage while ensuring stable sensor readings.

## Features
- Compact code design for the ATtiny85 (8KB Flash, 512B RAM).
- Uses **TinyWireM** for efficient I2C communication on ATtiny85.
- Accurate environmental data readings with averaging and validation mechanisms.
- Serial output via a software UART at **2400 baud** for reliable communication with low overhead.
- Circular buffer implementation ensures smooth data processing with minimal RAM usage.

---

## Requirements
### Hardware
- ATtiny85 microcontroller
- BME280 sensor (I²C mode, address `0x77`)
- USBasp programmer (for flashing firmware)
- 3.3V voltage regulator for BME280 stability

### Software
- **PlatformIO** for VSCode
- Libraries:
  - [TinyWireM](https://github.com/adafruit/TinyWireM)
  - [Tiny_BME280 Library](https://github.com/jasonacox/Tiny_BME280_Library.git)

---

## Installation

### Step 1: Clone the Repository
```bash
git clone <repository-url>
cd ATtiny85_BME280
```

### Step 2: Install Dependencies
Install the necessary libraries via PlatformIO:
```ini
lib_deps =
    TinyWireM
    https://github.com/jasonacox/Tiny_BME280_Library.git
```

### Step 3: Build the Firmware
In PlatformIO, run:
```bash
pio run
```

### Step 4: Upload to ATtiny85
Connect your USBasp programmer and run:
```bash
pio run -t upload
```

---

## Configuration
In `platformio.ini`:
- CPU frequency is set to **8MHz** for stable software UART.
- Optimization flags are added for size reduction and improved performance.

```ini
build_flags =
    -DF_CPU=8000000L
    -Os
    -ffunction-sections
    -fdata-sections
    -flto
    -Wl,--gc-sections
    -fno-exceptions
    -fno-rtti
    -fno-threadsafe-statics
```

---

## Usage
1. Power on the ATtiny85 with the BME280 sensor connected via I²C.
2. Use a serial monitor set to **2400 baud** with **time filters** enabled for easier data logging.
3. The firmware outputs sensor readings in the following format:

```
Temp: XXXX | Humidity: XXXX | Pressure: XXXX
```

Each value is printed as an integer representing:
- **Temperature** in hundredths of a degree Celsius
- **Humidity** in hundredths of a percent
- **Pressure** in hPa

---

## Debugging
- Initialization errors print an `X` for failed sensor detection.
- Sensor readings are validated and adjusted to filter out unrealistic values.

---

## License
This project is open-source and distributed under the **MIT License**.

---

## Author
Developed by Janne Palo as part of a humidity logger project.

For questions, issues, or improvements, please open an issue or submit a pull request.
