# Set the windows path you want to move the game inorder to run it (use /mnt/ befor the path so you can access you disk)
WIN_PATH := /mnt/c/Users/Georg/Desktop/

# Set the name of the output
EXEC := game

# paths
LIB := libs
INCLUDE := ./include  -I ./include/raylib
SRC := ./src
BUILD := ./build

#Program args
ARGS := 3

#Default compiler
CC := gcc
CXX := g++

#Debug mode
DEBUG := false

#Exta libs
LIBS := -lraylib
EXTRA :=

#For web emcc path
EMCC_PATH := /home/george/Downloads/emsdk/


################################### DO NOT EDIT BELOW THIS LINE UNLESS YOU KNOW WHAT YOU ARE DOING ###################################


# Makefile options
PLATFORM = LINUX

#compiler options
CFLAGS = -Wall -Werror -I$(INCLUDE)
CXXFLAGS = -I$(INCLUDE)
LDFLAGS = -lm

SRCS := $(shell find $(SRC) -name '*.c')
OBJS := $(SRCS:$(SRC)/%.c=$(SRC)/%.o)
OBJPATH := $(addprefix $(BUILD)/,$(notdir $(OBJS)))

SRCS_CXX := $(shell find $(SRC) -name '*.cpp')
OBJS_CXX := $(SRCS_CXX:$(SRC)/%.cpp=$(SRC)/%.opp)
OBJPATH_CXX := $(addprefix $(BUILD)/,$(notdir $(OBJS_CXX)))

ifeq ($(DEBUG),true)
	CFLAGS += -g3
else
	CFLAGS += -O3
endif

ifeq ($(PLATFORM),WIN)
	LDFLAGS += -L$(LIB)/windows/ $(LIBS)  -lgdi32 -lwinmm -lopengl32 -lpthread
	CC = x86_64-w64-mingw32-gcc
	CXX = x86_64-w64-mingw32-g++

	EXEC :=$(EXEC).exe
else ifeq ($(PLATFORM),LINUX)
	LDFLAGS += -L$(LIB)/linux  $(LIBS) -lGL -lpthread -ldl -lrt -lX11

	WIN_PATH = ./
else ifeq ($(PLATFORM),WEB)
	LDFLAGS += -L$(LIB)/web -lGL -lpthread -ldl -lrt -lX11 $(LIBS)

	OBJS := EMCC $(OBJS)

	SHELL := /bin/bash
	CC := emcc
	CXX := em++
endif

$(SRC)/%.opp: $(SRC)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ && mv $@ $(BUILD)

$(SRC)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ && mv $@ $(BUILD) 

# Για να φτιάξουμε τα k08.a/libraylib.a τρέχουμε το make στο lib directory.
# $(LIB)/%.a:
# 	$(MAKE) -C $(LIB) $*.a SUFFIX=$(PLATFORM)

$(EXEC):$(OBJS) $(OBJS_CXX) $(EXTRA)
	$(CXX) $(OBJPATH) $(OBJPATH_CXX) $(EXTRA) -o $(EXEC) $(LDFLAGS)
	-mv $(EXEC) $(WIN_PATH)

EMCC:
	$(EMCC_PATH)/emsdk activate latest
	source $(EMCC_PATH)/emsdk_env.sh

#Cleaning
PHONY clean:
	rm -f $(BUILD)/*.o $(BUILD)/*.opp $(EXEC) $(EXTRA) $(EXEC).exe

valgrind:
	$(MAKE) clean
	$(MAKE) $(EXEC) DEBUG=true
	valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all ./$(EXEC) $(ARGS)

run: $(EXEC)
	./$(EXEC) $(ARGS)
