/**
 * @file lcd_driver.c
 * @brief Driver for controlling the SPI-based LCD display.
 * Implements the display_driver.h interface using HAL functions.
 */

#include "modules/display_driver.h" // Use the module header file name
#include "hal/spi.h"
#include "hal/gpio.h"
#include "util/logger.h"
#include <util/delay.h> // For delays during initialization/commands
#include <string.h>     // For strlen in draw_string

// --- Internal Defines & State ---
static display_color_t current_fg_color = COLOR_WHITE;
static display_color_t current_bg_color = COLOR_BLACK;
// static const display_font_t *current_font = NULL; // Add if implementing fonts

// --- Low-Level LCD Communication ---

// Select the LCD chip
static inline void lcd_select(void) {
    hal_gpio_write(LCD_CS_PIN, false); // Active low chip select
}

// Deselect the LCD chip
static inline void lcd_deselect(void) {
    hal_gpio_write(LCD_CS_PIN, true);
}

// Send a command byte to the LCD
static void lcd_write_command(uint8_t cmd) {
    hal_gpio_write(LCD_DC_PIN, false); // D/C pin low for command
    lcd_select();
    hal_spi_transfer_byte(LCD_SPI_ID, cmd);
    lcd_deselect();
}

// Send a data byte to the LCD
static void lcd_write_data(uint8_t data) {
    hal_gpio_write(LCD_DC_PIN, true); // D/C pin high for data
    lcd_select();
    hal_spi_transfer_byte(LCD_SPI_ID, data);
    lcd_deselect();
}

// Send multiple data bytes
static void lcd_write_data_multi(const uint8_t *data, size_t len) {
    hal_gpio_write(LCD_DC_PIN, true); // D/C pin high for data
    lcd_select();
    hal_spi_write_multi(LCD_SPI_ID, data, len);
    lcd_deselect();
}

// Send a 16-bit color value
static void lcd_write_color(display_color_t color) {
    uint8_t color_bytes[2] = {(uint8_t)(color >> 8), (uint8_t)color};
    lcd_write_data_multi(color_bytes, 2);
}

// Set the drawing window (address window)
static void lcd_set_window(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
    // Send Column Address Set command (e.g., 0x2A)
    lcd_write_command(0x2A); // Example command
    uint8_t col_data[4] = {(uint8_t)(x0 >> 8), (uint8_t)x0, (uint8_t)(x1 >> 8), (uint8_t)x1};
    lcd_write_data_multi(col_data, 4);

    // Send Row Address Set command (e.g., 0x2B)
    lcd_write_command(0x2B); // Example command
    uint8_t row_data[4] = {(uint8_t)(y0 >> 8), (uint8_t)y0, (uint8_t)(y1 >> 8), (uint8_t)y1};
    lcd_write_data_multi(row_data, 4);

    // Send Memory Write command (e.g., 0x2C) to prepare for pixel data
    lcd_write_command(0x2C); // Example command
}

// --- Public API Implementation ---

void display_init(void) {
    log_info("LCD Driver: Initializing...");

    // Initialize GPIO pins for LCD control (CS, DC, RST)
    hal_gpio_init(LCD_CS_PIN, GPIO_MODE_OUTPUT_PP);
    hal_gpio_init(LCD_DC_PIN, GPIO_MODE_OUTPUT_PP);
    hal_gpio_init(LCD_RST_PIN, GPIO_MODE_OUTPUT_PP);
    hal_gpio_write(LCD_CS_PIN, true); // Deselect initially

    // Initialize SPI peripheral
    hal_spi_init(LCD_SPI_ID, LCD_SPI_CLOCK_SPEED, LCD_SPI_MODE, 0); // MSB first

    // Perform hardware reset sequence
    hal_gpio_write(LCD_RST_PIN, false);
    _delay_ms(10);
    hal_gpio_write(LCD_RST_PIN, true);
    _delay_ms(120); // Wait for display reset

    // Send LCD initialization commands (specific to the LCD controller chip)
    log_info("LCD Driver: Sending initialization sequence...");
    lcd_write_command(0x01); // Example: Software Reset
    _delay_ms(150);
    lcd_write_command(0x11); // Example: Sleep Out
    _delay_ms(255);
    lcd_write_command(0x3A); // Example: Pixel Format Set
    lcd_write_data(0x55);    // 16-bit/pixel (RGB565)
    lcd_write_command(0x29); // Example: Display ON
    _delay_ms(100);

    // Set default colors and clear screen
    display_set_foreground_color(LCD_DEFAULT_FG_COLOR);
    display_set_background_color(LCD_DEFAULT_BG_COLOR);
    display_clear(LCD_DEFAULT_BG_COLOR);

    log_info("LCD Driver: Initialization complete.");
}

void display_clear(display_color_t color) {
    log_debug("LCD: Clearing screen to 0x%04X", color);
    lcd_set_window(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);

    // Prepare color bytes (assuming RGB565)
    uint8_t hi = color >> 8;
    uint8_t lo = color & 0xFF;

    hal_gpio_write(LCD_DC_PIN, true); // Data mode
    lcd_select();
    // Write color data repeatedly for all pixels
    for (uint32_t i = 0; i < (uint32_t)LCD_WIDTH * LCD_HEIGHT; ++i) {
        hal_spi_transfer_byte(LCD_SPI_ID, hi);
        hal_spi_transfer_byte(LCD_SPI_ID, lo);
    }
    lcd_deselect();
}

