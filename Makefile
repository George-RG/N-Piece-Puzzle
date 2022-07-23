# paths
LIB = ./lib
INCLUDE = ./include
MODULES = ./modules

DEBUG = false

MY_OS = linux

#Program args
ARGS = 3

#compiler
CC = gcc

#compiler options
CFLAGS = -Wall -Werror -I$(INCLUDE)
LDFLAGS = -lm

objects= game.o interface.o 8piece_puzzle.o List.o RB.o PQ.o $(LIB)/libraylib.a

EXEC = game.exe

ifeq ($(DEBUG),true)
	CFLAGS += -g3 -O0
else
	CFLAGS += -O3
endif

ifeq ($(MY_OS),win)
	LDFLAGS += -lgdi32 -lwinmm -lopengl32
	CC = x86_64-w64-mingw32-gcc
else ifeq ($(MY_OS),linux)
	LDFLAGS += -ldl -lpthread -lGL
endif

$(EXEC):$(objects)
	$(CC) $(objects) -o $(EXEC) $(LDFLAGS)
	mv $(EXEC) /mnt/c/Users/Georg/Desktop/
	rm -f $(EXEC)

# Για να φτιάξουμε τα k08.a/libraylib.a τρέχουμε το make στο lib directory.
$(LIB)/%.a:
	$(MAKE) -C $(LIB) $*.a SUFFIX=$(MY_OS)

#For compiling the ADTs
List.o :
	$(CC) -c $(MODULES)/List.c $(LDFLAGS) $(CFLAGS)

RB.o :
	$(CC) -c $(MODULES)/RB.c $(LDFLAGS) $(CFLAGS)

PQ.o :
	$(CC) -c $(MODULES)/PQ.c $(LDFLAGS) $(CFLAGS)

#Cleaning
PHONY clean:
	rm -f $(objects) $(EXEC)
	cd /mnt/c/Users/Georg/Desktop/ 
	rm -f $(EXEC)

valgrind: $(EXEC)
	valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all ./$(EXEC) $(ARGS)

run: $(EXEC)
	./$(EXEC) $(ARGS)