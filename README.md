# Keiser M3i Bike BLE Emulator

This project emulates a Keiser M3i indoor bike using an ESP32-S3 microcontroller. It broadcasts BLE advertisements that mimic the data format of a real Keiser M3i bike, allowing it to be detected and used by applications that support Keiser M3i bikes.

## Features

- Emulates Keiser M3i bike BLE advertisements
- Simulates realistic RPM, power, and heart rate values
- Compatible with applications that support Keiser M3i bikes
- Uses ESP32's built-in BLE capabilities

## Hardware Requirements

- ESP32-S3-Zero board
- USB cable for programming and power

## Software Requirements

- PlatformIO IDE or VSCode with PlatformIO extension
- ESP32 Arduino Core

## Installation

1. Clone this repository
2. Open the project in PlatformIO
3. Connect your ESP32-S3 board
4. Build and upload the project

## Usage

1. Power on the ESP32-S3
2. The device will start broadcasting BLE advertisements
3. Use a compatible app (like the Keiser M3i app) to connect to the device
4. The app should detect the device as a Keiser M3i bike

## Data Format

The BLE advertisement contains the following data:
- Manufacturer ID: 0x0102
- Version: 6.30
- Equipment ID
- Cadence (RPM)
- Heart Rate
- Power
- Calories
- Duration
- Distance
- Gear

## Configuration

The simulation parameters can be adjusted in `src/main.cpp`:
- `cycleDurationMs`: Duration of one complete simulation cycle
- `stepIntervalMs`: Time between data updates
- RPM, power, and heart rate ranges

## Troubleshooting

If the device is not detected:
- Ensure BLE is enabled on your device
- Check that the app supports Keiser M3i bikes
- Verify the ESP32-S3 is powered and running

## License

This project is open source and available under the MIT License.

## Acknowledgments

- Based on the Keiser M3i bike BLE protocol
- Uses the ESP32 Arduino BLE library 