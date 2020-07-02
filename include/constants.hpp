#ifndef _CONSTANTS_HPP
#define _CONSTANTS_HPP

#include "SDL2/SDL.h"
#include <array>

using namespace std;

#define WIDTH 800
#define HEIGHT 480
#define CELL_SIZE 5
#define HCELLS (WIDTH / CELL_SIZE)
#define VCELLS (HEIGHT / CELL_SIZE)
#define OFF_GRID_CELLS 50
#define NUM_PATTERNS 28
#define NUM_COLOURS 12
#define FRAME_DELAY 30
#define DOUBLE_TAP_INTERVAL 325
#define BLOCK_TYPES 3
#define DEFAULT_PATTERN_FRAMES 1000
#define STARTUP_DELAY 1000

const SDL_Color COLOUR_WHEEL[NUM_COLOURS] = {
    {255, 0, 0, 255},
    {255, 127, 0, 255},
    {255, 255, 0, 255},
    {127, 255, 0, 255},
    {0, 255, 0, 255},
    {0, 255, 127, 255},
    {0, 255, 255, 255},
    {0, 127, 255, 255},
    {0, 0, 255, 255},
    {127, 0, 255, 255},
    {255, 0, 255, 255},
    {255, 0, 127, 255}
};

#endif
