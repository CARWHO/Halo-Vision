/**
 * @file gps_driver.c
 * @brief Driver for handling GPS communication and NMEA sentence parsing.
 * Interacts with the UART HAL to receive data and provides structured GPS info.
 *
 * This implementation focuses on parsing $GPRMC and $GPGGA sentences.
 */

#include "modules/gps.h" // Use the module header file name
#include "hal/uart.h"
#include "util/logger.h"
#include <string.h> // For strcmp, strlen, etc.
#include <stdlib.h> // For atof, atoi
#include <stdio.h>  // For sscanf (use with caution on embedded systems due to code size)

// --- Defines ---
#define NMEA_MAX_SENTENCE_LEN 82   // Maximum NMEA sentence length
#define NMEA_MAX_FIELDS 20         // Maximum number of comma-separated fields expected

// --- Internal State ---
static gps_data_t current_gps_data; // Holds the latest parsed data
static bool data_updated = false;   // Flag indicating new data is available
static bool data_valid_fix = false; // Flag indicating the current data represents a valid fix

// NMEA sentence buffer and parser state
static char nmea_buffer[NMEA_MAX_SENTENCE_LEN + 1];
static uint8_t nmea_buffer_index = 0;
static bool sentence_in_progress = false;

// --- NMEA Parsing Helper Functions ---

// Calculate NMEA checksum
static uint8_t nmea_calculate_checksum(const char *sentence) {
    uint8_t checksum = 0;
    // Checksum includes characters between '$' and '*'
    if (*sentence == '$') sentence++;
    while (*sentence && *sentence != '*') {
        checksum ^= *sentence++;
    }
    return checksum;
}

// Validate NMEA checksum
static bool nmea_validate_checksum(const char *sentence) {
    const char *checksum_str = strchr(sentence, '*');
    if (!checksum_str || strlen(checksum_str) < 3) {
        return false; // No '*' or checksum too short
    }
    uint8_t calculated_checksum = nmea_calculate_checksum(sentence);
    uint8_t received_checksum = (uint8_t)strtol(checksum_str + 1, NULL, 16);
    return calculated_checksum == received_checksum;
}

// Split NMEA sentence into fields
// Returns the number of fields found
static int nmea_split_fields(char *sentence, char *fields[], int max_fields) {
    int count = 0;
    fields[count++] = sentence; // First field starts after '$' (or at beginning if no '$')

    char* p = sentence;
    while (*p && *p != '*' && count < max_fields) {
        if (*p == ',') {
            *p = '\0'; // Null-terminate previous field
            fields[count++] = p + 1; // Next field starts after comma
        }
        p++;
    }
    // Null-terminate the last field (before '*')
    if (*p == '*') {
        *p = '\0';
    }
    return count;
}

// Parse $GPRMC sentence
static void parse_gprmc(char *fields[], int field_count) {
    if (field_count < 12) return; // Not enough fields for RMC

    // Field 1: Time (HHMMSS.sss) - Extract time components.
    if (strlen(fields[1]) >= 6) {
        // Actual parsing logic using sscanf or manual conversion would go here.
        current_gps_data.hour = 12; // Dummy data
        current_gps_data.minute = 34;
        current_gps_data.second = 56;
    }

    // Field 2: Status (A=Active/Valid, V=Void/Invalid) - Update fix validity.
    current_gps_data.fix_valid = (fields[2][0] == 'A');

    // Field 3, 4: Latitude (DDMM.mmmm, N/S) - Extract and convert.
    // Field 5, 6: Longitude (DDDMM.mmmm, E/W) - Extract and convert.
    // Actual parsing and conversion logic (e.g., atof, helper functions) needed here.
    current_gps_data.latitude = 40.7128f;  // Dummy data
    current_gps_data.longitude = -74.0060f; // Dummy data

    // Field 7: Speed over ground (knots) - Convert to float.
    current_gps_data.speed_knots = atof(fields[7]); // Keep atof for plausible value
    current_gps_data.speed_kmh = current_gps_data.speed_knots * 1.852f;

    // Field 8: Track angle (degrees true) - Convert to float.
    current_gps_data.course_deg = atof(fields[8]); // Keep atof

    // Field 9: Date (DDMMYY) - Extract date components.
    if (strlen(fields[9]) == 6) {
        // Actual parsing logic using sscanf or manual conversion would go here.
        current_gps_data.day = 5; // Dummy data
        current_gps_data.month = 5;
        current_gps_data.year = 2025;
    }

    // Fields 10, 11: Magnetic variation (optional) - Ignored.

    current_gps_data.seen_rmc = true;
    data_updated = true; // Mark data as updated
    data_valid_fix = current_gps_data.fix_valid; // Update overall validity
    log_debug("GPS: Parsed RMC, Valid: %d", current_gps_data.fix_valid);
}

