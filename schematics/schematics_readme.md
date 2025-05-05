# Schematics Overview

This document explains the functional blocks of the schematic diagrams for both the **Brain Module** (bike-mounted) and the **Display Module** (helmet-mounted).

---

## Brain Module Schematic

![Brain Module Schematic](image/brain.PNG)

1. **ATmega328 MCU**  
   - **U2**: ATMEGA328P-AU microcontroller  
   - **Clock**: 16 MHz crystal (X1) with load caps C7/C8  
   - **Decoupling**: C1–C4 (100 nF) on VCC/AVCC  
   - **Reset circuit**: R1 (10 kΩ pull-up) + C6 (100 nF), DTR line via SW1 for auto-reset  

2. **Programmer Interface**  
   - **U1**: FT232RL USB-to-UART bridge  
   - **Connector**: MICRO QTZB5.0 (VBUS, D+, D–, ID, GND)  
   - **Signals**: TXD/RXD ↔ MCU PD0/PD1, DTR ↔ reset  
   - **Indicators**: LED1/LED2 with R5/R6 (1 kΩ) for TX/RX activity  

3. **Bluetooth Interface**  
   - **U3**: Bluetooth module (PI0xx)  
   - **Level shifting**: R7 (1 kΩ) on TX, R8 (2 kΩ) on RX  
   - **Connections**: TX/RX, CTS/RTS to MCU; powered from 3.3 V rail  

4. **Power Regulation**  
   - **U5**: AMS1117-3.3 linear regulator  
   - **Input cap**: C10 (22 µF), **Output cap**: C9 (22 µF)  
   - **Battery input**: VBAT → VIN  

5. **Mechanical Mounts**  
   - **Screw footprints**: SCREW1–SCREW4 for enclosure attachment  

---

## Display Module Schematic

![Display Module Schematic](image/display.PNG)

1. **ATmega328 MCU**  
   - **U2**: ATMEGA328P-AU, identical clock & reset circuitry to Brain Module  

2. **Programmer Interface**  
   - **U1**: FT232RL, same USB-to-UART connections for firmware flashing  

3. **Bluetooth Receiver**  
   - **U5**: Bluetooth module wired for BLE RX only  

4. **Battery Charger**  
   - **U6**: MCP73831 Li-ion charger  
   - **Status LEDs**: LED3/LED4 with R8/R11 (470 Ω)  
   - **Input**: VBUS, **Output**: VBAT  

5. **TFT LCD Interface**  
   - **H5 header**:  
     - SCK (D13), MOSI (D11), MISO, CS (D10)  
     - RST (D8), D/C, SDCS, BL (backlight)  

6. **I²C Expansion**  
   - **H2 header**: SCL, SDA for external sensors  

7. **Voltage Booster**  
   - **U8**: MT3608 boost converter  
   - **Feedback**: R12 (7.5 kΩ), R13 (1 kΩ) → 5V_BOOST  
   - **Input cap**: C10 (22 µF), **Output cap**: C7 (22 µF)  

---

**All source files** (`.kicad_sch`, `.kicad_pcb`), Gerbers, BOM, Pick-and-Place, and STEP exports live under each module’s **Project Outputs/** folder.  
