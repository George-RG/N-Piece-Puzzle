#include "PQ.h"
#include "List.h"
#include "RB.h"
#include "raylib.h"

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Move;

typedef struct _state{
    int eval;
    char* representation;

    struct _state* parent;
    int moves;
    int min_child_moves;
    Move min_child;

    int** board;
    int blank_row;
    int blank_col;
    int size;
} State;

typedef State* PQItem;

typedef struct thread_data{
    ListPtr* result;
    State* input;
} thread_data;

//ListPtr solve(State* state, PQ Queue, ListPtr Visited);
void* solve_new(void* arg);
int isGoal(State state);
int compare(Pointer a, Pointer b);
int compare_evals(Pointer first,Pointer second);
void destroyfunc(Pointer ptr);
void evaluate (State* state);
int IsSolveable(State* state);
State* CopyState(State* cur_state);


