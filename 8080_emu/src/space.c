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

// Major Helper Functions
int mmap_invaders(char *path, cpu_state* cpu);

int main(){
    DEBUG_PRINT("%s\n", "PRKS 8080 Emulator to run Space Invaders....");
    WARN(0, "%s\n", "Hello from Warning....")

    // initializing a New CPU instance
    cpu_state* cpu = init_cpu_8080(0);  // For now don't know where PC initially points

    // memory Map the ROM
    char* rom_path = "./invaders_rom";
    int rom_FD;
    if ((rom_FD = mmap_invaders(rom_path, cpu)) == -1){
        fprintf(stderr, "Critical Error: Rom Load Failed.\n");
        exit(-1);
    }

    // Set PC to start of the ROM text Offset
    cpu->PC = 0;
    // Exec ROM FILE
    while(exec_inst(cpu)==1){}

    // Unmap the files and free the buffers.
    munmap(cpu->base, cpu->rom_size); // UnMap invaders.x
    close(rom_FD);
    
    free(cpu);
    return 0;
}

// Helper Functions

/**
 * @brief Memory maps the invaders ROM into the correct memory locations.
 * The way memory is mapped is documented at: http://www.emutalk.net/threads/38177-Space-Invaders
 * 
 * @param path to the folder containing the 4 ROM Chunks
 * @param cpu pointer to the CPU instance executing the ROM
 * @return int -1 if fail, array of file descriptors if pass
 */
int mmap_invaders(char *path, cpu_state* cpu){
    assert(cpu!=NULL);
    assert(path!=NULL);

    struct stat romstats;
    char file_path[256];
    int FD;
    
    sprintf(file_path, "%s/%s", path, "invaders.hgfe");
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
    if ((cpu->base = mmap((caddr_t)0, cpu->rom_size, PROT_READ, 
                                    MAP_PRIVATE, FD, 0)) ==  MAP_FAILED){
            WARN(0, "%s\n", "MAPPING FAILED.\n");
            return 0;
    }// Map invaders.efgh
    
    return FD;
}