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
    case 0x06:
        return (uint8_t *)mem_ref(&cpu->mem, cpu->HL);
    case 0x07:
        return &cpu->ACC;
    default:
        ILLEGAL_OP; // Will never reach here now.
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
 * @brief perform condition check on the cpu register state
 * 
 * @param cpu 
 * @param condition_identifier different condition checks for JMP, conditional OPS
 * @return uint8_t 1 if the condition is true, 0 if false
 */
uint8_t condition_check(cpu_state* cpu, condition_flags condition_identifier){
    switch (condition_identifier)
    {
    case NZ_check:
        if(!cpu->PSW.zero){
            return 1;
        }
        return 0;
    case Z_check:
        if(cpu->PSW.zero){
            return 1;
        }
        return 0;
    case NC_check:
        if(!cpu->PSW.carry){
            return 1;
        }
        return 0;
    case C_check:
        if(cpu->PSW.carry){
            return 1;
        }
        return 0;
    case PO_check:
        if(!cpu->PSW.parity){
            return 1;
        }
        return 0;
    case PE_check:
        if(cpu->PSW.parity){
            return 1;
        }
        return 0;
    case P_check:
        if(!cpu->PSW.sign){
            return 1;
        }
        return 0;
    case M_check:
        if(cpu->PSW.sign){
            return 1;
        }
        return 0;
    default:
        DEBUG_PRINT("%s\n", "Unknow Condition Code.");
        ILLEGAL_OP;
        exit(-2);
    }
}

/**
 * @brief Set the flags PSW status
 * 
 * @param cpu 
 * @param final_state the final value
 * @param flags to be set
 */
void set_flags(cpu_state* cpu ,uint32_t final_state, uint8_t flags){
    if(flags & SIGN_FLAG){
        cpu->PSW.sign = (final_state & 0x80) ? 1:0;
    } 
    if (flags & ZERO_FLAG){
        cpu->PSW.zero = (final_state & 0xFF) == 0 ? 1:0;
    }
    if (flags & AUX_FLAG){
        DEBUG_PRINT("%s\n", "AUX Flag is very specific to operation");
        ILLEGAL_OP;
    }
    if (flags & PARITY_FLAG){
        uint8_t pf = 1;
        // checking only for the fist 8 bytes
        uint8_t rr = final_state & 0xFF;
        // using xor to toggle the parity flag
        while(rr){
            pf ^= (rr&1);
            rr >>= 1;
        }
        cpu->PSW.parity = pf;
    }
    if (flags & CARRY_FLAG){
        cpu->PSW.carry = (final_state & 0x100) ? 1 : 0;
    }
    return;
}

/**
 * @brief sets the aux flag for all operations. Must convert all ops to a 
 * addition operation (compressed).
 * Does base_val + diff to recalc the result.
 * So for -ve bass -ve num's 2 complement. 
 * 
 * @todo not sure if this is correct.
 * @param cpu 
 * @param base_val the initial value
 * @param diff the value to be added to the base
 */
void aux_flag_set_add(cpu_state* cpu, uint32_t base_val, uint32_t diff){
    uint8_t xor = (base_val ^ diff) & 0x10; // basically if 0|1 -> 1 else 0
    uint8_t summ = (base_val + diff) & 0x10; // xor + propagate
    if( xor != summ ){
        cpu->PSW.aux = 1;
    } else {
        cpu->PSW.aux = 0;
    }
}

/**
 * @brief Compress program_status_word into a uint8_t as per
 * flag_bits
 * 
 * @param psw 
 * @return uint8_t 
 */
uint8_t compress_PSW(program_status_word psw){
    uint8_t status = 0;
    status |= psw.carry ? CARRY_FLAG : 0;
    status |= psw.aux ? AUX_FLAG : 0;
    status |= psw.sign ? SIGN_FLAG : 0;
    status |= psw.zero ? ZERO_FLAG : 0;
    status |= psw.parity ? PARITY_FLAG : 0;
    return status;
}

/**
 * @brief Inflate the uint8_t into a program_status_word by using
 * flag_bits as the mapping
 * 
 * @param status 
 * @return program_status_word 
 */
