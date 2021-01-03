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
#include <signal.h>

#include "debug.h"
#include "space.h"

int main(){
    DEBUG_PRINT("%s\n", "PRKS 8080 Emulator to run Space Invaders....");
    
    // initializing a New CPU instance
    cpu_state* cpu = init_cpu_8080(ROM_OFFSET);                                         // For now don't know where PC initially points
    invaders_window* game_window = init_game_window();
    if(game_window->window == 0x0){
        printf("Critical: Error opening Game window.\n");
        exit (-1);
    }

    // memory Map the ROM
    char* rom_path = "./invaders_rom";
    int rom_FD;
    if ((rom_FD = copy_invaders_rom(rom_path, cpu)) == -1){
        fprintf(stderr, "Critical Error: Rom Load Failed.\n");
        exit(-1);
    }

    while(!game_window->quit_event){
        if(!cpu->halt && exec_inst(cpu) == 1){
            // Stuff to do on successful exec
        } else {
            cpu->halt = 1;  // Explicity halt the CPU incase something 
                            // fails
        }

        // Check if there have been any events
        if(SDL_PollEvent(&(game_window->event))){
            process_SDL_event(cpu, game_window);
        }
    }

    // free the buffers.
    DEBUG_PRINT("%s\n", "Freeing SDL Mem");
    destroy_game_window(game_window);
    close(rom_FD);
    DEBUG_PRINT("%s\n", "Freeing RAM");
    free(cpu->mem.base);
    DEBUG_PRINT("%s\n", "Freeing Cpu");
    free(cpu);
    return 0;
}

// Helper Functions
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

void process_SDL_event(UNUSED cpu_state *cpu, invaders_window *game_window){
    DEBUG_PRINT("Recieved SDL Event: %x\n", game_window->event.type);
    switch (game_window->event.type)
    {
    case SDL_QUIT:
        game_window->quit_event = 1;
        break;
    default:
        DEBUG_PRINT("%s\n", "Unhandled Event!");
    }
}

invaders_window* init_game_window(){
    
    invaders_window* game_window = (invaders_window*)calloc(1, sizeof(invaders_window));    // Game Window

    // attempt to initialize graphics and timer system
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0)
    {
        printf("error initializing SDL: %s\n", SDL_GetError());
        return 0x0;
    }

    game_window->window = SDL_CreateWindow("Space Invaders! Call Pandu",
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!game_window->window)
    {
        printf("error creating window: %s\n", SDL_GetError());
        SDL_Quit();
	    return 0x0;
    }
    return game_window;
}

void destroy_game_window(invaders_window *game_window){
    // clean up resources before exiting
    SDL_DestroyWindow(game_window->window);
    free(game_window);
    SDL_Quit();
}