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
    void* base; /**< Base pointer, points to start of 16K memory chunk */
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
 * @brief Lowest level memory read access abstraction. Typecasts
 * the offset to void* + base to get the actual pointer. Populates the
 * value into val.
 * 
 * @param offset from the base ptr in bytes
 * @param cpu cpu context to exec with
 * @return uint8_t, byte read from the memory
 */
uint8_t mem_read(const cpu_state cpu, uint16_t offset);

/**
 * @brief Lowest level memory write access abstraction. Typecasts
 * the offset to void* + base to get the actual pointer. Populates from
 * val into the memory.
 * 
 * @param offset from the base ptr in bytes
 * @param val to write onto memory
 * @param cpu cpu context to exec with
 */
void mem_write(const cpu_state cpu, uint16_t offset, uint8_t val);

/**
 * @brief Print the state of CPU
 * 
 * @param cpu 
 */
void print_state(const cpu_state cpu);

/**
 * @brief Enum of each and every opcode.
 */
typedef enum {
    nop_instt = 0x0,
} Instruction_OPCODE;

#endif