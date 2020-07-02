#ifndef _GAME_HPP
#define _GAME_HPP

#include "coord.hpp"
#include "constants.hpp"
#include <array>
#include <unordered_map>
#include <vector>

class Game {
public:
    Game();
    ~Game();
    void load_pattern(int pattern_index);
    void step();
    void print_cells();

    array<unordered_map<Coord, unsigned int>, NUM_PATTERNS> patterns;
    array<int, NUM_PATTERNS> pattern_frames;
    unordered_map<Coord, unsigned int> lookup;
    int frames;

private:
    unsigned int count_neighbours(Coord coord, unordered_map<Coord, unsigned int> *cache);
    bool on_grid(Coord cood);
    vector<Coord> get_keys();
    
    static const array<pair<int, int>, 8> neighbourhood;
};

#endif
