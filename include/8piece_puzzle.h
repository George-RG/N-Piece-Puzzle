#include "PQ.h"
#include "List.h"
#include "RB.h"

typedef struct _state{
    int eval;
    char* representation;

    struct _state* parent;
    int moves;

    int** board;
    struct _state* parent;
    int moves;
    int blank_row;
    int blank_col;
    int size;
} State;

typedef State* PQItem;

//ListPtr solve(State* state, PQ Queue, ListPtr Visited);
ListPtr solve(State* state, PQ Queue, RB tree);
int isGoal(State state);
int compare(Pointer a, Pointer b);
int compare_evals(Pointer first,Pointer second);
void destroyfunc(Pointer ptr);
void evaluate (State* state);
int IsSolveable(State* state);


