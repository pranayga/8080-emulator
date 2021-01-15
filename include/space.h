/**
 * @file space.h
 * @author Pranay Garg (pranayga@andrew.cmu.edu)
 * @brief File containing #defines for spaceinvaders wrapper
 * @version 0.1
 * @date 2021-01-14
 * 
 */
#ifndef SPACE_H
#define SPACE_H

#include "cpu_8080.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>

#define ALIGNED_PREFIX (1<<16)  /** Prefix to get 16bit aligned memory */
#define ROM_OFFSET  0x0     /** offset to load the ROM **/
#define VRAM_OFFSET 0x2400  /** Location of VRAM **/
#define VRAM_SIZE   0x1C00  /** Size of VRAM **/
#define VRAM_DELAY  0x9     /** 112 Hz for 1000 ms */
#define half_1      0x2     /** Pending Intt to Call RST 1 */
#define full_2      0x4     /** Pending Intt to Call RST 2 */
// Invaders Stuff
#define WINDOW_WIDTH (256)  /** Window Width, later rotated */
#define WINDOW_HEIGHT (224) /** Window Height, later rotated */
#define GREEN_PIXEL 0xFF00  /** RGB888 GREEN Value */
#define BLACK_PIXEL 0x0     /** RGB888 Black Value */
///@{
/** Key Mapping TODO */
#define CREDIT_COIN SDLK_c
#define P1_START    SDLK_RETURN
#define P2_START    SDLK_s
#define P1_LEFT     SDLK_LEFT
#define P1_RIGHT    SDLK_RIGHT
#define P1_SHOOT    SDLK_UP
#define P2_LEFT     SDLK_a
#define P2_RIGHT    SDLK_d
#define P2_SHOOT    SDLK_w
///@}

/**
 * @brief invader window struct which keeps track of all the
 * window related state including the SDL components and timers
 * 
 */
typedef struct {
    SDL_Window *window;     /**< SDL Window Pointer */
    /** SDL surface corresponding to the SDL Window */
    SDL_Surface *surf;      
    uint32_t* pixels;       /**< Pixels below surf. Chnages on Resize */
    uint8_t quit_event;     /**< quit event triggered */
    SDL_Event event;        /**< temp event under processing */
    SDL_TimerID vram_timer; /**< vram_time to trigger at VRAM_DELAY */
} invaders_window;

/**
 * @brief Imaginary external Ports used to communicate with
 * the player and hardware.
 */
typedef struct {
    ///@{
    /** Input Ports */
    uint8_t port_0;
    uint8_t port_1;
    uint8_t port_2;
    ///@}
    ///@{
    /** Output Ports */
    uint8_t port_3;
    uint8_t port_5;
    ///@}

    // Shift Regs
    uint8_t shift_config;   /**< Shift Reg config to control Port 2 reads */
    /** Union to redirect Port 4 writes and Port 2 reads */
    union{
        struct{
            uint8_t y;      /**< least significant byte */
            uint8_t x;      /**< most significant byte  */
        };
        uint16_t hidden_reg;    /**< abstracted 15 byte register */
    };
} port_IO;

/**
 * @brief Copies the invaders ROM into the correct memory locations.
 * The way memory is mapped is documented at: http://www.emutalk.net/threads/38177-Space-Invaders
 * 
 * @param path to the folder containing the ROM
 * @param cpu pointer to the CPU instance executing the ROM
 * @return int -1 if fail, array of file descriptors if pass
 */
int copy_invaders_rom(char *path, cpu_state* cpu);

/**
 * @brief Space_IN, IN instruction function callback. Handles
 * all the `IN PORT` instruction IO
 * @note Intercept READs on port and handles according to Spec
 * @param port PORT to read/write
 * @return uint8_t Data Read
 */
uint8_t space_IN(uint8_t port);

/**
 * @brief Space_OUT, data to write callback
 * @note Handles the write according to the callback behaviour
 * @param port PORT to read/write
 * @param data Data to write
 */
void space_OUT(uint8_t port, uint8_t data);

/**
 * @brief processes a key_event by setting
 * the IO_Ports in order to pass the input into the
 * CPU state
 * @param key_event the key event being pressed/released
 */
void process_key_event(SDL_KeyboardEvent key_event);

// SDL Init
/**
 * @brief Set the pixel object to a static value.
 * 
 * @param pixels underlying uint32_t[] pointer for the SDL surface
 * @param x index to update
 * @param y index to update
 * @param state 0 for OFF, 1 for ON
 */
void set_pixel(uint32_t *pixels, uint32_t x, uint32_t y, uint8_t state);

/**
 * @brief Renders the Vram into the surface Pixel
 * 
 * @param cpu cpu emulating the game
 * @param pixels underlying uint32_t[] pointer for the SDL surface
 */
void render_vram(cpu_state *cpu, uint32_t *pixels);

/**
 * @brief callback which is triggered when scan line reaches
 * 1/2 of the display, or at 120Hz
 * 
 * @param interval Interval to trigger at
 * @param param NULL
 * @return uint32_t Interval
 */
uint32_t update_vram_cb(uint32_t interval, UNUSED void *param);

/**
 * @brief Initializes the SDL game window
 * 
 * @return invaders_window* pointer to the game window object
 */
invaders_window* init_game_window();

/**
 * @brief Destorys the game window object
 * 
 * @param game_window 
 */
void destroy_game_window(invaders_window *game_window);

/**
 * @brief Process the incoming SDL event. Called whenever a 
 * new event is seen during the polling process
 * 
 * @param cpu to modify using the event
 * @param game_window relating to the generated event
 */
void process_SDL_event(cpu_state *cpu, invaders_window *game_window);

#endif