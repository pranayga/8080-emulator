/**
 * @file cpu_8080.h
 * @author Pranay Garg (pranayga@andrew.cmu.edu)
 * @brief This file simulates and disassembles part of 8080 processor.
 * @version 0.1
 * @date 2020-12-20
 * 
 * @copyright Copyright (c) 2020 (CC0)
 * 
 */
#ifndef CPU_8080_H
#define CPU_8080_H

#include <inttypes.h>

/**
 * @brief cpu_state: This structure keeps runtime state of all the
 * registers in the CPU.
 */
typedef struct {
    ///@{
    /** General Purpose Registers in CPU */
    uint8_t B;      /**< Register 0, Pair B */
    uint8_t C;      /**< Register 1, Pair B */
    uint8_t D;      /**< Register 2, Pair D */
    uint8_t E;      /**< Register 3, Pair D */
    uint8_t H;      /**< Register 4, Pair H */
    uint8_t L;      /**< Register 5, Pair H */
    ///@}
    // TODO: Add The memory ref reg 6!?
    uint8_t A;      /**< Register 7 */
    uint8_t PSW;    /**< Program Status Word */

    ///@{
    /** Special Purpose 16bit Regs **/
    uint16_t SP;    /**< Stack Pointer */
    uint16_t PC;    /**< Program Counter */
    ///@}

    ///@{
    /** Additional state to make emulation smoother */
    uintptr_t base; /**< Base pointer, points to start of 16K memeory chunck */
    ///@}
} cpu_state;

/**
 * 
 * @brief Initialize a new cpu_8080 instance structure.
 * 
 * @param pc Program counter initializaiton
 * 
 * @return cpu_state* Pointer to the Malloced CPU state.
 * @note 
 * - the user is responsible for freeing the memory (for the CPU) once it's done
 * - currently expects the text section to be loaded into the lower 16 bit addressable
 * virtual memory.
 */
cpu_state* init_cpu_8080(uint16_t pc);

/**
 * @brief Executes the instruction pc is pointing to after incrementing it.
 * 
 * @param cpu 
 * @return int 
 */
int exec_inst(cpu_state* cpu);

/**
 * @brief Print the state of CPU
 * 
 * @param cpu 
 */
void print_state(const cpu_state cpu);

#endif