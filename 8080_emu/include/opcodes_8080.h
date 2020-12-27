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

#define ILLEGAL_OP DEBUG_PRINT("%s\n", "Illegal OP!");

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
    uint8_t size;           /**< Instruction size in bytes */
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
    DECOMPILE_PRINT(base_PC, "(%x)This Opcode has not been initialized.\n", op_code);
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
    DECOMPILE_PRINT(base_PC, "%s\n", "NOP");
    return 1;
}

int LXI_WRAP(UNUSED cpu_state* cpu, uint16_t base_PC, uint8_t op_code){
    // Xfil the short Reg.
    switch (op_code)
    {
    case 0x01:
        cpu->BC = short_mem_read(&cpu->mem, base_PC+1);
        DECOMPILE_PRINT(base_PC, "%s\n", "LXI BC");    
        break;
    case 0x11:
        cpu->DE = short_mem_read(&cpu->mem, base_PC+1);
        DECOMPILE_PRINT(base_PC, "%s\n", "LXI DE");
        break;
    case 0x21:
        cpu->HL = short_mem_read(&cpu->mem, base_PC+1);
        DECOMPILE_PRINT(base_PC, "%s\n", "LXI HL");
        break;
    case 0x31:
        cpu->SP = short_mem_read(&cpu->mem, base_PC+1);
        DECOMPILE_PRINT(base_PC, "%s\n", "LXI SP");
        break;
    default:
        ILLEGAL_OP;
        exit(-2);
    }
    return 1;
}

instt_8080_op opcode_lookup[0x100] = {
    [0x0]  = {.target_func = NOP_WRAP, .cycle_count = 4, .size = 1},    // NOP Instruction
    [0x1]  = {LXI_WRAP, 10, 3},
    // [0x2]  = {STAX_WRAP, 7, 1},
    // [0x3]  = {INX_WRAP, 5, 1},
    // [0x4]  = {INR_WRAP, 5, 1},
    // [0x5]  = {DCR_WRAP, 5, 1},
    // [0x6]  = {MVI_WRAP, 7, 2},
    // [0x7]  = {RLC_WRAP, 7, 1},
    [0x8]  = {NOP_WRAP, 4, 1},
    // [0x9]  = {DAD_WRAP, 10, 1},
    // [0xA]  = {LDAX_WRAP, 7, 1},
    // [0xB]  = {DCX_WRAP, 5, 1},
    // [0xC]  = {INR_WRAP, 5, 1},
    // [0xD]  = {DCR_WRAP, 5, 1},
    // [0xE]  = {MVI_WRAP, 7, 2},
    // [0x10] = {RRC_WRAP, 4, 1},
    [0x11] = {UNDEFINED_OP_WRAP, 255, 1},
    [0x12] = {UNDEFINED_OP_WRAP, 255, 1},
    [0x13] = {UNDEFINED_OP_WRAP, 255, 1},
};

#endif