/**
 * @file screen_updater.c
 * @brief Module responsible for updating the LCD screen content.
 * Retrieves data from other modules (BLE RX, Battery Status) and uses the
 * display driver to render the UI elements.
 */

#include "modules/screen_updater.h" // Use the module header file name (assuming it exists)
#include "modules/display_driver.h"
#include "modules/ble_rx.h"
#include "modules/battery_status.h" // Assuming header exists
#include "util/logger.h"
#include "config.h"
#include <stdio.h> // For sprintf

// --- Internal State ---
static display_nav_data_t last_nav_data;
static display_status_data_t last_status_data;
static battery_charge_state_t last_charge_state;
static uint8_t last_battery_percent;

// --- UI Drawing Functions ---

// Draws the navigation arrow area
static void draw_navigation_area(void) {
    log_debug("ScreenUpdater: Drawing Nav Area");
    // Clear area
    display_fill_rect(0, 0, LCD_WIDTH, LCD_HEIGHT / 2, COLOR_BLUE); // Example background

    // Draw instruction text
    display_set_foreground_color(COLOR_WHITE);
    display_set_background_color(COLOR_BLUE);
    // display_set_font(&font_large); // Set appropriate font
    display_draw_string(5, 10, last_nav_data.instruction);

    // Draw distance text
    char dist_str[20];
    snprintf(dist_str, sizeof(dist_str), "%u m", last_nav_data.distance_m);
    // display_set_font(&font_medium);
    display_draw_string(5, 30, dist_str);

    // Draw turn arrow icon (placeholder)
    // This would involve drawing lines or a bitmap based on instruction
    display_draw_line(LCD_WIDTH / 2, 50, LCD_WIDTH / 2 + 20, 60, COLOR_YELLOW);
    display_draw_line(LCD_WIDTH / 2, 70, LCD_WIDTH / 2 + 20, 60, COLOR_YELLOW);
}

// Draws the status bar area
static void draw_status_area(void) {
    log_debug("ScreenUpdater: Drawing Status Area");
    // Clear area
    display_fill_rect(0, LCD_HEIGHT - 20, LCD_WIDTH, 20, COLOR_BLACK); // Status bar background

    // Draw Battery Icon/Percentage
    char batt_str[10];
    snprintf(batt_str, sizeof(batt_str), "%d%%", last_battery_percent);
    if (last_charge_state == BATTERY_STATE_CHARGING) {
        strcat(batt_str, "+"); // Add indicator for charging
    }
    display_set_foreground_color(COLOR_GREEN);
    display_set_background_color(COLOR_BLACK);
    // display_set_font(&font_small);
    display_draw_string(5, LCD_HEIGHT - 15, batt_str);
    // Draw battery icon outline/fill based on percentage

    // Draw Signal Indicators
    display_color_t left_color = COLOR_WHITE;
    display_color_t right_color = COLOR_WHITE;
    // Use signal_state_t enum values (assuming they match Brain Module)
    enum { SIG_OFF, SIG_LEFT, SIG_RIGHT, SIG_HAZARD };
    if (last_status_data.signal_status == SIG_LEFT || last_status_data.signal_status == SIG_HAZARD) {
        left_color = COLOR_ORANGE;
    }
    if (last_status_data.signal_status == SIG_RIGHT || last_status_data.signal_status == SIG_HAZARD) {
        right_color = COLOR_ORANGE;
    }
    // Draw left arrow indicator
    display_fill_rect(LCD_WIDTH / 2 - 20, LCD_HEIGHT - 15, 10, 10, left_color);
    // Draw right arrow indicator
    display_fill_rect(LCD_WIDTH / 2 + 10, LCD_HEIGHT - 15, 10, 10, right_color);


    // Draw Speed
    char speed_str[15];
    snprintf(speed_str, sizeof(speed_str), "%u km/h", last_status_data.speed_kmh);
    display_set_foreground_color(COLOR_WHITE);
    display_draw_string(LCD_WIDTH - 50, LCD_HEIGHT - 15, speed_str);

    // Draw BLE connection status icon (placeholder)
    display_fill_rect(LCD_WIDTH - 10, LCD_HEIGHT - 15, 5, 10, ble_rx_is_connected() ? COLOR_BLUE : COLOR_RED);

}

// --- Public API Implementation ---

void screen_updater_init(void) {
    log_info("Screen Updater: Initializing...");
    // Initialize internal state with default values
    ble_rx_get_nav_data(&last_nav_data); // Get initial default nav message
    ble_rx_get_status_data(&last_status_data);
    last_battery_percent = battery_status_get_level_percent();
    last_charge_state = battery_status_get_charge_state();
    // Display driver should be initialized before this
    log_info("Screen Updater: Initialized.");
}

void screen_updater_update(void) {
    // This function is called periodically or when data changes.

    bool screen_needs_update = false;

    // Check for new data from BLE Receiver
    display_nav_data_t new_nav_data;
    if (ble_rx_get_nav_data(&new_nav_data)) {
        // Compare with last data to see if redraw is needed
        if (strcmp(last_nav_data.instruction, new_nav_data.instruction) != 0 ||
            last_nav_data.distance_m != new_nav_data.distance_m) {
            memcpy(&last_nav_data, &new_nav_data, sizeof(last_nav_data));
            screen_needs_update = true; // Redraw navigation area
            log_debug("ScreenUpdater: Nav data changed.");
        }
    }

    display_status_data_t new_status_data;
    if (ble_rx_get_status_data(&new_status_data)) {
         if (memcmp(&last_status_data, &new_status_data, sizeof(last_status_data)) != 0) {
             memcpy(&last_status_data, &new_status_data, sizeof(last_status_data));
             screen_needs_update = true; // Redraw status area
             log_debug("ScreenUpdater: Status data changed.");
         }
    }

    // Check for changes in battery status (polled separately)
    uint8_t new_batt_percent = battery_status_get_level_percent();
    battery_charge_state_t new_charge_state = battery_status_get_charge_state();
    if (new_batt_percent != last_battery_percent || new_charge_state != last_charge_state) {
        last_battery_percent = new_batt_percent;
        last_charge_state = new_charge_state;
        screen_needs_update = true; // Redraw status area
        log_debug("ScreenUpdater: Battery status changed.");
    }

    // If any data changed, redraw relevant parts of the screen
    if (screen_needs_update) {
        log_info("ScreenUpdater: Updating screen...");
        // Redraw specific areas based on which data changed, or redraw all for simplicity
        draw_navigation_area();
        draw_status_area();
        // display_refresh(); // Call if using a framebuffer
    } else {
        log_debug("ScreenUpdater: No changes detected, skipping redraw.");
    }
}


// Placeholder header file content (modules/screen_updater.h) would look like:
/*
#ifndef MODULES_SCREEN_UPDATER_H
#define MODULES_SCREEN_UPDATER_H

void screen_updater_init(void);
void screen_updater_update(void); // Call periodically or on data change

#endif // MODULES_SCREEN_UPDATER_H
*/
