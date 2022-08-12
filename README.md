# Graphical Representation of a Puzzle Solver

## The game
The program can solve the clasic [Sliding puzzle](https://en.wikipedia.org/wiki/Sliding_puzzle). 

### Board
The consept is very simple. The user can select the boardsize which will be the edge of the board. On the board we have tiles numbered from 1 to edge squered-1.With those tiles the board is coverded fully except a single spot.

### Rules
There is 1 very simple rule.The player can transfer to the missing spot any tile adjacent to it.

### Goal
The player must move the tiles to order them in decending order (1 int the top left corner) leaving the blank spot last on the bottom right corner

## The solver
The program uses the [IDA* search algorithm](https://en.wikipedia.org/wiki/Iterative_deepening_A*) to navigate from the given state of the game to the goal. In 3 simple steps:

- Expand from the current state(find all the possible stated after this)
- Evaluate all the new states
- Choose the state with the highest evaluation and set it as current
  
## The graphical enviroment
For the graphics the program uses a library called [Raylib](https://www.raylib.com/). The program can adapt to any board size since it creates all the not-repeating sprites in real time.

## Installation and Running

- Install the program by compiling the source code
    1. Download the source files and navigate to the downloaded folder.
    2. Make sure make is instaled.
    3. Type `make` in the downloaded folder.

> Thanks to [Mitsos](https://github.com/Jimminer) the program can now be compiled for windows and linux through wsl. All you have to do to compile for windows is add `MY_OS=win` to the make command. 

> Compilation for windows for WSL may require the gcc mingw package. For Ubuntu you can downloadit using apt by typing: `sudo apt-get install gcc-mingw-w64` 


- Run the program
    1. Type `./game`.

   
## User Interface

### Menu screen 
On the menu screen you will be asked to choose the following:
- **Size** or in other words the edge of the square puzzle.
- The **Current puzzle** that the game will start on.
- **Solving Mode** that can be either **auto** or **manual**.

> The puzzle must be entered using a spesific format. That is seperating every tile with a **space**.
> Example for a puzzle of size 4: `6 4 11 0 7 1 3 2 13 5 9 8 10 14 15 12`

On the same window you will find some buttons to help you:
- A **Copy** button that will copy the current puzzle text-box to your clipboard.(Ctrl+C)
- A **Paste** button that will paste your clipboard to the current puzzle textbox.(Ctrl+V)
- The **Start** button to start the puzzle (or use the **enter** key).
- **Exit** button that will close the game
- **Shuffle** button that will create a random puzzle for you to solve.

>For the **suffle** to work you first need to set the size of the puzzle.

### Auto Mode
When in auto mode:
- Press **[A]** to toggle between automatic procceding or manual.
- Press **[ENTER]** to Start/Procced the solution.
- Hit the **Back** to go back to the main menu and **carry the current state of the puzzle** with you.
  
### Manual Mode
When in manual mode:
- Use the **Arrow Keys** to solve the puzzle.
- Hit the **Back** to go back to the main menu and **carry the current state of the puzzle** with you.

> **When the puzzle is solved** the only thing you can do in both modes is press the back button


## Known bugs
- Some times the algorithm will mark a solvable puzzle as unsolvable.