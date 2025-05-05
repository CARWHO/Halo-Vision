/**
 * @file logger.c
 * @brief Simple logging utility for the Display Module.
 * Sends log messages over the BLE UART interface.
 */

#include "util/logger.h"
#include "hal/uart.h"
#include "config.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#if ENABLE_LOGGING

#define LOG_BUFFER_SIZE 128
static bool logger_initialized = false;

// Sends the string over the log UART.
static void log_send_string(const char *str) {
    if (!logger_initialized) {
       // Attempt basic init as fallback, but logger_init should be called first.
       hal_uart_init(LOG_UART_ID, LOG_UART_BAUD, 8, 1, 0);
       logger_initialized = true;
    }
    hal_uart_write(LOG_UART_ID, (const uint8_t*)str, strlen(str));
}

void logger_init(void) {
    // Initialize the UART used for logging.
    hal_uart_init(LOG_UART_ID, LOG_UART_BAUD, 8, 1, 0);
    logger_initialized = true;
    log_send_string("\r\n--- Display Logger Initialized ---\r\n");
}

void log_message(log_level_t level, const char *file, int line, const char *format, ...) {
    if (level < LOG_LEVEL) {
        return; // Skip message if below configured level
    }

    char buffer[LOG_BUFFER_SIZE];
    char *buf_ptr = buffer;
    int buf_remaining = LOG_BUFFER_SIZE;
    int written = 0;

    // Add Level Prefix
    const char *level_str;
    switch (level) {
        case LOG_LEVEL_DEBUG: level_str = "DBG"; break;
        case LOG_LEVEL_INFO:  level_str = "INF"; break;
        case LOG_LEVEL_WARN:  level_str = "WRN"; break;
        case LOG_LEVEL_ERROR: level_str = "ERR"; break;
        default:              level_str = "???"; break;
    }
    written = snprintf(buf_ptr, buf_remaining, "[%s] ", level_str);
    if (written < 0 || written >= buf_remaining) goto send_log;
    buf_ptr += written;
    buf_remaining -= written;

    // Format the user message
    va_list args;
    va_start(args, format);
    written = vsnprintf(buf_ptr, buf_remaining, format, args);
    va_end(args);
    if (written < 0 || written >= buf_remaining) {
         if (buf_remaining > 0) buffer[LOG_BUFFER_SIZE - 1] = '\0'; // Ensure termination
         goto send_log;
    }
    buf_ptr += written;
    buf_remaining -= written;

    // Add Newline
    if (buf_remaining >= 3) {
        *buf_ptr++ = '\r';
        *buf_ptr++ = '\n';
        *buf_ptr = '\0';
    } else {
        if (buf_remaining > 0) buffer[LOG_BUFFER_SIZE - 1] = '\0'; // Ensure termination
    }

send_log:
    log_send_string(buffer);
}

void log_char(char c) {
    // Log a single character.
    char buf[2] = {c, '\0'};
    log_send_string(buf);
}

#else // ENABLE_LOGGING is 0

// Empty stubs when logging is disabled.
void logger_init(void) {}
void log_message(log_level_t level, const char *file, int line, const char *format, ...) {
    (void)level; (void)file; (void)line; (void)format;
}
void log_char(char c) { (void)c; }

#endif // ENABLE_LOGGING
