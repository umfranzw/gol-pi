EXE = gol

SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
#HDRS = $(wildcard $(INC_DIR)/*.h)
OBJ = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

CC = g++
CPPFLAGS += -O3 -Wall -fopenmp --std=c++11 -I$(INC_DIR) -I/usr/include/SDL2
LDFLAGS += -fopenmp
LDLIBS += -O3 -lm -lSDL2 -lSDL2_gfx

.PHONY: all clean

all: $(OBJ_DIR) $(EXE)

$(EXE): $(OBJ)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CPPFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -r $(OBJ_DIR)
	rm $(EXE)

