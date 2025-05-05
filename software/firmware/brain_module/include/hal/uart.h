#ifndef HAL_UART_H
#define HAL_UART_H

/**
 * @file hal_uart.h
 * @brief Hardware Abstraction Layer for UART communication.
 * Supports both hardware UART (USART0) and potentially Software UART instances.
 */

#include <stdint.h>
#include <stddef.h> // For size_t
#include <stdbool.h>
#include "config.h" // Include config for UART IDs

// UART Interface Identifiers
typedef enum {
    UART_ID_0, // Hardware USART0 (e.g., for GPS)
    UART_ID_1, // Software UART instance 1 (e.g., for BLE)
    // Add more software UART IDs if needed
    UART_ID_MAX
} uart_id_t;

// Callback function pointer type for UART RX interrupt
// The callback receives the UART ID and the received data byte.
typedef void (*uart_rx_callback_t)(uart_id_t uart_id, uint8_t data);

/**
 * @brief Initializes a UART peripheral (Hardware or Software).
 * Configures baud rate, frame format, and enables the peripheral.
 * @param uart_id The UART peripheral identifier (e.g., GPS_UART_ID, BLE_UART_ID).
 * @param baud_rate The desired baud rate (e.g., 9600, 115200).
 * @param data_bits Number of data bits (typically 8).
 * @param stop_bits Number of stop bits (typically 1).
 * @param parity Parity setting (0=None, 1=Even, 2=Odd).
 */
void hal_uart_init(uart_id_t uart_id, uint32_t baud_rate, uint8_t data_bits, uint8_t stop_bits, uint8_t parity);

/**
 * @brief Sends a single byte over UART (blocking).
 * Waits until the UART transmit buffer is ready before sending.
 * @param uart_id The UART peripheral identifier.
 * @param data The byte to send.
 */
void hal_uart_put_char(uart_id_t uart_id, uint8_t data);

/**
 * @brief Sends a buffer of data over UART (blocking).
 * Transmits each byte sequentially using hal_uart_put_char.
 * @param uart_id The UART peripheral identifier.
 * @param buffer Pointer to the data buffer.
 * @param length Number of bytes to send.
 */
void hal_uart_write(uart_id_t uart_id, const uint8_t *buffer, size_t length);

/**
 * @brief Receives a single byte from UART (blocking).
 * Waits until a byte is received in the UART buffer.
 * @param uart_id The UART peripheral identifier.
 * @return The received byte. Returns 0 or -1 if no data (implementation dependent).
 */
int16_t hal_uart_get_char(uart_id_t uart_id);

/**
 * @brief Checks if data is available to read from the UART receive buffer.
 * @param uart_id The UART peripheral identifier.
 * @return true if at least one byte is available, false otherwise.
 */
bool hal_uart_data_available(uart_id_t uart_id);

/**
 * @brief Reads data from UART into a buffer (non-blocking).
 * Reads available data from the receive buffer up to the specified buffer length.
 * Best used after checking hal_uart_data_available() or within an RX interrupt context.
 * @param uart_id The UART peripheral identifier.
 * @param buffer Pointer to the buffer where received data will be stored.
 * @param length Maximum number of bytes to read into the buffer.
 * @return The number of bytes actually read (can be 0 if no data was available).
 */
size_t hal_uart_read(uart_id_t uart_id, uint8_t *buffer, size_t length);

/**
 * @brief Enables the UART Receive Complete interrupt.
 * Registers a callback function to be called when a byte is received.
 * @param uart_id The UART peripheral identifier.
 * @param callback The function to call when a byte is received via interrupt.
 */
void hal_uart_enable_rx_interrupt(uart_id_t uart_id, uart_rx_callback_t callback);

/**
 * @brief Disables the UART Receive Complete interrupt.
 * @param uart_id The UART peripheral identifier.
 */
void hal_uart_disable_rx_interrupt(uart_id_t uart_id);

/**
 * @brief Flushes the UART receive buffer.
 * Discards any unread data in the buffer.
 * @param uart_id The UART peripheral identifier.
 */
void hal_uart_flush_rx_buffer(uart_id_t uart_id);

#endif // HAL_UART_H
