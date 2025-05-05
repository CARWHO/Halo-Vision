#ifndef MODULES_BLE_RX_H
#define MODULES_BLE_RX_H

/**
 * @file ble_rx.h
 * @brief Interface for the BLE Receiver module.
 * Handles receiving and parsing data packets from the Brain Module via UART.
 */

#include <stdint.h>
#include <stdbool.h>

// Define structure to hold received status data
typedef struct {
    uint16_t battery_mv;
    uint8_t signal_status; // Corresponds to signal_state_t from Brain Module
    uint8_t speed_kmh;
    bool updated; // Flag indicating if new data arrived since last check
} display_status_data_t;

// Define structure to hold received navigation data
typedef struct {
    char instruction[64]; // Buffer for navigation instruction string
    uint16_t distance_m;  // Distance to next maneuver in meters
    bool updated; // Flag indicating if new data arrived since last check
} display_nav_data_t;

/**
 * @brief Initializes the BLE receiver module.
 * Sets up internal state for parsing incoming messages.
 * Note: UART initialization is handled by HAL/main.
 */
void ble_rx_init(void);

/**
 * @brief Processes a single character received from the BLE UART.
 * This function should be called from the UART RX handler or polling loop.
 * It buffers characters, identifies complete messages, and parses them.
 * @param received_char The character received via UART.
 */
void ble_rx_process_char(uint8_t received_char);

/**
 * @brief Checks if new status data has been received and parsed.
 * @return true if new status data is available, false otherwise.
 */
bool ble_rx_is_status_available(void);

/**
 * @brief Retrieves the latest received status data.
 * Copies the internal status data to the provided structure.
 * Resets the 'updated' flag internally.
 * @param data Pointer to a display_status_data_t structure to copy data into.
 * @return true if data was copied (i.e., new data was available), false otherwise.
 */
bool ble_rx_get_status_data(display_status_data_t *data);

/**
 * @brief Checks if new navigation data has been received and parsed.
 * @return true if new navigation data is available, false otherwise.
 */
bool ble_rx_is_nav_available(void);

/**
 * @brief Retrieves the latest received navigation data.
 * Copies the internal navigation data to the provided structure.
 * Resets the 'updated' flag internally.
 * @param data Pointer to a display_nav_data_t structure to copy data into.
 * @return true if data was copied (i.e., new data was available), false otherwise.
 */
bool ble_rx_get_nav_data(display_nav_data_t *data);

/**
 * @brief Gets the last known connection status based on received data.
 * (Simple implementation might just check if any data arrived recently).
 * @return true if considered connected, false otherwise.
 */
bool ble_rx_is_connected(void);


#endif // MODULES_BLE_RX_H
