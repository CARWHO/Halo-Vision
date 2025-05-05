/**
 * @file battery_status.c
 * @brief Module for monitoring battery and charging status on the Display Module.
 * Reads charger status pin and potentially battery voltage via ADC.
 */

#include "modules/battery_status.h" // Use the module header file name (assuming it exists)
#include "hal/gpio.h"
// #include "hal/adc.h" // Include if using ADC for direct voltage reading
#include "util/logger.h"
#include "config.h"

// --- Internal State ---
static battery_charge_state_t current_charge_state = BATTERY_STATE_UNKNOWN;
static uint8_t current_level_percent = 0; // Estimated battery percentage

// --- Assumed ADC HAL functions (if reading voltage directly) ---
// void hal_adc_init(void);
// void hal_adc_pin_init(uint8_t pin);
// uint16_t hal_adc_read_blocking(uint8_t channel);
// ---

// --- Public API Implementation ---

void battery_status_init(void) {
    log_info("Battery Status: Initializing...");
    // Configure the charger status pin as input (likely with pull-up if open-drain)
    hal_gpio_init(BATT_CHG_STAT_PIN, GPIO_MODE_INPUT_PULLUP);

    // Initialize ADC if reading voltage directly
    // hal_adc_init();
    // hal_adc_pin_init(BATT_SENSE_PIN);

    // Perform initial read
    battery_status_update();
    log_info("Battery Status: Initialized. Initial State: %d, Level: %d%%", current_charge_state, current_level_percent);
}

void battery_status_update(void) {
    // Read the charger status pin (MCP73831 STAT pin)
    // STAT is typically Open-Drain: Low = Charging, High-Z = Charge Complete/No Input
    // With pull-up: Low = Charging, High = Charge Complete/No Input
    bool charge_stat_pin_high = hal_gpio_read(BATT_CHG_STAT_PIN);

    // Determine charging state based on pin level
    if (charge_stat_pin_high) {
        // Could be Charge Complete or No Input Power - need voltage to differentiate?
        // Simple approach: Assume not charging if pin is high.
        current_charge_state = BATTERY_STATE_NOT_CHARGING; // Or BATTERY_STATE_CHARGED
    } else {
        current_charge_state = BATTERY_STATE_CHARGING;
    }
    log_debug("Battery Status: CHG_STAT Pin = %d -> State = %d", charge_stat_pin_high, current_charge_state);

    // Read battery voltage via ADC (if implemented)
    // uint16_t raw_adc = hal_adc_read_blocking(ADC_CHANNEL_FOR_BATT_SENSE);
    // Convert raw ADC to millivolts using divider ratio and Vref
    // float voltage_mv = ((float)raw_adc / BATTERY_ADC_MAX_VALUE) * BATTERY_ADC_VREF_MV * DIVIDER_RATIO;
    // Estimate percentage based on voltage (using Li-Po curve)
    // current_level_percent = voltage_to_lipo_percent(voltage_mv);

    // Placeholder percentage update (simulate discharge/charge)
    if (current_charge_state == BATTERY_STATE_CHARGING) {
        if (current_level_percent < 100) current_level_percent += 1;
    } else {
        if (current_level_percent > 0) current_level_percent -= 1;
    }
    if (current_level_percent > 100) current_level_percent = 100; // Clamp
    log_debug("Battery Status: Estimated Level = %d%%", current_level_percent);

}

battery_charge_state_t battery_status_get_charge_state(void) {
    return current_charge_state;
}

uint8_t battery_status_get_level_percent(void) {
    // Return the latest estimated battery percentage.
    return current_level_percent;
}

// Placeholder header file content (modules/battery_status.h) would look like:
/*
#ifndef MODULES_BATTERY_STATUS_H
#define MODULES_BATTERY_STATUS_H

#include <stdint.h>

// Battery charging states
typedef enum {
    BATTERY_STATE_UNKNOWN,
    BATTERY_STATE_CHARGING,
    BATTERY_STATE_NOT_CHARGING, // Discharging or idle
    BATTERY_STATE_CHARGED,      // Charge complete
    BATTERY_STATE_FAULT
} battery_charge_state_t;

void battery_status_init(void);
void battery_status_update(void); // Call periodically
battery_charge_state_t battery_status_get_charge_state(void);
uint8_t battery_status_get_level_percent(void);

#endif // MODULES_BATTERY_STATUS_H
*/
