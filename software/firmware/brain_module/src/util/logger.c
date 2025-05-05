/**
 * @file logger.c
 * @brief Simple logging utility for embedded systems.
 * Sends log messages over a designated UART interface.
 */

#include "util/logger.h"
#include "hal/uart.h"
#include "config.h" // Include config for LOG_LEVEL, LOG_UART_ID etc.
#include <stdio.h>  // For vsnprintf
#include <stdarg.h> // For va_list, va_start, va_end
#include <string.h> // For strlen

#if ENABLE_LOGGING

// --- Defines ---
#define LOG_BUFFER_SIZE 128 // Maximum length of a single formatted log message

// --- Internal State ---
static bool logger_initialized = false;

// --- Helper Function ---
// Sends the provided string over the designated log UART.
static void log_send_string(const char *str) {
    // Ensure logger is initialized before attempting to write.
    // A robust implementation might queue messages if called too early.
    if (!logger_initialized) {
       return; // Or handle error appropriately
    }
    hal_uart_write(LOG_UART_ID, (const uint8_t*)str, strlen(str));
}

// --- Public API Implementation ---

void logger_init(void) {
    // Initialize the UART peripheral designated for logging
    hal_uart_init(LOG_UART_ID, LOG_UART_BAUD, 8, 1, 0);
    logger_initialized = true;
    log_send_string("\r\n--- Logger Initialized ---\r\n");
}

void log_message(log_level_t level, const char *file, int line, const char *format, ...) {
    // Check if the message level is high enough to be logged
    if (level < LOG_LEVEL) {
        return;
    }

    char buffer[LOG_BUFFER_SIZE];
    char *buf_ptr = buffer;
    int buf_remaining = LOG_BUFFER_SIZE;
    int written = 0;

    // 1. Add Level Prefix
    const char *level_str;
    switch (level) {
        case LOG_LEVEL_DEBUG: level_str = "DBG"; break;
        case LOG_LEVEL_INFO:  level_str = "INF"; break;
        case LOG_LEVEL_WARN:  level_str = "WRN"; break;
        case LOG_LEVEL_ERROR: level_str = "ERR"; break;
        default:              level_str = "???"; break;
    }
    written = snprintf(buf_ptr, buf_remaining, "[%s] ", level_str);
    if (written < 0 || written >= buf_remaining) goto send_log; // Error or overflow
    buf_ptr += written;
    buf_remaining -= written;

    // Optional: Add File and Line (can consume significant buffer space)
    #ifdef LOG_INCLUDE_FILE_LINE
    // Optionally include file and line number in the log output.
    const char *filename = file; // Basic implementation uses full path.
    // Could add logic here to extract just the filename from the path.
    written = snprintf(buf_ptr, buf_remaining, "(%s:%d) ", filename, line);
    if (written < 0 || written >= buf_remaining) goto send_log; // Check for errors/overflow
    buf_ptr += written;
    buf_remaining -= written;
    #else
    (void)file; // Mark as unused if not logging file/line
    (void)line;
    #endif

    // 2. Format the user message
    va_list args;
    va_start(args, format);
    written = vsnprintf(buf_ptr, buf_remaining, format, args);
    va_end(args);
    if (written < 0 || written >= buf_remaining) {
         // Ensure null termination even on overflow
         if (buf_remaining > 0) buffer[LOG_BUFFER_SIZE - 1] = '\0';
         goto send_log;
    }
    buf_ptr += written;
    buf_remaining -= written;


    // 3. Add Newline
    if (buf_remaining >= 3) { // Need space for \r\n\0
        *buf_ptr++ = '\r';
        *buf_ptr++ = '\n';
        *buf_ptr = '\0';
    } else {
        // Not enough space for newline, just terminate
        if (buf_remaining > 0) buffer[LOG_BUFFER_SIZE - 1] = '\0';
    }

send_log:
    log_send_string(buffer);
}

void log_char(char c) {
    // Simple function to log a single character (useful for echoing RX data)
    char buf[2] = {c, '\0'};
    log_send_string(buf);
}

#else // ENABLE_LOGGING is 0

// Provide empty stubs when logging is disabled to avoid compilation errors
void logger_init(void) {}
void log_message(log_level_t level, const char *file, int line, const char *format, ...) {
    (void)level; (void)file; (void)line; (void)format; // Mark as unused
}
void log_char(char c) { (void)c; }

#endif // ENABLE_LOGGING
