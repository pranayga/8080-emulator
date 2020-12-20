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
} cpu_state;


#endif