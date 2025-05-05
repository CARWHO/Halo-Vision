#ifndef HAL_SPI_H
#define HAL_SPI_H

/**
 * @file hal_spi.h
 * @brief Hardware Abstraction Layer for SPI communication (Display Module).
 */

#include <stdint.h>
#include <stddef.h> // For size_t
#include "config.h" // Include config for SPI IDs

// SPI Interface Identifiers (ATmega328P has one hardware SPI)
typedef enum {
    SPI_ID_0, // Hardware SPI
    SPI_ID_MAX
} spi_id_t;

/**
 * @brief Initializes the SPI peripheral in master mode.
 * @param spi_id The SPI peripheral identifier (must be SPI_ID_0).
 * @param clock_speed Desired SPI clock frequency (e.g., 4000000 for 4MHz).
 * @param mode SPI mode (0, 1, 2, or 3) defining clock polarity and phase.
 * @param bit_order 0 for MSB first, 1 for LSB first.
 */
void hal_spi_init(spi_id_t spi_id, uint32_t clock_speed, uint8_t mode, uint8_t bit_order);

/**
 * @brief Transfers (sends and receives) a single byte over SPI.
 * This is a blocking operation.
 * @param spi_id The SPI peripheral identifier.
 * @param data The byte to send.
 * @return The byte received from the slave device during the transfer.
 */
uint8_t hal_spi_transfer_byte(spi_id_t spi_id, uint8_t data);

/**
 * @brief Transfers multiple bytes over SPI.
 * Sends data from tx_buffer and stores received data into rx_buffer.
 * tx_buffer and rx_buffer can be the same for loopback or overwrite.
 * If rx_buffer is NULL, received data is discarded.
 * If tx_buffer is NULL, sends dummy bytes (e.g., 0xFF).
 * @param spi_id The SPI peripheral identifier.
 * @param tx_buffer Pointer to the buffer containing data to send (or NULL).
 * @param rx_buffer Pointer to the buffer to store received data (or NULL).
 * @param length Number of bytes to transfer.
 */
void hal_spi_transfer_multi(spi_id_t spi_id, const uint8_t *tx_buffer, uint8_t *rx_buffer, size_t length);

/**
 * @brief Sends multiple bytes over SPI, discarding received data.
 * Convenience function for write-only operations.
 * @param spi_id The SPI peripheral identifier.
 * @param tx_buffer Pointer to the buffer containing data to send.
 * @param length Number of bytes to send.
 */
void hal_spi_write_multi(spi_id_t spi_id, const uint8_t *tx_buffer, size_t length);

/**
 * @brief Receives multiple bytes over SPI, sending dummy bytes (0xFF).
 * Convenience function for read-only operations.
 * @param spi_id The SPI peripheral identifier.
 * @param rx_buffer Pointer to the buffer to store received data.
 * @param length Number of bytes to receive.
 */
void hal_spi_read_multi(spi_id_t spi_id, uint8_t *rx_buffer, size_t length);

#endif // HAL_SPI_H
