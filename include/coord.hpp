#ifndef _COORD_HPP
#define _COORD_HPP

#include "constants.hpp"

using namespace std;

class Coord {
public:
    Coord(int x, int y);
    bool operator==(const Coord &rhs) const;
    
    int x;
    int y;
};

namespace std {
    template<>
    class hash<Coord> {
    public:
        size_t operator()(const Coord &coord) const {
            //return hash<unsigned int>()(coord.y * WIDTH + coord.x);
            return hash<int>()(coord.x) ^ hash<int>()(coord.y);
        }
    };

    template<>
    class less<Coord> {
    public:
        bool operator()(const Coord &lhs, const Coord &rhs) const {
            bool result = false;
            if (lhs.x < rhs.x) {
                result = true;
            }
            else if (lhs.x == rhs.x) {
                if (lhs.y < rhs.y) {
                    result = true;
                }
            }

            return result;
        }
    };
}

#endif
