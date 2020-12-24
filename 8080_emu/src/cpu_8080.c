/**
 * @file cpu_8080.c
 * @author Pranay Garg (pranayga@andrew.cmu.edu)
 * @brief This file contains a software abstraction of intel 8080 CPU, enough to run invaders
 * @version 0.1
 * @date 2020-12-20
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include "debug.h"
#include "cpu_8080.h"

#define UNUSED __attribute__((unused))
#define EXEC_INST()


// Internal Helpers
//============================================================================
/**
 * @brief Wrapper over No Op
 * 
 * @param cpu Cpu state
 * @return int 1 of success, 0 if fail
 */
static inline int NOP_WRAP(UNUSED cpu_state* cpu){
    DEBUG_PRINT("%x : NOP\n", nop_instt);
    return 1;
}

//============================================================================

// Main Externally visible Functions
cpu_state* init_cpu_8080(uint16_t pc){
    // Malloc a new struct
    cpu_state* cpu = (cpu_state*)calloc(1, sizeof(cpu_state));
    cpu->PC = pc;
    return cpu;
}

int exec_inst(UNUSED cpu_state* cpu){
    Instruction_OPCODE Instt = mem_read(*cpu, cpu->PC);
    switch (Instt)
    {
    case nop_instt:
        cpu->PC++;
        NOP_WRAP(cpu);
        break;
    default:
        WARN(0, "%s\n", "UNKNOWN OPCODE.");
        return 0;
    }
    return 1;
}

void print_state(const cpu_state cpu){
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
    printf("SP:%d\n", cpu.SP);
    printf("PC:%d\n", cpu.PC);
    printf("Intt:%d\n", cpu.intt);
    printf("======IMG=====\n");
    printf("Base:%p\n", cpu.base);
    printf("==============\n");
}

uint8_t mem_read(const cpu_state cpu, uint16_t offset){
    uint8_t *target_addr = (uint8_t *)((uintptr_t)cpu.base | offset);
    return *target_addr;
}

void mem_write(const cpu_state cpu, uint16_t offset, uint8_t val){
    uint8_t *target_addr = (uint8_t *)((uintptr_t)cpu.base | offset);
    *target_addr = val;
}