program_status_word decompress_PSW(uint8_t status){
    program_status_word new_status;
    new_status.carry = status & CARRY_FLAG ? 1 : 0;
    new_status.aux = status & AUX_FLAG ? 1 : 0;
    new_status.sign = status & SIGN_FLAG ? 1 : 0;
    new_status.zero = status & ZERO_FLAG ? 1 : 0;
    new_status.parity = status & PARITY_FLAG ? 1 : 0;
    return new_status;
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
    return -1;
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
    uint8_t reg_patt = (0x38 & op_code) >> 3;
    uint8_t imm_data = mem_read(&cpu->mem, base_PC+1);
    uint8_t* target_dest = ref_byte_reg(cpu, reg_patt);
    *target_dest = imm_data;
    DECOMPILE_PRINT(base_PC, "MVI REG(%x), %x\n", reg_patt, imm_data);
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
        short_mem_write(&cpu->mem, cpu->SP, cpu->PC);       // Saving Return Addr
        cpu->PC = short_mem_read(&cpu->mem, base_PC+1);     // Reading the new PC
        DECOMPILE_PRINT(base_PC, "CALL %x\n", cpu->PC );    // Logging
        break;
    default:
        // There was CALL at 0x[D-F]D which didn't have stuff
        // in the manual
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
    *dest_reg = *src_reg;
    DECOMPILE_PRINT(base_PC, "MOV REGDest(%x), REGSrc(%x)\n", dest_reg_patt, src_reg_patt);
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
int HLT_WRAP(UNUSED cpu_state* cpu, uint16_t base_PC, UNUSED uint8_t op_code){
    DECOMPILE_PRINT(base_PC, "%s\n", "HLT");
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
    uint8_t reg_patt = (0x30 & op_code) >> 4;
    uint16_t* target_dest = ref_short_reg(cpu, reg_patt);
    *target_dest += 1;
    DECOMPILE_PRINT(base_PC, "INX REGP(%x)\n", reg_patt);
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
    uint8_t target_reg = (op_code & 0x38) >> 3;
    uint16_t target_data = *(ref_byte_reg(cpu, target_reg));
    uint16_t base_data = target_data;
    // Update The data
    target_data -= 1;
    *(ref_byte_reg(cpu, target_reg)) = (uint8_t)target_data;
    // Update Flags
    set_flags(cpu, target_data, SIGN_FLAG | ZERO_FLAG | PARITY_FLAG );
    aux_flag_set_add(cpu, base_data, -1);
    DECOMPILE_PRINT(base_PC, "DCR REG(%x)\n", target_reg);
    return 1;
}

/**
 * @brief Conditional JMP statements
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int JCon_WRAP(cpu_state* cpu, uint16_t base_PC, uint8_t op_code){
    if(condition_check(cpu, (0x38 & op_code)>>3)){
        cpu->PC = short_mem_read(&cpu->mem, base_PC+1);
    }
    DECOMPILE_PRINT(base_PC, "JMP Con(%x) %x\n", (0x38 & op_code)>>3, 
        short_mem_read(&cpu->mem, base_PC+1));
    return 1;
}

/**
 * @brief (Return) 
 * (PCl) ((SP));
 * (PCH) ((SP) + 1);
 * (SP) (SP) + 2;
 * The content of the memory location whose address
 * is specified in register SP is moved to the low-order
 * eight bits of register PC.
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int RET_WRAP(cpu_state* cpu, uint16_t base_PC, uint8_t op_code){
    if(op_code == 0xC9){
        cpu->PC = short_mem_read(&cpu->mem, cpu->SP);
        cpu->SP += 2;
        DECOMPILE_PRINT(base_PC, "%s\n", "RET");
    } else {
        ILLEGAL_OP;
        exit(-2);
    }
    return 1;
}

/**
 * @brief Conditional version of `RET_WRAP`
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int RCon_WRAP(cpu_state* cpu, uint16_t base_PC, uint8_t op_code){
    DECOMPILE_PRINT(base_PC, "RET Cond(%x)\n", (op_code & 0x38) >> 3);
    if(condition_check(cpu, (op_code & 0x38) >> 3)){
        cpu->PC = short_mem_read(&cpu->mem, cpu->SP);
        cpu->SP += 2;
    }
    return 1;
}

/**
 * @brief Comparison operation
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int CMP_WRAP(cpu_state* cpu, uint16_t base_PC, uint8_t op_code){
    uint16_t acc_reg = cpu->ACC;
    uint16_t compare_src = *(ref_byte_reg(cpu, (op_code & 0x07)));
    // Perform Comparison
    uint16_t diff = acc_reg - compare_src;
    // Update Flags
    set_flags(cpu, diff, SIGN_FLAG | ZERO_FLAG | PARITY_FLAG | CARRY_FLAG);
    aux_flag_set_add(cpu, acc_reg, -compare_src);
    DECOMPILE_PRINT(base_PC, "CMP REG(%x)\n", (op_code & 0x07));
    return 1;
}

/**
 * @brief Compare A with(-) (byte 2)
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int CPI_WRAP(cpu_state* cpu, uint16_t base_PC, UNUSED uint8_t op_code){
    uint16_t acc_reg = cpu->ACC;
    uint16_t compare_src = mem_read(&cpu->mem, base_PC+1);
    // Perform Comparison
    uint16_t diff = acc_reg - compare_src;
    // Update Flags
    set_flags(cpu, diff, SIGN_FLAG | ZERO_FLAG | PARITY_FLAG | CARRY_FLAG);
    aux_flag_set_add(cpu, acc_reg, -compare_src);
    DECOMPILE_PRINT(base_PC, "CPI %x\n", compare_src);
    return 1;
}

/**
 * @brief The content of content register, is moved into the memory 
 * whose address is specified by the SP. Note: for RP-11b
 * it transforms into PUSH PSW
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int PUSH_WRAP(cpu_state* cpu, uint16_t base_PC, UNUSED uint8_t op_code){
    uint8_t reg_patt = (0x30 & op_code) >> 4;
    // Swap out SP for PSW.
    if(reg_patt == 0x3){
        cpu->SP -= 2;
        mem_write(&cpu->mem, cpu->SP, compress_PSW(cpu->PSW));
        mem_write(&cpu->mem, cpu->SP+1, cpu->ACC);
        DECOMPILE_PRINT(base_PC, "%s\n", "PUSH PSW");
    } else {
        uint16_t* target_dest = ref_short_reg(cpu, reg_patt);
        cpu->SP -= 2;
        short_mem_write(&cpu->mem, cpu->SP, *target_dest);
        DECOMPILE_PRINT(base_PC, "PUSH REGP(%x)\n", reg_patt);
    }
    return 1;
}

/**
 * @brief The content of the memory location, whose address
 * is specified by the SP is pused into content Reg. Note: for RP-11b
 * it transforms into POP PSW
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int POP_WRAP(cpu_state* cpu, uint16_t base_PC, UNUSED uint8_t op_code){
    uint8_t reg_patt = (0x30 & op_code) >> 4;
    // Swap out SP for PSW.
    if(reg_patt == 0x3){
        cpu->PSW = decompress_PSW(mem_read(&cpu->mem, cpu->SP));
        cpu->ACC = mem_read(&cpu->mem, cpu->SP+1);
        cpu->SP += 2;
        DECOMPILE_PRINT(base_PC, "%s\n", "POP PSW");
    } else {
        uint16_t* target_dest = ref_short_reg(cpu, reg_patt);
        *target_dest = short_mem_read(&cpu->mem, cpu->SP);
        cpu->SP += 2;
        DECOMPILE_PRINT(base_PC, "POP REGP(%x)\n", reg_patt);
    }
    return 1;
}

/**
 * @brief (Add register pair to Hand L) (H) (L) ..- (H) (L) + (rh) (rl)
 * The content of the register pair rp is added to the
 * content of the register pair Hand L. The result is
 * placed in the register pair Hand L. 
 * @note: Only the CY flag is affected.
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int DAD_WRAP(cpu_state* cpu, uint16_t base_PC, uint8_t op_code){
    uint8_t reg_patt = (0x30 & op_code) >> 4;
    uint16_t* target_dest = ref_short_reg(cpu, reg_patt);
    uint32_t temp = cpu->HL;
    temp += *target_dest;
    // Update HL
    cpu->HL = temp & 0xFFFF;
    // Set Carry Flag
    cpu->PSW.carry = (cpu->HL < temp) ? 1 : 0;
    DECOMPILE_PRINT(base_PC, "DAD REG(%x)\n", reg_patt);
    return 1;
}

/**
 * @brief Xchanges HL <--> DE
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int XCHG_WRAP(cpu_state* cpu, uint16_t base_PC, UNUSED uint8_t op_code){
    uint16_t temp = cpu->HL;
    cpu->HL = cpu->DE;
    cpu->DE = temp;
    DECOMPILE_PRINT(base_PC, "%s\n", "XCHG");
    return 1;
}

/**
 * @brief Copies a byte of data from ACC to PORT
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int OUT_WRAP(cpu_state* cpu, uint16_t base_PC, UNUSED uint8_t op_code){
    uint8_t port = mem_read(&cpu->mem, base_PC+1);
    io_machine_OUT(port, cpu->ACC);
    DECOMPILE_PRINT(base_PC, "OUT %x\n", port);
    return 1;
}

/**
 * @brief Copies a byte of data from the PORT defined by
 * byte(2) to ACC
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int IN_WRAP(cpu_state* cpu, uint16_t base_PC, uint8_t op_code){
    uint8_t port = mem_read(&cpu->mem, base_PC+1);
    cpu->ACC = io_machine_IN(port);
    DECOMPILE_PRINT(op_code, "IN %x\n", port);
    return 1;
}

/**
 * @brief Store *(RP) <- A
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int STAX_WRAP(cpu_state* cpu, uint16_t base_PC, uint8_t op_code){
    uint8_t reg_patt = (0x30 & op_code) >> 4;
    if(reg_patt < 2){
        uint16_t* target_ref = ref_short_reg(cpu, reg_patt);
        mem_write(&cpu->mem, *target_ref, cpu->ACC);
        DECOMPILE_PRINT(base_PC, "STAX *(REGP(%x)), A\n", reg_patt);
    } else {
        ILLEGAL_OP;
        exit(-2);
    }
    return 1;
}

/**
 * @brief (A) = (A) /\ (r)
 * The content of register r is logically anded with the
 * content of the accumulator. The result is placed in
 * the accumulator. The CY flag is cleared
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int ANA_WRAP(cpu_state* cpu, uint16_t base_PC, uint8_t op_code){
    uint8_t reg_patt = (0x07 & op_code);
    uint8_t *target_reg = ref_byte_reg(cpu, reg_patt);
    // Perform AND
    uint8_t base_val = cpu->ACC;
    uint8_t base_target = *target_reg;
    cpu->ACC &= (*target_reg);
    // Update Flags
    set_flags(cpu, cpu->ACC, ALL_BUT_AUX_FLAG);
    // https://www.quora.com/What-is-the-auxiliary-carry-set-when-ANA-R-instruction-is-executed-in-an-8085-CPU
    cpu->PSW.aux = ((base_val | base_target) & 0x08) ? 1 : 0;
    DECOMPILE_PRINT(base_PC, "ANA REG(%x)\n", reg_patt);
    return 1;
}

/**
 * @brief (L) <-- ((byte 3)(byte 2))
 * (H) <-- ((byte 3) (byte 2) + 1)
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int LHLD_WRAP(cpu_state* cpu, uint16_t base_PC, UNUSED uint8_t op_code){
    uint16_t target_addr = short_mem_read(&cpu->mem, base_PC+1);
    cpu->HL = short_mem_read(&cpu->mem, target_addr);
    DECOMPILE_PRINT(base_PC, "LHLD %x\n", target_addr);
    return 1;
}

/**
 * @brief (AND immediate)
 * (A) = (A) & (byte 2)
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int ANI_WRAP(cpu_state* cpu, uint16_t base_PC, UNUSED uint8_t op_code){
    uint8_t target_data = mem_read(&cpu->mem, base_PC+1);
    uint8_t base_val = cpu->ACC;
    cpu->ACC &= target_data;
    set_flags(cpu, cpu->ACC, ALL_BUT_AUX_FLAG);
    // https://www.quora.com/What-is-the-auxiliary-carry-set-when-ANA-R-instruction-is-executed-in-an-8085-CPU
    cpu->PSW.aux = ((base_val | target_data) & 0x08) ? 1 : 0;
    DECOMPILE_PRINT(base_PC, "ANI %x\n", target_data);
    return 1;
}

/**
 * @brief *((byte 3)(byte 2)) = ACC
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int STA_WRAP(cpu_state* cpu, uint16_t base_PC, UNUSED uint8_t op_code){
    uint16_t target_loc = short_mem_read(&cpu->mem, base_PC+1); 
    mem_write(&cpu->mem, target_loc, cpu->ACC);
    DECOMPILE_PRINT(base_PC, "STA %x\n", target_loc);
    return 1;
}

/**
 * @brief (increment Reg) (r) <- (r) + 1
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int INR_WRAP(cpu_state* cpu, uint16_t base_PC, uint8_t op_code){
    uint8_t reg_patt = (op_code & 0x38) >> 3;
    uint16_t target_data = *(ref_byte_reg(cpu, reg_patt));
    uint16_t base_data = target_data;
    // Update The data
    target_data += 1;
    *(ref_byte_reg(cpu, reg_patt)) = (uint8_t)target_data;
    // Update Flags
    set_flags(cpu, target_data, SIGN_FLAG | ZERO_FLAG | PARITY_FLAG );
    aux_flag_set_add(cpu, base_data, +1);
    DECOMPILE_PRINT(base_PC, "INR Reg(%x)\n", reg_patt);
    return 1;
}

/**
 * @brief (Rotate Right)
 * (An) <- (An-1); (A7) <- A0
 * (Cy) <- (A0)
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int RRC_WRAP(cpu_state* cpu, uint16_t base_PC, UNUSED uint8_t op_code){
    uint8_t lsb = 0x1 & cpu->ACC;
    cpu->ACC >>= 1;
    if(lsb){
        cpu->ACC |= 0x80;
        cpu->PSW.carry = 1;
    } else {
        cpu->PSW.carry = 0;
    }
    DECOMPILE_PRINT(base_PC, "%s\n", "RRC");
    return 1;
}

/**
 * @brief (Load Accumulator direct)
 * (A) = *((byte 3)(byte 2))
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int LDA_WRAP(cpu_state* cpu, uint16_t base_PC, UNUSED uint8_t op_code){
    uint16_t target_addr = short_mem_read(&cpu->mem, base_PC+1);
    cpu->ACC = short_mem_read(&cpu->mem, target_addr);
    DECOMPILE_PRINT(base_PC, "LDA %x\n", target_addr);
    return 1;
}

/**
 * @brief (Exclusive OR Register)
 * (A) = (A) ^ (r)
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int XRA_WRAP(cpu_state* cpu, uint16_t base_PC, UNUSED uint8_t op_code){
    uint8_t reg_patt = (op_code & 0x07);
    uint8_t *target_reg = ref_byte_reg(cpu, reg_patt);
    uint16_t temp = (*target_reg) ^ cpu->ACC;
    cpu->ACC = temp;
    // Setting flags
    set_flags(cpu, temp, ALL_BUT_AUX_FLAG);
    cpu->PSW.aux = 0;
    DECOMPILE_PRINT(base_PC, "XRA Reg(%x)\n", reg_patt);
    return 1;
}

/**
 * @brief Enable Interrupts
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int EI_WRAP(cpu_state* cpu, uint16_t base_PC, UNUSED uint8_t op_code){
    cpu->intt = 1;
    DECOMPILE_PRINT(base_PC, "%s\n", "EI");
    return 1;
}

/**
 * @brief Disable Interrupts
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int DI_WRAP(cpu_state* cpu, uint16_t base_PC, UNUSED uint8_t op_code){
    cpu->intt = 0;
    DECOMPILE_PRINT(base_PC, "%s\n", "DI");
    return 1;
}

/**
 * @brief (L) --> ((byte 3)(byte 2))
 * (H) --> ((byte 3) (byte 2) + 1)
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int SHLD_WRAP(cpu_state* cpu, uint16_t base_PC, UNUSED uint8_t op_code){
    uint16_t target_addr = short_mem_read(&cpu->mem, base_PC+1);
    short_mem_write(&cpu->mem, target_addr, cpu->HL);
    DECOMPILE_PRINT(base_PC, "SHLD %x\n", target_addr);
    return 1;
}

/**
 * @brief (reg pair) = (reg pair) - 1
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int DCX_WRAP(cpu_state* cpu, uint16_t base_PC, UNUSED uint8_t op_code){
    uint8_t reg_patt = (op_code & 0x30) >> 4;
    uint16_t* target_reg = ref_short_reg(cpu, reg_patt);
    (*target_reg)--;
    DECOMPILE_PRINT(base_PC, "DCX REGP(%x)\n", reg_patt);
    return 1;
}

/**
 * @brief (Rotate left)
 * (An+l) <-- (An) ; (AO) <-- (A7); (CY) <-- (A7)
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int RLC_WRAP(cpu_state* cpu, uint16_t base_PC, UNUSED uint8_t op_code){
    uint8_t msb = (cpu->ACC & 0x80) ? 1 : 0;
    cpu->ACC <<= 1;
    if(msb){
        cpu->ACC |= msb;
        cpu->PSW.carry = 1;
    } else {
        cpu->PSW.carry = 0;
    }
    DECOMPILE_PRINT(base_PC, "%s\n", "RLC");
    return 1;
}

/**
 * @brief (Rotate left through carry)
 * (An+1) <-- (An) ; (CY) <-- (A7) ; (AO) <-- (CY)
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int RAL_WRAP(cpu_state* cpu, uint16_t base_PC, UNUSED uint8_t op_code){
    uint8_t msb = (cpu->ACC & 0x80) ? 1 : 0;
    cpu->ACC = (cpu->ACC << 1) | cpu->PSW.carry;
    if(msb){
        cpu->PSW.carry = 1;
    } else {
        cpu->PSW.carry = 0;
    }
    DECOMPILE_PRINT(base_PC, "%s\n", "RAL");
    return 1;
}

/**
 * @brief (Rotate right through carry)
 * (An) <-- (An+l) ; (CY) <-- (AO) ; (A7) <-- (CY)
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int RAR_WRAP(cpu_state* cpu, uint16_t base_PC, UNUSED uint8_t op_code){
    uint8_t lsb = cpu->ACC & 0x01;
    cpu->ACC = (cpu->ACC >> 1) | (cpu->PSW.carry ? 0x80 : 0x0);
    if(lsb){
        cpu->PSW.carry = 1;
    } else {
        cpu->PSW.carry = 0;
    }
    DECOMPILE_PRINT(base_PC, "%s\n", "RAR");
    return 1;
}

/**
 * @brief Ccondition addr (Condition call)
 * If (CCC),
 * ((SP) -1) (PCH)
 * ((SP) - 2) (PCl)
 * (SP) (SP) - 2
 * (PC) (byte 3) (byte 2)
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int CCon_WRAP(cpu_state* cpu, uint16_t base_PC, uint8_t op_code){
    if(condition_check(cpu, (0x38 & op_code)>>3)){
        cpu->SP -= 2;
        short_mem_write(&cpu->mem, cpu->SP, cpu->PC);       // Saving Return Addr
        cpu->PC = short_mem_read(&cpu->mem, base_PC+1);     // Reading the new PC
    }
    DECOMPILE_PRINT(base_PC, "CALL Con(%x) %x\n", (0x38 & op_code)>>3, 
        short_mem_read(&cpu->mem, base_PC+1));
    return 1;
}

/**
 * @brief (Subtract immediate with borrow)
 * (A) <-- (A) - (byte 2) - (CY)
 * The contents of the second byte of the instruction
 * and the contents of the CY flag are both subtracted
 * from the accumulator. The result is placed in the
 * accumulator
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int SBI_WRAP(cpu_state* cpu, uint16_t base_PC, UNUSED uint8_t op_code){
    uint8_t target_data = mem_read(&cpu->mem, base_PC+1);
    uint16_t temp =  cpu->ACC;
    temp = temp - target_data - cpu->PSW.carry;
    // Set flags
    set_flags(cpu, temp, ALL_BUT_AUX_FLAG);
    aux_flag_set_add(cpu, cpu->ACC, -target_data - cpu->PSW.carry);
    cpu->ACC = temp;
    DECOMPILE_PRINT(base_PC, "SBI %x\n", target_data);
    return 1;
}

/**
 * @brief (Add Register)
 * (A) <-- (A) + (r)
 * The content of register r is added to the content of the
 * accumulator. The result is placed in the accumulator.
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int ADD_WRAP(cpu_state* cpu, uint16_t base_PC, uint8_t op_code){
    uint8_t reg_patt = (op_code & 0x07);
    uint16_t temp = *(ref_byte_reg(cpu, reg_patt));
    temp += cpu->ACC;
    // Set Flags
    set_flags(cpu, temp, ALL_BUT_AUX_FLAG);
    aux_flag_set_add(cpu, cpu->ACC, *(ref_byte_reg(cpu, reg_patt)));
    cpu->ACC = temp;
    DECOMPILE_PRINT(base_PC, "ADD REG(%x)\n", reg_patt);
    return 1;
}

/**
 * @brief (Add Immediate)
 * (A) <-- (A) + (byte 2)
 * The content of the second byte of the instruction is
 * added to the content of the accumulator. The result
 * is placed in the accumulator
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int ADI_WRAP(cpu_state* cpu, uint16_t base_PC, UNUSED uint8_t op_code){
    uint16_t temp = mem_read(&cpu->mem, base_PC+1);
    temp += cpu->ACC;
    // Set Flags
    set_flags(cpu, temp, ALL_BUT_AUX_FLAG);
    aux_flag_set_add(cpu, cpu->ACC, mem_read(&cpu->mem, base_PC+1));
    cpu->ACC = temp;
    DECOMPILE_PRINT(base_PC, "ADI %x\n", mem_read(&cpu->mem, base_PC+1));
    return 1;
}

/**
 * @brief (Add Register with carry)
 * (A) <-- (A) + (r) + (CY)
 * The content of register r and the content of the carry
 * bit are added to the content of the accumulator. The
 * result is placed in the accumulator
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int ADC_WRAP(cpu_state* cpu, uint16_t base_PC, uint8_t op_code){
    uint8_t reg_patt = (op_code & 0x07);
    uint16_t temp = *(ref_byte_reg(cpu, reg_patt));
    temp += cpu->ACC + cpu->PSW.carry;
    // Set Flags
    set_flags(cpu, temp, ALL_BUT_AUX_FLAG);
    aux_flag_set_add(cpu, cpu->ACC, *(ref_byte_reg(cpu, reg_patt)) + cpu->PSW.carry);
    cpu->ACC = temp;
    DECOMPILE_PRINT(base_PC, "ADC REG(%x)\n", reg_patt);
    return 1;
}

/**
 * @brief (Add immediate with carry)
 * (A) <-- (A) + (byte 2) + (CY)
 * The content of the second byte of the instruction and
 * the content of the CY flag are added to the contents
 * of the accumulator. The result is placed in the accumulator
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int ACI_WRAP(cpu_state* cpu, uint16_t base_PC, UNUSED uint8_t op_code){
    uint16_t temp = mem_read(&cpu->mem, base_PC+1);
    temp += cpu->ACC + cpu->PSW.carry;
    // Set Flags
    set_flags(cpu, temp, ALL_BUT_AUX_FLAG);
    aux_flag_set_add(cpu, cpu->ACC, mem_read(&cpu->mem, base_PC+1) + cpu->PSW.carry);
    cpu->ACC = temp;
    DECOMPILE_PRINT(base_PC, "ACI %x\n", mem_read(&cpu->mem, base_PC+1));
    return 1;
}

/**
 * @brief (Subtract Register)
 * (A) <-- (A) - (r)
 * The content of register r is subtracted from the content 
 * of the accumulator. The result is placed in the accumulator.
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int SUB_WRAP(cpu_state* cpu, uint16_t base_PC, uint8_t op_code){
    uint8_t reg_patt = (op_code & 0x07);
    uint16_t temp = cpu->ACC;
    temp -= *(ref_byte_reg(cpu, reg_patt));
    // Set Flags
    set_flags(cpu, temp, ALL_BUT_AUX_FLAG);
    aux_flag_set_add(cpu, cpu->ACC, - (*(ref_byte_reg(cpu, reg_patt))));
    cpu->ACC = temp;
    DECOMPILE_PRINT(base_PC, "SUB REG(%x)\n", reg_patt);
    return 1;
}

/**
 * @brief (Subtract immediate)
 * (A) <-- (A) - (byte 2)
 * The content of the second byte of the instruction is
 * subtracted from the content of the accumulator. The
 * result is placed in the accumulator
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int SUI_WRAP(cpu_state* cpu, uint16_t base_PC, UNUSED uint8_t op_code){
    uint16_t temp = cpu->ACC;
    temp -= mem_read(&cpu->mem, base_PC+1);
    // Set Flags
    set_flags(cpu, temp, ALL_BUT_AUX_FLAG);
    aux_flag_set_add(cpu, cpu->ACC, - (mem_read(&cpu->mem, base_PC+1)));
    cpu->ACC = temp;
    DECOMPILE_PRINT(base_PC, "SUI %x\n", mem_read(&cpu->mem, base_PC+1));
    return 1;
}

/**
 * @brief (Subtract Register with borrow)
 * (A) <-- (A) - (r) - (CY)
 * The content of register r and the content of the CY
 * flag are both subtracted from the accumulator. The
 * result is placed in the accumulator.
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int SBB_WRAP(cpu_state* cpu, uint16_t base_PC, uint8_t op_code){
    uint8_t reg_patt = (op_code & 0x07);
    uint16_t temp = cpu->ACC;
    temp -= (*(ref_byte_reg(cpu, reg_patt)) + cpu->PSW.carry);
    // Set Flags
    set_flags(cpu, temp, ALL_BUT_AUX_FLAG);
    aux_flag_set_add(cpu, cpu->ACC, - (*(ref_byte_reg(cpu, reg_patt))) - 1);
    cpu->ACC = temp;
    DECOMPILE_PRINT(base_PC, "SBB REG(%x)\n", reg_patt);
    return 1;
}

/**
 * @brief (OR Register)
 * (A) <-- (A) | (r)
 * The content of register r is inclusive-OR'd with the
 * content of the accumulator. The result is placed in
 * the accumulator. The CY and AC flags are cleared.
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int ORA_WRAP(cpu_state* cpu, uint16_t base_PC, uint8_t op_code){
    uint8_t reg_patt = (op_code & 0x07);
    uint8_t *target_reg = ref_byte_reg(cpu, reg_patt);
    uint16_t temp = (*target_reg) | cpu->ACC;
    cpu->ACC = temp;
    // Setting flags
    set_flags(cpu, temp, ALL_BUT_AUX_FLAG);
    cpu->PSW.aux = 0;
    DECOMPILE_PRINT(base_PC, "ORA Reg(%x)\n", reg_patt);
    return 1;
}

/**
 * @brief (Exchange stack top with Hand L)
 * (L) <--> ((SP)) ;
 * (H) <--> ((SP) + 1) ;
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int XTHL_WRAP(cpu_state* cpu, uint16_t base_PC, UNUSED uint8_t op_code){
    uint16_t temp = short_mem_read(&cpu->mem, cpu->SP);
    short_mem_write(&cpu->mem,cpu->SP,cpu->HL);
    cpu->HL = temp;
    DECOMPILE_PRINT(base_PC, "%s\n", "XTHL");
    return 1;
}

/**
 * @brief (Jump Hand l indirect - move Hand L to PC)
 * (PCH) <-- (H)
 * (PCl) <-- (l)
 * The content of register H is moved to the high-order
 * eight bits of register PC. The content of register l is
 * moved to the low-order eight bits of register PC.
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int PCHL_WRAP(cpu_state* cpu, uint16_t base_PC, UNUSED uint8_t op_code){
    cpu->PC = cpu->HL;
    DECOMPILE_PRINT(base_PC, "%s\n", "PCHL");
    return 1;
}

/**
 * @brief (OR Immediate)
 * (A) <-- (A) V (byte 2)
 * The content of the second byte of the instruction is
 * inclusive-OR'd with the content of the accumulator.
 * The result is placed in the accumulator. The CY and
 * AC flags are cleared.
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int ORI_WRAP(cpu_state* cpu, uint16_t base_PC, UNUSED uint8_t op_code){
     uint8_t target_data = mem_read(&cpu->mem, base_PC+1);
    cpu->ACC |= target_data;
    set_flags(cpu, cpu->ACC, ALL_BUT_AUX_FLAG);
    cpu->PSW.aux = 0;
    DECOMPILE_PRINT(base_PC, "ORI %x\n", target_data);
    return 1;
}

/**
 * @brief (Exclusive OR Immediate)
 * (A) <-- (A) ^ (byte 2)
 * The content of the second byte of the instruction is
 * exclusive-O R'd with the content of the accumu lator.
 * The result is placed in the accumulator. The CY and
 * AC flags are cleared.
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int XRI_WRAP(cpu_state* cpu, uint16_t base_PC, UNUSED uint8_t op_code){
    uint8_t target_data = mem_read(&cpu->mem, base_PC+1);
    cpu->ACC ^= target_data;
    set_flags(cpu, cpu->ACC, ALL_BUT_AUX_FLAG);
    cpu->PSW.aux = 0;
    DECOMPILE_PRINT(base_PC, "XRI %x\n", target_data);
    return 1;
}

/**
 * @brief Complement accumulator (A) <- (~A)
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int CMA_WRAP(cpu_state* cpu, uint16_t base_PC, UNUSED uint8_t op_code){
    cpu->ACC = ~(cpu->ACC);
    DECOMPILE_PRINT(base_PC, "%s\n", "CMA");
    return 1;
}

/**
 * @brief Complement carry (CY) <- (!CY)
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int CMC_WRAP(cpu_state* cpu, uint16_t base_PC, UNUSED uint8_t op_code){
    cpu->PSW.carry = !(cpu->PSW.carry);
    DECOMPILE_PRINT(base_PC, "%s\n", "CMC");
    return 1;
}

/**
 * @brief Set Carry (CY) <- (1)
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int STC_WRAP(cpu_state* cpu, uint16_t base_PC, UNUSED uint8_t op_code){
    cpu->PSW.carry = 1;
    DECOMPILE_PRINT(base_PC, "%s\n", "STC");
    return 1;
}

/**
 * @brief (Decimal Adjust Accumulator)
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int DAA_WRAP(cpu_state* cpu, uint16_t base_PC, UNUSED uint8_t op_code){
    if((cpu->ACC & 0xF) > 0x9 || cpu->PSW.aux){
        cpu->PSW.aux = 0;
        if(((cpu->ACC & 0xF) + 0x06) > 0xF){
            cpu->PSW.aux = 1;
        }
        if((cpu->ACC + 6) & 0x100){
            cpu->PSW.carry = 1;
        }
        cpu->ACC += 0x6;
    }
    if((cpu->ACC & 0xF0) >> 4 > 0x9 || cpu->PSW.carry == 1){
        if(((cpu->ACC >> 4) + 0x06) > 0xF){
            cpu->PSW.carry = 1;
        }
        cpu->ACC += 0x60;
    }
    DECOMPILE_PRINT(base_PC, "%s\n", "DAA");
    return 1;
}

/**
 * @brief (Restart)
 * ((SP) - 1) <-- (PCH);
 * ((SP) - 2) <-- (PCl);
 * (SP) <-- (SP) - 2;
 * (PC) <-- 8* (NNN);
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int RST_WRAP(cpu_state* cpu, uint16_t base_PC, uint8_t op_code){
    cpu->SP -= 2;
    short_mem_write(&cpu->mem, cpu->SP, cpu->PC);       // Saving Return Addr
    // Generating new position
    uint16_t target_addr = ((op_code & 0x38) >> 3);     // Generate new target Addr
    cpu->PC = target_addr * 8;                          // Writing New PC
    DECOMPILE_PRINT(base_PC, "RST %x\n", target_addr);  // Logging
    return 1;
}

/**
 * @brief (Exchange stack top with H and L)
 * (L) <--> ((SP));
 * (H) <--> ((SP) + 1);
 * The content of the L register is exchanged with the
 * content of the memory location whose address is
 * specified by the content of register SP. The content
 * of the H register is exchanged with the content of the
 * memory location whose address is one more than the
 * content of register SP.
 * 
 * @param cpu 
 * @param base_PC 
 * @param op_code 
 * @return int 
 */
int SPHL_WRAP(cpu_state* cpu, uint16_t base_PC, UNUSED uint8_t op_code){
    uint16_t temp = cpu->HL;
    cpu->HL = short_mem_read(&cpu->mem, cpu->SP);
    short_mem_write(&cpu->mem, cpu->SP, temp);
    DECOMPILE_PRINT(base_PC, "%s\n", "SPHL");
    return 1;
}


instt_8080_op opcode_lookup[0x100] = {
    [0x00] = {.target_func = NOP_WRAP, .cycle_count = 4, .size = 1},    // NOP Instruction
    [0x01] = {LXI_WRAP, 10, 3},
    [0x02] = {STAX_WRAP, 7, 1},
    [0x03] = {INX_WRAP, 5, 1},
    [0x04] = {INR_WRAP, 5, 1},
    [0x05] = {DCR_WRAP, 5, 1},
    [0x06] = {MVI_WRAP, 7, 2},
    [0x07] = {RLC_WRAP, 7, 1},
    [0x08] = {NOP_WRAP, 4, 1},
    [0x09] = {DAD_WRAP, 10, 1},
    [0x0A] = {LDAX_WRAP, 7, 1},
    [0x0B] = {DCX_WRAP, 5, 1},
    [0x0C] = {INR_WRAP, 5, 1},
    [0x0D] = {DCR_WRAP, 5, 1},
    [0x0E] = {MVI_WRAP, 7, 2},
    [0x0F] = {RRC_WRAP, 4, 1},
    [0x10] = {NOP_WRAP, 4, 1},
    [0x11] = {LXI_WRAP, 10, 3},
    [0x12] = {STAX_WRAP, 7, 1},
    [0x13] = {INX_WRAP, 5, 1},
    [0x14] = {INR_WRAP, 5, 1},
    [0x15] = {DCR_WRAP, 5, 1},
    [0x16] = {MVI_WRAP, 7, 2},
    [0x17] = {RAL_WRAP, 4, 1},
    [0x18] = {NOP_WRAP, 4, 1},
    [0x19] = {DAD_WRAP, 10, 1},
    [0x1A] = {LDAX_WRAP, 7, 1},
    [0x1B] = {DCX_WRAP, 5, 1},
    [0x1C] = {INR_WRAP, 5, 1},
    [0x1D] = {DCR_WRAP, 5, 1},
    [0x1E] = {MVI_WRAP, 7, 2},
    [0x1F] = {RAR_WRAP, 4, 1},
    [0x20] = {NOP_WRAP, 4, 1},
    [0x21] = {LXI_WRAP, 10, 3},
    [0x22] = {SHLD_WRAP, 16, 3},
    [0x23] = {INX_WRAP, 5, 1},
    [0x24] = {INR_WRAP, 5, 1},
    [0x25] = {DCR_WRAP, 5, 1},
    [0x26] = {MVI_WRAP, 7, 2},
    [0x27] = {DAA_WRAP, 4, 1},
    [0x28] = {NOP_WRAP, 4, 1},
    [0x29] = {DAD_WRAP, 10, 1},
    [0x2A] = {LHLD_WRAP, 16, 3},
    [0x2B] = {DCX_WRAP, 5, 1},
    [0x2C] = {INR_WRAP, 5, 1},
    [0x2D] = {DCR_WRAP, 5, 1},
    [0x2E] = {MVI_WRAP, 7, 2},
    [0x2F] = {CMA_WRAP, 4, 1},
    [0x30] = {NOP_WRAP, 4, 1},
    [0x31] = {LXI_WRAP, 10, 3},
    [0x32] = {STA_WRAP, 13, 3},
    [0x33] = {INX_WRAP, 5, 1},
    [0x34] = {INR_WRAP, 5, 1},
    [0x35] = {DCR_WRAP, 5, 1},
    [0x36] = {MVI_WRAP, 10, 2},
    [0x37] = {STC_WRAP, 4, 1},
    [0x38] = {NOP_WRAP, 4, 1},
    [0x39] = {DAD_WRAP, 10, 1},
    [0x3A] = {LDA_WRAP, 13, 3},
    [0x3B] = {DCX_WRAP, 5, 1},
    [0x3C] = {INR_WRAP, 5, 1},
    [0x3D] = {DCR_WRAP, 5, 1},
    [0x3E] = {MVI_WRAP, 7, 2},
    [0x3F] = {CMC_WRAP, 4, 1},
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
    [0x80] = {ADD_WRAP, 4, 1},
    [0x81] = {ADD_WRAP, 4, 1},
    [0x82] = {ADD_WRAP, 4, 1},
    [0x83] = {ADD_WRAP, 4, 1},
    [0x84] = {ADD_WRAP, 4, 1},
    [0x85] = {ADD_WRAP, 4, 1},
    [0x86] = {ADD_WRAP, 4, 1},
    [0x87] = {ADD_WRAP, 4, 1},
    [0x88] = {ADC_WRAP, 4, 1},
    [0x89] = {ADC_WRAP, 4, 1},
    [0x8A] = {ADC_WRAP, 4, 1},
    [0x8B] = {ADC_WRAP, 4, 1},
    [0x8C] = {ADC_WRAP, 4, 1},
    [0x8D] = {ADC_WRAP, 4, 1},
    [0x8E] = {ADC_WRAP, 4, 1},
    [0x8F] = {ADC_WRAP, 4, 1},
    [0x90] = {SUB_WRAP, 4, 1},
    [0x91] = {SUB_WRAP, 4, 1},
    [0x92] = {SUB_WRAP, 4, 1},
    [0x93] = {SUB_WRAP, 4, 1},
    [0x94] = {SUB_WRAP, 4, 1},
    [0x95] = {SUB_WRAP, 4, 1},
    [0x96] = {SUB_WRAP, 4, 1},
    [0x97] = {SUB_WRAP, 4, 1},
    [0x98] = {SBB_WRAP, 4, 1},
    [0x99] = {SBB_WRAP, 4, 1},
    [0x9A] = {SBB_WRAP, 4, 1},
    [0x9B] = {SBB_WRAP, 4, 1},
    [0x9C] = {SBB_WRAP, 4, 1},
    [0x9D] = {SBB_WRAP, 4, 1},
    [0x9E] = {SBB_WRAP, 4, 1},
    [0x9F] = {SBB_WRAP, 4, 1},
    [0xA0] = {ANA_WRAP, 4, 1},
    [0xA1] = {ANA_WRAP, 4, 1},
    [0xA2] = {ANA_WRAP, 4, 1},
    [0xA3] = {ANA_WRAP, 4, 1},
    [0xA4] = {ANA_WRAP, 4, 1},
    [0xA5] = {ANA_WRAP, 4, 1},
    [0xA6] = {ANA_WRAP, 4, 1},
    [0xA7] = {ANA_WRAP, 4, 1},
    [0xA8] = {XRA_WRAP, 4, 1},
    [0xA9] = {XRA_WRAP, 4, 1},
    [0xAA] = {XRA_WRAP, 4, 1},
    [0xAB] = {XRA_WRAP, 4, 1},
    [0xAC] = {XRA_WRAP, 4, 1},
    [0xAD] = {XRA_WRAP, 4, 1},
    [0xAE] = {XRA_WRAP, 4, 1},
    [0xAF] = {XRA_WRAP, 4, 1},
    [0xB0] = {ORA_WRAP, 4, 1},
    [0xB1] = {ORA_WRAP, 4, 1},
    [0xB2] = {ORA_WRAP, 4, 1},
    [0xB3] = {ORA_WRAP, 4, 1},
    [0xB4] = {ORA_WRAP, 4, 1},
    [0xB5] = {ORA_WRAP, 4, 1},
    [0xB6] = {ORA_WRAP, 4, 1},
    [0xB7] = {ORA_WRAP, 4, 1},
    [0xB8] = {CMP_WRAP, 4, 1},
    [0xB9] = {CMP_WRAP, 4, 1},
    [0xBA] = {CMP_WRAP, 4, 1},
    [0xBB] = {CMP_WRAP, 4, 1},
    [0xBC] = {CMP_WRAP, 4, 1},
    [0xBD] = {CMP_WRAP, 4, 1},
    [0xBE] = {CMP_WRAP, 4, 1},
    [0xBF] = {CMP_WRAP, 4, 1},
    [0xC0] = {RCon_WRAP, 11, 1},
    [0xC1] = {POP_WRAP, 10, 1},
    [0xC2] = {JCon_WRAP, 10, 3},
    [0xC3] = {JMP_WRAP, 10, 3},
    [0xC4] = {CCon_WRAP, 17, 3},
    [0xC5] = {PUSH_WRAP, 11, 1},
    [0xC6] = {ADI_WRAP, 7, 2},
    [0xC7] = {RST_WRAP, 11, 1},
    [0xC8] = {RCon_WRAP, 11, 1},
    [0xC9] = {RET_WRAP, 10, 1},
    [0xCA] = {JCon_WRAP, 10, 3},
    [0xCB] = {JMP_WRAP, 10, 3},
    [0xCC] = {CCon_WRAP, 17, 3},
    [0xCD] = {CALL_WRAP, 17, 3},
    [0xCE] = {ACI_WRAP, 7, 2},
    [0xCF] = {RST_WRAP, 11, 1},
    [0xD0] = {RCon_WRAP, 11, 1},
    [0xD1] = {POP_WRAP, 10, 1},
    [0xD2] = {JCon_WRAP, 10, 3},
    [0xD3] = {OUT_WRAP, 10, 2},
    [0xD4] = {CCon_WRAP, 17, 3},
    [0xD5] = {PUSH_WRAP, 11, 1},
    [0xD6] = {SUI_WRAP, 7, 2},
    [0xD7] = {RST_WRAP, 11, 1},
    [0xD8] = {RCon_WRAP, 11, 1},
    [0xD9] = {RET_WRAP, 10, 1},
    [0xDA] = {JCon_WRAP, 10, 3},
    [0xDB] = {IN_WRAP, 10, 2},
    [0xDC] = {CCon_WRAP, 17, 3},
    [0xDD] = {CALL_WRAP, 17, 3},
    [0xDE] = {SBI_WRAP, 7, 2},
    [0xDF] = {RST_WRAP, 11, 1},
    [0xE0] = {RCon_WRAP, 11, 1},
    [0xE1] = {POP_WRAP, 10, 1},
    [0xE2] = {JCon_WRAP, 10, 3},
    [0xE3] = {XTHL_WRAP, 18, 1},
    [0xE4] = {CCon_WRAP, 17, 3},
    [0xE5] = {PUSH_WRAP, 11, 1},
    [0xE6] = {ANI_WRAP, 7, 2},
    [0xE7] = {RST_WRAP, 11, 1},
    [0xE8] = {RCon_WRAP, 11, 1},
    [0xE9] = {PCHL_WRAP, 5, 1},
    [0xEA] = {JCon_WRAP, 10, 3},
    [0xEB] = {XCHG_WRAP, 5, 1},
    [0xEC] = {CCon_WRAP, 17, 3},
    [0xED] = {CALL_WRAP, 17, 3},
    [0xEE] = {XRI_WRAP, 7, 2},
    [0xEF] = {RST_WRAP, 11, 1},
    [0xF0] = {RCon_WRAP, 11, 1},
    [0xF1] = {POP_WRAP, 10, 1},
    [0xF2] = {JCon_WRAP, 10, 3},
    [0xF3] = {DI_WRAP, 4, 1},
    [0xF4] = {CCon_WRAP, 17, 3},
    [0xF5] = {PUSH_WRAP, 11, 1},
    [0xF6] = {ORI_WRAP, 7, 2},
    [0xF7] = {RST_WRAP, 11, 1},
    [0xF8] = {RCon_WRAP, 11, 1},
    [0xF9] = {SPHL_WRAP, 5, 1},
    [0xFA] = {JCon_WRAP, 10, 3},
    [0xFB] = {EI_WRAP, 4, 1},
    [0xFC] = {CCon_WRAP, 17, 3},
    [0xFD] = {CALL_WRAP, 17, 3},
    [0xFE] = {CPI_WRAP, 7, 2},
    [0xFF] = {RST_WRAP, 11, 1},
};

#endif