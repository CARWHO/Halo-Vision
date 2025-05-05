#ifndef MODULES_BATTERY_H
#define MODULES_BATTERY_H

/**
 * @file battery.h
 * @brief Interface for the Battery Monitoring module.
 * Provides functions to read and interpret the battery voltage level.
 */

#include <stdint.h>

/**
 * @brief Initializes the battery monitoring module.
 * Configures necessary hardware (like ADC channels and GPIO pins)
 * required for reading the battery voltage.
 */
void battery_monitor_init(void);

/**
 * @brief Reads the raw sensor value related to battery voltage.
 * This is typically an ADC reading before conversion and calibration.
 * @return The raw sensor reading (e.g., 10-bit ADC value). Returns specific error value if read fails.
 */
uint16_t battery_monitor_read_raw(void);

/**
 * @brief Gets the current battery voltage, converted and calibrated.
 * @return Battery voltage in millivolts (mV).
 */
uint16_t battery_monitor_get_voltage_mv(void);

/**
 * @brief Estimates the battery charge level as a percentage.
 * This calculation is based on the measured voltage and typically requires
 * a voltage-to-percentage mapping specific to the battery chemistry.
 * @return Battery level percentage (0-100).
 */
uint8_t battery_monitor_get_level_percent(void);


#endif // MODULES_BATTERY_H
