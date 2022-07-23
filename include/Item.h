#ifndef ITEM_H
#define ITEM_H

typedef struct _state{
    int eval;
    char* representation;

    int** board;
    struct _state* parent;
    int moves;
    int blank_row;
    int blank_col;
    int size;
} State;

typedef State* PQItem;

#endif /* ITEM_H */
