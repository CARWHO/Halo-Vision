#ifndef HAL_GPIO_H
#define HAL_GPIO_H

/**
 * @file hal_gpio.h
 * @brief Hardware Abstraction Layer for GPIO pins (Display Module).
 */

#include <stdint.h>
#include <stdbool.h>
#include "config.h" // Include pin definitions

// GPIO Pin Modes
typedef enum {
    GPIO_MODE_INPUT,
    GPIO_MODE_INPUT_PULLUP,
    GPIO_MODE_OUTPUT_PP, // Push-Pull Output
    GPIO_MODE_ANALOG
} gpio_mode_t;

// GPIO Interrupt Edge Types
typedef enum {
    GPIO_INT_EDGE_FALLING,
    GPIO_INT_EDGE_RISING,
    GPIO_INT_EDGE_BOTH,
    GPIO_INT_LEVEL_LOW
} gpio_interrupt_edge_t;

// Callback function pointer type for GPIO interrupts
typedef void (*gpio_interrupt_callback_t)(uint8_t pin);

/**
 * @brief Initializes a specific GPIO pin.
 * @param pin The pin identifier (e.g., LCD_CS_PIN).
 * @param mode The desired pin mode (e.g., GPIO_MODE_OUTPUT_PP).
 */
void hal_gpio_init(uint8_t pin, gpio_mode_t mode);

/**
 * @brief Sets the output level of a GPIO pin.
 * @param pin The pin identifier.
 * @param state true for high, false for low.
 */
void hal_gpio_write(uint8_t pin, bool state);

/**
 * @brief Reads the input level of a GPIO pin.
 * @param pin The pin identifier (e.g., BATT_CHG_STAT_PIN).
 * @return true if high, false if low.
 */
bool hal_gpio_read(uint8_t pin);

/**
 * @brief Toggles the output level of a GPIO pin.
 * @param pin The pin identifier.
 */
void hal_gpio_toggle(uint8_t pin);

/**
 * @brief Configures an interrupt for a GPIO pin.
 * @param pin The pin identifier.
 * @param edge The edge/condition to trigger on.
 * @param callback Function to call on interrupt.
 * @return true if successful, false otherwise.
 */
bool hal_gpio_configure_interrupt(uint8_t pin, gpio_interrupt_edge_t edge, gpio_interrupt_callback_t callback);

/**
 * @brief Enables the interrupt for a specific GPIO pin.
 * @param pin The pin identifier.
 */
void hal_gpio_enable_interrupt(uint8_t pin);

/**
 * @brief Disables the interrupt for a specific GPIO pin.
 * @param pin The pin identifier.
 */
void hal_gpio_disable_interrupt(uint8_t pin);

#endif // HAL_GPIO_H
