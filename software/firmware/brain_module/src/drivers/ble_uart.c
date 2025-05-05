/**
 * @file ble_uart.c
 * @brief Driver for communicating with the BLE module (e.g., HC-05/06) via UART.
 * Handles sending formatted commands/data and potentially receiving responses.
 */

#include "modules/ble_uart.h" // Use the module header file name
#include "hal/uart.h"
#include "util/logger.h"
#include <stdio.h>  // For snprintf
#include <string.h> // For strlen, memcpy

// --- Defines ---
#define BLE_CMD_BUFFER_SIZE 128 // Max size for formatted commands/data packets

// --- Internal State ---
static bool ble_connected = false; // Placeholder connection status

// --- Helper Functions ---

// Send a data packet over the BLE UART interface.
static bool send_packet(const uint8_t *packet, size_t length) {
    if (length == 0) return false;

    // Add framing or encoding if required by the communication protocol.
    // For this example, send raw data.
    hal_uart_write(BLE_UART_ID, packet, length);
    log_debug("BLE: Sent %d bytes", length);
    return true; // Indicate success
}

// --- Public API Implementation ---

void ble_uart_init(void) {
    log_info("BLE UART: Initializing...");
    // Initialize the UART peripheral used for BLE communication
    hal_uart_init(BLE_UART_ID, BLE_UART_BAUD, 8, 1, 0); // Configure UART settings

    // Perform any necessary BLE module setup commands here.
    // This might involve sending specific AT commands to configure the module.

    ble_connected = false; // Assume not connected initially
    log_info("BLE UART: Initialized on UART %d.", BLE_UART_ID);
}

bool ble_uart_send_data(const uint8_t *data, size_t length) {
    log_debug("BLE UART: Sending raw data (%d bytes)", length);
    return send_packet(data, length);
}

bool ble_uart_send_nav_update(const char* instruction, uint16_t distance) {
    char buffer[BLE_CMD_BUFFER_SIZE];

    // Format the navigation update message (example format: "NAV:Instruction,Distance\n")
    int len = snprintf(buffer, BLE_CMD_BUFFER_SIZE, "NAV:%s,%u\n",
                       instruction ? instruction : "NULL",
                       distance);

    if (len > 0 && len < BLE_CMD_BUFFER_SIZE) {
        log_info("BLE UART: Sending Nav Update: %s", buffer);
        return send_packet((uint8_t*)buffer, len);
    } else {
        log_error("BLE UART: Failed to format nav update (buffer overflow?)");
        return false;
    }
}

bool ble_uart_send_status_update(uint16_t battery_voltage_mv, uint8_t signal_status, uint8_t speed_kmh) {
    char buffer[BLE_CMD_BUFFER_SIZE];

    // Format the status update message (example format: "STATUS:Batt_mV,Signals,Speed_kmh\n")
    int len = snprintf(buffer, BLE_CMD_BUFFER_SIZE, "STATUS:%u,%u,%u\n",
                       battery_voltage_mv,
                       signal_status,
                       speed_kmh);

     if (len > 0 && len < BLE_CMD_BUFFER_SIZE) {
        log_info("BLE UART: Sending Status: %s", buffer);
        return send_packet((uint8_t*)buffer, len);
    } else {
        log_error("BLE UART: Failed to format status update (buffer overflow?)");
        return false;
    }
}

void ble_uart_process_char(uint8_t received_char) {
    // Process incoming characters received from the BLE module via UART.
    // Buffer characters until a complete message/response is received.
    static char ble_rx_buffer[BLE_CMD_BUFFER_SIZE];
    static uint8_t ble_rx_idx = 0;

    if (received_char == '\n') {
        ble_rx_buffer[ble_rx_idx] = '\0'; // Null-terminate
        if (ble_rx_idx > 0 && ble_rx_buffer[ble_rx_idx - 1] == '\r') {
             ble_rx_buffer[ble_rx_idx - 1] = '\0'; // Remove CR if present for cleaner logging
        }
        log_debug("BLE RX: %s", ble_rx_buffer);

        // Parse the received line (ble_rx_buffer) for known responses or data packets.
        // Update internal state (e.g., ble_connected) based on parsed messages.
        if (strstr(ble_rx_buffer, "CONNECT") != NULL) { // Simplified check
            log_info("BLE UART: Connection detected");
            ble_connected = true;
        } else if (strstr(ble_rx_buffer, "DISCONNECT") != NULL) { // Simplified check
            log_info("BLE UART: Disconnection detected");
            ble_connected = false;
        } else {
            // Handle other received data/responses as needed.
        }

        ble_rx_idx = 0; // Reset buffer for next message
    } else if (ble_rx_idx < sizeof(ble_rx_buffer) - 1) {
        ble_rx_buffer[ble_rx_idx++] = received_char;
    } else {
        // Buffer overflow, reset
        ble_rx_idx = 0;
    }
}

bool ble_uart_is_connected(void) {
    // Return the last known connection status.
    return ble_connected;
}
