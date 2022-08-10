#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"

#include "interface.h"

// Global variables
bool in_menu = true;
Graphics Graf;

bool keys_update(void)
{
    return IsKeyDown(KEY_ENTER);
}

void update_and_draw()
{
    interface_draw_frame(&Graf, &in_menu);
}

int main(int argc, char *argv[])
{
    // Initialization
    Graf = malloc(sizeof(gra_state));
    Graf->board_size = -1;
    Graf->move_list = NULL;
    Graf->positions = NULL;

    // Graphics
    interface_init();

    // Η κλήση αυτή καλεί συνεχόμενα την update_and_draw μέχρι ο χρήστης να κλείσει το παράθυρο
    start_main_loop(update_and_draw);

    return 0;
}