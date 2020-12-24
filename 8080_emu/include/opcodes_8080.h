#ifndef OPCODES_8080_H
#define OPCODES_8080_H

#include <inttypes.h>
#include "cpu_8080.h"
#include "memory_8080.h"

/**
 * @brief Enum of each and every opcode.
 */
typedef enum {
    nop_instt = 0x0,
} Instruction_OPCODE;

/**
 * @brief Wrapper over No Op
 * 
 * @param cpu Cpu state
 * @return int 1 of success, 0 if fail
 */
int NOP_WRAP(UNUSED cpu_state* cpu){
    DEBUG_PRINT("%x : NOP\n", nop_instt);
    return 1;
}

#endif