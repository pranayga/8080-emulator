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
#include "memory_8080.h"

#define UNUSED __attribute__((unused))

/**
 * @brief program_status_word: An exdended PSW for easy checks and
 * sets of PSW without flagging. 
 */
typedef struct {
    uint8_t carry;  /**< Status of the `carry` bit in PSW */
    uint8_t aux;    /**< Status of the `auxiliary carry` bit in PSW */
    uint8_t sign;   /**< Status of the `sign` bit in PSW (1 if -ve) */
    uint8_t zero;   /**< Status of the `zero` bit in PSW (1 if 0) */
    uint8_t parity; /**< Status of the `parity` bit in PSW (1 if num of 1 even) */
} program_status_word;

/**
 * @brief ENUMS for PSW_FLAGS
 * 
 */
typedef enum {
    SIGN_FLAG   = 1 << 0,
    ZERO_FLAG   = 1 << 1,
    AUX_FLAG    = 1 << 3,
    PARITY_FLAG = 1 << 5,
    CARRY_FLAG  = 1 << 7,
} flag_bits;
/** @brief All but the aux flag options */
# define ALL_BUT_AUX_FLAG (SIGN_FLAG | ZERO_FLAG | PARITY_FLAG | CARRY_FLAG)

/**
 * @brief different condition checks for JMP, conditional OPS
 * 
 */
typedef enum {
    NZ_check    = 0x0, /** not zero (Z = 0) */
    Z_check     = 0x1, /** zero (2 = 1) */
    NC_check    = 0x2, /** no carry (CY = 0) */
    C_check     = 0x3, /** carry (CY = 1) */
    PO_check    = 0x4, /** parity odd (P = 0) */
    PE_check    = 0x5, /** parity even (P = 1) */
    P_check     = 0x6, /** plus (Sign = 0) */
    M_check     = 0x7, /** minus(Sign = 1) */
} condition_flags;

/**
 * @brief cpu_state: This structure keeps runtime state of all the
 * registers in the CPU.
 */
typedef struct {
    ///@{
    /** General Purpose Registers in CPU */
    union{
        struct {
            uint8_t C;      /**< Register 1, Pair B */
            uint8_t B;      /**< Register 0, Pair B */
            uint8_t E;      /**< Register 3, Pair D */
            uint8_t D;      /**< Register 2, Pair D */
            uint8_t L;      /**< Register 5, Pair H */
            uint8_t H;      /**< Register 4, Pair H */
        };
        struct {
            uint16_t BC;    /**< Extended Reg Pair BC */
            uint16_t DE;    /**< Extended Reg Pair DE */
            uint16_t HL;    /**< Extended Reg Pair HL */
        };
    };
    ///@}
    // TODO: Add The memory ref reg 6!?
    uint8_t ACC;                /**< Register 7 */
    program_status_word PSW;    /**< Program Status Word */

    ///@{
    /** Special Purpose 16bit Regs **/
    uint16_t SP;    /**< Stack Pointer */
    uint16_t PC;    /**< Program Counter */
    uint8_t intt;   /**< Interrupt status Reg */ //(Don't know if works)
    ///@}

    ///@{
    /** Additional state to make emulation smoother */
    v_memory mem; /**< mem pointer, points to vMemeory chunck */
    uint16_t rom_size; /** Size of the ROM currenlty loaded */
    ///@}
} cpu_state;

/**
 * 
 * @brief Initialize a new cpu_8080 instance structure. Everything is initialized to 0.
 * 
 * @param pc Program counter initialization
 * 
 * @return cpu_state* Pointer to the Malloced CPU state.
 * @note 
 * - the user is responsible for freeing the memory (for the CPU) once it's done
 * - expects base to be initialized by the user to a 64KB aligned memory
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
 * @brief Recompile mode.
 * 
 * @param cpu 
 * @param next_inst 
 * @return int 
 */
int decompile_inst(cpu_state* cpu, uint16_t* next_inst);

/**
 * @brief Function to write data to the IO port
 * 
 * @param port where the data is written to
 * @param data which is written
 */
void io_machine_OUT(uint8_t port, uint16_t data);

/**
 * @brief Function to read a byte of data from the port
 * 
 * @param port from which the data is read
 * @return uint8_t 
 */
uint8_t io_machine_IN(uint8_t port);

/**
 * @brief Print the state of CPU
 * 
 * @param cpu 
 */
void print_state(const cpu_state cpu);

#endif