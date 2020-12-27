/**
 * @file opcodes_8080.h
 * @author Pranay Garg (pranayga@andrew.cmu.edu)
 * @brief C wrappers for major opcodes. Broken from cpu_8080 to better manage codebase.
 * @version 0.1
 * @date 2020-12-24
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#ifndef OPCODES_8080_H
#define OPCODES_8080_H

#include <inttypes.h>
#include "cpu_8080.h"
#include "memory_8080.h"

/**
 * @brief Wrapper over No Op
 * 
 * @param cpu Cpu state
 * @return int 1 of success, 0 if fail
 */
int NOP_WRAP(UNUSED cpu_state* cpu){
    return 1;
}

#endif