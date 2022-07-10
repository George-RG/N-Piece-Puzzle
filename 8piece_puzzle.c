#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "8piece_puzzle.h"

void expand(State* cur_state, PQ Queue, ListPtr Visited);
State* CopyState(State* cur_state);
ListPtr ReturnSolution(State* state);


ListPtr solve(State* state, PQ Queue, ListPtr Visited)
{
    State* cur_state = state;
    while(isGoal(*cur_state) != 1)
    {
        expand(cur_state,Queue,Visited);

        cur_state = Remove(Queue);

        ListInsert(cur_state,Visited);
    }

    ListPtr move_list = ReturnSolution(cur_state);
        
    return move_list;
}

void expand(State* cur_state, PQ Queue, ListPtr Visited)
{
    int size = cur_state->size;

    int RowOffset[]={-1,0,+1,0};
    int ColOffset[]={0,-1,0,+1};

    int new_col;
    int new_row;

    State* new_state; 

    int i;
    for(i=0; i<4; i++)
    {
        new_col = cur_state->blank_col + ColOffset[i];
        new_row = cur_state->blank_row + RowOffset[i];

        if(new_col >= 0 && new_col < size && new_row >= 0 && new_row < size /* && (cur_state->parent == NULL || (new_col != cur_state->parent->blank_col && new_row != cur_state->parent->blank_row)) */ )
        {
            new_state = CopyState(cur_state);   
            new_state->board[cur_state->blank_row][cur_state->blank_col] = cur_state->board[new_row][new_col];
            new_state->board[new_row][new_col]=0;
            new_state->blank_row = new_row;
            new_state->blank_col = new_col;
            
            if(ListFind(Visited,new_state) == -1)
            {
                evaluate(new_state);
                Insert(new_state,Queue);
            }
            else
            {
                destroyfunc(new_state);
            }
        }
    }
}

State* CopyState(State* cur_state)
{
    State* new_state =  malloc(sizeof(State));
    new_state->parent = cur_state;
    new_state->moves = cur_state->moves+1;
    new_state->size = cur_state->size;


    new_state->board = malloc(sizeof(int*) * new_state->size);
    for(int i=0; i<new_state->size; i++)
        new_state->board[i] = malloc(sizeof(int) * new_state->size);

    for(int i=0; i<new_state->size; i++)
        for(int j=0; j<new_state->size; j++)
            new_state->board[i][j] = cur_state->board[i][j];

    return new_state;        
}

int compare_same(PQItem a, PQItem b)
{
    int size = a->size;
    
    if(size != b->size)
        return -1;
    
    int i,j;
    for(i=0; i<size; i++)
        for(j=0; j<size; j++)
            if(a->board[i][j] != b->board[i][j])
                return -1;

    return 0;
}

void destroyfunc(PQItem a)
{
    if(a == NULL)
        return;

    for(int i=0; i<a->size; i++)
        free(a->board[i]);

    free(a->board);

    free(a);
}

int compare_evals(PQItem a,PQItem b)
{
    if(a->eval <= b->eval)
        return 1;

    return -1;
}

void evaluate (State* state)
{
    int size = state->size;
    int mScore=0,counter=1;
    int i,j;
    for (i=0; i<size; i++)
    {
        for(j=0; j<size; j++)
        {
            if(state->board[i][j] == 0)
            {
                state->blank_row = i;
                state->blank_col = j;
            }

            if(state->board[i][j] != 0)
            {
                if(state->board[i][j] != counter)
                {
                    int cur = state->board[i][j] - 1;
                    int wRow = floor(cur / size);
                    int wCol = floor(cur % size);

                    mScore += abs(i - wRow);
                    mScore += abs(j - wCol);
                }
            }
            if(counter == (size*size)-1
            )
                counter = 0;
            else    
                counter++;
        }
    }

    state->eval = mScore + state->moves;
    return;
}

int isGoal(State state)
{
    int size = state.size;
    int counter=1;
    int i,j;
    for(i=0; i<size; i++)
        for(j=0; j<size; j++)
        {
            int current = state.board[i][j];
            if(current != counter)
                return 0;
            
            if(counter == (size*size)-1)
                counter = 0;
            else    
                counter++;
        }
    
    return 1;
}

ListPtr ReturnSolution(State* state)
{
    ListPtr list = CreateList(compare_same,NULL);
    while(state != NULL)
    {
        ListInsert(state,list);
        
        state = state->parent;
    }
    return list;
}