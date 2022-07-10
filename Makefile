# paths
LIB = ./lib
INCLUDE = ./include
MODULES = ./modules

#Program args
ARGS = 3

#compiler
CC = gcc

#compiler options
CFLAGS = -Wall -Werror -g3 -I$(INCLUDE)
LDFLAGS = -lm

objects= game.o interface.o 8piece_puzzle.o List.o PQImplementation.o $(LIB)/libraylib.a

EXEC = game

# Παράμετροι της βιβλιοθήκης raylib
include $(LIB)/libraylib.mk

$(EXEC):$(objects)
	$(CC) $(objects) -o $(EXEC) $(LDFLAGS)

# Για να φτιάξουμε τα k08.a/libraylib.a τρέχουμε το make στο lib directory.
$(LIB)/%.a:
	$(MAKE) -C $(LIB) $*.a

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