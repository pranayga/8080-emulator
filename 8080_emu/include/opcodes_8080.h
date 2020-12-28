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
 * @brief The bit pattern designating one of the registers 
 * A,B,C,D,E,H,L (DDD=destination, SSS=source)
 * 111 -> ACC,  000---101 -> B,C,D,E,H,L.
 * This function returns a reference to the bytes which helps
 * simulate the register
 * @param cpu 
 * @param reg_identifier 
 * @return uint8_t* (uint8_t*)-1 if illegal or address otherwise 
 */
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
        return (uint8_t*)-1;
    }
}

/**
 * @brief The bit pattern designating one of the 
 * register pairs B,D,H,SP
 * 00---11 -> B,D,H,SP.
 * This function returns a reference to the short which helps
 * simulate the register
 * @param cpu 
 * @param reg_identifier 
 * @return uint16_t* (uint16_t*)-1 if illegal or address otherwise
 */
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
        return (uint16_t*)-1;
    }
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
    DECOMPILE_PRINT(base_PC, "LXI REGP(%x), %x\n", reg_patt, imm_data);
    return 1;
}

/**
 * @brief (PC) (byte 3) (byte 2) -- 
 * Control is transferred to the instruction whose address is specified in byte 3 and byte 2 of the current
 * instruction.
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
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

/**
 * @brief The current PC (pointer to next instruction) is move to the top
 * of SP and the SP is decremented to make space for the pointer. Next, the
 * new start of function address is moved to PC.
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
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

/**
 * @brief Load accumulator indirect: The content of the memory location, whose address
 * is in the register pair rp, is moved to register A
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 * @note only register pairs rp=B (registers B and C·) or rp=D
 * (registers D and E) may be specified
 */
int LDAX_WRAP(cpu_state* cpu, uint16_t base_PC, uint8_t op_code){
    uint8_t reg_patt = (0x30 & op_code) >> 4;
    if(reg_patt < 2){
        uint16_t* target_dest = ref_short_reg(cpu, reg_patt);
        cpu->ACC = mem_read(&cpu->mem, *target_dest);
        DECOMPILE_PRINT(base_PC, "LDAX REGP(%x), %x\n", reg_patt, cpu->ACC);
    } else {
        ILLEGAL_OP;
        exit(-2);
    }
    return 1;
}

