#ifndef HAL_UART_H
#define HAL_UART_H

/**
 * @file hal_uart.h
 * @brief Hardware Abstraction Layer for UART communication (Display Module).
 */

#include <stdint.h>
#include <stddef.h> // For size_t
#include <stdbool.h>
#include "config.h" // Include config for UART IDs

// UART Interface Identifiers (ATmega328P has one hardware UART)
typedef enum {
    UART_ID_0, // Hardware USART0 (used for BLE)
    UART_ID_MAX
} uart_id_t;

// Callback function pointer type for UART RX interrupt
typedef void (*uart_rx_callback_t)(uart_id_t uart_id, uint8_t data);

/**
 * @brief Initializes the UART peripheral.
 * @param uart_id The UART peripheral identifier (must be UART_ID_0).
 * @param baud_rate Desired baud rate (e.g., 115200 for BLE).
 * @param data_bits Number of data bits (usually 8).
 * @param stop_bits Number of stop bits (usually 1).
 * @param parity Parity setting (0=None).
 */
void hal_uart_init(uart_id_t uart_id, uint32_t baud_rate, uint8_t data_bits, uint8_t stop_bits, uint8_t parity);

/**
 * @brief Sends a single byte over UART (blocking).
 * @param uart_id The UART peripheral identifier.
 * @param data The byte to send.
 */
void hal_uart_put_char(uart_id_t uart_id, uint8_t data);

/**
 * @brief Sends a buffer of data over UART (blocking).
 * @param uart_id The UART peripheral identifier.
 * @param buffer Pointer to the data buffer.
 * @param length Number of bytes to send.
 */
void hal_uart_write(uart_id_t uart_id, const uint8_t *buffer, size_t length);

/**
 * @brief Receives a single byte from UART (blocking or non-blocking).
 * @param uart_id The UART peripheral identifier.
 * @return The received byte, or -1 if no data available (non-blocking).
 */
int16_t hal_uart_get_char(uart_id_t uart_id);

/**
 * @brief Checks if data is available to read from UART.
 * @param uart_id The UART peripheral identifier.
 * @return true if data is available, false otherwise.
 */
bool hal_uart_data_available(uart_id_t uart_id);

/**
 * @brief Reads available data from UART into a buffer (non-blocking).
 * @param uart_id The UART peripheral identifier.
 * @param buffer Pointer to the buffer to store received data.
 * @param length Maximum number of bytes to read.
 * @return Number of bytes actually read.
 */
size_t hal_uart_read(uart_id_t uart_id, uint8_t *buffer, size_t length);

/**
 * @brief Enables UART Receive Complete interrupt.
 * @param uart_id The UART peripheral identifier.
 * @param callback Function to call when a byte is received.
 */
void hal_uart_enable_rx_interrupt(uart_id_t uart_id, uart_rx_callback_t callback);

/**
 * @brief Disables UART Receive Complete interrupt.
 * @param uart_id The UART peripheral identifier.
 */
void hal_uart_disable_rx_interrupt(uart_id_t uart_id);

/**
 * @brief Flushes the UART receive buffer.
 * @param uart_id The UART peripheral identifier.
 */
void hal_uart_flush_rx_buffer(uart_id_t uart_id);

#endif // HAL_UART_H
