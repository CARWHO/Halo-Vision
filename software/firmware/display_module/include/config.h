#ifndef DISPLAY_CONFIG_H
#define DISPLAY_CONFIG_H

/**
 * @file config.h
 * @brief Central configuration file for the Halo Vision Display Module firmware.
 */

#include <stdint.h>

// --- System Configuration ---
#define F_CPU 16000000UL // System clock frequency: 16 MHz (from schematic)

// --- Peripheral Configuration ---

// UART for BLE Communication (Hardware USART0)
#define BLE_UART_ID         UART_ID_0 // Maps to HW USART0
#define BLE_UART_BAUD       115200UL  // Baud rate matching Brain Module BLE
#define BLE_UART_RX_BUFFER_SIZE 128   // Buffer for incoming data from Brain Module
#define BLE_UART_TX_BUFFER_SIZE 32    // Buffer for outgoing acknowledgements (if any)

// SPI for LCD Communication (Hardware SPI)
#define LCD_SPI_ID          SPI_ID_0 // Maps to HW SPI
#define LCD_SPI_MODE        0        // SPI Mode (CPOL=0, CPHA=0 typical for LCDs)
#define LCD_SPI_CLOCK_SPEED 4000000UL // SPI Clock Speed (e.g., 4 MHz)

// I2C (Hardware TWI) - For potential external sensors via header
#define EXTERNAL_I2C_ID     I2C_ID_0 // Maps to HW TWI
#define I2C_CLOCK_SPEED     100000UL // 100 kHz standard I2C speed

// --- Pin Definitions (ATmega328P QFP32 package mapping - based on schematic) ---

// Port D Pins
#define UART0_RXD_PIN       PD0 // Pin 2 (QFP32) - BLE UART RX
#define UART0_TXD_PIN       PD1 // Pin 3 (QFP32) - BLE UART TX
// PD2, PD3, PD4 potentially available or used for I2C header?

// Port C Pins
#define BATT_CHG_STAT_PIN   PC0 // Pin 23 (QFP32) - MCP73831 Status Output (Input)
#define BATT_SENSE_PIN      PC1 // Pin 24 (QFP32) - Battery Voltage Sense (ADC1)
// PC4 (SDA), PC5 (SCL) used by I2C Header

// Port B Pins
#define LCD_CS_PIN          PB2 // Pin 16 (QFP32) - SPI Chip Select for LCD (Output, SS)
#define LCD_MOSI_PIN        PB3 // Pin 17 (QFP32) - SPI MOSI (Output)
#define LCD_MISO_PIN        PB4 // Pin 18 (QFP32) - SPI MISO (Input, if needed)
#define LCD_SCK_PIN         PB5 // Pin 19 (QFP32) - SPI Clock (Output)
#define LCD_DC_PIN          PB0 // Pin 14 (QFP32) - LCD Data/Command Select (Output)
#define LCD_RST_PIN         PB1 // Pin 15 (QFP32) - LCD Reset (Output)

// --- Module Configuration ---

// LCD Driver
#define LCD_WIDTH           128 // Example LCD width in pixels
#define LCD_HEIGHT          160 // Example LCD height in pixels
#define LCD_DEFAULT_BG_COLOR 0x0000 // Black background
#define LCD_DEFAULT_FG_COLOR 0xFFFF // White foreground

// Battery Status
#define BATTERY_ADC_VREF_MV 3300.0f  // ADC reference voltage (3.3V regulator)
#define BATTERY_ADC_MAX_VALUE 1023   // 10-bit ADC
// Voltage divider for battery sense (if used directly, schematic implies charger handles it)
// #define BATTERY_SENSE_R1_OHMS 10000.0f
// #define BATTERY_SENSE_R2_OHMS 10000.0f // Example 1:1 divider if Vbat <= Vref

// Screen Updater
#define SCREEN_UPDATE_INTERVAL_MS 100 // How often to refresh screen elements

// BLE Receiver Protocol
#define BLE_MSG_TERMINATOR '\n' // Character indicating end of message from Brain Module

// --- Feature Flags ---
#define ENABLE_LOGGING          1      // 1 to enable logging, 0 to disable
#define LOG_LEVEL               LOG_LEVEL_INFO // Default log level
#define LOG_UART_ID             BLE_UART_ID // Send logs over BLE UART
#define LOG_UART_BAUD           BLE_UART_BAUD

#endif // DISPLAY_CONFIG_H
