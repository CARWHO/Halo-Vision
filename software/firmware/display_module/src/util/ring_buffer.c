/**
 * @file ring_buffer.c
 * @brief Generic ring buffer implementation (Display Module).
 */

#include "util/ring_buffer.h"
#include <string.h> // For memcpy, memset
#include <stddef.h> // For NULL

// --- Public API Implementation ---

void ring_buffer_init(ring_buffer_t *rb, uint8_t *buffer, rb_size_t size) {
    if (!rb || !buffer || size == 0) {
        return; // Invalid arguments
    }
    rb->buffer = buffer;
    rb->size = size;
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
}

bool ring_buffer_write(ring_buffer_t *rb, uint8_t data) {
    if (!rb || ring_buffer_is_full(rb)) {
        return false; // Buffer full or invalid
    }
    rb->buffer[rb->head] = data;
    rb->head = (rb->head + 1) % rb->size;
    rb->count++;
    return true;
}

bool ring_buffer_read(ring_buffer_t *rb, uint8_t *data) {
    if (!rb || !data || ring_buffer_is_empty(rb)) {
        return false; // Buffer empty or invalid arguments
    }
    *data = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) % rb->size;
    rb->count--;
    return true;
}

bool ring_buffer_peek(const ring_buffer_t *rb, uint8_t *data, rb_size_t offset) {
     if (!rb || !data || offset >= rb->count) {
        return false; // Invalid arguments or offset out of bounds
    }
    rb_size_t peek_index = (rb->tail + offset) % rb->size;
    *data = rb->buffer[peek_index];
    return true;
}

rb_size_t ring_buffer_bytes_available(const ring_buffer_t *rb) {
    return rb ? rb->count : 0;
}

rb_size_t ring_buffer_space_remaining(const ring_buffer_t *rb) {
    return rb ? (rb->size - rb->count) : 0;
}

bool ring_buffer_is_empty(const ring_buffer_t *rb) {
    return rb ? (rb->count == 0) : true;
}

bool ring_buffer_is_full(const ring_buffer_t *rb) {
    return rb ? (rb->count == rb->size) : true;
}

void ring_buffer_clear(ring_buffer_t *rb) {
    if (rb) {
        rb->head = 0;
        rb->tail = 0;
        rb->count = 0;
    }
}

rb_size_t ring_buffer_write_multi(ring_buffer_t *rb, const uint8_t *data, rb_size_t length) {
    if (!rb || !data || length == 0) return 0;
    rb_size_t bytes_written = 0;
    while (bytes_written < length && ring_buffer_write(rb, data[bytes_written])) {
        bytes_written++;
    }
    return bytes_written;
}

rb_size_t ring_buffer_read_multi(ring_buffer_t *rb, uint8_t *buffer, rb_size_t length) {
     if (!rb || !buffer || length == 0) return 0;
    rb_size_t bytes_read = 0;
    while (bytes_read < length && ring_buffer_read(rb, &buffer[bytes_read])) {
        bytes_read++;
    }
    return bytes_read;
}
