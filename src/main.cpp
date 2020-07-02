
#include <unordered_map>
#include "SDL2/SDL.h"
#include "SDL2/SDL2_gfxPrimitives.h"
#include "coord.hpp"
#include "game.hpp"
#include "constants.hpp"
#include <omp.h>
#include <chrono>
#include <iostream>

using namespace std;

//prototypes
void render(SDL_Renderer *ren, SDL_Texture *tex);
void draw_cell_custom(SDL_Renderer *ren, Sint16 _x, Sint16 _y, Sint16 radius, SDL_Color colour);
void update_tex(SDL_Texture *tex, unordered_map<Coord, unsigned int> *cells, SDL_Renderer *ren, int block_type);
Uint32 single_touch_callback(Uint32 interval, void *data);
void startup_delay(SDL_Renderer *ren);

void render(SDL_Renderer *ren, SDL_Texture *tex) {
    SDL_SetRenderTarget(ren, NULL);
    SDL_RenderCopy(ren, tex, NULL, NULL);
    SDL_RenderPresent(ren);
}

void draw_cell_custom(SDL_Renderer *ren, Sint16 _x, Sint16 _y, Sint16 radius, SDL_Color colour) {
    SDL_SetRenderDrawColor(ren, colour.r, colour.g, colour.b, colour.a);
    Sint16 x = radius - 1;
    Sint16 y = 0;
    Sint16 tx = 1;
    Sint16 ty = 1;
    Sint16 err = tx - (radius << 1);
    
    while (x >= y) {
        SDL_RenderDrawPoint(ren, _x + x, _y - y);
        SDL_RenderDrawPoint(ren, _x + x, _y + y);
        SDL_RenderDrawPoint(ren, _x - x, _y - y);
        SDL_RenderDrawPoint(ren, _x - x, _y + y);
        SDL_RenderDrawPoint(ren, _x + y, _y - x);
        SDL_RenderDrawPoint(ren, _x + y, _y + x);
        SDL_RenderDrawPoint(ren, _x - y, _y - x);
        SDL_RenderDrawPoint(ren, _x - y, _y + x);
        
        if (err <= 0) {
            y++;
            err += ty;
            ty += 2;
        }
        if (err > 0) {
            x--;
            tx += 2;
            err += tx - (radius << 1);
        }
    }
}

