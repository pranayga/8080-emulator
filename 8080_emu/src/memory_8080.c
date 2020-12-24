#include <stdlib.h>
#include "memory_8080.h"

uint8_t mem_read(v_memory* mem, uint16_t offset){
    uint8_t *target_addr = (uint8_t *)((uintptr_t)mem->base | offset);
    return *target_addr;
}

void mem_write(v_memory* mem, uint16_t offset, uint8_t val){
    uint8_t *target_addr = (uint8_t *)((uintptr_t)mem->base | offset);
    *target_addr = val;
}