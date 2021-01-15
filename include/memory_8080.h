/**
 * @file memory_8080.h
 * @author Pranay Garg (pranayga@andrew.cmu.edu)
 * @brief Memory abstraction for 8080 cpu.
 * @version 0.1
 * @date 2020-12-24
 * 
 */
#ifndef MEMORY_8080_H
#define MEMORY_8080_H

#include <inttypes.h>

/**
 * @brief Memory wrapper
 * 
 */
typedef struct {
    void* base; /**< Base virtual memory, 64KB aligned */
} v_memory;

/**
 * @brief plain reference to the memory location
 * 
 * @param offset from the base ptr in bytes
 * @param mem cpu's memeory context under exec
 * @return void*, memory reference to the target location
 */
void* mem_ref(v_memory* mem, uint16_t offset);

/**
 * @brief Lowest level memory read access abstraction. Typecasts
 * the offset to void* + base to get the actual pointer. Return the value
 * 
 * @param offset from the base ptr in bytes
 * @param mem cpu's memeory context under exec
 * @return uint8_t, byte read from the memory
 */
uint8_t mem_read(v_memory* mem, uint16_t offset);

/**
 * @brief Wrapper over memory access primitives to read a byte off
 * the base offset void* + base. Returns the value
 * 
 * @param offset from the base ptr in bytes
 * @param mem cpu's memeory context under exec
 * @return uint16_t, short read from the memory
 */
uint16_t short_mem_read(v_memory* mem, uint16_t offset);

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

/**
 * @brief Wrapper over memory access primitives to read a byte off
 * the base offset void* + base. Populates from
 * val into the memory.
 * 
 * @param offset from the base ptr in bytes
 * @param val to write onto memory
 * @param mem cpu's memeory context under exec
 */
void short_mem_write(v_memory* mem, uint16_t offset, uint16_t val);

#endif