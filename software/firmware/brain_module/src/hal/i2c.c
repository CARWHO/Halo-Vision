/**
 * @file i2c.c
 * @brief I2C (TWI) HAL implementation for ATmega328P.
 * Provides blocking master mode functions for I2C communication.
 */

#include "hal/i2c.h"
#include "util/logger.h"
#include <util/delay.h> // For timeouts

// --- Defines ---
#define I2C_TIMEOUT_MS 10 // Timeout for I2C operations

// --- Internal Helper Functions ---

// Waits for the current I2C operation to complete, with a timeout mechanism.
static bool i2c_wait_for_complete(void) {
    // Simulate waiting for hardware completion.
    _delay_us(50); // Simulate hardware delay
    // In a real implementation, check hardware status flags and handle timeouts.
    return true; // Assume success for placeholder
}

// Initiates an I2C START condition on the bus.
static i2c_status_t i2c_start(void) {
    // Send START condition command to hardware.
    if (!i2c_wait_for_complete()) return I2C_ERROR_TIMEOUT;
    // Check hardware status to confirm START was sent successfully.
    log_debug("I2C: START sent");
    return I2C_OK; // Assume success
}

// Initiates an I2C STOP condition on the bus.
static void i2c_stop(void) {
    // Send STOP condition command to hardware.
    // No status check typically needed, but ensure command is sent.
    log_debug("I2C: STOP sent");
}

// Sends the slave address byte (with R/W bit) onto the bus.
static i2c_status_t i2c_send_address(uint8_t address_rw) {
    // Load address into hardware data register and initiate transmission.
    if (!i2c_wait_for_complete()) return I2C_ERROR_TIMEOUT;
    // Check hardware status for ACK/NACK from the slave.
    log_debug("I2C: Address 0x%02X sent", address_rw);
    // Return appropriate status (I2C_OK, I2C_ERROR_SLA_W_NACK, I2C_ERROR_SLA_R_NACK).
    return I2C_OK; // Assume ACK received
}

// Sends a single data byte onto the bus.
static i2c_status_t i2c_send_data(uint8_t data) {
    // Load data into hardware data register and initiate transmission.
    if (!i2c_wait_for_complete()) return I2C_ERROR_TIMEOUT;
    // Check hardware status for ACK/NACK from the slave.
    log_debug("I2C: Data 0x%02X sent", data);
    // Return appropriate status (I2C_OK, I2C_ERROR_DATA_TX_NACK).
    return I2C_OK; // Assume ACK received
}

// Receives a data byte from the bus and sends an ACK.
static i2c_status_t i2c_receive_data_ack(uint8_t *data) {
    // Configure hardware to receive data and send ACK afterwards.
    if (!i2c_wait_for_complete()) return I2C_ERROR_TIMEOUT;
    // Check hardware status to confirm data received successfully.
    // Read data from hardware data register.
    *data = 0xAB; // Provide dummy data
    log_debug("I2C: Data 0x%02X received (ACK)", *data);
    return I2C_OK; // Assume success
}

// Receives a data byte from the bus and sends a NACK (used for the last byte).
static i2c_status_t i2c_receive_data_nack(uint8_t *data) {
    // Configure hardware to receive data and send NACK afterwards.
    if (!i2c_wait_for_complete()) return I2C_ERROR_TIMEOUT;
    // Check hardware status to confirm data received successfully.
    // Read data from hardware data register.
    *data = 0xCD; // Provide dummy data
    log_debug("I2C: Data 0x%02X received (NACK)", *data);
    return I2C_OK; // Assume success
}


// --- Public API Implementation ---

void hal_i2c_init(i2c_id_t i2c_id, uint32_t clock_speed) {
    if (i2c_id != I2C_ID_0) {
        log_error("I2C: Invalid ID %d for init", i2c_id);
        return;
    }
    // Configure the I2C hardware peripheral (e.g., TWI on ATmega).
    log_info("I2C: Init ID %d, Speed %lu Hz", i2c_id, clock_speed);
    // Set clock speed (e.g., configure TWBR register).
    // Enable the I2C peripheral.
}

i2c_status_t hal_i2c_write(i2c_id_t i2c_id, uint8_t device_address, const uint8_t *data, size_t length, bool stop) {
    if (i2c_id != I2C_ID_0) return I2C_ERROR_UNKNOWN;

    i2c_status_t status;

    // Initiate START condition.
    status = i2c_start();
    if (status != I2C_OK) {
        i2c_stop(); // Attempt to clean up bus on error.
        return status;
    }

    // Send slave address with Write bit.
    status = i2c_send_address((device_address << 1) | 0);
    if (status != I2C_OK) {
        i2c_stop();
        return status;
    }

    // Send data bytes sequentially.
    for (size_t i = 0; i < length; ++i) {
        status = i2c_send_data(data[i]);
        if (status != I2C_OK) {
            i2c_stop();
            return status;
        }
    }

    // Send STOP condition if requested.
    if (stop) {
        i2c_stop();
    }

    return I2C_OK;
}

