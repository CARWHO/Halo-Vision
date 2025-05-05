#ifndef HAL_GPIO_H
#define HAL_GPIO_H

/**
 * @file hal_gpio.h
 * @brief Hardware Abstraction Layer for General Purpose Input/Output pins for ATmega328P.
 */

#include <stdint.h>
#include <stdbool.h>

// Define symbolic names for pins, mapping to MCU-specific identifiers if necessary.
// For simplicity here, we use the definitions from config.h directly.
#include "config.h" // Include pin definitions

// GPIO Pin Modes
typedef enum {
    GPIO_MODE_INPUT,          // High-impedance input
    GPIO_MODE_INPUT_PULLUP,   // Input with internal pull-up resistor enabled
    GPIO_MODE_OUTPUT_PP,      // Output Push-Pull (standard output)
    GPIO_MODE_ANALOG          // Analog input (disables digital input buffer)
} gpio_mode_t;

// GPIO Interrupt Edge Types
typedef enum {
    GPIO_INT_EDGE_LOW_LEVEL, // Interrupt on low level (for INT0/INT1)
    GPIO_INT_EDGE_ANY_CHANGE,// Interrupt on any logical change (for INT0/INT1)
    GPIO_INT_EDGE_FALLING,   // Interrupt on falling edge
    GPIO_INT_EDGE_RISING,    // Interrupt on rising edge
    GPIO_INT_PIN_CHANGE      // Interrupt on pin change (for PCINT) - edge determined by reading pin state
} gpio_interrupt_edge_t;

// Callback function pointer type for GPIO interrupts
typedef void (*gpio_interrupt_callback_t)(uint8_t pin); // Pass pin number to callback

/**
 * @brief Initializes a specific GPIO pin.
 * Configures the pin's direction (input/output) and pull-up resistor state.
 * @param pin The pin identifier (e.g., LEFT_SIGNAL_PIN).
 * @param mode The desired pin mode (e.g., GPIO_MODE_INPUT_PULLUP).
 */
void hal_gpio_init(uint8_t pin, gpio_mode_t mode);

/**
 * @brief Sets the output level of a GPIO pin configured as output.
 * @param pin The pin identifier.
 * @param state true to set the pin high, false to set it low.
 */
void hal_gpio_write(uint8_t pin, bool state);

/**
 * @brief Reads the input level of a GPIO pin.
 * @param pin The pin identifier.
 * @return true if the pin level is high, false if low.
 */
bool hal_gpio_read(uint8_t pin);

/**
 * @brief Toggles the output level of a GPIO pin configured as output.
 * @param pin The pin identifier.
 */
void hal_gpio_toggle(uint8_t pin);

/**
 * @brief Configures an external or pin change interrupt for a GPIO pin.
 * Associates a callback function to be executed when the interrupt occurs.
 * Note: Pin must be capable of generating the specified interrupt type (INTx or PCINTx).
 * @param pin The pin identifier (e.g., LEFT_SIGNAL_PIN, SPEED_SENSOR_PIN).
 * @param edge The edge or condition that triggers the interrupt.
 * @param callback The function to call when the interrupt occurs.
 * @return true if configuration is successful, false otherwise (e.g., pin not interrupt capable).
 */
bool hal_gpio_configure_interrupt(uint8_t pin, gpio_interrupt_edge_t edge, gpio_interrupt_callback_t callback);

/**
 * @brief Enables the interrupt for a specific GPIO pin after configuration.
 * @param pin The pin identifier.
 */
void hal_gpio_enable_interrupt(uint8_t pin);

/**
 * @brief Disables the interrupt for a specific GPIO pin.
 * @param pin The pin identifier.
 */
void hal_gpio_disable_interrupt(uint8_t pin);

#endif // HAL_GPIO_H
