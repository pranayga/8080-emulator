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

static port_IO space_docks;

int main(){
    DEBUG_PRINT("%s\n", "PRKS 8080 Emulator to run Space Invaders....");
    
    // initializing a New CPU instance
    cpu_state* cpu = init_cpu_8080(ROM_OFFSET, &space_IN, &space_OUT);                                         // For now don't know where PC initially points
    invaders_window* game_window = init_game_window();
    if(game_window->window == 0x0){
        printf("Critical: Error opening Game window.\n");
        exit (-1);
    }

    // Init PORT IO
    space_docks.port_0 = 0x0E; // Base
    space_docks.port_1 = 0x09; // Base + Credit
    space_docks.port_2 = 0x03; // SIX spaceShips

    // The code only supports SDL_PIXELFORMAT_RGB888 window encoding
    if(game_window->surf->format->format != SDL_PIXELFORMAT_RGB888){
        printf("Window is using nonstandard PIXEL format. Please use SDL_PIXELFORMAT_RGB888\n");
    }

    // memory Map the ROM
    char* rom_path = "./invaders_rom";
    int rom_FD;
    if ((rom_FD = copy_invaders_rom(rom_path, cpu)) == -1){
        fprintf(stderr, "Critical Error: Rom Load Failed.\n");
        exit(-1);
    }

    // Init the screen to white color
    uint32_t *pixels = game_window->surf->pixels;
    for(uint32_t y = 0; y < WINDOW_HEIGHT; y++){
        for(uint32_t x = 0; x < WINDOW_WIDTH; x++){
            set_pixel(pixels, x, y, 1); 
        }
    }
    SDL_UpdateWindowSurface(game_window->window);

    // Setup the Diplay Update Timer with CB
    game_window->vram_timer = SDL_AddTimer(VRAM_DELAY, update_vram_cb, NULL);

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

    DEBUG_PRINT("Do I have to lock: %x\n", SDL_MUSTLOCK(game_window->surf));

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

uint8_t space_IN(uint8_t port){
    DEBUG_PRINT("Space PORT_%x IN wrapper Triggered!\n", port);
    switch (port)
    {
    case 0:
        return space_docks.port_0;
    case 1:
        return space_docks.port_1;
    case 2:
        return space_docks.port_2;
    case 3:
        assert(space_docks.shift_config <= 7);
        uint8_t temp = (uint8_t)((space_docks.hidden_reg)>>(8-space_docks.shift_config));
        return temp;
    default:
        DEBUG_PRINT("Unusual Port_%x READ access.\n", port);
        exit(-10);
    }
    return 0;
}

void space_OUT(uint8_t port, uint8_t data){
    DEBUG_PRINT("Space PORT_%x OUT:%x wrapper Triggered!\n", port, data);
    switch (port)
    {
    case 2:
        assert(data<=7);
        space_docks.shift_config = data;
        break;
    case 3:
        space_docks.port_3 = data;
        break;
    case 4:
        space_docks.y = space_docks.x;
        space_docks.x = data;
        break;
    case 5:
        space_docks.port_5 = data;
        break;
    case 6:
        // Ignore Watchdog writes;
        break;
    default:
        DEBUG_PRINT("Unusual Port_%x WRITE:%x access.\n", port, data);
        exit(-10);
    }
}

void process_key_event(SDL_KeyboardEvent key_event){
    if(key_event.type == SDL_KEYDOWN){
        switch (key_event.keysym.sym)
        {
        case CREDIT_COIN:
            space_docks.port_1 |= 0x1;
            break;
        case P1_START:
            space_docks.port_1 |= 0x1 << 0x2;
            break;
        case P2_START:
            space_docks.port_1 |= 0x1 << 0x1;
            break;
        case P1_LEFT:
            space_docks.port_1 |= 0x1 << 0x5;
            break;
        case P1_RIGHT:
            space_docks.port_1 |= 0x1 << 0x6;
            break;
        case P1_SHOOT:
            space_docks.port_1 |= 0x1 << 0x4;
            break;
        case P2_LEFT:
            space_docks.port_2 |= 0x1 << 0x5;
            break;
        case P2_RIGHT:
            space_docks.port_1 |= 0x1 << 0x6;
            break;
        case P2_SHOOT:
            space_docks.port_1 |= 0x1 << 0x4;
            break;
        default:
            break;
        }
    } else {
        switch (key_event.keysym.sym)
        {
        case CREDIT_COIN:
            space_docks.port_1 &= ~0x1;
            break;
        case P1_START:
            space_docks.port_1 &= ~(0x1 << 0x2);
            break;
        case P2_START:
            space_docks.port_1 &= ~(0x1 << 0x1);
            break;
        case P1_LEFT:
            space_docks.port_1 &= ~(0x1 << 0x5);
            break;
        case P1_RIGHT:
            space_docks.port_1 &= ~(0x1 << 0x6);
            break;
        case P1_SHOOT:
            space_docks.port_1 &= ~(0x1 << 0x4);
            break;
        case P2_LEFT:
            space_docks.port_2 &= ~(0x1 << 0x5);
            break;
        case P2_RIGHT:
            space_docks.port_1 &= ~(0x1 << 0x6);
            break;
        case P2_SHOOT:
            space_docks.port_1 &= ~(0x1 << 0x4);
            break;
        default:
            break;
        }
    }
}

/***** SDL Helpers ***/

void process_SDL_event(UNUSED cpu_state *cpu, invaders_window *game_window){
    switch (game_window->event.type)
    {
    case SDL_QUIT:
        game_window->quit_event = 1;
        break;
    case SDL_USEREVENT:
        /* and now we can call the function we wanted to call in the timer but couldn't because of the multithreading problems */
        if(game_window->event.user.code == 0){
            // Pend the Intt based on event
            cpu->pend_intt |= (uintptr_t)(game_window->event.user.data1);
            // Update App window at Every Full update
            if((uintptr_t)(game_window->event.user.data1) == full_2){
                render_vram(cpu, game_window->pixels);
                SDL_UpdateWindowSurface(game_window->window);
            }
        }
        break;
    case SDL_KEYDOWN:
        DEBUG_PRINT("Key: %c pressed, isfake: %d.\n", game_window->event.key.keysym.sym, game_window->event.key.repeat);
        if(!game_window->event.key.repeat){
            process_key_event(game_window->event.key);
        }
        break;
    case SDL_KEYUP:
        DEBUG_PRINT("Key: %c Released.\n", game_window->event.key.keysym.sym);
        process_key_event(game_window->event.key);
        break;
    default:
        DEBUG_PRINT("%s\n", "Unhandled Event!");
    }
}

uint32_t update_vram_cb(UNUSED uint32_t interval, UNUSED void *param){
    static uintptr_t update_state = half_1;

    SDL_Event event;
    SDL_UserEvent userevent;

    userevent.type = SDL_USEREVENT;
    userevent.code = 0;
    userevent.data1 = (void*)update_state;

    event.type = SDL_USEREVENT;
    event.user = userevent;

    SDL_PushEvent(&event);

    update_state = update_state == half_1 ? full_2 : half_1; // Toggling state
    return(interval);
}

void render_vram(cpu_state *cpu, uint32_t *pixels){
    uint32_t pix_index = 0;
    uintptr_t vram_start = (uintptr_t)mem_ref(&cpu->mem, VRAM_OFFSET);
    uintptr_t vram_end = vram_start + VRAM_SIZE;
    uint8_t vram_data;
    
    // Inflate
    static uint32_t temp_buff[WINDOW_WIDTH*WINDOW_HEIGHT];
    for(;vram_start < vram_end; vram_start++){
        vram_data = *(uint8_t*)(vram_start);
        temp_buff[pix_index++] = (vram_data & 0x1) ? GREEN_PIXEL : BLACK_PIXEL;
        temp_buff[pix_index++] = (vram_data & 0x2) >> 1 ? GREEN_PIXEL : BLACK_PIXEL;
        temp_buff[pix_index++] = (vram_data & 0x4) >> 2 ? GREEN_PIXEL : BLACK_PIXEL;
        temp_buff[pix_index++] = (vram_data & 0x8) >> 3 ? GREEN_PIXEL : BLACK_PIXEL;
        temp_buff[pix_index++] = (vram_data & 0x16) >> 4 ? GREEN_PIXEL : BLACK_PIXEL;
        temp_buff[pix_index++] = (vram_data & 0x32) >> 5 ? GREEN_PIXEL : BLACK_PIXEL;
        temp_buff[pix_index++] = (vram_data & 0x64) >> 6 ? GREEN_PIXEL : BLACK_PIXEL;
        temp_buff[pix_index++] = (vram_data & 0x128) >> 7 ? GREEN_PIXEL : BLACK_PIXEL;
    }

    // Rotate
    pix_index = 0;
    for(int16_t x = (WINDOW_WIDTH - 1); x >= 0 ; x--){
        for(uint16_t y = 0; y < WINDOW_HEIGHT; y++){
            pixels[pix_index++] = temp_buff[x + (y * WINDOW_WIDTH)];
        }
    }
}

void set_pixel(uint32_t *pixels, uint32_t x, uint32_t y, UNUSED uint8_t state){
    pixels[x + y * WINDOW_WIDTH] = state ? ((y < 40 || y > 200) ? WHITE_PIXEL : GREEN_PIXEL) : BLACK_PIXEL;
}

invaders_window* init_game_window(){
    
    invaders_window* game_window = (invaders_window*)calloc(1, sizeof(invaders_window));    // Game Window

    // attempt to initialize graphics and timer system
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "error initializing SDL: %s\n", SDL_GetError());
        return 0x0;
    }

    game_window->window = SDL_CreateWindow("Space Invaders! Call Pandu",
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       WINDOW_HEIGHT, WINDOW_WIDTH, 0);
    if (!game_window->window)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "error creating window: %s\n", SDL_GetError());
        SDL_Quit();
	    return 0x0;
    }

    game_window->surf = SDL_GetWindowSurface(game_window->window);
    if (game_window->surf == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "SDL_GetWindowSurface() failed: %s", SDL_GetError());
        exit(1);
    }

    game_window->pixels = game_window->surf->pixels;

    return game_window;
}

void destroy_game_window(invaders_window *game_window){
    // clean up resources before exiting
    SDL_DestroyWindow(game_window->window);
    free(game_window);
    SDL_Quit();
}