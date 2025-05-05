#ifndef MODULES_DISPLAY_DRIVER_H
#define MODULES_DISPLAY_DRIVER_H

/**
 * @file display_driver.h
 * @brief Interface for the LCD Display Driver module.
 * Provides functions to initialize the display and draw graphics primitives.
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h> // For size_t

// Define color type (e.g., RGB565)
typedef uint16_t display_color_t;

// Define common colors (RGB565 format)
#define COLOR_BLACK       0x0000
#define COLOR_WHITE       0xFFFF
#define COLOR_RED         0xF800
#define COLOR_GREEN       0x07E0
#define COLOR_BLUE        0x001F
#define COLOR_YELLOW      0xFFE0
#define COLOR_CYAN        0x07FF
#define COLOR_MAGENTA     0xF81F
#define COLOR_ORANGE      0xFD20 // Example custom color

// Define font structure (placeholder)
typedef struct {
    const uint8_t *font_data;
    uint8_t width;
    uint8_t height;
    // Add other font properties if needed
} display_font_t;

/**
 * @brief Initializes the LCD display hardware and driver.
 * Configures SPI, GPIO pins (CS, DC, RST), and sends initialization commands to the LCD controller.
 */
void display_init(void);

/**
 * @brief Clears the entire display to a specified color.
 * @param color The color to fill the screen with.
 */
void display_clear(display_color_t color);

/**
 * @brief Sets the foreground color for subsequent drawing operations.
 * @param color The desired foreground color.
 */
void display_set_foreground_color(display_color_t color);

/**
 * @brief Sets the background color for text rendering.
 * @param color The desired background color.
 */
void display_set_background_color(display_color_t color);

/**
 * @brief Draws a single pixel at the specified coordinates.
 * @param x X-coordinate.
 * @param y Y-coordinate.
 * @param color Color of the pixel.
 */
void display_draw_pixel(int16_t x, int16_t y, display_color_t color);

/**
 * @brief Draws a line between two points.
 * @param x0 Starting X-coordinate.
 * @param y0 Starting Y-coordinate.
 * @param x1 Ending X-coordinate.
 * @param y1 Ending Y-coordinate.
 * @param color Color of the line.
 */
void display_draw_line(int16_t x0, int16_t y0, int16_t x1, int16_t y1, display_color_t color);

/**
 * @brief Draws a rectangle outline.
 * @param x Top-left X-coordinate.
 * @param y Top-left Y-coordinate.
 * @param w Width of the rectangle.
 * @param h Height of the rectangle.
 * @param color Color of the outline.
 */
void display_draw_rect(int16_t x, int16_t y, int16_t w, int16_t h, display_color_t color);

/**
 * @brief Draws a filled rectangle.
 * @param x Top-left X-coordinate.
 * @param y Top-left Y-coordinate.
 * @param w Width of the rectangle.
 * @param h Height of the rectangle.
 * @param color Fill color.
 */
void display_fill_rect(int16_t x, int16_t y, int16_t w, int16_t h, display_color_t color);

/**
 * @brief Draws a circle outline.
 * @param x0 Center X-coordinate.
 * @param y0 Center Y-coordinate.
 * @param r Radius of the circle.
 * @param color Color of the outline.
 */
void display_draw_circle(int16_t x0, int16_t y0, int16_t r, display_color_t color);

/**
 * @brief Draws a filled circle.
 * @param x0 Center X-coordinate.
 * @param y0 Center Y-coordinate.
 * @param r Radius of the circle.
 * @param color Fill color.
 */
void display_fill_circle(int16_t x0, int16_t y0, int16_t r, display_color_t color);

/**
 * @brief Sets the font to use for text rendering.
 * @param font Pointer to the font definition structure.
 */
void display_set_font(const display_font_t *font);

/**
 * @brief Draws a single character at the specified position.
 * Uses the currently set font, foreground, and background colors.
 * @param x Top-left X-coordinate for the character.
 * @param y Top-left Y-coordinate for the character.
 * @param c The character to draw.
 */
void display_draw_char(int16_t x, int16_t y, char c);

/**
 * @brief Draws a null-terminated string at the specified position.
 * Uses the currently set font, foreground, and background colors.
 * @param x Top-left X-coordinate for the start of the string.
 * @param y Top-left Y-coordinate for the start of the string.
 * @param str The string to draw.
 */
void display_draw_string(int16_t x, int16_t y, const char *str);

/**
 * @brief Draws a bitmap image onto the display.
 * @param x Top-left X-coordinate for the bitmap.
 * @param y Top-left Y-coordinate for the bitmap.
 * @param w Width of the bitmap.
 * @param h Height of the bitmap.
 * @param bitmap Pointer to the bitmap data (e.g., array of display_color_t).
 */
void display_draw_bitmap(int16_t x, int16_t y, int16_t w, int16_t h, const display_color_t *bitmap);

/**
 * @brief Refreshes a portion or the entire display from a back buffer (if used).
 * For displays without hardware acceleration, this might not do anything if drawing directly.
 */
void display_refresh(void);


#endif // MODULES_DISPLAY_DRIVER_H
