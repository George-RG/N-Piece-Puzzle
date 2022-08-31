# Set the windows path you want to move the game inorder to run it (use /mnt/ befor the path so you can access you disk)
WIN_PATH = /mnt/c/Users/Georg/Desktop/
EXEC = game

# paths
LIB = ./lib
INCLUDE = ./include
INCLUDE2 = ./include/raylib
MODULES = ./modules

DEBUG = false

MY_OS = linux
MOVE = YES

#Program args
ARGS = 3

#compiler
CC = gcc

#compiler options
CFLAGS = -Wall -Werror -I$(INCLUDE) -I$(INCLUDE2) -std=c99
LDFLAGS = -lm

objects= game.o interface.o List.o RB.o PQ.o ai_solver.o $(LIB)/libraylib.a

ifeq ($(DEBUG),true)
	CFLAGS += -g3 -O0
else
	CFLAGS += -O3
endif

ifeq ($(MY_OS),win)
	LDFLAGS += -lgdi32 -lwinmm -lopengl32 -lpthread
	CC = x86_64-w64-mingw32-gcc
	
	Exec += .exe
else ifeq ($(MY_OS),linux)
	LDFLAGS += -ldl -lpthread -lGL

	MOVE = FALSE 
endif

$(EXEC):$(objects)
	$(CC) $(objects) -o $(EXEC) $(LDFLAGS)
	mv $(EXEC) /mnt/c/Users/Georg/Desktop/

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

game.o :
	$(CC) -c game.c $(LDFLAGS) $(CFLAGS)

ai_solver.o :
	$(CC) -c ai_solver.c $(LDFLAGS) $(CFLAGS)

interface.o :
	$(CC) -c interface.c $(LDFLAGS) $(CFLAGS)


#Cleaning
PHONY clean:
	rm -f $(objects) $(EXEC)
#cd /mnt/c/Users/Georg/Desktop/ 
#rm -f $(EXEC)

valgrind: $(EXEC)
	valgrind --error-exitcode=1 --leak-check=full --show-leak-kinds=all ./$(EXEC) $(ARGS)

run: $(EXEC)
	./$(EXEC) $(ARGS)
