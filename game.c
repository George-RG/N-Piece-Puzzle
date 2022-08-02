#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"

#include "interface.h"

//Global variables
bool in_menu=true;
Graphics Graf;
int auto_test = 2;

bool keys_update(void)
{   
    return IsKeyDown(KEY_ENTER); 
}


void update_and_draw() {
    if(IsKeyPressed(KEY_A))
    {
        auto_test = !auto_test;
    }

	interface_draw_frame(&Graf,(keys_update() ||auto_test),&in_menu);
}



int main (int argc , char* argv[])
{
    //Initialization
    Graf = malloc(sizeof(gra_state));
    Graf->board_size = -1;
    Graf->move_list = NULL;
    Graf->positions = NULL;


    int N = 1;

    //Graphics
	interface_init();

    // Η κλήση αυτή καλεί συνεχόμενα την update_and_draw μέχρι ο χρήστης να κλείσει το παράθυρο
	start_main_loop(update_and_draw);

    return 0;

    in_menu = false;

    // if(argc != 2)
    // {
    //     printf("Usage: ./puzzle \"Dimention\"\n");
    //     return -1;
    // }
    // int N = atoi(argv[1]);

    //int i,j;
    State* initial_state = malloc(sizeof(State));
    // initial_state->board = malloc(sizeof(int*) * N);
    // for(i=0; i<N; i++)
    //     initial_state->board[i] = malloc(sizeof(int) * N);
    // initial_state->moves=0;
    // initial_state->parent=NULL;
    // initial_state->size = N;

    // int flag=0;
    // while(flag == 0)
    // {
    //     flag = 1;
    //     printf("Insert Puzzle pieces by row (0 - %d)\n",(N*N)-1);
    //     for(i=0; i<N; i++)
    //     {
    //         printf("Row %2d: ",i+1);
    //         for(j=0; j<N; j++)// Made by Ioanna Nikolaidi
    //         {
    //             scanf("%d",&(initial_state->board[i][j]));
    //             if(initial_state->board[i][j] > (N*N)-1)
    //             {   
    //                 flag=0;
    //                 break;
    //             }     
    //         }
    //         if(flag==0)
    //             break;
    //     }
    // }

    // if(!IsSolveable(initial_state))
    // {
    //     printf("Puzzle is not solvable\n");
    //     return -1;
    // }

    // if(isGoal(*initial_state) == 1)
    // {
    //     printf("This puzzle is already solved\n");
    //     return 0;
    // }

    // RB tree = RB_Initialize(destroyfunc,compare);
    // PQ Queue = PQ_Initialize(compare_evals,destroyfunc);

    // evaluate(initial_state);

    // RB_InsertKey(tree,initial_state);

    // Graf = create_gra_state(N);

    // Graf->move_list = solve(initial_state,Queue,tree);

    // while(auto_play>1 || auto_play < 0)
    // {
    //     printf("Type 1 for auto solve or 0 for step solving (Proceed steps with [ENTER]): ");
    //     //scanf("%d",&auto_play);
    //     printf("You can enable/disable auto mode later by pressing [A]\n");
    // }

    // //Graphics
	// interface_init();

	// // Η κλήση αυτή καλεί συνεχόμενα την update_and_draw μέχρι ο χρήστης να κλείσει το παράθυρο
	// start_main_loop(update_and_draw);

	// interface_close();

    // PQ_Destroy(Queue);
    // RB_Destroy(tree);
    // free_gra_state(Graf);

    return 0;
}


// int main (int argc , char* argv[])
// {
//     if(argc != 2)
//     {
//         printf("Usage: ./puzzle \"Dimention\"\n");
//         return -1;
//     }
//     int N = atoi(argv[1]);


//     int i,j;
//     State* initial_state = malloc(sizeof(State));
//     initial_state->board = malloc(sizeof(int*) * N);
//     for(i=0; i<N; i++)
//         initial_state->board[i] = malloc(sizeof(int) * N);
//     initial_state->moves=0;
//     initial_state->parent=NULL;
//     initial_state->size = N;

//     int flag=0;
//     while(flag == 0)
//     {
//         flag = 1;
//         printf("Insert Puzzle pieces by row (0 - %d)\n",(N*N)-1);
//         for(i=0; i<N; i++)
//         {
//             printf("Row %2d: ",i+1);
//             for(j=0; j<N; j++)// Made by Ioanna Nikolaidi
//             {
//                 if(scanf("%d",&(initial_state->board[i][j])) == 0)
//                 {
//                     printf("Invalid input\n");
//                     flag = 0;
//                     break;
//                 }
//                 if(initial_state->board[i][j] > (N*N)-1)
//                 {   
//                     flag=0;
//                     break;
//                 }     
//             }
//             if(flag==0)
//                 break;
//         }
//     }    

//     if(isGoal(*initial_state) == 1)
//     {
//         printf("Solution:\n");
//         //PrintBoards(initial_state);
//         return 0;
//     }
    
//     evaluate(initial_state);

//     solve_new(initial_state);

//     return 0;
// }