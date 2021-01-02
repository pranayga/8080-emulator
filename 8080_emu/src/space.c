/**
 * @file space.c
 * @author Pranay Garg (pranayga@andrew.cmu.edu)
 * @brief The main driver to run invaders, the ATARI game.
 * @version 0.1
 * @date 2020-12-20
 * 
 * @copyright Copyright (c) 2020
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <inttypes.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <assert.h>
#include "cpu_8080.h"
#include "debug.h"

#define ALIGNED_PREFIX (1<<16)
#define ROM_OFFSET 0x100

// Major Helper Functions
int copy_invaders_rom(char *path, cpu_state* cpu);

int main(){
    DEBUG_PRINT("%s\n", "PRKS 8080 Emulator to run Space Invaders....");
    WARN(0, "%s\n", "Hello from Warning....")

    // initializing a New CPU instance
    cpu_state* cpu = init_cpu_8080(ROM_OFFSET);  // For now don't know where PC initially points

    // memory Map the ROM
    char* rom_path = "./invaders_rom";
    int rom_FD;
    if ((rom_FD = copy_invaders_rom(rom_path, cpu)) == -1){
        fprintf(stderr, "Critical Error: Rom Load Failed.\n");
        exit(-1);
    }

    // Exec ROM FILE
    // printf("Starting Decompiling......\n");
    // uint16_t next_PC = 0;
    // while(decompile_inst(cpu, &next_PC) == 1){}
    
    // printf("Starting Exec......\n");
    // UNUSED char temp;
    // int num_to_exec;
    // printf("Enter Num of Inst to skip: ");
    // scanf("%d", &num_to_exec);
    // num_to_exec--;
    // while(exec_inst(cpu) == 1){
    //     if((--num_to_exec)>0){
    //         continue;
    //     }
    //     print_state(*cpu);
    //     temp = getc(stdin);
    // }

    printf("Starting Exec......\n");
    while(exec_inst(cpu) == 1){
        if(cpu->PC == 0x0689){
            printf("Emulator Failed. Dumping & Dying.....\n");
            print_state(*cpu);
            exit(-2);
        }
    }

    // free the buffers.
    close(rom_FD);
    free(cpu->mem.base);
    free(cpu);
    return 0;
}

// Helper Functions

/**
 * @brief Copies the invaders ROM into the correct memory locations.
 * The way memory is mapped is documented at: http://www.emutalk.net/threads/38177-Space-Invaders
 * 
 * @param path to the folder containing the ROM
 * @param cpu pointer to the CPU instance executing the ROM
 * @return int -1 if fail, array of file descriptors if pass
 */
int copy_invaders_rom(char *path, cpu_state* cpu){
    assert(cpu!=NULL);
    assert(path!=NULL);

    struct stat romstats;
    char file_path[256];
    int FD;
    
    // sprintf(file_path, "%s/%s", path, "invaders.hgfe");
    sprintf(file_path, "%s", "debug.bin");
    if (stat(file_path, &romstats) == -1) {
        WARN(0, "%s\n", "stat failure");
        return 0;
    }
    cpu->rom_size = romstats.st_size;

    if ((FD = open(file_path, O_RDONLY, S_IRUSR | S_IRGRP)) == -1) {
        WARN(0, "%s\n", "open failure");
        return 0;
   }

    // Memory mapping
    cpu->mem.base = aligned_alloc(ALIGNED_PREFIX, UINT16_MAX);
    if (read(FD, cpu->mem.base + ROM_OFFSET, cpu->rom_size) == 0){
        WARN(0, "%s\n", "ROM_LOAD_FAILED.\n");
        return 0;
    }// Map invaders.efgh
    
    return FD;
}