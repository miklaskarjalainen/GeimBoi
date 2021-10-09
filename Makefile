# (Simple) Makefile for linux #

CPP     := g++
ARGS    := -std=c++17 # c++17 needed for variant (FileIni.hpp), would like to get this to c++14 tho.

LIBS    := -lSDL2 -lSDL2main -pthread 
INCLUDE := -IVendor/include

SRCS    := GeimBoi/*.cpp GeimBoi/application/*.cpp GeimBoi/application/imgui/*.cpp GeimBoi/core/*.cpp GeimBoi/core/opcode/*.cpp GeimBoi/core/mbc/*.cpp 
OBJS    := $(patsubst %.cpp,%.o,$(wildcard $(SRCS)/*.cpp))
BIN     := bin/Release/gbBoy.o

# Arguments
build:=debug
profiling:=yes

ifeq ($(build), release)
	ARGS += -O3
endif
ifeq ($(build), debug)
	ARGS += -g
endif

profiling:=yes
ifeq ($(profiling), yes)
	ARGS += -DDO_PROFILING
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
