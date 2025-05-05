/**
 * @file spi.c
 * @brief SPI HAL implementation for ATmega328P (Display Module).
 */

#include "hal/spi.h"
#include "util/logger.h"
#include <stddef.h> // For NULL

// --- Public API Implementation ---

void hal_spi_init(spi_id_t spi_id, uint32_t clock_speed, uint8_t mode, uint8_t bit_order) {
    if (spi_id != SPI_ID_0) {
        log_error("SPI: Invalid ID %d for init", spi_id);
        return;
    }
    log_info("SPI: Init ID %d, Speed %lu Hz, Mode %d, Order %d", spi_id, clock_speed, mode, bit_order);

    // Configure GPIO pins for MOSI, SCK, SS as outputs. MISO as input.
    // (Handled by specific driver using the SPI, e.g., LCD driver sets CS)
    // hal_gpio_init(LCD_MOSI_PIN, GPIO_MODE_OUTPUT_PP);
    // hal_gpio_init(LCD_SCK_PIN, GPIO_MODE_OUTPUT_PP);
    // hal_gpio_init(LCD_MISO_PIN, GPIO_MODE_INPUT); // MISO is input

    // Configure SPI hardware registers (SPCR, SPSR).
    // Set clock rate (SPR0, SPR1, SPI2X bits).
    // Set SPI mode (CPOL, CPHA bits).
    // Set bit order (DORD bit).
    // Enable SPI in master mode (SPE=1, MSTR=1).
}

uint8_t hal_spi_transfer_byte(spi_id_t spi_id, uint8_t data) {
    if (spi_id != SPI_ID_0) return 0;

    // Load data into SPI data register (SPDR).
    // SPDR = data;

    // Wait for transmission complete flag (SPIF in SPSR).
    // while (!(SPSR & (1 << SPIF)));

    // Read received data from SPDR.
    // uint8_t received_data = SPDR;
    uint8_t received_data = data ^ 0xFF; // Simulate received data

    log_debug("SPI: Transfer Byte: Sent=0x%02X, Rcvd=0x%02X", data, received_data);
    return received_data;
}

void hal_spi_transfer_multi(spi_id_t spi_id, const uint8_t *tx_buffer, uint8_t *rx_buffer, size_t length) {
    if (spi_id != SPI_ID_0) return;

    for (size_t i = 0; i < length; ++i) {
        uint8_t byte_to_send = tx_buffer ? tx_buffer[i] : 0xFF; // Send 0xFF if no tx_buffer
        uint8_t received_byte = hal_spi_transfer_byte(spi_id, byte_to_send);
        if (rx_buffer) {
            rx_buffer[i] = received_byte;
        }
    }
}

void hal_spi_write_multi(spi_id_t spi_id, const uint8_t *tx_buffer, size_t length) {
    // Call transfer multi, discarding received data.
    hal_spi_transfer_multi(spi_id, tx_buffer, NULL, length);
}

void hal_spi_read_multi(spi_id_t spi_id, uint8_t *rx_buffer, size_t length) {
    // Call transfer multi, sending dummy bytes.
    hal_spi_transfer_multi(spi_id, NULL, rx_buffer, length);
}
