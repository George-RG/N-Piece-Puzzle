#include "ai_solver.h"
#include "graphics_state.h"

#include "graphics_state.h"

#include "raylib.h"

#define SCREEN_HEIGHT 900
#define PUZZLE_HEIGHT 900
#define SCREEN_WIDTH  1300
#define PUZZLE_WIDTH 900

#define ANIMATION_SPEED 20 // (Edge/ANIMATION_SPEED)

// Αρχικοποιεί το interface του παιχνιδιού
void interface_init();

// Κλείνει το interface του παιχνιδιού
void interface_close();

// Σχεδιάζει ένα frame με την τωρινή κατάσταση του παιχνδιού
void interface_draw_frame(Graphics* state_ptr, bool play, bool *in_menu);

Graphics create_gra_state(int size);
void free_gra_state(Graphics gr);

Graphics create_dumy_state(void);