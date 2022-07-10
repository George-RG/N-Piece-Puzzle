#include "interface.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

char* int_to_ascii(int num);

Texture* textures;
bool first_time = true;
bool first_end = true;
Sound clap;

void interface_init() 
{
	// Αρχικοποίηση του παραθύρου
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Puzzle");
	SetTargetFPS(60);

	textures = malloc(sizeof(Texture) * 8);

	textures[0]= LoadTextureFromImage(LoadImage("assets/1.png"));
    textures[1]= LoadTextureFromImage(LoadImage("assets/2.png"));
	textures[2]= LoadTextureFromImage(LoadImage("assets/3.png"));
	textures[3]= LoadTextureFromImage(LoadImage("assets/4.png"));
	textures[4]= LoadTextureFromImage(LoadImage("assets/5.png"));
	textures[5]= LoadTextureFromImage(LoadImage("assets/6.png"));
	textures[6]= LoadTextureFromImage(LoadImage("assets/7.png"));
	textures[7]= LoadTextureFromImage(LoadImage("assets/8.png"));

	InitAudioDevice();
	clap = LoadSound("assets/clap.wav");
}

void interface_draw_frame(Graphics state,bool play,int autoplay)
{
	BeginDrawing();

	// Καθαρισμός, θα τα σχεδιάσουμε όλα από την αρχή
	ClearBackground(BLACK);
	
	State* prev = ListGetNth(state->move_list,1);

    if((play == false && state->trans.in_transition == false) || isGoal(*prev) || first_time == true)
    {
        for(int i=0; i<state->board_size; i++)
            for(int j=0; j<state->board_size; j++)
				if(prev->board[i][j] != 0)
				{
					DrawRectangle(state->positions[i][j].x+10,state->positions[i][j].y+10,state->edge-20,state->edge-20,WHITE);
					//DrawTexture(textures[ListGetNth(state->move_list,1)->board[i][j]-1],state->positions[i][j].x,state->positions[i][j].y,WHITE);
					char* buf = int_to_ascii(ListGetNth(state->move_list,1)->board[i][j]);
					DrawText(
						buf,
						state->positions[i][j].x + state->edge/2 - MeasureText(buf, (int)(state->edge/2))/2, 
						state->positions[i][j].y + state->edge/2 - 50,
						(int)(state->edge/2),BLACK
						);
					free(buf);
				}

		if(isGoal(*prev))
		{	
			DrawText(
				"PRESS [ESC] TO EXIT",
				GetScreenWidth() / 2 - MeasureText("PRESS [ESC] TO EXIT", 40) / 2,
				GetScreenHeight() / 2 - 70, 40, RED
			);

			if(first_end)
			{
				PlaySound(clap);
				first_end = false;
			}
		}
		else if(first_time)
		{
			DrawText(
				"PRESS [ENTER] TO START",
				GetScreenWidth() / 2 - MeasureText("PRESS [ENTER] TO START", 40) / 2,
				GetScreenHeight() / 2 - 70, 40, RED
			);

			if(IsKeyDown(KEY_ENTER))
				first_time = false;
		}
		else
		{
			DrawText(
				"PRESS [ENTER] TO PROCEED",
				20,20, 
				20, BLUE
			);

			DrawText(
				"PRESS [A] TO ENABLE AUTO-PLAY",
				GetScreenWidth() - MeasureText("PRESS [A] TO ENABLE AUTO-PLAY", 20) - 20,
				20, 20, BLUE
			);		
		}
	}
	else
	{
		State *next = ListGetNth(state->move_list,2);

		if(!state->trans.in_transition)
		{
			state->trans.in_transition = true;

			state->trans.offset_row = prev->blank_row - next->blank_row;
			state->trans.offset_col = prev->blank_col - next->blank_col;

			for(int i=0; i<state->board_size; i++)
            	for(int j=0; j<state->board_size; j++)
					if(prev->board[i][j] != 0)
					{
						DrawRectangle(state->positions[i][j].x+10,state->positions[i][j].y+10,state->edge-20,state->edge-20,WHITE);
						//DrawTexture(textures[ListGetNth(state->move_list,1)->board[i][j]-1],state->positions[i][j].x,state->positions[i][j].y,WHITE);
						char* buf = int_to_ascii(ListGetNth(state->move_list,1)->board[i][j]);
						DrawText(
							buf,
							state->positions[i][j].x + state->edge/2 - MeasureText(buf, (int)(state->edge/2))/2, 
							state->positions[i][j].y + state->edge/2 - 50,
							(int)(state->edge/2),BLACK
							);
						free(buf);
					}
		}
		else
		{
			float off_row = state->trans.offset_row * (state->edge/ANIMATION_SPEED);
			float off_col = state->trans.offset_col * (state->edge/ANIMATION_SPEED);
			state->positions[next->blank_row][next->blank_col].x += off_col;
			state->positions[next->blank_row][next->blank_col].y += off_row;

			for(int i=0; i<state->board_size; i++)
            	for(int j=0; j<state->board_size; j++)
					if(ListGetNth(state->move_list,1)->board[i][j] != 0)
					{
						DrawRectangle(state->positions[i][j].x+10,state->positions[i][j].y+10,state->edge-20,state->edge-20,WHITE);
						//DrawTexture(textures[ListGetNth(state->move_list,1)->board[i][j]-1],state->positions[i][j].x,state->positions[i][j].y,WHITE);
						char* buf = int_to_ascii(ListGetNth(state->move_list,1)->board[i][j]);
						DrawText(
							buf,
							state->positions[i][j].x + state->edge/2 - MeasureText(buf, (int)(state->edge/2))/2, 
							state->positions[i][j].y + state->edge/2 - 50,
							(int)(state->edge/2),BLACK
							);
						free(buf);
					}

			point blank_next = state->positions[next->blank_row][next->blank_col];
			point blank_prev = state->positions[prev->blank_row][prev->blank_col];

			if(blank_next.x == blank_prev.x && blank_next.y == blank_prev.y)
			{
				ListRemove_nth(state->move_list,1);
				state->trans.in_transition = false;
				off_row = state->trans.offset_row * state->edge;
				off_col = state->trans.offset_col * state->edge;
				state->positions[next->blank_row][next->blank_col].x -= off_col;
				state->positions[next->blank_row][next->blank_col].y -= off_row;
			}
		}

		if(autoplay)
		{
			DrawText(
				"PRESS [A] TO DISABLE AUTO-PLAY",
				GetScreenWidth() - MeasureText("PRESS [A] TO DISABLE AUTO-PLAY", 20) - 20,
				20, 20, BLUE
			);	
		}
	}

	EndDrawing();
}

void interface_close() 
{
	free(textures);
	CloseAudioDevice();
	CloseWindow();
}

char* int_to_ascii(int num)
{
	char* buf;
	if(num<=9)
	{
		buf = malloc(sizeof(char)*2);
		buf[0] = num + 48;
		buf[1] = 0;
	}
	else
	{
		buf = malloc(sizeof(char)*3);
		buf[0] = (num / 10) + 48;
		buf[1] = (num % 10) + 48;
		buf[2] = 0;
	}
	return buf;
}