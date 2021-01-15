/**
 * @file memory_8080.c
 * @author Pranay Garg (pranayga@andrew.cmu.edu)
 * @brief Memory wrapper codebase
 * @version 0.1
 * @date 2020-12-24
 * 
 */
#include <stdlib.h>
#include "memory_8080.h"

void* mem_ref(v_memory* mem, uint16_t offset){
    return (void *)((uintptr_t)mem->base | offset);
}

uint8_t mem_read(v_memory* mem, uint16_t offset){
    uint8_t *target_addr = (uint8_t *)mem_ref(mem, offset);
    return *target_addr;
}

uint16_t short_mem_read(v_memory* mem, uint16_t offset){
    uint16_t *target_addr = (uint16_t *)mem_ref(mem, offset);
    return *target_addr;
}

void mem_write(v_memory* mem, uint16_t offset, uint8_t val){
    uint8_t *target_addr = (uint8_t *)mem_ref(mem, offset);
    *target_addr = val;
}

void short_mem_write(v_memory* mem, uint16_t offset, uint16_t val){
    uint16_t *target_addr = (uint16_t *)mem_ref(mem, offset);
    *target_addr = val;
}