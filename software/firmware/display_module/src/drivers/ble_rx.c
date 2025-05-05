/**
 * @file ble_rx.c
 * @brief BLE Receiver module implementation.
 * Parses incoming UART data from the Brain Module.
 */

#include "modules/ble_rx.h" // Use the module header file name
#include "hal/uart.h"
#include "util/logger.h"
#include <string.h> // For strncmp, sscanf, strcpy
#include <stdlib.h> // For atoi

// --- Internal State ---
static display_status_data_t current_status_data;
static display_nav_data_t current_nav_data;
static bool is_connected = false; // Simple connection status flag

// Receive buffer state
#define BLE_RX_BUFFER_LEN 128
static char rx_buffer[BLE_RX_BUFFER_LEN];
static uint8_t rx_buffer_idx = 0;

// --- Internal Helper Functions ---

// Parses a complete message received in rx_buffer
static void parse_ble_message(const char *message) {
    log_debug("BLE RX: Parsing: %s", message);

    // Example parsing based on Brain Module formats:
    // "NAV:Instruction,Distance\n"
    // "STATUS:Batt_mV,Signals,Speed_kmh\n"

    if (strncmp(message, "NAV:", 4) == 0) {
        // Parse navigation update
        char instruction_buf[sizeof(current_nav_data.instruction)] = {0};
        unsigned int distance = 0;
        // Use sscanf carefully, check return value
        int parsed_count = sscanf(message + 4, "%63[^,],%u", instruction_buf, &distance);
        if (parsed_count == 2) {
            strncpy(current_nav_data.instruction, instruction_buf, sizeof(current_nav_data.instruction) - 1);
            current_nav_data.distance_m = (uint16_t)distance;
            current_nav_data.updated = true;
            is_connected = true; // Assume connected if data arrives
            log_info("BLE RX: Parsed Nav - Instr='%s', Dist=%u", current_nav_data.instruction, current_nav_data.distance_m);
        } else {
            log_warn("BLE RX: Failed to parse NAV message format");
        }
    } else if (strncmp(message, "STATUS:", 7) == 0) {
        // Parse status update
        unsigned int batt = 0, signals = 0, speed = 0;
        int parsed_count = sscanf(message + 7, "%u,%u,%u", &batt, &signals, &speed);
        if (parsed_count == 3) {
            current_status_data.battery_mv = (uint16_t)batt;
            current_status_data.signal_status = (uint8_t)signals;
            current_status_data.speed_kmh = (uint8_t)speed;
            current_status_data.updated = true;
            is_connected = true; // Assume connected if data arrives
            log_info("BLE RX: Parsed Status - Batt=%u, Sig=%u, Spd=%u", current_status_data.battery_mv, current_status_data.signal_status, current_status_data.speed_kmh);
        } else {
             log_warn("BLE RX: Failed to parse STATUS message format");
        }
    } else {
        log_warn("BLE RX: Unknown message format: %s", message);
    }
}

// --- Public API Implementation ---

void ble_rx_init(void) {
    log_info("BLE Receiver: Initializing...");
    memset(&current_status_data, 0, sizeof(current_status_data));
    memset(&current_nav_data, 0, sizeof(current_nav_data));
    strcpy(current_nav_data.instruction, "Connecting..."); // Default initial message
    current_status_data.updated = false;
    current_nav_data.updated = false;
    rx_buffer_idx = 0;
    is_connected = false;
    // UART for BLE is initialized in main/hardware_init
    log_info("BLE Receiver: Initialized.");
}

void ble_rx_process_char(uint8_t received_char) {
    // Buffer incoming characters until a terminator is found.
    if (received_char == BLE_MSG_TERMINATOR) {
        // End of message detected
        if (rx_buffer_idx > 0) { // Ignore empty messages
            rx_buffer[rx_buffer_idx] = '\0'; // Null-terminate the string
            parse_ble_message(rx_buffer);
        }
        rx_buffer_idx = 0; // Reset buffer index for next message
    } else if (rx_buffer_idx < (BLE_RX_BUFFER_LEN - 1)) {
        // Add character to buffer if space available
        // Ignore non-printable characters? (Optional)
        if (received_char >= ' ' && received_char <= '~') {
             rx_buffer[rx_buffer_idx++] = (char)received_char;
        }
    } else {
        // Buffer overflow, discard current message and reset
        log_warn("BLE RX: Buffer overflow, discarding message.");
        rx_buffer_idx = 0;
    }
}

bool ble_rx_is_status_available(void) {
    return current_status_data.updated;
}

bool ble_rx_get_status_data(display_status_data_t *data) {
    if (!data || !current_status_data.updated) {
        return false;
    }
    memcpy(data, &current_status_data, sizeof(display_status_data_t));
    current_status_data.updated = false; // Clear flag after reading
    return true;
}

bool ble_rx_is_nav_available(void) {
    return current_nav_data.updated;
}

bool ble_rx_get_nav_data(display_nav_data_t *data) {
     if (!data || !current_nav_data.updated) {
        return false;
    }
    memcpy(data, &current_nav_data, sizeof(display_nav_data_t));
    current_nav_data.updated = false; // Clear flag after reading
    return true;
}

bool ble_rx_is_connected(void) {
    // Simple connection check: assume disconnected if no data received for a while?
    // Or rely solely on the flag set when messages are parsed.
    // Add timeout logic here if needed.
    return is_connected;
}
