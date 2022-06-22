# (Simple) Makefile for linux #

CPP     := g++
ARGS    := -std=c++14

LIBS    := -lSDL2 -lSDL2main -lboost_filesystem
INCLUDE := -Ivendor/include

SRCS    := src/*.cpp src/application/*.cpp src/application/gui/*.cpp src/application/imgui/*.cpp src/core/*.cpp src/core/apu/*.cpp src/core/opcode/*.cpp src/core/mbc/*.cpp 
OBJS    := $(patsubst %.cpp,%.o,$(wildcard $(SRCS)/*.cpp))
BIN     := bin/Release/gbBoy.o

# Arguments
build := debug

ifeq ($(build), release)
	ARGS += -O3
endif
ifeq ($(build), debug)
	ARGS += -g
	BIN = bin/Debug/gbBoy.o
endif

# Rules 
all: program
	$(BIN)

program: $(OBJS)
	$(CPP) $(OBJS) $(LIBS) -o $(BIN) $(ARGS)

%.o: %.cpp
	$(CPP) $(INCLUDE) $(ARGS) $< -c -o $@

clean:
	rm $(OBJS)
