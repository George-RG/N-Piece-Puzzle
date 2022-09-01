# Set the windows path you want to move the game inorder to run it (use /mnt/ befor the path so you can access you disk)
WIN_PATH = /mnt/c/Users/Georg/Desktop/

# Set the name of the output
EXEC = game

# paths
LIB = ./lib
INCLUDE = ./include  -I ./include/raylib
SRC = ./src
BUILD = ./build

#Program args
ARGS = 3

#Default compiler
CC = gcc

#Debug mode
DEBUG = false

#Exta object files
EXTRA := $(LIB)/libraylib.a


################################### DO NOT EDIT BELOW THIS LINE UNLESS YOU KNOW WHAT YOU ARE DOING ###################################


# Makefile options
PLATFORM = linux
MOVE = YES

#compiler options
CFLAGS = -Wall -Werror -I$(INCLUDE) -std=c99
LDFLAGS = -lm

SRCS := $(shell find $(SRC) -name '*.c')
OBJS = $(SRCS:$(SRC)/%.c=$(SRC)/%.o)
OBJPATH := $(addprefix $(BUILD)/,$(notdir $(OBJS)))

ifeq ($(DEBUG),true)
	CFLAGS += -g3 -O0
else
	CFLAGS += -O3
endif

ifeq ($(PLATFORM),win)
	LDFLAGS += -lgdi32 -lwinmm -lopengl32 -lpthread
	CC = x86_64-w64-mingw32-gcc
	
	Exec += .exe
else ifeq ($(PLATFORM),linux)
	LDFLAGS += -ldl -lpthread -lGL

	MOVE = FALSE 
else ifeq ($(PLATFORM),web)
	LDFLAGS += -lm -lGL
	

endif

$(SRC)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ && mv $@ $(BUILD) 

$(MODULES)/%.o: $(MODULES)/%.c
	cd $(BUILD) && $(CC) $(CFLAGS) -c $< -o $@

# Για να φτιάξουμε τα k08.a/libraylib.a τρέχουμε το make στο lib directory.
$(LIB)/%.a:
	$(MAKE) -C $(LIB) $*.a SUFFIX=$(PLATFORM)

$(EXEC):$(OBJS) $(EXTRA)
	$(CC) $(OBJPATH) $(EXTRA) -o $(EXEC) $(LDFLAGS)
#mv $(EXEC) /mnt/c/Users/Georg/Desktop/

#Cleaning
PHONY clean:
	rm -f $(BUILD)/*.o $(EXEC) $(EXTRA) $(EXEC).exe

valgrind: $(EXEC)
	valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all ./$(EXEC) $(ARGS)

run: $(EXEC)
	./$(EXEC) $(ARGS)
