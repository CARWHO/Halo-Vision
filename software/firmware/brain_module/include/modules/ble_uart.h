#ifndef MODULES_BLE_UART_H
#define MODULES_BLE_UART_H

/**
 * @file ble_uart.h
 * @brief Interface for BLE UART communication module.
 * Provides functions to send formatted data packets over a UART link
 * intended for a Bluetooth Low Energy module (like HC-05/06).
 */

#include <stdint.h>
#include <stddef.h> // For size_t
#include <stdbool.h>

/**
 * @brief Initializes the BLE UART module.
 * Configures the underlying UART peripheral and prepares the BLE module
 * for communication (if necessary).
 */
void ble_uart_init(void);

/**
 * @brief Sends raw data over the BLE UART link.
 * @param data Pointer to the data buffer to send.
 * @param length Number of bytes to send from the buffer.
 * @return true if the data was successfully queued or sent, false otherwise.
 */
bool ble_uart_send_data(const uint8_t *data, size_t length);

/**
 * @brief Sends a formatted navigation update message over BLE UART.
 * Encapsulates navigation instructions and distance into a defined protocol format.
 * @param instruction A string describing the navigation maneuver (e.g., "Turn left").
 * @param distance Distance to the maneuver (e.g., in meters).
 * @return true if the message was successfully formatted and sent/queued, false otherwise.
 */
bool ble_uart_send_nav_update(const char* instruction, uint16_t distance);

/**
 * @brief Sends a formatted status update message over BLE UART.
 * Encapsulates system status information (battery, signals, speed) into a defined protocol format.
 * @param battery_voltage_mv Current battery voltage in millivolts.
 * @param signal_status Current status of turn signals (e.g., using signal_state_t enum).
 * @param speed_kmh Current speed in kilometers per hour.
 * @return true if the message was successfully formatted and sent/queued, false otherwise.
 */
bool ble_uart_send_status_update(uint16_t battery_voltage_mv, uint8_t signal_status, uint8_t speed_kmh);

/**
 * @brief Processes a single character received from the BLE UART.
 * This function should be called for each byte received from the BLE module's UART.
 * It handles buffering, message framing, and parsing of incoming data or responses.
 * @param received_char The character received from the BLE UART.
 */
void ble_uart_process_char(uint8_t received_char);

/**
 * @brief Checks the current connection status of the BLE module.
 * @return true if the module is believed to be connected to a peer device, false otherwise.
 *         Note: Status might be based on parsed messages or status pins.
 */
bool ble_uart_is_connected(void);

#endif // MODULES_BLE_UART_H
