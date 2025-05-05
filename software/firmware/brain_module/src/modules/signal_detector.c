/**
 * @file signal_detector.c
 * @brief Module for detecting motorcycle turn signal activation.
 * Reads GPIO pins connected to turn signal indicators and applies debouncing.
 */

#include "modules/signal.h" // Use the module header file name
#include "hal/gpio.h"
#include "util/logger.h"
// #include "hal/timer.h" // Assuming a timer HAL for periodic checks

// --- Internal State ---
static signal_state_t current_signal_state = SIGNAL_STATE_OFF;

// Debouncing state for left signal
static bool left_signal_raw_state = false; // Assumes active low signals (pull-up enabled)
static bool left_signal_debounced_state = false;
static uint32_t left_signal_last_change_time_ms = 0; // Requires a system timer/tick

// Debouncing state for right signal
static bool right_signal_raw_state = false;
static bool right_signal_debounced_state = false;
static uint32_t right_signal_last_change_time_ms = 0;

// Function to get current time (replace with actual timer HAL call)
static uint32_t get_current_time_ms(void) {
    // Placeholder implementation - requires a real timer source.
    static uint32_t time_ms = 0;
    time_ms += 10; // Simulate time passing with each call
    return time_ms;
}
// --- End Assumed Timer HAL ---


// --- Internal Helper Functions ---

// Applies debouncing logic to a single signal input pin.
static bool debounce_signal(uint8_t pin, bool *raw_state, bool *debounced_state, uint32_t *last_change_time) {
    bool current_pin_state = hal_gpio_read(pin);
    uint32_t now = get_current_time_ms(); // Use the timer function
    bool state_changed = false;

    if (current_pin_state != *raw_state) {
        // State has changed, update raw state and reset debounce timer.
        *raw_state = current_pin_state;
        *last_change_time = now;
        log_debug("Signal Pin %d: Raw state changed to %d", pin, *raw_state);
    } else {
        // State is stable, check if debounce time has elapsed.
        if ((now - *last_change_time >= SIGNAL_DEBOUNCE_TIME_MS) && (*debounced_state != *raw_state)) {
            // Debounce time elapsed and state differs, update debounced state.
            *debounced_state = *raw_state;
            state_changed = true;
            log_info("Signal Pin %d: Debounced state changed to %d", pin, *debounced_state);
        }
    }
    return state_changed; // Return true if the debounced state changed
}

// --- Public API Implementation ---

void signal_detector_init(void) {
    log_info("Signal Detector: Initializing...");

    // Initialize GPIO pins for signal inputs with pull-ups (assuming active low signals)
    hal_gpio_init(LEFT_SIGNAL_PIN, GPIO_MODE_INPUT_PULLUP);
    hal_gpio_init(RIGHT_SIGNAL_PIN, GPIO_MODE_INPUT_PULLUP);

    // Initialize debouncing states based on initial pin readings
    left_signal_raw_state = hal_gpio_read(LEFT_SIGNAL_PIN);
    left_signal_debounced_state = left_signal_raw_state;
    right_signal_raw_state = hal_gpio_read(RIGHT_SIGNAL_PIN);
    right_signal_debounced_state = right_signal_raw_state;

    uint32_t now = get_current_time_ms(); // Use the timer function
    left_signal_last_change_time_ms = now;
    right_signal_last_change_time_ms = now;

    current_signal_state = SIGNAL_STATE_OFF; // Start with signals off

    log_info("Signal Detector: Initialized. Left=%d, Right=%d (Raw)", left_signal_raw_state, right_signal_raw_state);
}

void signal_detector_update(void) {
    // This function should be called periodically (e.g., every 10-20ms)
    // to update the debouncing logic.

    bool left_changed = debounce_signal(LEFT_SIGNAL_PIN,
                                        &left_signal_raw_state,
                                        &left_signal_debounced_state,
                                        &left_signal_last_change_time_ms);

    bool right_changed = debounce_signal(RIGHT_SIGNAL_PIN,
                                         &right_signal_raw_state,
                                         &right_signal_debounced_state,
                                         &right_signal_last_change_time_ms);

    // Determine the final signal state based on the debounced inputs.
    // Assumes the hardware uses active-low signals (signal ON = pin LOW).
    bool left_on = !left_signal_debounced_state;  // Signal is ON if debounced state is LOW (false)
    bool right_on = !right_signal_debounced_state; // Signal is ON if debounced state is LOW (false)

    signal_state_t new_state = SIGNAL_STATE_OFF;
    if (left_on && right_on) {
        new_state = SIGNAL_STATE_HAZARD; // Both signals active -> Hazard lights
    } else if (left_on) {
        new_state = SIGNAL_STATE_LEFT;
    } else if (right_on) {
        new_state = SIGNAL_STATE_RIGHT;
    }

    if (new_state != current_signal_state) {
        log_info("Signal Detector: State changed from %d to %d", current_signal_state, new_state);
        current_signal_state = new_state;
        // Optionally notify other modules immediately on change
        // nav_logic_set_signal_state(current_signal_state);
    }
}

signal_state_t signal_detector_get_state(void) {
    return current_signal_state;
}