void update_tex(SDL_Texture *tex, unordered_map<Coord, unsigned int> *cells, SDL_Renderer *ren) { //, int block_type) {
    SDL_SetRenderTarget(ren, tex);
    SDL_SetRenderDrawColor(ren, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(ren);

    for (pair<Coord, unsigned int> cell : *cells) {
        Coord coord = cell.first;
        unsigned int colour_index = cell.second;
        SDL_Color colour = COLOUR_WHEEL[colour_index];
        draw_cell_custom(ren, coord.x * CELL_SIZE + (CELL_SIZE / 2), coord.y * CELL_SIZE + (CELL_SIZE / 2), CELL_SIZE / 2, colour);
        
        //const int padding = 0;
        //switch (block_type) {        
            //case 0:
            //draw_cell_custom(ren, coord.x * CELL_SIZE + (CELL_SIZE / 2), coord.y * CELL_SIZE + (CELL_SIZE / 2), CELL_SIZE / 2, colour);
            //break;
            
            //case 1:
            //boxRGBA(ren, coord.x * CELL_SIZE + padding, coord.y * CELL_SIZE + padding, coord.x * CELL_SIZE + CELL_SIZE - padding, coord.y * CELL_SIZE + CELL_SIZE - padding, colour.r, colour.g, colour.b, colour.a);
            //break;
            
            //case 2:
            //circleRGBA(ren, coord.x * CELL_SIZE + (CELL_SIZE / 2), coord.y * CELL_SIZE + (CELL_SIZE / 2), CELL_SIZE / 2, colour.r, colour.g, colour.b, colour.a);
            //break;
        //}
    }
}

Uint32 single_touch_callback(Uint32 interval, void *data) {
    int *consec_taps = (int *) data;
    if (*consec_taps == 1) {
        SDL_Event event;
        event.type = SDL_KEYDOWN;
        event.key.keysym.sym = SDLK_DOWN;
        SDL_PushEvent(&event);
    }
    
    return 0;
}

void startup_delay(SDL_Renderer *ren) {
    SDL_SetRenderTarget(ren, NULL);
    SDL_SetRenderDrawColor(ren, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(ren);
    SDL_RenderPresent(ren);
    SDL_Delay(STARTUP_DELAY);
}

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER))
    {
        printf("SDL_Init error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
    
    SDL_ShowCursor(0);
    SDL_Window *win = SDL_CreateWindow("Conway's Game of Life", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_FULLSCREEN);
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    SDL_Texture *tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_TARGET, WIDTH, HEIGHT);
    
    //wait a bit to allow the device to fully boot up
    //(prevents lag on first few frames)
    startup_delay(ren);

    Game game;
    int pattern_index = 0;
    game.load_pattern(pattern_index);
    
    SDL_Event e;
    bool quit = false;
    chrono::time_point<chrono::high_resolution_clock> start_time;
    chrono::time_point<chrono::high_resolution_clock> end_time;
    chrono::duration<double> delta;
    chrono::duration<double> frame_delay((double) FRAME_DELAY);
    double gap;
    //Uint32 last_tap = 0;
    //int consec_taps = 0;
    //int block_type = 0;
    
    update_tex(tex, &game.lookup, ren); //, block_type);
    start_time = chrono::high_resolution_clock::now();
    render(ren, tex);
    
    int frames = 0;
    while (!quit) {
        //draw next frame
        game.step();
        update_tex(tex, &game.lookup, ren); //, block_type);
        
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_KEYDOWN) {
                switch(e.key.keysym.sym) {
                case SDLK_ESCAPE:
                    quit = true;
                    break;
                case SDLK_DOWN:
                    //consec_taps = 0;
                    frames = 0;
                    //once we've cycled through all patterns, change up the block type
                    //if (pattern_index + 1 == NUM_PATTERNS) {
                        //block_type = (block_type + 1) % BLOCK_TYPES;
                    //}
                    pattern_index = (pattern_index + 1) % NUM_PATTERNS;
                    game.load_pattern(pattern_index);
                    break;
                case SDLK_UP:
                    //consec_taps = 0;
                    frames = 0;
                    pattern_index = (pattern_index - 1 + NUM_PATTERNS) % NUM_PATTERNS;
                    game.load_pattern(pattern_index);
                    break;
                
                case SDLK_z:
                    cout << pattern_index << " " << frames << endl;
                    break;
                }
            }
            
            else if (e.type == SDL_FINGERDOWN) {
                //last_tap = e.tfinger.timestamp;
                //if (consec_taps == 0) {
                    //consec_taps++;
                    //SDL_AddTimer(DOUBLE_TAP_INTERVAL, single_touch_callback, &consec_taps);
                //}
                //else if (e.tfinger.timestamp - last_tap < DOUBLE_TAP_INTERVAL) {
                    //consec_taps++;
                    //if (consec_taps >= 2) {
                        //consec_taps = 0;
                        //block_type = (block_type + 1) % BLOCK_TYPES;
                    //}
                //}
                
                frames = 0;
                pattern_index = (pattern_index + 1) % NUM_PATTERNS;
                game.load_pattern(pattern_index);
            }
        }
        
        //wait any remaining time before rendering next frame
        end_time = chrono::high_resolution_clock::now();
        delta = chrono::duration<double>(end_time - start_time);
        //note: must convert to milliseconds here
        gap = (FRAME_DELAY - delta.count() / 1000000.0f);
        if (gap > 0) {
            SDL_Delay((unsigned int) gap);
        }
        //render current frame
        start_time = chrono::high_resolution_clock::now();
        render(ren, tex);
        
        frames++;
        if (frames >= game.frames) {
            SDL_Event event;
            event.type = SDL_KEYDOWN;
            event.key.keysym.sym = SDLK_DOWN;
            SDL_PushEvent(&event);
        }
        
    }

    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    
    return EXIT_SUCCESS;
}
