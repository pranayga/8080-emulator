#ifndef MEMORY_8080_H
#define MEMORY_8080_H

#include <inttypes.h>

/**
 * @brief Memory wrapper
 * 
 */
typedef struct {
    void* base;
} v_memory;

/**
 * @brief Lowest level memory read access abstraction. Typecasts
 * the offset to void* + base to get the actual pointer. Populates the
 * value into val.
 * 
 * @param offset from the base ptr in bytes
 * @param mem cpu's memeory context under exec
 * @return uint8_t, byte read from the memory
 */
uint8_t mem_read(v_memory* mem, uint16_t offset);

/**
 * @brief Lowest level memory write access abstraction. Typecasts
 * the offset to void* + base to get the actual pointer. Populates from
 * val into the memory.
 * 
 * @param offset from the base ptr in bytes
 * @param val to write onto memory
 * @param mem cpu's memeory context under exec
 */
void mem_write(v_memory* mem, uint16_t offset, uint8_t val);

#endif