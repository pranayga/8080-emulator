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
#include "opcodes_8080.h"

// Main Externally visible Functions
cpu_state* init_cpu_8080(uint16_t pc){
    // Malloc a new struct
    cpu_state* cpu = (cpu_state*)calloc(1, sizeof(cpu_state));
    cpu->PC = pc;
    return cpu;
}

int exec_inst(UNUSED cpu_state* cpu){
    uint8_t Instt = mem_read(&cpu->mem, cpu->PC);
    /** TODO: Eventually Switch with a functor setup. */
    switch (Instt)
    {
    case 0x0: //NOP
    case 0x8:
        printf("%x : NOP\n", cpu->PC);
        cpu->PC++;
        NOP_WRAP(cpu);
        break;
    case 0xC3: //JMP
        printf("%x : JMP %x\n", cpu->PC, short_mem_read(&cpu->mem, cpu->PC+1));
        cpu->PC += 3;
        break;
    default:
        printf("%x : (%x)UNKNOWN\n", cpu->PC, Instt);
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
    printf("Base:%p\n", cpu.mem.base);
    printf("==============\n");
}
