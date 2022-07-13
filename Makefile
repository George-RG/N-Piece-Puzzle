# paths
LIB = ./lib
INCLUDE = ./include
MODULES = ./modules

MY_OS = linux

#Program args
ARGS = 3

#compiler
CC = gcc

#compiler options
CFLAGS = -Wall -Werror -g3 -I$(INCLUDE)
LDFLAGS = -lm

objects= game.o interface.o 8piece_puzzle.o List.o PQImplementation.o $(LIB)/libraylib.a

EXEC = game

ifeq ($(MY_OS),win)
	LDFLAGS += -lgdi32 -lwinmm -lopengl32
	CC = x86_64-w64-mingw32-gcc
else ifeq ($(MY_OS),linux)
	LDFLAGS += -ldl -lpthread -lGL
endif

$(EXEC):$(objects)
	$(CC) $(objects) -o $(EXEC) $(LDFLAGS)
#cp $(EXEC) /mnt/c/Users/Georg/Desktop/

# Για να φτιάξουμε τα k08.a/libraylib.a τρέχουμε το make στο lib directory.
$(LIB)/%.a:
	$(MAKE) -C $(LIB) $*.a SUFFIX=$(MY_OS)

#For compiling the ADTs
List.o :
	$(CC) -c $(MODULES)/List.c $(LDFLAGS) $(CFLAGS)

PQImplementation.o :
	$(CC) -c $(MODULES)/PQImplementation.c $(LDFLAGS) $(CFLAGS)

#Cleaning
PHONY clean:
	rm -f $(objects) $(EXEC)

valgrind: $(EXEC)
	valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all ./$(EXEC) $(ARGS)

run: $(EXEC)
	./$(EXEC) $(ARGS)