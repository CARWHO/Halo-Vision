/**
 * @file main.c
 * @brief Main application entry point for the Halo Vision Brain Module.
 * Initializes hardware abstraction layers (HAL), application modules,
 * and runs the main processing loop.
 */

#include <stdint.h>
#include <stdbool.h>
#include <util/delay.h> // For basic delays if needed

// Include HAL headers
#include "hal/gpio.h"
#include "hal/uart.h"
#include "hal/i2c.h"
// #include "hal/timer.h" // Include if a timer HAL is used

// Include Module headers
#include "modules/gps.h"
#include "modules/ble_uart.h"
#include "modules/battery.h"
#include "modules/nav_logic.h"
#include "modules/signal.h"

// Include Utilities
#include "util/logger.h"
#include "util/ring_buffer.h" // May be used internally by HAL/modules
#include "config.h"          // System configuration constants

// --- Private Function Prototypes ---
static void hardware_init(void);
static void modules_init(void);
static void main_loop(void);
static void process_communication(void);
static void process_sensors(void);
static void run_logic_updates(void);

// --- Global Variables / State (Use Sparingly) ---
// Example: System tick or flags for periodic tasks
volatile uint32_t system_ticks_ms = 0; // Assume updated by a timer ISR

/**
 * @brief Main function - entry point of the application.
 */
int main(void) {
    // Initialize hardware peripherals first.
    hardware_init();

    // Initialize application modules.
    modules_init();

    log_info("---------------------------------");
    log_info("Halo Vision Brain Module Starting");
    log_info("---------------------------------");

    // Enter the main processing loop.
    main_loop();

    // This point should ideally never be reached.
    return 0;
}

/**
 * @brief Initializes core hardware peripherals using the HAL.
 */
static void hardware_init(void) {
    // Initialize GPIO pins for signals, sensors, etc.
    // Pins are defined in config.h
    hal_gpio_init(LEFT_SIGNAL_PIN, GPIO_MODE_INPUT_PULLUP);
    hal_gpio_init(RIGHT_SIGNAL_PIN, GPIO_MODE_INPUT_PULLUP);
    hal_gpio_init(SPEED_SENSOR_PIN, GPIO_MODE_INPUT_PULLUP); // Assuming pullup needed
    // Battery sense pin init is handled by battery_monitor_init -> hal_gpio_init

    // Initialize UART interfaces (GPS, BLE/Log)
    // Note: UART init for GPS/BLE is handled within their respective module inits.
    // Logger init handles its own UART.

    // Initialize I2C interface (if used by any sensors directly)
    hal_i2c_init(MAIN_I2C_ID, I2C_CLOCK_SPEED);

    // Initialize Timers (if used for periodic tasks, debouncing, etc.)
    // hal_timer_init(...);
    // hal_timer_start(...);

    // Enable global interrupts if required by HAL components (e.g., UART RX, Timers)
    // sei(); // Needs <avr/interrupt.h>

    log_debug("Hardware initialization complete.");
}

/**
 * @brief Initializes all application-level modules.
 */
static void modules_init(void) {
    // Logger needs to be initialized first if other modules log during init.
    logger_init();

    // Initialize remaining modules
    battery_monitor_init();
    gps_init();
    ble_uart_init();
    signal_detector_init();
    nav_logic_init();

    log_debug("Application modules initialization complete.");
}

/**
 * @brief Main application super-loop.
 * Continuously processes inputs, runs logic, and generates outputs.
 */
static void main_loop(void) {
    uint32_t last_status_update_ms = 0;
    uint32_t last_nav_update_ms = 0;

    while (1) {
        // Update system time (crude simulation if no timer ISR)
        // system_ticks_ms++;
        // _delay_ms(1); // Very basic delay if no timer

        // 1. Process Communication Interfaces (UARTs, I2C)
        process_communication();

        // 2. Process Sensor Inputs (GPIOs, ADC)
        process_sensors();

        // 3. Run Core Logic Updates (Navigation, State Machines)
        // Run periodically based on system time or flags set by ISRs/polling.
        uint32_t now = system_ticks_ms; // Use actual timer value if available
        if (now - last_nav_update_ms >= NAV_UPDATE_INTERVAL_MS) {
             run_logic_updates();
             last_nav_update_ms = now;
        }

        // 4. Send Periodic Status Updates via BLE
        if (now - last_status_update_ms >= BLE_STATUS_UPDATE_INTERVAL_MS) {
            if (ble_uart_is_connected()) {
                 uint16_t batt_mv = battery_monitor_get_voltage_mv();
                 signal_state_t sig_state = signal_detector_get_state();
                 float speed = gps_get_speed_kmh(); // Use latest speed from GPS/NavLogic
                 ble_uart_send_status_update(batt_mv, sig_state, (uint8_t)speed);
            }
             last_status_update_ms = now;
        }

        // 5. Optional: Enter low-power sleep mode if idle.
        // sleep_cpu();
    }
}

/**
 * @brief Handles processing of incoming data from communication interfaces.
 */
static void process_communication(void) {
    // Poll GPS UART for incoming NMEA characters
    while (hal_uart_data_available(GPS_UART_ID)) {
        int16_t data = hal_uart_get_char(GPS_UART_ID);
        if (data != -1) {
            gps_process_char((uint8_t)data);
        }
    }

    // Poll BLE UART for incoming commands or responses
    while (hal_uart_data_available(BLE_UART_ID)) {
         int16_t data = hal_uart_get_char(BLE_UART_ID);
        if (data != -1) {
            ble_uart_process_char((uint8_t)data);
            // Also pass to logger if echoing input is desired
            // log_char((char)data);
        }
    }

    // Handle I2C communication if needed (e.g., polling sensors)
}

/**
 * @brief Handles reading and processing of sensor inputs.
 */
static void process_sensors(void) {
    // Update turn signal detection logic (reads GPIOs, debounces)
    signal_detector_update();

    // Read battery voltage (reads ADC) - reading might happen within get_voltage_mv
    // battery_monitor_get_voltage_mv(); // Called periodically in main loop for status update

    // Read speed sensor (if separate from GPS, e.g., Hall effect)
    // speed_sensor_update(); // Would read GPIO/timer/interrupts
}

/**
 * @brief Executes core application logic updates.
 */
static void run_logic_updates(void) {
    // Update navigation logic based on latest GPS data and sensor inputs
    if (gps_is_data_available()) {
        gps_data_t gps_reading;
        if (gps_get_data(&gps_reading)) {
            nav_logic_set_gps_data(&gps_reading);
        } else {
            nav_logic_set_gps_data(NULL); // Indicate invalid fix
        }
    }
    // Pass other relevant sensor data to nav logic if needed
    // nav_logic_set_speed(...); // Speed might be set directly from GPS data
    nav_logic_set_signal_state(signal_detector_get_state());

    // Trigger navigation logic calculation/update
    nav_logic_update(); // This function now also sends the BLE nav update
}
