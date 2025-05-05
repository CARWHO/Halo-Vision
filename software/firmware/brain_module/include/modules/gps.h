#ifndef MODULES_GPS_H
#define MODULES_GPS_H

/**
 * @file gps.h
 * @brief GPS Module Interface for parsing NMEA sentences.
 * Handles communication with a GPS receiver via UART and provides structured data.
 */

#include <stdint.h>
#include <stdbool.h>
#include "config.h" // Include config for GPS UART settings

// Structure to hold parsed GPS data
typedef struct {
    // Time
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint16_t millisecond; // Often available in RMC/GGA

    // Date
    uint8_t day;
    uint8_t month;
    uint16_t year;

    // Position
    float latitude;     // Degrees (positive N, negative S)
    float longitude;    // Degrees (positive E, negative W)
    float altitude_msl; // Altitude above mean sea level (meters)

    // Movement
    float speed_knots;  // Speed over ground (knots)
    float speed_kmh;    // Speed over ground (km/h)
    float course_deg;   // Course over ground (degrees true)

    // Fix Quality
    uint8_t fix_quality; // 0=Invalid, 1=GPS fix, 2=DGPS fix, etc. (from GGA)
    bool fix_valid;      // True if fix_quality > 0 and data is considered reliable
    uint8_t satellites_tracked; // Number of satellites used in fix (from GGA)

    // Raw NMEA sentence flags (optional, for debugging)
    bool seen_gga;
    bool seen_rmc;
    // Add flags for other sentences if parsed (e.g., GSA, GSV)

} gps_data_t;

/**
 * @brief Initializes the GPS module.
 * Sets up the underlying UART communication (using GPS_UART_ID from config.h)
 * and initializes the NMEA parser state.
 */
void gps_init(void);

/**
 * @brief Processes incoming characters from the GPS UART.
 * This function should be called frequently with each byte received from the
 * GPS receiver (e.g., from a UART RX interrupt or polling loop).
 * It builds and parses NMEA sentences ($GPGGA, $GPRMC, etc.).
 * @param received_char The character received from the GPS UART.
 */
void gps_process_char(uint8_t received_char);

/**
 * @brief Checks if a complete and valid set of GPS data has been received and parsed.
 * Typically checks if essential sentences like RMC and/or GGA have been updated.
 * @return true if new, valid data is available since the last call to gps_get_data, false otherwise.
 */
bool gps_is_data_available(void);

/**
 * @brief Retrieves the latest parsed GPS data.
 * Copies the internal GPS data structure to the provided pointer.
 * @param data Pointer to a gps_data_t structure where the latest data will be copied.
 * @return true if the copied data represents a valid fix (gps_data.fix_valid is true), false otherwise.
 *         This function may also reset the "data available" flag internally.
 */
bool gps_get_data(gps_data_t *data);

/**
 * @brief Gets the current speed reading from the GPS data.
 * Convenience function to access the speed directly.
 * @return Speed in km/h if the fix is valid, or 0.0f (or a negative value) if invalid.
 */
float gps_get_speed_kmh(void);

/**
 * @brief Gets the current location (latitude and longitude).
 * Convenience function to access location directly.
 * @param latitude Pointer to store latitude in degrees.
 * @param longitude Pointer to store longitude in degrees.
 * @return true if the location data is valid (fix is valid), false otherwise.
 */
bool gps_get_location(float *latitude, float *longitude);

#endif // MODULES_GPS_H
