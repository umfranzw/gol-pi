#include "game.hpp"
#include "constants.hpp"
#include "rle_reader.hpp"
#include <iostream>
#include <sstream>
#include <omp.h>

const array<pair<int, int>, 8> Game::neighbourhood = {
    make_pair(-1, 1), make_pair(0, 1), make_pair(1, 1),
    make_pair(-1, 0), make_pair(1, 0),
    make_pair(-1, -1), make_pair(0, -1), make_pair(1, -1)
};

Game::Game() {
    for (int i = 0; i < NUM_PATTERNS; i++) {
        stringstream ss;
        ss << "/home/pi/gol/patterns/" << i << ".rle";
        string filename = ss.str();
        RleReader reader(filename);
        this->patterns[i] = reader.lookup;
        this->pattern_frames[i] = reader.frames;
    }
}

Game::~Game() {
}

void Game::load_pattern(int pattern_index) {
    this->lookup = this->patterns[pattern_index];
    this->frames =  this->pattern_frames[pattern_index];
}

void Game::print_cells() {
    for (pair<Coord, unsigned int> cell : this->lookup) {
        Coord coord = cell.first;
        unsigned int colour = cell.second;
        cout << "(" << coord.x << ", " << coord.y << "): " << colour << endl;
    }
}

vector<Coord> Game::get_keys() {
    vector<Coord> keys;
    for (auto it = this->lookup.begin(); it != this->lookup.end(); it++) {
        keys.push_back(it->first);
    }

    return keys;
}

void Game::step() {
    unordered_map<Coord, unsigned int> new_sublookups[4];
    unordered_map<Coord, unsigned int> caches[4];
    vector<Coord> keys = this->get_keys();
    int chunk_size = keys.size() / 4;
#pragma omp parallel
    {
        int id = omp_get_thread_num();
        int start = chunk_size * id;
        int end = (id == 3) ? keys.size() : start + chunk_size;

        for (auto it = keys.begin() + start; it != keys.begin() + end; it++) {
            Coord coord = *it;
            unsigned int colour_index = this->lookup[coord];
            unsigned int neighbours = count_neighbours(coord, &caches[id]);
        
            if (neighbours == 2 || neighbours == 3) {
                new_sublookups[id][coord] = colour_index;
            }

            for (pair<int, int> offset : Game::neighbourhood) {
                Coord pos = Coord(coord.x + offset.first, coord.y + offset.second);
                if (this->lookup.find(pos) == this->lookup.end() && this->on_grid(pos)) {
                    if (this->count_neighbours(pos, &caches[id]) == 3) {
                        new_sublookups[id][pos] = (colour_index + 1) % NUM_COLOURS;
                    }
                }
            }
        }
    }

// #pragma omp parallel num_threads(3)
//     {
//         int id = omp_get_thread_num();
//         if (id < 2) {
//             int dest = id;
//             int src = dest + 2;
//             for (auto it = new_sublookups[src].begin(); it != new_sublookups[src].end(); it++) {
//                 new_sublookups[dest][it->first] = it->second;
//             }
//         }
//         else {
//             this->lookup.clear();
//         }
//     }

    #pragma omp parallel sections
    {
        #pragma omp section
        {
            int dest = 0;
            int src = 2;
            for (auto it = new_sublookups[src].begin(); it != new_sublookups[src].end(); it++) {
                new_sublookups[dest][it->first] = it->second;
            }
        }
        #pragma omp section
        {
            int dest = 1;
            int src = 3;
            for (auto it = new_sublookups[src].begin(); it != new_sublookups[src].end(); it++) {
                new_sublookups[dest][it->first] = it->second;
            }
        }
        #pragma omp section
        {
            this->lookup.clear();
        }
    }
    
    for (int src = 0; src < 2; src++) {
        for (auto it = new_sublookups[src].begin(); it != new_sublookups[src].end(); it++) {
            this->lookup[it->first] = it->second;
        }
    }
}

unsigned int Game::count_neighbours(Coord coord, unordered_map<Coord, unsigned int> *cache) {
    unsigned int count = 0;
    
    auto it = cache->find(coord);
    if (it != cache->end()) {
        count = it->second;
    }

    else {
        for (pair<int, int> offset : Game::neighbourhood) {
            Coord pos = Coord(coord.x + offset.first, coord.y + offset.second);
            count += (int) (this->lookup.find(pos) != this->lookup.end());
        }
        (*cache)[coord] = count;
    }

    return count;
}

bool Game::on_grid(Coord coord) {
    return (coord.x >= -OFF_GRID_CELLS && coord.x < HCELLS + OFF_GRID_CELLS &&
            coord.y >= -OFF_GRID_CELLS && coord.y < VCELLS + OFF_GRID_CELLS);
}