void display_set_foreground_color(display_color_t color) {
    current_fg_color = color;
}

void display_set_background_color(display_color_t color) {
    current_bg_color = color;
}

void display_draw_pixel(int16_t x, int16_t y, display_color_t color) {
    if (x < 0 || x >= LCD_WIDTH || y < 0 || y >= LCD_HEIGHT) return; // Bounds check
    log_debug("LCD: Draw Pixel (%d, %d) Color=0x%04X", x, y, color);
    lcd_set_window(x, y, x, y);
    lcd_write_color(color);
}

void display_draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, display_color_t color) {
    // Basic line drawing algorithm (e.g., Bresenham's) would be implemented here.
    // For placeholder, just draw start/end pixels.
    log_debug("LCD: Draw Line (%d,%d) to (%d,%d)", x0, y0, x1, y1);
    display_draw_pixel(x0, y0, color);
    display_draw_pixel(x1, y1, color);
}

void display_draw_rect(int16_t x, int16_t y, int16_t w, int16_t h, display_color_t color) {
    // Draw the four lines of the rectangle.
    log_debug("LCD: Draw Rect (%d,%d) W=%d H=%d", x, y, w, h);
    // display_draw_line(x, y, x + w - 1, y, color);
    // display_draw_line(x, y + h - 1, x + w - 1, y + h - 1, color);
    // display_draw_line(x, y, x, y + h - 1, color);
    // display_draw_line(x + w - 1, y, x + w - 1, y + h - 1, color);
}

void display_fill_rect(int16_t x, int16_t y, int16_t w, int16_t h, display_color_t color) {
    if (x >= LCD_WIDTH || y >= LCD_HEIGHT || w <= 0 || h <= 0) return;
    // Clip coordinates
    if (x < 0) { w += x; x = 0; }
    if (y < 0) { h += y; y = 0; }
    if (x + w > LCD_WIDTH) w = LCD_WIDTH - x;
    if (y + h > LCD_HEIGHT) h = LCD_HEIGHT - y;
    if (w <= 0 || h <= 0) return;

    log_debug("LCD: Fill Rect (%d,%d) W=%d H=%d Color=0x%04X", x, y, w, h, color);
    lcd_set_window(x, y, x + w - 1, y + h - 1);

    uint8_t hi = color >> 8;
    uint8_t lo = color & 0xFF;
    uint32_t num_pixels = (uint32_t)w * h;

    hal_gpio_write(LCD_DC_PIN, true); // Data mode
    lcd_select();
    for (uint32_t i = 0; i < num_pixels; ++i) {
        hal_spi_transfer_byte(LCD_SPI_ID, hi);
        hal_spi_transfer_byte(LCD_SPI_ID, lo);
    }
    lcd_deselect();
}

void display_draw_circle(int16_t x0, int16_t y0, int16_t r, display_color_t color) {
    // Circle drawing algorithm (e.g., Midpoint circle algorithm) implementation.
    log_debug("LCD: Draw Circle (%d,%d) R=%d", x0, y0, r);
}

void display_fill_circle(int16_t x0, int16_t y0, int16_t r, display_color_t color) {
    // Filled circle algorithm implementation.
    log_debug("LCD: Fill Circle (%d,%d) R=%d", x0, y0, r);
}

void display_set_font(const display_font_t *font) {
    // Store the pointer to the font definition.
    // current_font = font;
    log_debug("LCD: Set Font");
}

void display_draw_char(int16_t x, int16_t y, char c) {
    // Render character using the current font data and colors.
    log_debug("LCD: Draw Char '%c' at (%d,%d)", c, x, y);
    // Requires font data and rendering logic.
    // Example: Draw a placeholder rectangle for the char
    // display_fill_rect(x, y, 6, 8, current_fg_color); // Assuming 6x8 font
}

void display_draw_string(int16_t x, int16_t y, const char *str) {
    log_debug("LCD: Draw String \"%s\" at (%d,%d)", str, x, y);
    int16_t current_x = x;
    while (*str) {
        display_draw_char(current_x, y, *str++);
        current_x += 6; // Advance X by font width (assuming 6px wide)
        // Add checks for line wrapping if needed
        if (current_x >= LCD_WIDTH) break;
    }
}

void display_draw_bitmap(int16_t x, int16_t y, int16_t w, int16_t h, const display_color_t *bitmap) {
    if (x >= LCD_WIDTH || y >= LCD_HEIGHT || w <= 0 || h <= 0 || !bitmap) return;
    // Clip coordinates
    // ... (clipping logic similar to fill_rect) ...

    log_debug("LCD: Draw Bitmap (%d,%d) W=%d H=%d", x, y, w, h);
    lcd_set_window(x, y, x + w - 1, y + h - 1);

    uint32_t num_pixels = (uint32_t)w * h;
    const uint8_t *bitmap_bytes = (const uint8_t*)bitmap;

    hal_gpio_write(LCD_DC_PIN, true); // Data mode
    lcd_select();
    // Send bitmap data (2 bytes per pixel for RGB565)
    hal_spi_write_multi(LCD_SPI_ID, bitmap_bytes, num_pixels * 2);
    lcd_deselect();
}

void display_refresh(void) {
    // If using a framebuffer/backbuffer, copy its contents to the display here.
    // For direct drawing, this function might not be needed.
    log_debug("LCD: Refresh (No-op in direct draw mode)");
}
