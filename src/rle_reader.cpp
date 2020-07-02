#include "rle_reader.hpp"
#include "constants.hpp"
#include <fstream>
#include <sstream>

RleReader::RleReader(string filename) {
    this->filename = filename;
    this->width = -1;
    this->height = -1;
    this->frames = DEFAULT_PATTERN_FRAMES;
    int row = 0;
    int col = 0;
    Coord centre = Coord(HCELLS / 2, VCELLS / 2);

    ifstream input_file;
    input_file.open(filename);

    stringstream txt_stream;
    for (string line; getline(input_file, line); ) {
        if (line[0] == 'x') {
            this->get_dims(line);
        }
        
        else if (line[0] == 'F') {
            this->get_frames(line);
        }

        else if (isdigit(line[0]) || line[0] == 'b' || line[0] == 'o' || line[0] == '$') {
            txt_stream << line;
        }
    }
    string txt = txt_stream.str();
    int pos = 0;
    int count = -1;
    char tag = ' ';
    while (this->get_next(txt, &pos, &count, &tag)) {
        for (int i = 0; i < count; i++) {
            if (tag == 'o') {
                int x = col + centre.x - this->width / 2;
                int y = row + centre.y - this->height / 2;
                lookup[Coord(x, y)] = 0;
                col++;
            }
            else if (tag == 'b') {
                col++;
            }
            else if (tag == '$') {
                row++;
                col = 0;
            }
        }
    }
    
    input_file.close();
}

bool RleReader::get_next(string txt, int *pos, int *count, char *tag) {
    bool have_next = *pos < (int) txt.size() && txt[*pos] != '!';

    if (have_next) {
        //get count (if present)
        stringstream count_stream;
        *count = 1;
        while (isdigit(txt[*pos])) {
            count_stream << txt[*pos];
            (*pos)++;
        }
        string count_str = count_stream.str();
        if (count_str != "") {
            *count = stoi(count_str);
        }

        //get tag
        *tag = txt[*pos];
        (*pos)++;
    }
    
    return have_next;
}

void RleReader::get_frames(string line) {
    int pos = 0;
    stringstream val_stream;
    while (!isdigit(line[pos]) && pos < (int) line.size()) {
        pos++;
    }
    while (isdigit(line[pos]) && pos < (int) line.size()) {
        val_stream << line[pos];
        pos++;
    }
    string val_str = val_stream.str();
    if (val_str.size() > 0) {
        this->frames = stoi(val_str);
    }
}

void RleReader::get_dims(string line) {
    int pos = 0;
    this->width = this->get_dim(line, &pos, 'x');
    this->height = this->get_dim(line, &pos, 'y');
}

int RleReader::get_dim(string line, int *pos, char tag) {
    int dim = -1;
    while (line[*pos] == tag || line[*pos] == ' ' || line[*pos] == '=') {
        (*pos)++;
    }
    stringstream val_stream;
    while (isdigit(line[*pos])) {
        val_stream << line[*pos];
        (*pos)++;
    }
    dim = stoi(val_stream.str());

    while (line[*pos] == ',' || line[*pos] == ' ') {
        (*pos)++;
    }
    
    return dim;
}
