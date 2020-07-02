#ifndef _RLE_READER_HPP
#define _RLE_READER_HPP

#include <unordered_map>
#include <string>
#include "coord.hpp"

using namespace std;

class RleReader {
public:
    RleReader(string filename);

    unordered_map<Coord, unsigned int> lookup;
    string filename;
    int width;
    int height;
    int frames;

private:
    bool get_next(string txt, int *pos, int *count, char *tag);
    void get_dims(string line);
    int get_dim(string line, int *pos, char tag);
    void get_frames(string line);
};

#endif
