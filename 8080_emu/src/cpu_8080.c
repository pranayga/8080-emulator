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

cpu_state* init_cpu_8080(uint16_t pc){
    // Malloc a new struct
    cpu_state* cpu = (cpu_state*)calloc(1, sizeof(cpu_state));
    cpu->PC = pc;
    return cpu;
}

int exec_inst(UNUSED cpu_state* cpu){
    WARN(0, "%s\n", "Unimplemented.");
    exit(-2);
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
    printf("PSW:%x\n", cpu.PSW);
    printf("SP:%d\n", cpu.SP);
    printf("PC:%d\n", cpu.PC);
    printf("======IMG=====\n");
    printf("Base:%p\n", cpu.base);
    printf("==============\n");
}