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

#define NUM_ROM_FILES   4
#define ROM_CHUNK_SIZE  0x800
#define TWO_POW_SIXTEEN 1<<15

// Major Helper Functions
int* mmap_invaders(char *path, void* base);
// Helper of Helpers Functions
int check_open_roms(char *path, int* FDs);


int main(){
    DEBUG_PRINT("%s\n", "PRKS 8080 Emulator to run Space Invaders....");
    WARN(0, "%s\n", "Hello from Warning....")

    // initializing a New CPU instance
    cpu_state* cpu = init_cpu_8080(0);              // For now don't know where PC initially points
    cpu->base = aligned_alloc(TWO_POW_SIXTEEN, UINT16_MAX);   // Alloc aligned 64 KB

    // memory Map the ROM
    char* rom_path = "./invaders_rom";
    int* rom_FDs;
    if ((rom_FDs = mmap_invaders(rom_path, cpu->base)) == NULL){
        fprintf(stderr, "Critical Error: Rom Load Failed.\n");
        exit(-1);
    }

    // TODO: Do the exec here.

    // Unmap the files and free the buffers.
    for(uint8_t itr=0; itr<NUM_ROM_FILES; itr++){ // Map h, g, f ,e <0-3>
        munmap(cpu->base+(ROM_CHUNK_SIZE*itr), ROM_CHUNK_SIZE); // UnMap invaders.x
        close(rom_FDs[itr]);
    }
    free(cpu->base);
    free(cpu);
    return 0;
}

// Helper Functions

/**
 * @brief Memory maps the invaders ROM into the correct memory locations.
 * The way memory is mapped is documented at: http://www.emutalk.net/threads/38177-Space-Invaders
 * 
 * @param path to the folder containing the 4 ROM Chunks
 * @param base pointer to the CPU instance executing the ROM
 * @return int* NULL if fail, array of file descriptors if pass
 */
int* mmap_invaders(char *path, void* base){
    assert(base!=NULL);
    assert(path!=NULL);
    DEBUG_PRINT("Invaders Path: %s, BasePTR: %p\n", path, base);

    // Check & Open All Roms
    int* FDs = (int *)calloc(NUM_ROM_FILES, sizeof(int));
    if(!check_open_roms(path, FDs)){
        WARN(0, "%s\n", "ROM open failed.\n");
        return 0;
    }

    // Memory mapping
    for(uint8_t itr=0; itr<NUM_ROM_FILES; itr++){ // Map h, g, f ,e <0-3>
        mmap(base+(ROM_CHUNK_SIZE*itr), ROM_CHUNK_SIZE, PROT_READ, MAP_PRIVATE, FDs[itr], 0); // Map invaders.x
    }

    return FDs;
}

// Helper of Helper Functions

int check_open_roms(char* path, int* FDs){
    struct stat mmapstat;
    char file_path[256];
    
    sprintf(file_path, "%s/%s", path, "invaders.h");
    if (stat(file_path, &mmapstat) == -1) {
        WARN(0, "%s\n", "stat failure");
        return 0;
    }
    if ((FDs[0] = open(file_path, O_RDONLY)) == -1) {
        WARN(0, "%s\n", "open failure");
        return 0;
   }

   sprintf(file_path, "%s/%s", path, "invaders.g");
    if (stat(file_path, &mmapstat) == -1) {
        WARN(0, "%s\n", "stat failure");
        return 0;
    }
    if ((FDs[1] = open(file_path, O_RDONLY)) == -1) {
        WARN(0, "%s\n", "open failure");
        return 0;
   }

   sprintf(file_path, "%s/%s", path, "invaders.f");
    if (stat(file_path, &mmapstat) == -1) {
        WARN(0, "%s\n", "stat failure");
        return 0;
    }
    if ((FDs[2] = open(file_path, O_RDONLY)) == -1) {
        WARN(0, "%s\n", "open failure");
        return 0;
   }

   sprintf(file_path, "%s/%s", path, "invaders.e");
    if (stat(file_path, &mmapstat) == -1) {
        WARN(0, "%s\n", "stat failure");
        return 0;
    }
    if ((FDs[3] = open(file_path, O_RDONLY)) == -1) {
        WARN(0, "%s\n", "open failure");
        return 0;
   }

   return 1;
}