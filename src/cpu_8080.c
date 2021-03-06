/**
 * @file cpu_8080.c
 * @author Pranay Garg (pranayga@andrew.cmu.edu)
 * @brief This file contains a software abstraction of intel 8080 CPU, enough to run invaders
 * @version 0.1
 * @date 2020-12-20
 * 
 */

#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include "debug.h"
#include "cpu_8080.h"
#include "opcodes_8080.h"

// Main Externally visible Functions
cpu_state* init_cpu_8080(uint16_t pc, uint8_t (*in_cb)(uint8_t), void (*out_cb)(uint8_t, uint8_t)){
    // Malloc a new struct
    cpu_state* cpu = (cpu_state*)calloc(1, sizeof(cpu_state));
    cpu->PC = pc;
    cpu->SP = 0xF000;
    cpu->IN_Func = &io_machine_IN;
    cpu->OUT_Func = &io_machine_OUT;
    // Swap the pointers if anything was passed by the
    // initializer
    if(in_cb != NULL){
        cpu->IN_Func = in_cb;
    }
    if(out_cb != NULL){
        cpu->OUT_Func = out_cb;
    }
    return cpu;
}

int exec_inst(cpu_state* cpu){

    // Check if Intt Available, if so exec that instead
    if(cpu->intt && cpu->pend_intt){
        // Entering Intt Handler, disable Intt
        cpu->intt = 0;
        uint8_t index = 0;
        for(uint16_t mask = 0x1; mask <= 0x8; mask <<= 1){
            if(cpu->pend_intt & mask){
                uint8_t op_code = 0xC7 | (index << 3);
                cpu->pend_intt &= (~mask);  // Marking Intt as handled
                return RST_WRAP(cpu, 0xFFFF, op_code);
            }
            index++;
        }
        // Shoild never reach here
        abort();
    }

    // If not, normal exec
    uint8_t Instt = mem_read(&cpu->mem, cpu->PC);
    uint16_t inital_pc_ptr = cpu->PC;
    cpu->PC += opcode_lookup[Instt].size;
    
    // TODO: Remove
    if(opcode_lookup[Instt].target_func == 0x0){
         opcode_lookup[Instt].target_func = UNDEFINED_OP_WRAP;
    }

    int ret = opcode_lookup[Instt].target_func(cpu, inital_pc_ptr, Instt);
    return ret;
}

int decompile_inst(cpu_state* cpu, uint16_t* next_inst){
    uint8_t Instt = mem_read(&cpu->mem, (*next_inst));
    cpu->PC = (*next_inst);

    (*next_inst) += opcode_lookup[Instt].size;

    uint16_t inital_pc_ptr = cpu->PC;
    
    // TODO: Remove
    if(opcode_lookup[Instt].target_func == 0x0){
         opcode_lookup[Instt].target_func = UNDEFINED_OP_WRAP;
    }

    int ret = opcode_lookup[Instt].target_func(cpu, inital_pc_ptr, Instt);
    return ret;
}
/**
 * @brief Default function for IO_WRAP
 * This function would be triggered on an `OUT`
 * instruction. Usually overriden by the caller
 * during setup
 * 
 * @param port 
 * @param data 
 */
void io_machine_OUT(UNUSED uint8_t port, UNUSED uint8_t data){
    DEBUG_PRINT("%s\n", "UNINPLEMENTED MACHINE_OUT");
    return;
}

/**
 * @brief Default function for IO_WRAP
 * This function would be triggered on an `IN`
 * instruction. Usually overriden by the caller
 * during setup
 * 
 * @param port 
 * @return uint8_t 
 */
uint8_t io_machine_IN(UNUSED uint8_t port){
    DEBUG_PRINT("%s\n", "UNINPLEMENTED MACHINE_IN");
    return 0x0;
}

void print_state(const cpu_state cpu){
    printf("+++++++++++++++++++++++++++++++++++++++++++++++++\n");
    printf("CPU State Dump:\n");
    printf("======GP======\n");
    printf("B:%x\n", cpu.B);
    printf("C:%x\n", cpu.C);
    printf("D:%x\n", cpu.D);
    printf("E:%x\n", cpu.E);
    printf("H:%x\n", cpu.H);
    printf("L:%x\n", cpu.L);
    printf("=====SPCL=====\n");
    printf("ACC:%x\n", cpu.ACC);
    printf("PSW: C:%x A:%x S:%x Z:%x P:%x\n",   cpu.PSW.carry, cpu.PSW.aux, cpu.PSW.sign,
                                                cpu.PSW.zero, cpu.PSW.parity);
    printf("SP:%x\n", cpu.SP);
    printf("PC:%x\n", cpu.PC);
    printf("Intt:%x\n", cpu.intt);
    printf("======IMG=====\n");
    printf("Base:%p\n", cpu.mem.base);
    printf("==============\n");
}
