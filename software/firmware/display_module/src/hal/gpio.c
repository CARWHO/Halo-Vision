/**
 * @file gpio.c
 * @brief GPIO HAL implementation for ATmega328P (Display Module).
 */

#include "hal/gpio.h"
#include "util/logger.h"
#include <stddef.h>

// Internal storage for interrupt callbacks if needed
typedef struct {
    gpio_interrupt_callback_t callback;
    uint8_t pin_number;
} gpio_interrupt_config_t;

#define MAX_GPIO_INTERRUPTS 5 // Example limit
static gpio_interrupt_config_t interrupt_configs[MAX_GPIO_INTERRUPTS];
static uint8_t interrupt_count = 0;

// --- Public API Implementation ---

void hal_gpio_init(uint8_t pin, gpio_mode_t mode) {
    // Configure pin direction and pull-up state based on mode.
    log_debug("GPIO: Init Pin %d, Mode %d", pin, mode);
    // Hardware register manipulation (DDRx, PORTx) happens here.
}

void hal_gpio_write(uint8_t pin, bool state) {
    // Set the output level of the specified pin.
    log_debug("GPIO: Write Pin %d, State %d", pin, state);
    // Hardware register manipulation (PORTx) happens here.
}

bool hal_gpio_read(uint8_t pin) {
    // Read the input level of the specified pin.
    log_debug("GPIO: Read Pin %d", pin);
    // Hardware register manipulation (PINx) happens here.
    // Return dummy value for simulation.
    static bool dummy_read_state = false;
    if (pin == BATT_CHG_STAT_PIN) { // Simulate charging status changing
         dummy_read_state = !dummy_read_state;
         return dummy_read_state;
    }
    return false;
}

void hal_gpio_toggle(uint8_t pin) {
    // Toggle the output level of the specified pin.
    log_debug("GPIO: Toggle Pin %d", pin);
    // Hardware register manipulation (PINx = 1 << bit) happens here.
}

bool hal_gpio_configure_interrupt(uint8_t pin, gpio_interrupt_edge_t edge, gpio_interrupt_callback_t callback) {
    if (interrupt_count >= MAX_GPIO_INTERRUPTS) {
        log_error("GPIO: Max interrupts reached (%d)", MAX_GPIO_INTERRUPTS);
        return false;
    }
    // Configure hardware interrupt registers (EICRA, PCICR, etc.).
    // Store callback information.
    log_info("GPIO: Cfg Int Pin %d, Edge %d, CB %p", pin, edge, callback);
    interrupt_configs[interrupt_count].callback = callback;
    interrupt_configs[interrupt_count].pin_number = pin;
    interrupt_count++;
    return true; // Assume success
}

void hal_gpio_enable_interrupt(uint8_t pin) {
    // Enable the specific interrupt mask bit (EIMSK, PCMSKx).
    log_debug("GPIO: Enable Int Pin %d", pin);
}

void hal_gpio_disable_interrupt(uint8_t pin) {
    // Disable the specific interrupt mask bit.
    log_debug("GPIO: Disable Int Pin %d", pin);
}

// ISR definitions would go here, calling the stored callbacks.
// e.g., ISR(INT0_vect) { ... call callback for INT0 pin ... }
// e.g., ISR(PCINT0_vect) { ... check which pin changed, call callback ... }
