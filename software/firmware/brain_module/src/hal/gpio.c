/**
 * @file gpio.c
 * @brief GPIO Hardware Abstraction Layer implementation for ATmega328P.
 * Provides functions to control GPIO pins, including configuration,
 * read/write operations, and interrupt handling.
 */

#include "hal/gpio.h"
#include "util/logger.h" // For logging status/errors
#include <stddef.h>      // For NULL

// --- Internal Data Structures ---

// Structure to hold callback information for each interrupt pin
typedef struct {
    gpio_interrupt_callback_t callback;
    uint8_t pin_number; // Store the pin number for the callback context
} gpio_interrupt_config_t;

// Allocate storage for callbacks (adjust size based on available interrupt pins)
// ATmega328P has INT0, INT1, and 24 PCINT pins (grouped into 3 banks)
#define MAX_GPIO_INTERRUPTS (2 + 24)
static gpio_interrupt_config_t interrupt_configs[MAX_GPIO_INTERRUPTS];
static uint8_t interrupt_count = 0; // Track configured interrupts

// Helper function to find interrupt config by pin
static gpio_interrupt_config_t* find_interrupt_config(uint8_t pin) {
    for (uint8_t i = 0; i < interrupt_count; ++i) {
        if (interrupt_configs[i].pin_number == pin) {
            return &interrupt_configs[i];
        }
    }
    return NULL;
}

// --- Public API Implementation ---

void hal_gpio_init(uint8_t pin, gpio_mode_t mode) {
    // Configure the specified pin with the given mode (Input/Output/Pullup/Analog).
    log_debug("GPIO: Init Pin %d, Mode %d", pin, mode);
    // Hardware-specific configuration would happen here.
}

void hal_gpio_write(uint8_t pin, bool state) {
    // Set the output state (High/Low) for a pin configured as output.
    log_debug("GPIO: Write Pin %d, State %d", pin, state);
    // Hardware-specific write operation would happen here.
}

bool hal_gpio_read(uint8_t pin) {
    // Read the current input state of the specified pin.
    log_debug("GPIO: Read Pin %d", pin);
    // Hardware-specific read operation would happen here.
    // Return a dummy value for demonstration.
    static bool dummy_state = false;
    dummy_state = !dummy_state;
    return dummy_state;
}

void hal_gpio_toggle(uint8_t pin) {
    // Toggle the output state of a pin configured as output.
    log_debug("GPIO: Toggle Pin %d", pin);
    // Hardware-specific toggle operation would happen here.
}

bool hal_gpio_configure_interrupt(uint8_t pin, gpio_interrupt_edge_t edge, gpio_interrupt_callback_t callback) {
    if (interrupt_count >= MAX_GPIO_INTERRUPTS) {
        log_error("GPIO: Max interrupts configured (%d)", MAX_GPIO_INTERRUPTS);
        return false;
    }
    // Configure the specified pin to trigger an interrupt on a given edge/condition
    // and associate the provided callback function.
    log_info("GPIO: Cfg Int Pin %d, Edge %d, CB %p", pin, edge, callback);

    // Store callback association for simulation purposes.
    interrupt_configs[interrupt_count].callback = callback;
    interrupt_configs[interrupt_count].pin_number = pin;
    interrupt_count++;
    // Hardware-specific interrupt configuration would happen here.
    return true; // Assume success
}

void hal_gpio_enable_interrupt(uint8_t pin) {
    // Enable the previously configured interrupt for the specified pin.
    log_debug("GPIO: Enable Int Pin %d", pin);
    // Hardware-specific interrupt enable operation would happen here.
}

void hal_gpio_disable_interrupt(uint8_t pin) {
    // Disable the interrupt for the specified pin.
    log_debug("GPIO: Disable Int Pin %d", pin);
    // Hardware-specific interrupt disable operation would happen here.
}

// Interrupt Service Routines would normally be defined here, calling the
// associated callbacks stored in interrupt_configs.
