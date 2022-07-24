#include "PQ.h"
#include "ListInterface.h"
#include "RB.h"

#define SCREEN_HEIGHT 900
#define SCREEN_WIDTH  900

//ListPtr solve(State* state, PQ Queue, ListPtr Visited);
ListPtr solve(State* state, PQ Queue, RB tree);
int isGoal(State state);
int compare(Pointer a, Pointer b);
int compare_evals(Pointer first,Pointer second);
void destroyfunc(Pointer ptr);
void evaluate (State* state);
int IsSolveable(State* state);


