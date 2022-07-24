#include "8piece_puzzle.h"
#include "graphics_state.h"

#include "graphics_state.h"

#include "raylib.h"

// Αρχικοποιεί το interface του παιχνιδιού
void interface_init();

// Κλείνει το interface του παιχνιδιού
void interface_close();

// Σχεδιάζει ένα frame με την τωρινή κατάσταση του παιχνδιού
void interface_draw_frame(Graphics* state_ptr, bool play, int autoplay, bool *in_menu);

Graphics create_gra_state(int size);
void free_gra_state(Graphics gr);

Graphics* create_dumy_state(void);