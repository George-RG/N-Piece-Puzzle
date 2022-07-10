#include "PQInterface.h"
#include "ListInterface.h"

#define SCREEN_HEIGHT 900
#define SCREEN_WIDTH  900

ListPtr solve(State* state, PQ Queue, ListPtr Visited);
int isGoal(State state);
int compare_same(PQItem a, PQItem b);
int compare_evals(PQItem a,PQItem b);
void destroyfunc(PQItem a);
void evaluate (State* state);


