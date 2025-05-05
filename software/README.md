# Halo Vision Software

This directory contains the software components for the Halo Vision project.

## Firmware

The `firmware/` directory holds the embedded software for the two main hardware modules:

- **`firmware/brain_module/`**: Firmware for the main processing unit. This module handles GPS data parsing, navigation logic, sensor reading (turn signals, speed, battery), and BLE communication with the Display Module. It runs on an ATmega328P microcontroller.

- **`firmware/display_module/`**: Firmware for the helmet-mounted display unit. This module receives data from the Brain Module via BLE, manages the LCD display, monitors its own battery and charging status, and renders the user interface. It also runs on an ATmega328P microcontroller.

Each firmware directory contains:
    - `include/`: Header files (.h) defining interfaces and configurations.
    - `src/`: Source code files (.c) implementing the firmware logic, drivers, and HAL.
    - `Makefile`: Build script for compiling the firmware using `avr-gcc`.

## Host Tools

The `host_tools/` directory contains utility scripts for development and maintenance:

- **`flash_firmware.py`**: (Placeholder) Script for flashing compiled firmware onto the microcontrollers.
- **`diagnostics.py`**: (Placeholder) Script for running diagnostics or communicating with the modules for debugging.

## Building Firmware

Navigate to either the `firmware/brain_module` or `firmware/display_module` directory and use the provided `Makefile`. Requires the `avr-gcc` toolchain and `avrdude` (for flashing).

```bash
# Example for Brain Module
cd firmware/brain_module
make all  # Compile the firmware
make flash # Flash the firmware (requires programmer configuration in Makefile)
make clean # Remove build artifacts
```
