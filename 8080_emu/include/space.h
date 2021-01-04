#ifndef SPACE_H
#define SPACE_H

#include "cpu_8080.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>

#define ALIGNED_PREFIX (1<<16)
#define ROM_OFFSET  0x0
#define VRAM_OFFSET 0x2400
#define VRAM_SIZE   0x1C00
#define VRAM_DELAY  0x25    // 50 Hz for 1000 ms
// Invaders Stuff
#define WINDOW_WIDTH (256)
#define WINDOW_HEIGHT (224)
#define WHITE_PIXEL 0xFFFFFF
#define RED_PIXEL   0xFF0000
#define GREEN_PIXEL 0xFF00
#define BLUE_PIXEL  0xFF
#define BLACK_PIXEL 0x0

typedef struct {
    SDL_Window *window;
    SDL_Surface *surf;
    uint32_t* pixels;
    uint8_t quit_event;
    SDL_Event event;
    SDL_TimerID vram_timer;
} invaders_window;

/**
 * @brief Copies the invaders ROM into the correct memory locations.
 * The way memory is mapped is documented at: http://www.emutalk.net/threads/38177-Space-Invaders
 * 
 * @param path to the folder containing the ROM
 * @param cpu pointer to the CPU instance executing the ROM
 * @return int -1 if fail, array of file descriptors if pass
 */
int copy_invaders_rom(char *path, cpu_state* cpu);

// SDL Init
void set_pixel(uint32_t *pixels, uint32_t x, uint32_t y, uint8_t state);
void render_vram(cpu_state *cpu, uint32_t *pixels);
uint32_t update_vram_cb(uint32_t interval, void *param);
invaders_window* init_game_window();
void destroy_game_window(invaders_window *game_window);
void process_SDL_event(cpu_state *cpu, invaders_window *game_window);

#endif