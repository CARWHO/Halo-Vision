/**
 * @file uart.c
 * @brief UART HAL implementation for ATmega328P (Display Module).
 * Primarily used for BLE communication. Uses ring buffers.
 */

#include "hal/uart.h"
#include "util/ring_buffer.h"
#include "util/logger.h"
#include <stddef.h> // For NULL

// --- Configuration ---
// Buffer sizes defined in config.h (BLE_UART_RX_BUFFER_SIZE, etc.)

// --- Internal Data Structures ---
static uint8_t hw_uart0_rx_buf_data[BLE_UART_RX_BUFFER_SIZE];
static ring_buffer_t hw_uart0_rx_rb;
static uint8_t hw_uart0_tx_buf_data[BLE_UART_TX_BUFFER_SIZE];
static ring_buffer_t hw_uart0_tx_rb;
static uart_rx_callback_t hw_uart0_rx_callback = NULL;

// --- Helper Functions ---
static ring_buffer_t* get_rx_rb(uart_id_t uart_id) {
    if (uart_id == UART_ID_0) return &hw_uart0_rx_rb;
    return NULL;
}

static ring_buffer_t* get_tx_rb(uart_id_t uart_id) {
    if (uart_id == UART_ID_0) return &hw_uart0_tx_rb;
    return NULL;
}

// --- Public API Implementation ---

void hal_uart_init(uart_id_t uart_id, uint32_t baud_rate, uint8_t data_bits, uint8_t stop_bits, uint8_t parity) {
    if (uart_id != UART_ID_0) {
        log_error("UART: Invalid ID %d for init", uart_id);
        return;
    }
    log_info("UART: Init ID %d, Baud %lu", uart_id, baud_rate);

    // Initialize ring buffers
    ring_buffer_init(&hw_uart0_rx_rb, hw_uart0_rx_buf_data, BLE_UART_RX_BUFFER_SIZE);
    ring_buffer_init(&hw_uart0_tx_rb, hw_uart0_tx_buf_data, BLE_UART_TX_BUFFER_SIZE);
    hw_uart0_rx_callback = NULL;

    // Configure hardware UART0 registers (UBRR0, UCSR0A/B/C).
    // Set baud rate, frame format (8N1), enable RX/TX.
}

void hal_uart_put_char(uart_id_t uart_id, uint8_t data) {
    if (uart_id != UART_ID_0) return;
    // Blocking write: Wait for UDRE0 flag, then write to UDR0.
    log_debug("UART: Put Char ID %d: 0x%02X", uart_id, data);
    // Or, add to TX ring buffer and enable UDRIE0 interrupt.
}

void hal_uart_write(uart_id_t uart_id, const uint8_t *buffer, size_t length) {
    if (uart_id != UART_ID_0) return;
    // Send multiple bytes using hal_uart_put_char or TX ring buffer.
    for (size_t i = 0; i < length; ++i) {
        hal_uart_put_char(uart_id, buffer[i]);
    }
}

int16_t hal_uart_get_char(uart_id_t uart_id) {
    if (uart_id != UART_ID_0) return -1;
    ring_buffer_t* rb = get_rx_rb(uart_id);
    uint8_t data;
    if (rb && ring_buffer_read(rb, &data)) {
        return data;
    }
    // Optional: Blocking read if buffer empty (check RXC0 flag, read UDR0).
    return -1; // No data available
}

bool hal_uart_data_available(uart_id_t uart_id) {
    if (uart_id != UART_ID_0) return false;
    ring_buffer_t* rb = get_rx_rb(uart_id);
    // Check if RX ring buffer is not empty.
    return (rb && !ring_buffer_is_empty(rb));
    // Or check hardware RXC0 flag if not using buffers.
}

size_t hal_uart_read(uart_id_t uart_id, uint8_t *buffer, size_t length) {
    if (uart_id != UART_ID_0) return 0;
    ring_buffer_t* rb = get_rx_rb(uart_id);
    size_t bytes_read = 0;
    if (rb) {
        // Read multiple bytes from RX ring buffer.
        while (bytes_read < length && ring_buffer_read(rb, &buffer[bytes_read])) {
            bytes_read++;
        }
    }
    return bytes_read;
}

void hal_uart_enable_rx_interrupt(uart_id_t uart_id, uart_rx_callback_t callback) {
    if (uart_id != UART_ID_0) return;
    log_debug("UART: Enable RX Int ID %d", uart_id);
    hw_uart0_rx_callback = callback;
    // Enable hardware RX Complete interrupt (RXCIE0 in UCSR0B).
    // Ensure global interrupts enabled elsewhere.
}

void hal_uart_disable_rx_interrupt(uart_id_t uart_id) {
     if (uart_id != UART_ID_0) return;
    log_debug("UART: Disable RX Int ID %d", uart_id);
    hw_uart0_rx_callback = NULL;
    // Disable hardware RX Complete interrupt (RXCIE0 in UCSR0B).
}

void hal_uart_flush_rx_buffer(uart_id_t uart_id) {
    if (uart_id != UART_ID_0) return;
    ring_buffer_t* rb = get_rx_rb(uart_id);
    if (rb) {
        ring_buffer_clear(rb);
    }
    // May need to read UDR0 hardware register to clear flags.
}

// ISR(USART_RX_vect) would read UDR0, write to hw_uart0_rx_rb, call callback.
// ISR(USART_UDRE_vect) would read from hw_uart0_tx_rb, write to UDR0.
