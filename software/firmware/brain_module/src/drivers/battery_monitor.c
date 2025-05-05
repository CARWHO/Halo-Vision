/**
 * @file battery_monitor.c
 * @brief Driver for monitoring the motorcycle battery voltage.
 * Uses ADC to read voltage via a voltage divider.
 */

#include "modules/battery.h" // Use the module header file name
#include "hal/gpio.h"
#include "util/logger.h"
#include <math.h> // For floating point math if needed
#include <util/delay.h> // For dummy delays

// --- Internal State ---
static uint16_t last_raw_adc_value = 0;
static uint16_t last_voltage_mv = 0;
typedef uint8_t adc_channel_t; // Define type locally if no HAL header

// --- Public API Implementation ---

void battery_monitor_init(void) {
    log_info("Battery Monitor: Initializing...");
    // Setup necessary hardware resources for battery monitoring.
    hal_gpio_init(BATTERY_SENSE_PIN, GPIO_MODE_ANALOG);
    log_info("Battery Monitor: Initialized on Pin %d", BATTERY_SENSE_PIN);
}

uint16_t battery_monitor_read_raw(void) {
    // Read the raw sensor value associated with the battery voltage.
    adc_channel_t adc_channel = 0; // Determine channel based on BATTERY_SENSE_PIN
    // Simulate reading
    last_raw_adc_value = (last_raw_adc_value + 50) % 1024; // Simulate changing value
    log_debug("Battery Monitor: Read Raw Channel %d -> %u", adc_channel, last_raw_adc_value);
    _delay_us(100); // Simulate conversion time
    return last_raw_adc_value;
}

uint16_t battery_monitor_get_voltage_mv(void) {
    // Retrieve the raw ADC value representing the battery level.
    uint16_t raw_adc = battery_monitor_read_raw();

    // Convert the raw ADC reading into millivolts using calibration constants.
    float adc_voltage_mv = ((float)raw_adc / BATTERY_ADC_MAX_VALUE) * BATTERY_ADC_VREF_MV;
    float input_voltage_mv = adc_voltage_mv * (BATTERY_R1_OHMS + BATTERY_R2_OHMS) / BATTERY_R2_OHMS;

    // Handle potential floating point issues (optional)
    // if (isnan(input_voltage_mv) || isinf(input_voltage_mv)) {
    //     log_error("Battery Monitor: Invalid voltage calculation");
    last_voltage_mv = (uint16_t)input_voltage_mv;
    log_debug("Battery Monitor: Raw=%u -> Voltage=%u mV", raw_adc, last_voltage_mv);

    return last_voltage_mv;
}

uint8_t battery_monitor_get_level_percent(void) {
    // Estimate battery percentage based on the current voltage reading.
    const uint16_t VOLTAGE_MAX_MV = 14400; // Voltage considered 100%
    const uint16_t VOLTAGE_MIN_MV = 11800; // Voltage considered 0%

    uint16_t current_voltage_mv = battery_monitor_get_voltage_mv();

    if (current_voltage_mv >= VOLTAGE_MAX_MV) {
        return 100;
    } else if (current_voltage_mv <= VOLTAGE_MIN_MV) {
        return 0;
    } else {
        // Linear interpolation
        uint32_t range = VOLTAGE_MAX_MV - VOLTAGE_MIN_MV;
        uint32_t level = ((uint32_t)current_voltage_mv - VOLTAGE_MIN_MV) * 100 / range;
        return (uint8_t)level;
    }
}