/**
 * @brief All different kinds of move operations:
 * - Move Register
 * - Move from Memory (*HL -> Reg)
 * - Move to memory (Reg -> *HL)
 * The immediate versions are in the `MVI_WRAP` function
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int MOV_WRAP(cpu_state* cpu, uint16_t base_PC, uint8_t op_code){
    uint8_t dest_reg_patt = (0x38 & op_code) >> 3;
    uint8_t src_reg_patt = (0x07 & op_code);
    uint8_t *dest_reg = ref_byte_reg(cpu, dest_reg_patt);
    uint8_t *src_reg = ref_byte_reg(cpu, src_reg_patt);
    if(dest_reg != (uint8_t*)-1 && src_reg != (uint8_t*)-1){
        *dest_reg = *src_reg;
        DECOMPILE_PRINT(base_PC, "MOV REGDest(%x), REGSrc(%x)\n", dest_reg_patt, src_reg_patt);
    } else if (dest_reg != (uint8_t*)-1 && src_reg == (uint8_t*)-1){
        *dest_reg = mem_read(&cpu->mem, cpu->HL);
        DECOMPILE_PRINT(base_PC, "MOV REGDest(%x), M\n", dest_reg_patt);
    } else if (dest_reg == (uint8_t*)-1 && src_reg != (uint8_t*)-1){
        mem_write(&cpu->mem, cpu->HL, *src_reg);
        DECOMPILE_PRINT(base_PC, "MOV M, REGSrc(%x)\n", src_reg_patt);
    } else {
        ILLEGAL_OP;
        exit(-2);
    }
    return 1;
}

/**
 * @brief The processor is stopped. The registers and flags are
 * unaffected.
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int HLT_WRAP(UNUSED cpu_state* cpu, UNUSED uint16_t base_PC, UNUSED uint8_t op_code){
    while (1) {}
    return -1;
}

/**
 * @brief (Increment register pair) (rh) (rl) <-- (rh) (rl) + 1
 * The content of the register pair rp is incremented by one. 
 * @note No condition flags are affected.
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int INX_WRAP(cpu_state* cpu, uint16_t base_PC, uint8_t op_code){
    if((op_code & 0xC3) == 0x03){
        uint8_t reg_patt = (0x30 & op_code) >> 4;
        uint16_t* target_dest = ref_short_reg(cpu, reg_patt);
        *target_dest += 1;
        DECOMPILE_PRINT(base_PC, "INX REGP(%x)\n", reg_patt);
    }else {
        ILLEGAL_OP;
        exit(-2);
    }
    return 1;
}

/**
 * @brief (Decrement Register) The content of register r is decremented by one.
 * @note All condition flag except CY are affected.
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int DCR_WRAP(cpu_state* cpu, uint16_t base_PC, uint8_t op_code){
    if(op_code == 0x35){    // Type B
        uint8_t target_data = mem_read(&cpu->mem, cpu->HL);
        // Update The data
        target_data--;
        mem_write(&cpu->mem, cpu->HL, target_data);
        // Update Flags

        DECOMPILE_PRINT(base_PC, "%s\n", "DCR M");
    } else if ( (op_code & 0xC7) == 0x05 ){ // Type A
        uint8_t target_reg = (op_code & 0x38) >> 3;
        uint8_t *target_data = ref_byte_reg(cpu, target_reg);
        // Update The data
        *target_data -= 1;
        // Update Flags

        DECOMPILE_PRINT(base_PC, "DCR REG(%x)\n", target_reg);
    } else {
        ILLEGAL_OP;
        exit(-2);
    }
    return 1;
}

instt_8080_op opcode_lookup[0x100] = {
    [0x00] = {.target_func = NOP_WRAP, .cycle_count = 4, .size = 1},    // NOP Instruction
    [0x01] = {LXI_WRAP, 10, 3},
    // [0x2]  = {STAX_WRAP, 7, 1},
    [0x03] = {INX_WRAP, 5, 1},
    // [0x4]  = {INR_WRAP, 5, 1},
    // [0x5]  = {DCR_WRAP, 5, 1},
    [0x06] = {MVI_WRAP, 7, 2},
    // [0x7]  = {RLC_WRAP, 7, 1},
    [0x08] = {NOP_WRAP, 4, 1},
    // [0x9]  = {DAD_WRAP, 10, 1},
    [0x0A] = {LDAX_WRAP, 7, 1},
    // [0xB]  = {DCX_WRAP, 5, 1},
    // [0xC]  = {INR_WRAP, 5, 1},
    // [0xD]  = {DCR_WRAP, 5, 1},
    [0x0E] = {MVI_WRAP, 7, 2},
    // [0x10] = {RRC_WRAP, 4, 1},
    [0x11] = {LXI_WRAP, 10, 3},
    [0x13] = {INX_WRAP, 5, 1},
    [0x16] = {MVI_WRAP, 7, 2},
    [0x1A] = {LDAX_WRAP, 7, 1},
    [0x1E] = {MVI_WRAP, 7, 2},
    [0x21] = {LXI_WRAP, 10, 3},
    [0x23] = {INX_WRAP, 5, 1},
    [0x26] = {MVI_WRAP, 7, 2},
    [0x2E] = {MVI_WRAP, 7, 2},
    [0x31] = {LXI_WRAP, 10, 3},
    [0x33] = {INX_WRAP, 5, 1},
    [0x36] = {MVI_WRAP, 10, 2},
    [0x3E] = {MVI_WRAP, 7, 2},
    [0x40] = {MOV_WRAP, 5, 1},
    [0x41] = {MOV_WRAP, 5, 1},
    [0x42] = {MOV_WRAP, 5, 1},
    [0x43] = {MOV_WRAP, 5, 1},
    [0x44] = {MOV_WRAP, 5, 1},
    [0x45] = {MOV_WRAP, 5, 1},
    [0x46] = {MOV_WRAP, 5, 1},
    [0x47] = {MOV_WRAP, 5, 1},
    [0x48] = {MOV_WRAP, 5, 1},
    [0x49] = {MOV_WRAP, 5, 1},
    [0x4A] = {MOV_WRAP, 5, 1},
    [0x4B] = {MOV_WRAP, 5, 1},
    [0x4C] = {MOV_WRAP, 5, 1},
    [0x4D] = {MOV_WRAP, 5, 1},
    [0x4E] = {MOV_WRAP, 5, 1},
    [0x4F] = {MOV_WRAP, 5, 1},
    [0x50] = {MOV_WRAP, 5, 1},
    [0x51] = {MOV_WRAP, 5, 1},
    [0x52] = {MOV_WRAP, 5, 1},
    [0x53] = {MOV_WRAP, 5, 1},
    [0x54] = {MOV_WRAP, 5, 1},
    [0x55] = {MOV_WRAP, 5, 1},
    [0x56] = {MOV_WRAP, 5, 1},
    [0x57] = {MOV_WRAP, 5, 1},
    [0x58] = {MOV_WRAP, 5, 1},
    [0x59] = {MOV_WRAP, 5, 1},
    [0x5A] = {MOV_WRAP, 5, 1},
    [0x5B] = {MOV_WRAP, 5, 1},
    [0x5C] = {MOV_WRAP, 5, 1},
    [0x5D] = {MOV_WRAP, 5, 1},
    [0x5E] = {MOV_WRAP, 5, 1},
    [0x5F] = {MOV_WRAP, 5, 1},
    [0x60] = {MOV_WRAP, 5, 1},
    [0x61] = {MOV_WRAP, 5, 1},
    [0x62] = {MOV_WRAP, 5, 1},
    [0x63] = {MOV_WRAP, 5, 1},
    [0x64] = {MOV_WRAP, 5, 1},
    [0x65] = {MOV_WRAP, 5, 1},
    [0x66] = {MOV_WRAP, 5, 1},
    [0x67] = {MOV_WRAP, 5, 1},
    [0x68] = {MOV_WRAP, 5, 1},
    [0x69] = {MOV_WRAP, 5, 1},
    [0x6A] = {MOV_WRAP, 5, 1},
    [0x6B] = {MOV_WRAP, 5, 1},
    [0x6C] = {MOV_WRAP, 5, 1},
    [0x6D] = {MOV_WRAP, 5, 1},
    [0x6E] = {MOV_WRAP, 5, 1},
    [0x6F] = {MOV_WRAP, 5, 1},
    [0x70] = {MOV_WRAP, 5, 1},
    [0x71] = {MOV_WRAP, 5, 1},
    [0x72] = {MOV_WRAP, 5, 1},
    [0x73] = {MOV_WRAP, 5, 1},
    [0x74] = {MOV_WRAP, 5, 1},
    [0x75] = {MOV_WRAP, 5, 1},
    [0x76] = {HLT_WRAP, 7, 1},
    [0x77] = {MOV_WRAP, 5, 1},
    [0x78] = {MOV_WRAP, 5, 1},
    [0x79] = {MOV_WRAP, 5, 1},
    [0x7A] = {MOV_WRAP, 5, 1},
    [0x7B] = {MOV_WRAP, 5, 1},
    [0x7C] = {MOV_WRAP, 5, 1},
    [0x7D] = {MOV_WRAP, 5, 1},
    [0x7E] = {MOV_WRAP, 5, 1},
    [0x7F] = {MOV_WRAP, 5, 1},
    [0xC3] = {JMP_WRAP, 10, 3},
    [0xCD] = {CALL_WRAP, 17, 3},
    [0xDD] = {CALL_WRAP, 17, 3},
    [0xED] = {CALL_WRAP, 17, 3},
    [0xFD] = {CALL_WRAP, 17, 3},
};

#endif