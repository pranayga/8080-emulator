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

uint8_t* ref_byte_reg(cpu_state* cpu ,uint8_t reg_identifier){
    switch (reg_identifier)
    {
    case 0x0:
        return &cpu->B;
    case 0x01:
        return &cpu->C;
    case 0x02:
        return &cpu->D;
    case 0x03:
        return &cpu->E;
    case 0x04:
        return &cpu->H;
    case 0x05:
        return &cpu->L;
    case 0x07:
        return &cpu->ACC;
    default:
        ILLEGAL_OP;
        exit(-2);
    }
    ILLEGAL_OP;
    return 0x0;
}

uint16_t* ref_short_reg(cpu_state* cpu ,uint8_t reg_identifier){
    switch (reg_identifier)
    {
    case 0x0:
        return &cpu->BC;
    case 0x01:
        return &cpu->DE;
    case 0x02:
        return &cpu->HL;
    case 0x03:
        return &cpu->SP;
    default:
        ILLEGAL_OP;
        exit(-2);
    }
    ILLEGAL_OP;
    return 0x0;
}

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

/**
 * @brief Load register pair immediate
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int LXI_WRAP(cpu_state* cpu, uint16_t base_PC, uint8_t op_code){
    uint8_t reg_patt = (0x30 & op_code) >> 4;
    uint16_t imm_data = short_mem_read(&cpu->mem, base_PC+1);
    uint16_t* target_dest = ref_short_reg(cpu, reg_patt);
    *target_dest = imm_data;
    DECOMPILE_PRINT(base_PC, "LXI REG(%x), %x\n", reg_patt, imm_data);
    return 1;
}

int JMP_WRAP(cpu_state* cpu, uint16_t base_PC, uint8_t op_code){
    switch (op_code)
    {
    case 0xC3:
        cpu->PC = short_mem_read(&cpu->mem, base_PC+1);
        DECOMPILE_PRINT(base_PC, "JMP %x\n", cpu->PC);
        break;
    case 0xCB:
        DEBUG_PRINT("%s\n", "UNINIMPLEMENTED.");
        ILLEGAL_OP;
        exit(-3);
    default:
        ILLEGAL_OP;
        exit(-2);
    }
    return 1;
}

/**
 * @brief Move Immediate
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int MVI_WRAP(cpu_state* cpu, uint16_t base_PC, uint8_t op_code){
    if(op_code == 0x36){    // Type B
        mem_write(&cpu->mem, cpu->HL, mem_read(&cpu->mem, base_PC+1));
        DECOMPILE_PRINT(base_PC, "MVI *(HL), %x\n", mem_read(&cpu->mem, base_PC+1));
    } else if ( (op_code & 0xC7) == 0x06 ){ // Type A
        uint8_t reg_patt = (0x38 & op_code) >> 3;
        uint8_t imm_data = mem_read(&cpu->mem, base_PC+1);
        uint8_t* target_dest = ref_byte_reg(cpu, reg_patt);
        *target_dest = imm_data;
        DECOMPILE_PRINT(base_PC, "MVI REG(%x), %x\n", reg_patt, imm_data);
    } else {
        ILLEGAL_OP;
        exit(-2);
    }
    return 1;
}

int CALL_WRAP(cpu_state* cpu, uint16_t base_PC, uint8_t op_code){
    switch (op_code)
    {
    case 0xCD:
        cpu->SP -= 2;
        short_mem_write(&cpu->mem, cpu->SP, cpu->PC);   // Saving Return Addr
        cpu->PC = short_mem_read(&cpu->mem, base_PC+1); // Reading the new PC
        break;
    default:
        // TODO: Few missing instruction not defined in the
        // system manual.
        ILLEGAL_OP;
        break;
    }
    return 1;
}

instt_8080_op opcode_lookup[0x100] = {
    [0x00] = {.target_func = NOP_WRAP, .cycle_count = 4, .size = 1},    // NOP Instruction
    [0x01] = {LXI_WRAP, 10, 3},
    // [0x2]  = {STAX_WRAP, 7, 1},
    // [0x3]  = {INX_WRAP, 5, 1},
    // [0x4]  = {INR_WRAP, 5, 1},
    // [0x5]  = {DCR_WRAP, 5, 1},
    [0x6]  = {MVI_WRAP, 7, 2},
    // [0x7]  = {RLC_WRAP, 7, 1},
    [0x8]  = {NOP_WRAP, 4, 1},
    // [0x9]  = {DAD_WRAP, 10, 1},
    // [0xA]  = {LDAX_WRAP, 7, 1},
    // [0xB]  = {DCX_WRAP, 5, 1},
    // [0xC]  = {INR_WRAP, 5, 1},
    // [0xD]  = {DCR_WRAP, 5, 1},
    [0xE]  = {MVI_WRAP, 7, 2},
    // [0x10] = {RRC_WRAP, 4, 1},
    [0x11] = {LXI_WRAP, 10, 3},
    [0x16] = {MVI_WRAP, 7, 2},
    [0x1E] = {MVI_WRAP, 7, 2},
    [0x21] = {LXI_WRAP, 10, 3},
    [0x26] = {MVI_WRAP, 7, 2},
    [0x2E] = {MVI_WRAP, 7, 2},
    [0x31] = {LXI_WRAP, 10, 3},
    [0x36] = {MVI_WRAP, 10, 2},
    [0x3E] = {MVI_WRAP, 7, 2},
    [0xC3] = {JMP_WRAP, 10, 3},
    [0xCD] = {CALL_WRAP, 17, 3},
    [0xDD] = {CALL_WRAP, 17, 3},
    [0xED] = {CALL_WRAP, 17, 3},
    [0xFD] = {CALL_WRAP, 17, 3},
};

#endif