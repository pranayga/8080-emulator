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
 * @brief wrapping for all the CPU opcodes for emulation
 * and easy calling.
 */
typedef  int (* OP_WRAP)(cpu_state* cpu, uint16_t base_PC, uint8_t op_code);

/**
 * @brief a struct to define the meta data for intel 8080 opcode.
 * Contains the binding to the functor and the cycles it takes to complete.
 */
typedef struct {
    OP_WRAP target_func;    /**< Functor to the handling function */
    uint8_t cycle_count;    /**< number of clock cycles it takes to complete */
} instt_8080_op;

/**
 * @brief Undefined OPCODE Functor 
 * 
 * @param cpu Cpu State
 * @param base_PC pointer to the location of the instruction start
 * @param op_code of the instruction under execution
 * @return int 1 of success, 0 if fail
 */
int UNDEFINED_OP_WRAP(UNUSED cpu_state* cpu, uint16_t base_PC, uint8_t op_code){
    printf("%x: (%x)This function has not been initialized.\n", base_PC, op_code);
    exit(-2);
    return 1;
}

/**
 * @brief Wrapper over No Op
 * 
 * @param cpu Cpu state
 * @param base_PC pointer to the location of the instruction start
 * @param op_code of the instruction under execution
 * @return int 1 of success, 0 if fail
 */
int NOP_WRAP(UNUSED cpu_state* cpu, uint16_t base_PC, UNUSED uint8_t op_code){
    printf("%x : NOP\n", base_PC);
    return 1;
}

instt_8080_op opcode_lookup[0x100] = {
    {NOP_WRAP, 1},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
    {UNDEFINED_OP_WRAP, 255},
};

#endif