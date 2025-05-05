/**
 * @file nav_logic.c
 * @brief Navigation Logic Module.
 * Processes GPS data and potentially other inputs (speed, signals)
 * to generate navigation instructions or guidance information.
 * (Placeholder implementation)
 */

#include "modules/nav_logic.h" // Use the module header file name
#include "modules/gps.h"
#include "modules/ble_uart.h" // To send updates
#include "util/logger.h"
#include <string.h> // For memcpy
#include <math.h>   // For fabs, etc.

// --- Internal State ---
static gps_data_t current_gps_state;
static bool gps_fix_is_valid = false;
static float current_speed_kmh = 0.0f;
// Add state for route, waypoints, turn instructions etc. if implementing actual navigation
static char current_instruction[64] = "Proceed straight";
static uint16_t distance_to_next_m = 1000;

// --- Internal Helper Functions ---

// Updates the current navigation guidance based on the latest GPS data and route.
static void update_navigation_guidance(void) {
    if (!gps_fix_is_valid) {
        // Handle case where there is no valid GPS signal.
        strcpy(current_instruction, "Waiting for GPS fix...");
        distance_to_next_m = 0;
        log_warn("NavLogic: No valid GPS fix for guidance update.");
        // Send update via BLE to inform display
        ble_uart_send_nav_update(current_instruction, distance_to_next_m);
        return;
    }

    // Perform route matching, calculate distance/bearing to next maneuver,
    // determine turn instructions, etc.
    // This is where the core navigation algorithm resides.

    // Simulate guidance update for demonstration:
    // Simulate distance decreasing based on current speed.
    float speed_mps = current_speed_kmh / 3.6f;
    float time_interval_s = (float)NAV_UPDATE_INTERVAL_MS / 1000.0f;
    float distance_covered_m = speed_mps * time_interval_s;

    if (distance_to_next_m > distance_covered_m) {
        distance_to_next_m -= (uint16_t)distance_covered_m;
    } else {
        distance_to_next_m = 0;
    }

    // Simulate changing instruction when close
    if (distance_to_next_m < 50 && distance_to_next_m > 0) {
         strcpy(current_instruction, "Turn right soon");
    } else if (distance_to_next_m == 0) {
         strcpy(current_instruction, "Arrived (Simulated)");
         // Load next instruction/waypoint here...
         distance_to_next_m = 2000; // Simulate next leg
         strcpy(current_instruction, "Proceed straight (New Leg)");
    }

    log_debug("NavLogic: Update: Instr='%s', Dist=%um", current_instruction, distance_to_next_m);

    // Send update via BLE
    ble_uart_send_nav_update(current_instruction, distance_to_next_m);
}


// --- Public API Implementation ---

void nav_logic_init(void) {
    log_info("Navigation Logic: Initializing...");
    memset(&current_gps_state, 0, sizeof(current_gps_state));
    gps_fix_is_valid = false;
    current_speed_kmh = 0.0f;
    distance_to_next_m = 1000; // Initial dummy distance
    strcpy(current_instruction, "Initializing Navigation...");
    log_info("Navigation Logic: Initialized.");
}

void nav_logic_set_gps_data(const gps_data_t *data) {
    if (data != NULL && data->fix_valid) {
        memcpy(&current_gps_state, data, sizeof(gps_data_t));
        gps_fix_is_valid = true;
        // Use GPS speed directly if available and valid
        if (data->speed_kmh >= 0.0f) { // Check for valid speed indication
             nav_logic_set_speed(data->speed_kmh);
        }
        log_debug("NavLogic: Received valid GPS data.");
    } else {
        gps_fix_is_valid = false;
        // Keep last known speed? Or reset? Resetting might be safer.
        nav_logic_set_speed(0.0f);
        log_warn("NavLogic: Received invalid or NULL GPS data.");
    }
}

void nav_logic_set_speed(float speed_kmh) {
    // Apply smoothing (e.g., exponential moving average)
    current_speed_kmh = (SPEED_SMOOTHING_FACTOR * speed_kmh) + ((1.0f - SPEED_SMOOTHING_FACTOR) * current_speed_kmh);
    if (fabsf(current_speed_kmh) < 0.1f) { // Clamp small values to zero
        current_speed_kmh = 0.0f;
    }
    log_debug("NavLogic: Speed updated to %.1f km/h", current_speed_kmh);
}

void nav_logic_set_signal_state(signal_state_t signals) {
    // Use signal state if needed for navigation adjustments
    // e.g., detect if user signaled before a turn instruction
    log_debug("NavLogic: Signal state received: %d", signals);
    // Placeholder - not used in current logic
}

void nav_logic_update(void) {
    // This function is called periodically (e.g., every NAV_UPDATE_INTERVAL_MS)
    // to recalculate guidance and send updates.
    log_debug("NavLogic: Periodic update triggered.");
    update_navigation_guidance();
}

// --- Functions to get current navigation state (if needed by other modules) ---

const char* nav_logic_get_current_instruction(void) {
    return current_instruction;
}

uint16_t nav_logic_get_distance_to_next(void) {
    return distance_to_next_m;
}
