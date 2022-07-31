#ifndef G_STATE_H
#define G_STATE_H
typedef struct _point
{
    float x;
    float y;
} point;

struct transition
{
    int in_transition;
    int offset_row;
    int offset_col;
};

typedef struct _gra_state
{
    int board_size;
    point** positions;
    float edge;
    int move_num;
    ListPtr move_list;
    struct transition trans;
} gra_state;

typedef gra_state* Graphics;

#endif /* G_STATE_H */

