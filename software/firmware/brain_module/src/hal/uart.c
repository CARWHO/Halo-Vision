/**
 * @file uart.c
 * @brief UART HAL implementation for ATmega328P.
 * Handles communication for Hardware USART0 and potentially Software UART instances.
 * Uses ring buffers for transmit/receive buffering, especially with interrupts.
 */

#include "hal/uart.h"
#include "util/ring_buffer.h" // Assumes a ring buffer utility exists
#include "util/logger.h"
#include <stddef.h> // For NULL

// --- Configuration ---
// Define buffer sizes (can be overridden by config.h if needed)
#ifndef HW_UART0_RX_BUFFER_SIZE
#define HW_UART0_RX_BUFFER_SIZE 64
#endif
#ifndef HW_UART0_TX_BUFFER_SIZE
#define HW_UART0_TX_BUFFER_SIZE 64
#endif
#ifndef SW_UART1_RX_BUFFER_SIZE
#define SW_UART1_RX_BUFFER_SIZE 32
#endif
#ifndef SW_UART1_TX_BUFFER_SIZE
#define SW_UART1_TX_BUFFER_SIZE 32
#endif

// --- Internal Data Structures ---

// Buffers and state for Hardware UART0
static uint8_t hw_uart0_rx_buf_data[HW_UART0_RX_BUFFER_SIZE];
static ring_buffer_t hw_uart0_rx_rb;
static uint8_t hw_uart0_tx_buf_data[HW_UART0_TX_BUFFER_SIZE];
static ring_buffer_t hw_uart0_tx_rb;
static uart_rx_callback_t hw_uart0_rx_callback = NULL;

// Buffers and state for Software UART1 (Example)
// A real Software UART implementation (like Arduino's SoftwareSerial) would be needed here.
// These are just placeholders for the HAL structure.
static uint8_t sw_uart1_rx_buf_data[SW_UART1_RX_BUFFER_SIZE];
static ring_buffer_t sw_uart1_rx_rb;
static uint8_t sw_uart1_tx_buf_data[SW_UART1_TX_BUFFER_SIZE];
static ring_buffer_t sw_uart1_tx_rb;
static uart_rx_callback_t sw_uart1_rx_callback = NULL;


// --- Helper Functions ---

// Get ring buffer pointers based on UART ID
static ring_buffer_t* get_rx_rb(uart_id_t uart_id) {
    if (uart_id == UART_ID_0) return &hw_uart0_rx_rb;
    if (uart_id == UART_ID_1) return &sw_uart1_rx_rb;
    return NULL;
}

static ring_buffer_t* get_tx_rb(uart_id_t uart_id) {
    if (uart_id == UART_ID_0) return &hw_uart0_tx_rb;
    if (uart_id == UART_ID_1) return &sw_uart1_tx_rb;
    return NULL;
}

// --- Public API Implementation ---

void hal_uart_init(uart_id_t uart_id, uint32_t baud_rate, uint8_t data_bits, uint8_t stop_bits, uint8_t parity) {
    log_info("UART: Init ID %d, Baud %lu", uart_id, baud_rate);

    if (uart_id == UART_ID_0) {
        // Initialize ring buffers
        ring_buffer_init(&hw_uart0_rx_rb, hw_uart0_rx_buf_data, HW_UART0_RX_BUFFER_SIZE);
        ring_buffer_init(&hw_uart0_tx_rb, hw_uart0_tx_buf_data, HW_UART0_TX_BUFFER_SIZE);
        hw_uart0_rx_callback = NULL;

        // Configure hardware UART0 registers for baud rate, frame format,
        // and enable transmitter/receiver.

    } else if (uart_id == UART_ID_1) {
        // Initialize ring buffers for software UART
        ring_buffer_init(&sw_uart1_rx_rb, sw_uart1_rx_buf_data, SW_UART1_RX_BUFFER_SIZE);
        ring_buffer_init(&sw_uart1_tx_rb, sw_uart1_tx_buf_data, SW_UART1_TX_BUFFER_SIZE);
        sw_uart1_rx_callback = NULL;

        // Configure GPIO pins and timers required for software UART operation.
        log_info("UART: Software UART ID %d init (simulated)", uart_id);

    } else {
        log_error("UART: Invalid ID %d for init", uart_id);
    }
}

