#ifndef HAL_I2C_H
#define HAL_I2C_H

/**
 * @file hal_i2c.h
 * @brief Hardware Abstraction Layer for I2C (TWI) communication on ATmega328P.
 */

#include <stdint.h>
#include <stddef.h> // For size_t
#include <stdbool.h>
#include "config.h" // Include config for I2C IDs

// I2C Interface Identifiers (ATmega328P has one TWI module)
typedef enum {
    I2C_ID_0, // Hardware TWI
    I2C_ID_MAX
} i2c_id_t;

// I2C Status Codes (based on ATmega TWI status codes)
typedef enum {
    I2C_OK = 0,
    I2C_ERROR_START_FAILED,
    I2C_ERROR_REPEATED_START_FAILED,
    I2C_ERROR_SLA_W_NACK, // Slave Address + Write NACKed
    I2C_ERROR_SLA_R_NACK, // Slave Address + Read NACKed
    I2C_ERROR_DATA_TX_NACK,
    I2C_ERROR_DATA_RX_NACK, // Should not happen in master mode usually
    I2C_ERROR_ARB_LOST,     // Arbitration Lost
    I2C_ERROR_TIMEOUT,
    I2C_ERROR_BUS_BUSY,
    I2C_ERROR_UNKNOWN
} i2c_status_t;

/**
 * @brief Initializes the I2C peripheral (Hardware TWI).
 * Configures the clock speed and enables the TWI module.
 * @param i2c_id The I2C peripheral identifier (must be I2C_ID_0).
 * @param clock_speed The desired I2C clock speed (e.g., 100000 for 100kHz SCL).
 */
void hal_i2c_init(i2c_id_t i2c_id, uint32_t clock_speed);

/**
 * @brief Writes data to an I2C slave device.
 * Handles START, slave address + W, data bytes, and STOP conditions.
 * This is a blocking implementation.
 * @param i2c_id The I2C peripheral identifier.
 * @param device_address The 7-bit address of the slave device.
 * @param data Pointer to the data buffer to write.
 * @param length Number of bytes to write.
 * @param stop True to generate a STOP condition after writing, false to keep bus active (for repeated START).
 * @return I2C_OK on success, or an i2c_status_t error code on failure.
 */
i2c_status_t hal_i2c_write(i2c_id_t i2c_id, uint8_t device_address, const uint8_t *data, size_t length, bool stop);

/**
 * @brief Reads data from an I2C slave device.
 * Handles START, slave address + R, data bytes (with ACK/NACK), and STOP conditions.
 * This is a blocking implementation.
 * @param i2c_id The I2C peripheral identifier.
 * @param device_address The 7-bit address of the slave device.
 * @param data Pointer to the buffer where read data will be stored.
 * @param length Number of bytes to read.
 * @param stop True to generate a STOP condition after reading, false to keep bus active.
 * @return I2C_OK on success, or an i2c_status_t error code on failure.
 */
i2c_status_t hal_i2c_read(i2c_id_t i2c_id, uint8_t device_address, uint8_t *data, size_t length, bool stop);

/**
 * @brief Writes data to a specific register of an I2C device.
 * Combines a write operation (for the register address) followed by another write (for the data).
 * Uses a repeated START condition if the device supports it.
 * @param i2c_id The I2C peripheral identifier.
 * @param device_address The 7-bit address of the slave device.
 * @param reg_address The address of the register to write to.
 * @param data Pointer to the data buffer containing the value(s) to write.
 * @param length Number of bytes to write to the register.
 * @return I2C_OK on success, or an i2c_status_t error code on failure.
 */
i2c_status_t hal_i2c_write_register(i2c_id_t i2c_id, uint8_t device_address, uint8_t reg_address, const uint8_t *data, size_t length);

/**
 * @brief Reads data from a specific register of an I2C device.
 * Performs a write operation (for the register address) followed by a repeated START and a read operation.
 * @param i2c_id The I2C peripheral identifier.
 * @param device_address The 7-bit address of the slave device.
 * @param reg_address The address of the register to read from.
 * @param data Pointer to the buffer where the read data will be stored.
 * @param length Number of bytes to read from the register.
 * @return I2C_OK on success, or an i2c_status_t error code on failure.
 */
i2c_status_t hal_i2c_read_register(i2c_id_t i2c_id, uint8_t device_address, uint8_t reg_address, uint8_t *data, size_t length);

/**
 * @brief Checks if an I2C device acknowledges its address on the bus.
 * Sends a START condition and the slave address, then checks for ACK/NACK. Sends STOP.
 * @param i2c_id The I2C peripheral identifier.
 * @param device_address The 7-bit address of the slave device to probe.
 * @return true if the device acknowledges (ACK received), false otherwise (NACK or error).
 */
bool hal_i2c_probe(i2c_id_t i2c_id, uint8_t device_address);

/**
 * @brief Resets the I2C peripheral in case of bus lock-up.
 * Attempts to force STOP/START conditions or re-initializes the hardware.
 * @param i2c_id The I2C peripheral identifier.
 */
void hal_i2c_reset(i2c_id_t i2c_id);

#endif // HAL_I2C_H
