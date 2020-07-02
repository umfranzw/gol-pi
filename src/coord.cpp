#include "coord.hpp"
#include "constants.hpp"

Coord::Coord(int x, int y) {
    this->x = x;
    this->y = y;
}

bool Coord::operator==(const Coord &rhs) const {
    return this->x == rhs.x && this->y == rhs.y;
}
