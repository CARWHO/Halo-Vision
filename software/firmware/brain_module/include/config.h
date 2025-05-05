#ifndef CONFIG_H
#define CONFIG_H

/**
 * @file config.h
 * @brief Central configuration file for the Halo Vision Brain Module firmware.
 * Defines hardware pins, peripheral settings, and module parameters.
 */

#include <stdint.h> // Standard integer types

// --- System Configuration ---
#define F_CPU 16000000UL // System clock frequency: 16 MHz (from schematic crystal X1)

// --- Peripheral Configuration ---

// UART for GPS (Hardware USART0)
#define GPS_UART_ID         UART_ID_0 // Maps to HW USART0
#define GPS_UART_BAUD       9600UL    // Standard GPS NMEA baud rate
#define GPS_UART_RX_BUFFER_SIZE 256   // Buffer size for incoming NMEA sentences

// UART for BLE (HC-05/06 style) Module
#define BLE_UART_ID         UART_ID_1 // Assigns ID for BLE communication channel
#define BLE_UART_BAUD       115200UL  // Baud rate for BLE communication
#define BLE_UART_TX_BUFFER_SIZE 128   // Transmit buffer size for BLE UART
#define BLE_UART_RX_BUFFER_SIZE 64    // Buffer for incoming BLE responses/data

// I2C (Hardware TWI)
#define MAIN_I2C_ID         I2C_ID_0 // Maps to HW TWI
#define I2C_CLOCK_SPEED     100000UL // 100 kHz standard I2C speed

// --- Pin Definitions (ATmega328P QFP32 package mapping) ---
// These map symbolic names to actual MCU pins based on the schematic.

// Port D Pins
#define UART0_RXD_PIN       PD0 // Pin 2 (QFP32) - Used by GPS_UART_ID
#define UART0_TXD_PIN       PD1 // Pin 3 (QFP32) - Used by GPS_UART_ID
#define LEFT_SIGNAL_PIN     PD2 // Pin 4 (QFP32) - External Interrupt INT0 capable
#define RIGHT_SIGNAL_PIN    PD3 // Pin 5 (QFP32) - External Interrupt INT1 capable
#define SPEED_SENSOR_PIN    PD4 // Pin 6 (QFP32) - Pin Change Interrupt PCINT20 capable

// Port C Pins
#define BATTERY_SENSE_PIN   PC0 // Pin 23 (QFP32) - ADC Channel 0
#define I2C_SDA_PIN         PC4 // Pin 27 (QFP32) - Hardware TWI SDA
#define I2C_SCL_PIN         PC5 // Pin 28 (QFP32) - Hardware TWI SCL

// Port B Pins (Example for Software UART for BLE)
#define BLE_SW_UART_RX_PIN  PB0 // Pin 14 (QFP32) - Example pin for Software UART RX
#define BLE_SW_UART_TX_PIN  PB1 // Pin 15 (QFP32) - Example pin for Software UART TX

// --- Module Configuration ---

// Battery Monitor (using ADC on BATTERY_SENSE_PIN)
// Assumes a voltage divider: Vin --- R1 --- (ADC_PIN) --- R2 --- GND
#define BATTERY_R1_OHMS     10000.0f // Resistor R1 value in Ohms
#define BATTERY_R2_OHMS     2200.0f  // Resistor R2 value in Ohms
#define BATTERY_ADC_VREF_MV 3300.0f  // ADC reference voltage (from 3.3V regulator) in millivolts
#define BATTERY_ADC_MAX_VALUE 1023   // Max value for 10-bit ADC

// Navigation Logic
#define NAV_UPDATE_INTERVAL_MS 1000 // How often navigation logic recalculates (milliseconds)
#define SPEED_SMOOTHING_FACTOR 0.5f // Factor for exponential moving average of speed

// BLE Communication Protocol
#define BLE_PACKET_START_BYTE   0xAA
#define BLE_PACKET_END_BYTE     0x55
#define BLE_STATUS_UPDATE_INTERVAL_MS 500 // How often to send status updates (milliseconds)

// Signal Detection
#define SIGNAL_DEBOUNCE_TIME_MS 50 // Debounce time for turn signal inputs (milliseconds)

// --- Feature Flags ---
#define ENABLE_LOGGING          1      // 1 to enable logging, 0 to disable
#define LOG_LEVEL               LOG_LEVEL_INFO // Default log level (DEBUG, INFO, WARN, ERROR)
#define LOG_UART_ID             BLE_UART_ID // Send logs over BLE UART (or GPS_UART_ID for debug)
#define LOG_UART_BAUD           BLE_UART_BAUD

#endif // CONFIG_H