// Parse $GPGGA sentence
static void parse_gpgga(char *fields[], int field_count) {
     if (field_count < 10) return; // Not enough fields for GGA

    // Field 1: Time (HHMMSS.sss) - Can update time if needed, RMC usually preferred
    // Field 2, 3: Latitude (DDMM.mmmm, N/S) - Can update if needed
    // Field 4, 5: Longitude (DDDMM.mmmm, E/W) - Can update if needed

    // Field 6: Fix Quality (0=Invalid, 1=GPS, 2=DGPS, ...)
    current_gps_data.fix_quality = atoi(fields[6]);
    // Update overall validity based on GGA quality if RMC wasn't seen or was void
    if (!current_gps_data.seen_rmc || fields[2][0] == 'V') {
         current_gps_data.fix_valid = (current_gps_data.fix_quality > 0);
    }

    // Field 7: Number of satellites tracked
    current_gps_data.satellites_tracked = atoi(fields[7]);

    // Field 8: Horizontal Dilution of Precision (HDOP) - Ignored for now

    // Field 9, 10: Altitude (Meters, M) - Convert to float.
    current_gps_data.altitude_msl = atof(fields[9]); // Keep atof

    // Other fields: Geoid separation, DGPS age, DGPS station ID - Ignored.

    current_gps_data.seen_gga = true;
    data_updated = true; // Mark data as updated
    data_valid_fix = current_gps_data.fix_valid; // Update overall validity
    log_debug("GPS: Parsed GGA, Quality: %d, Sats: %d", current_gps_data.fix_quality, current_gps_data.satellites_tracked);
}

// Process a complete, validated NMEA sentence
static void process_nmea_sentence(char *sentence) {
    log_debug("GPS: Processing: %s", sentence);

    char *fields[NMEA_MAX_FIELDS];
    // Skip the '$'
    int field_count = nmea_split_fields(sentence + 1, fields, NMEA_MAX_FIELDS);

    if (field_count < 1) return;

    // Identify sentence type and call appropriate parser
    if (strncmp(fields[0], "GPRMC", 5) == 0 || strncmp(fields[0], "GNRMC", 5) == 0) {
        parse_gprmc(fields, field_count);
    } else if (strncmp(fields[0], "GPGGA", 5) == 0 || strncmp(fields[0], "GNGGA", 5) == 0) {
        parse_gpgga(fields, field_count);
    }
    // Add handlers for other sentences (GSA, GSV, VTG) if needed
    // else if (strncmp(fields[0], "GPVTG", 5) == 0) { parse_gpvtg(fields, field_count); }
    else {
        log_debug("GPS: Ignoring sentence type: %s", fields[0]);
    }
}


// --- Public API Implementation ---

void gps_init(void) {
    log_info("GPS: Initializing...");
    // Initialize UART for GPS communication
    hal_uart_init(GPS_UART_ID, GPS_UART_BAUD, 8, 1, 0); // 8N1 configuration

    // Reset internal state
    memset(&current_gps_data, 0, sizeof(current_gps_data));
    data_updated = false;
    data_valid_fix = false;
    nmea_buffer_index = 0;
    sentence_in_progress = false;

    // Enable UART RX interrupt if desired, otherwise rely on polling gps_process_char
    // hal_uart_enable_rx_interrupt(GPS_UART_ID, gps_uart_rx_handler); // Need handler function
    log_info("GPS: Initialized. Waiting for data on UART %d.", GPS_UART_ID);
}

void gps_process_char(uint8_t received_char) {
    // Handle NMEA sentence framing and buffering
    if (received_char == '$') {
        // Start of a new sentence
        nmea_buffer_index = 0;
        nmea_buffer[nmea_buffer_index++] = received_char;
        sentence_in_progress = true;
    } else if (sentence_in_progress) {
        // Continue buffering characters
        if (nmea_buffer_index < NMEA_MAX_SENTENCE_LEN) {
            nmea_buffer[nmea_buffer_index++] = received_char;
        } else {
            // Buffer overflow, discard sentence
            sentence_in_progress = false;
            log_warn("GPS: NMEA buffer overflow");
        }

        // Check for end of sentence (CR LF)
        if (received_char == '\n') {
            nmea_buffer[nmea_buffer_index] = '\0'; // Null-terminate

            // Check preceding character was CR
            if (nmea_buffer_index > 1 && nmea_buffer[nmea_buffer_index - 2] == '\r') {
                 nmea_buffer[nmea_buffer_index - 2] = '\0'; // Terminate before CR LF
            } else {
                 nmea_buffer[nmea_buffer_index - 1] = '\0'; // Terminate before LF only
            }


            // Validate checksum and process
            if (nmea_validate_checksum(nmea_buffer)) {
                process_nmea_sentence(nmea_buffer);
            } else {
                log_warn("GPS: Invalid checksum: %s", nmea_buffer);
            }
            sentence_in_progress = false; // Ready for next sentence
        }
    }
    // Ignore characters received outside of a sentence ($...*xx\r\n)
}

bool gps_is_data_available(void) {
    // Data is considered available if essential sentences have been updated
    // and the fix is marked as valid.
    bool available = data_updated;
    // Reset flag after checking, so it only returns true once per update cycle
    // data_updated = false; // Resetting here might be too soon if gps_get_data isn't called immediately
    return available;
}

bool gps_get_data(gps_data_t *data) {
    if (data == NULL) return false;

    // Copy internal data structure
    memcpy(data, &current_gps_data, sizeof(gps_data_t));

    // Reset the update flag now that data has been retrieved
    data_updated = false;

    // Return the validity status of the copied data
    return data_valid_fix;
}

float gps_get_speed_kmh(void) {
    if (data_valid_fix) {
        return current_gps_data.speed_kmh;
    } else {
        return 0.0f; // Or indicate invalid speed differently, e.g., -1.0f
    }
}

bool gps_get_location(float *latitude, float *longitude) {
    if (data_valid_fix && latitude != NULL && longitude != NULL) {
        *latitude = current_gps_data.latitude;
        *longitude = current_gps_data.longitude;
        return true;
    } else {
        // Optionally set output pointers to 0 or NaN
        if (latitude) *latitude = 0.0f;
        if (longitude) *longitude = 0.0f;
        return false;
    }
}

// UART RX Handler would typically call gps_process_char(data)
// when a byte is received on the GPS UART.