i2c_status_t hal_i2c_read(i2c_id_t i2c_id, uint8_t device_address, uint8_t *data, size_t length, bool stop) {
     if (i2c_id != I2C_ID_0 || length == 0) return I2C_ERROR_UNKNOWN;

    i2c_status_t status;

    // Initiate START condition.
    status = i2c_start();
    if (status != I2C_OK) {
        i2c_stop();
        return status;
    }

    // Send slave address with Read bit.
    status = i2c_send_address((device_address << 1) | 1);
    if (status != I2C_OK) {
        i2c_stop();
        return status;
    }

    // Receive data bytes sequentially.
    for (size_t i = 0; i < length; ++i) {
        bool is_last_byte = (i == length - 1);
        if (is_last_byte) {
            // Receive last byte and send NACK.
            status = i2c_receive_data_nack(&data[i]);
        } else {
            // Receive byte and send ACK.
            status = i2c_receive_data_ack(&data[i]);
        }
        if (status != I2C_OK) {
            i2c_stop();
            return status;
        }
    }

    // Send STOP condition if requested.
    if (stop) {
        i2c_stop();
    }

    return I2C_OK;
}

i2c_status_t hal_i2c_write_register(i2c_id_t i2c_id, uint8_t device_address, uint8_t reg_address, const uint8_t *data, size_t length) {
    if (i2c_id != I2C_ID_0) return I2C_ERROR_UNKNOWN;

    i2c_status_t status;

    // Initiate START condition.
    status = i2c_start();
    if (status != I2C_OK) { i2c_stop(); return status; }

    // Send slave address with Write bit.
    status = i2c_send_address((device_address << 1) | 0);
    if (status != I2C_OK) { i2c_stop(); return status; }

    // Send the register address to write to.
    status = i2c_send_data(reg_address);
    if (status != I2C_OK) { i2c_stop(); return status; }

    // Send the data bytes for the register.
    for (size_t i = 0; i < length; ++i) {
        status = i2c_send_data(data[i]);
        if (status != I2C_OK) { i2c_stop(); return status; }
    }

    // Send STOP condition.
    i2c_stop();

    return I2C_OK;
}

i2c_status_t hal_i2c_read_register(i2c_id_t i2c_id, uint8_t device_address, uint8_t reg_address, uint8_t *data, size_t length) {
    if (i2c_id != I2C_ID_0 || length == 0) return I2C_ERROR_UNKNOWN;

    i2c_status_t status;

    // --- Write Phase: Send register address ---
    status = i2c_start();
    if (status != I2C_OK) { i2c_stop(); return status; }

    status = i2c_send_address((device_address << 1) | 0);
    if (status != I2C_OK) { i2c_stop(); return status; }

    status = i2c_send_data(reg_address);
    if (status != I2C_OK) { i2c_stop(); return status; }

    // --- Read Phase ---
    // Send Repeated START condition.
    status = i2c_start();
    if (status != I2C_OK) { i2c_stop(); return status; }

    // Send slave address with Read bit.
    status = i2c_send_address((device_address << 1) | 1);
    if (status != I2C_OK) { i2c_stop(); return status; }

    // Receive data bytes.
    for (size_t i = 0; i < length; ++i) {
        bool is_last_byte = (i == length - 1);
        if (is_last_byte) {
            status = i2c_receive_data_nack(&data[i]); // NACK last byte
        } else {
            status = i2c_receive_data_ack(&data[i]); // ACK intermediate bytes
        }
        if (status != I2C_OK) { i2c_stop(); return status; }
    }

    // Send STOP condition.
    i2c_stop();

    return I2C_OK;
}

bool hal_i2c_probe(i2c_id_t i2c_id, uint8_t device_address) {
    if (i2c_id != I2C_ID_0) return false;

    i2c_status_t status;
    bool ack_received = false;

    // Send START.
    status = i2c_start();
    if (status == I2C_OK) {
        // Send address + Write bit and check for ACK/NACK.
        status = i2c_send_address((device_address << 1) | 0);
        if (status == I2C_OK) {
            ack_received = true; // Device acknowledged.
        }
    }
    // Always send STOP after probing.
    i2c_stop();

    log_debug("I2C: Probe Addr 0x%02X -> %s", device_address, ack_received ? "ACK" : "NACK/Error");
    return ack_received;
}

void hal_i2c_reset(i2c_id_t i2c_id) {
    if (i2c_id != I2C_ID_0) return;
    // Attempt to recover the I2C bus if it gets stuck.
    log_warn("I2C: Resetting TWI peripheral");
    // This might involve disabling/re-enabling the peripheral,
    // or manually toggling clock/data lines in some cases.
    // Re-initialize after reset attempt.
    hal_i2c_init(i2c_id, I2C_CLOCK_SPEED); // Re-run init
}
