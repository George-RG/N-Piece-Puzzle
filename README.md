# Graphical Representation of a Puzzle Solver

## The game
The programm can solve the clasic [Sliding puzzle](https://en.wikipedia.org/wiki/Sliding_puzzle). 

### Board
The consept is very simple. The user can select the boardsize which will be the edge of the board. On the board we have tiles numbered from 1 to edge squered-1.With those tiles the board is coverded fully except a single spot.

### Rules
There is 1 very simple rule.The player can transfer to the missing spot any tile adjacent to it.

### Goal
The player must move the tiles to order them in decending order (1 int the top left corner) leaving the blank spot last on the bottom right corner

## The solver
The program uses the [A* search algorithm](https://en.wikipedia.org/wiki/A*_search_algorithm) to navigate from the given state of the game to the goal. In 3 simple steps:

- Expand from the current state(find all the possible stated after this)
- Evaluate all the new states
- Choose the state with the highest evaluation and set it as current
  
## The graphical enviroment
For the graphics the program uses a library called [Raylib](https://www.raylib.com/). The program can adapt to any board size since it creates alla the not-repeating sprites in real time.

## Installation and Running

- Intall the program by compiling the source code
    1. Download the source files and navigate to the downloaded folder.
    2. Make sure make is instaled.
    3. Type make in the downloaded folder.

- Run the program
    1. Type ./game "Edge" where the edge is the edge of the board.
    2. Type row by row the numbers of the tiles in the starting board. 
    3. Choose Auto or Manual progression
   
## User Interface
-   ***Start*** the solving with the __ENTER__
-   ***Toggle*** auto solving on/off with  __A__
-   In Manual mode ***procced*** to the next move with __ENTER__
-   When the solving is done ***exit*** with the key __ESC__