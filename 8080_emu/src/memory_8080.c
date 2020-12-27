/**
 * @file memory_8080.c
 * @author Pranay Garg (pranayga@andrew.cmu.edu)
 * @brief Memory wrapper codebase
 * @version 0.1
 * @date 2020-12-24
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#include <stdlib.h>
#include "memory_8080.h"

uint8_t mem_read(v_memory* mem, uint16_t offset){
    uint8_t *target_addr = (uint8_t *)((uintptr_t)mem->base | offset);
    return *target_addr;
}

uint16_t short_mem_read(v_memory* mem, uint16_t offset){
    uint16_t *target_addr = (uint16_t *)((uintptr_t)mem->base | offset);
    return *target_addr;
}

void mem_write(v_memory* mem, uint16_t offset, uint8_t val){
    uint8_t *target_addr = (uint8_t *)((uintptr_t)mem->base | offset);
    *target_addr = val;
}

void short_mem_write(v_memory* mem, uint16_t offset, uint16_t val){
    uint16_t *target_addr = (uint16_t *)((uintptr_t)mem->base | offset);
    *target_addr = val;
}