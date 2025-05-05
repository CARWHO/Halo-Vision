/**
 * @file main.c
 * @brief Main application entry point for the Halo Vision Display Module.
 */

#include <stdint.h>
#include <stdbool.h>
#include <util/delay.h>

// Include HAL headers
#include "hal/gpio.h"
#include "hal/uart.h"
#include "hal/spi.h"
// #include "hal/i2c.h" // Include if I2C is used
// #include "hal/timer.h"

// Include Module headers/drivers
#include "modules/display_driver.h"
#include "modules/ble_rx.h"
#include "modules/battery_status.h" // Assuming header exists
#include "modules/screen_updater.h" // Assuming header exists

// Include Utilities
#include "util/logger.h"
#include "util/ring_buffer.h"
#include "config.h"

// --- Private Function Prototypes ---
static void hardware_init(void);
static void modules_init(void);
static void main_loop(void);
static void process_ble_input(void);
static void update_system_status(void);

// --- Global Variables / State ---
volatile uint32_t system_ticks_ms = 0; // Assume updated by a timer ISR

/**
 * @brief Main function - entry point.
 */
int main(void) {
    hardware_init();
    modules_init();

    log_info("-----------------------------------");
    log_info("Halo Vision Display Module Starting");
    log_info("-----------------------------------");

    main_loop();

    return 0; // Should not be reached
}

/**
 * @brief Initializes core hardware peripherals.
 */
static void hardware_init(void) {
    // GPIOs for LCD control (CS, DC, RST) are initialized within display_init() -> hal_gpio_init()
    // GPIO for Battery Charger Status is initialized within battery_status_init() -> hal_gpio_init()

    // Initialize UART for BLE communication
    // Note: UART init is handled by logger_init() as it uses the same UART.
    // If separate UARTs were used, init BLE UART here:
    // hal_uart_init(BLE_UART_ID, BLE_UART_BAUD, 8, 1, 0);

    // Initialize SPI for LCD communication
    // Note: SPI init is handled by display_init() -> hal_spi_init()

    // Initialize I2C if needed for external sensors via header
    // hal_i2c_init(EXTERNAL_I2C_ID, I2C_CLOCK_SPEED);

    // Initialize Timers for periodic tasks
    // hal_timer_init(...);
    // hal_timer_start(...);

    // Enable global interrupts if needed
    // sei();

    log_debug("Hardware initialization complete.");
}

/**
 * @brief Initializes application modules.
 */
static void modules_init(void) {
    // Logger first
    logger_init();

    // Initialize drivers and modules
    display_init(); // Initializes LCD hardware (SPI, GPIOs)
    ble_rx_init();
    battery_status_init(); // Initializes battery monitoring (GPIO, ADC)
    screen_updater_init(); // Initializes UI state

    log_debug("Application modules initialization complete.");
}

/**
 * @brief Main application loop.
 */
static void main_loop(void) {
    uint32_t last_screen_update_ms = 0;
    uint32_t last_battery_update_ms = 0;

    while (1) {
        // Update system time (simulation)
        // system_ticks_ms++;
        // _delay_ms(1);

        // 1. Process incoming BLE data
        process_ble_input();

        // 2. Update system status (Battery) periodically
        uint32_t now = system_ticks_ms; // Use actual timer value
        if (now - last_battery_update_ms >= 1000) { // Update battery status every second
            update_system_status();
            last_battery_update_ms = now;
        }

        // 3. Update screen content periodically
        if (now - last_screen_update_ms >= SCREEN_UPDATE_INTERVAL_MS) {
            screen_updater_update(); // Checks for data changes and redraws if needed
            last_screen_update_ms = now;
        }

        // 4. Low power mode?
        // sleep_cpu();
    }
}

/**
 * @brief Processes incoming characters from the BLE UART.
 */
static void process_ble_input(void) {
    while (hal_uart_data_available(BLE_UART_ID)) {
        int16_t data = hal_uart_get_char(BLE_UART_ID);
        if (data != -1) {
            ble_rx_process_char((uint8_t)data); // Pass character to BLE receiver module
        }
    }
}

/**
 * @brief Updates status monitoring modules (e.g., battery).
 */
static void update_system_status(void) {
    battery_status_update(); // Read charger status, update level estimate
}
