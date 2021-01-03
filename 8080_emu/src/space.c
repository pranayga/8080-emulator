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

#include <signal.h>

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>

#define ALIGNED_PREFIX (1<<16)
#define ROM_OFFSET 0x0

// Invaders Stuff
#define WINDOW_WIDTH (256)
#define WINDOW_HEIGHT (224)

// Major Helper Functions
int copy_invaders_rom(char *path, cpu_state* cpu);
// SDL Init
SDL_Window * init_game_window();
void destroy_game_window(SDL_Window *win);

int main(){
    DEBUG_PRINT("%s\n", "PRKS 8080 Emulator to run Space Invaders....");

    SDL_Window *game_win = init_game_window();
    if(game_win == 0x0){
        printf("Critical: Error opening Game window.\n");
        exit (-1);
    }
    
    // wait a few seconds
    SDL_Delay(5000);
    
    // Destroy
    destroy_game_window(game_win);
    
    // initializing a New CPU instance
    cpu_state* cpu = init_cpu_8080(ROM_OFFSET);  // For now don't know where PC initially points

    // memory Map the ROM
    char* rom_path = "./invaders_rom";
    int rom_FD;
    if ((rom_FD = copy_invaders_rom(rom_path, cpu)) == -1){
        fprintf(stderr, "Critical Error: Rom Load Failed.\n");
        exit(-1);
    }

    // Actual Emulation Code
    // printf("Starting Exec......\n");
    // while(exec_inst(cpu) == 1){}

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
    cpu->mem.base = aligned_alloc(ALIGNED_PREFIX, UINT16_MAX);
    if (read(FD, cpu->mem.base + ROM_OFFSET, cpu->rom_size) == 0){
        WARN(0, "%s\n", "ROM_LOAD_FAILED.\n");
        return 0;
    }// Map invaders.efgh
    
    return FD;
}

/***** SDL Helpers ***/

SDL_Window * init_game_window(){
    // attempt to initialize graphics and timer system
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0)
    {
        printf("error initializing SDL: %s\n", SDL_GetError());
        return 0x0;
    }

    SDL_Window* win = SDL_CreateWindow("Hello, CS50!",
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!win)
    {
        printf("error creating window: %s\n", SDL_GetError());
        SDL_Quit();
	    return 0x0;
    }
    return win;
}

void destroy_game_window(SDL_Window *win){
    // clean up resources before exiting
    SDL_DestroyWindow(win);
    SDL_Quit();
}