void hal_uart_put_char(uart_id_t uart_id, uint8_t data) {
    // Send a single byte over the specified UART interface (blocking).
    if (uart_id == UART_ID_0) {
        // Hardware UART: Wait for transmit buffer empty, then write data.
    } else if (uart_id == UART_ID_1) {
        // Software UART: Perform timed bit-banging to transmit the byte.
    }
    // If using transmit interrupts/buffers, this would add to the buffer
    // and potentially enable the transmit interrupt.
    log_debug("UART: Put Char ID %d: 0x%02X", uart_id, data);
}

void hal_uart_write(uart_id_t uart_id, const uint8_t *buffer, size_t length) {
    // Send multiple bytes by calling put_char repeatedly.
    for (size_t i = 0; i < length; ++i) {
        hal_uart_put_char(uart_id, buffer[i]); // Send byte by byte (blocking)
    }
}

int16_t hal_uart_get_char(uart_id_t uart_id) {
    ring_buffer_t* rb = get_rx_rb(uart_id);
    uint8_t data;

    if (rb) {
        // --- Buffered Read ---
        if (ring_buffer_read(rb, &data)) {
            return data; // Return data read from buffer
        } else {
            // If buffer is empty, optionally perform a blocking read.
            // This is less common if using interrupts or polling data_available.
            // if (uart_id == UART_ID_0) { /* Wait for hardware RX flag and read */ }
            // else if (uart_id == UART_ID_1) { /* Wait for software UART reception */ }
            return -1; // Indicate no data currently available
        }
    }
    return -1; // Invalid UART ID
}

bool hal_uart_data_available(uart_id_t uart_id) {
    ring_buffer_t* rb = get_rx_rb(uart_id);
    // Check if the receive ring buffer for the specified UART has data.
    ring_buffer_t* rb = get_rx_rb(uart_id);
    if (rb) {
        return !ring_buffer_is_empty(rb);
    }
    // Alternatively, could check hardware status flags directly if not using buffers.
    return false;
}

size_t hal_uart_read(uart_id_t uart_id, uint8_t *buffer, size_t length) {
    ring_buffer_t* rb = get_rx_rb(uart_id);
    size_t bytes_read = 0;
    if (rb) {
        while (bytes_read < length && ring_buffer_read(rb, &buffer[bytes_read])) {
            bytes_read++;
        }
    }
    return bytes_read;
}

void hal_uart_enable_rx_interrupt(uart_id_t uart_id, uart_rx_callback_t callback) {
    // Enable the receive interrupt for the specified UART and register the callback.
    log_debug("UART: Enable RX Int ID %d", uart_id);
    if (uart_id == UART_ID_0) {
        hw_uart0_rx_callback = callback;
        // Configure hardware registers to enable RX interrupt.
    } else if (uart_id == UART_ID_1) {
        sw_uart1_rx_callback = callback;
        // Configure pin change interrupts or timers for software UART RX.
    }
    // Ensure global interrupts are enabled elsewhere (e.g., sei()).
}

void hal_uart_disable_rx_interrupt(uart_id_t uart_id) {
    // Disable the receive interrupt for the specified UART.
    log_debug("UART: Disable RX Int ID %d", uart_id);
    if (uart_id == UART_ID_0) {
        hw_uart0_rx_callback = NULL;
        // Configure hardware registers to disable RX interrupt.
    } else if (uart_id == UART_ID_1) {
        sw_uart1_rx_callback = NULL;
        // Disable pin change interrupts or timers for software UART RX.
    }
}

void hal_uart_flush_rx_buffer(uart_id_t uart_id) {
    // Discard any unread data in the receive buffer.
    ring_buffer_t* rb = get_rx_rb(uart_id);
    if (rb) {
        ring_buffer_clear(rb);
    }
    // May also need to clear hardware flags or read dummy data from the
    // hardware data register to ensure the interrupt flag is cleared.
}

// Interrupt Service Routines (ISRs) for UART RX/TX would typically reside here.
// They would read/write data from/to the hardware and interact with the
// ring buffers and callbacks. Example:
// ISR(USART_RX_vect) { /* Read UDR0 -> write to hw_uart0_rx_rb -> call hw_uart0_rx_callback */ }
// ISR(USART_UDRE_vect) { /* Read from hw_uart0_tx_rb -> write to UDR0 -> disable UDRIE if buffer empty */ }
