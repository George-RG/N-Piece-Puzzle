#include "interface.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define CYAN \
	(Color) { 0, 145, 143, 255 } // Cyan
#define LLGRAY \
	(Color) { 220, 220, 220, 255 } // Light Light Gray

typedef enum play_mode {
	NONE,
	AUTO,
	MANUAL
}play_mode;

char *int_to_ascii(int num);

// Interface Globals
Texture *textures;
bool first_time = true;
bool first_end = true;
Sound clap;

// Text Box globals
int framesCounter = 0;
char *puzzle_str = NULL;
int letter_count = 0;
int OLD_MAX = -1;

// Size Box globals
int framsCounterSize = 0;
char size_str[] = {0, 0, 0};
int size_count = 0;

// Mode Globals
play_mode cur_mode = NONE;


void interface_init()
{
	// Αρχικοποίηση του παραθύρου
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Puzzle");
	SetTargetFPS(60);

	textures = malloc(sizeof(Texture) * 8);

	InitAudioDevice();
	clap = LoadSound("assets/clap.wav");
}

void interface_draw_frame(Graphics state, bool play, int autoplay, bool *in_menu)
{
	if (*in_menu)
	{
		int MAX_CHAR = -1;
		//int spaces;

		// Update the string
		if (state->board_size != -1)
		{
			MAX_CHAR = state->board_size * state->board_size;

			int temp = MAX_CHAR;
			if (temp > 9)
			{
				temp -= 9;
				temp *= 2;
				temp += 9;
			}
			MAX_CHAR--;
			//spaces = MAX_CHAR;
			MAX_CHAR += temp;
		}

		if (MAX_CHAR != OLD_MAX)
		{
			OLD_MAX = MAX_CHAR;

			if (puzzle_str != NULL)
				free(puzzle_str);

			puzzle_str = malloc(sizeof(char) * (MAX_CHAR + 1));
			puzzle_str[0] = '\0';
			letter_count = 0;
		}

		int min = 300;

		if (MAX_CHAR != -1)
		{
			char *max_string;
			int temp = 0;

			max_string = malloc(sizeof(char) * MAX_CHAR + 1);
			for (int i = 0; i < MAX_CHAR; i++)
			{
				if(i < 9 + 9) // 9 chars and 9 spaces
				{
					if(i % 2 == 0)
						max_string[i] = '0';
					else
						max_string[i] = ' ';
				}
				else 
				{
					if((i-20) % 3 == 0)
						max_string[i] = ' ';
					else if(i < 19 + 9) // 19 chars and 9 spaces
					{
						if(temp % 2 == 0)
							max_string[i] = '1';
						else
							max_string[i] = '0';
						
						temp++;
					}
					else	
						max_string[i] = '0';
				}
			}

			max_string[MAX_CHAR] = '\0';

			if (min < MeasureText(max_string, 35))
				min = MeasureText(max_string, 35);

			free(max_string);
		}

		Rectangle textBox = (Rectangle){SCREEN_WIDTH / 2 - min / 2, 300, min, 50};
		Rectangle sizeBox = (Rectangle){SCREEN_WIDTH / 2 + (MeasureText("Puzzle Size", 30) + 50 + 10) / 2 - 50, 190, 50, 50};
		//Rectangle enterBox = (Rectangle){SCREEN_WIDTH / 2 + 130, SCREEN_HEIGHT / 2 - 25, 50, 50};

		Rectangle autoBox = (Rectangle){SCREEN_WIDTH/2 - (200 + 200 + 100)/2, 450, 200, 140}; 
		Rectangle manBox = (Rectangle){SCREEN_WIDTH/2 + (200 + 200 + 100)/2 - 200, 450, 200, 140}; 
		bool mouseOnText = false;
		bool mouseOnSize = false;

		//Text Boxes Update

		// Update the mouse position
		if (CheckCollisionPointRec(GetMousePosition(), textBox))
			mouseOnText = true;
		else
			mouseOnText = false;

		if (CheckCollisionPointRec(GetMousePosition(), sizeBox))
			mouseOnSize = true;
		else
			mouseOnSize = false;

		if (mouseOnText)
		{
			// Set the window's cursor to the I-Beam
			SetMouseCursor(MOUSE_CURSOR_IBEAM);

			// Update the puzzle string
			int key = GetCharPressed();

			// Check if more characters have been pressed on the same frame
			while (key > 0)
			{
				// NOTE: Only allow keys in range [48..57] (0..9)
				if ((((key >= '0') && (key <= '9')) || (key == ' ')) && (letter_count < MAX_CHAR))
				{
					puzzle_str[letter_count] = (char)key;
					puzzle_str[letter_count + 1] = '\0'; // Add null terminator at the end of the string.
					letter_count++;
				}

				key = GetCharPressed(); // Check next character in the queue
			}

			if (IsKeyPressed(KEY_BACKSPACE))
			{
				letter_count--;
				if (letter_count < 0)
					letter_count = 0;
				puzzle_str[letter_count] = '\0';
			}
		}
		else if (mouseOnSize)
		{
			// Set the window's cursor to the I-Beam
			SetMouseCursor(MOUSE_CURSOR_IBEAM);

			// Update the puzzle string
			int key = GetCharPressed();

			// Check if more characters have been pressed on the same frame
			while (key > 0)
			{
				// NOTE: Only allow keys in range [48..57] (0..9)
				if ((key >= 48) && (key <= 57) && (size_count < 2))
				{
					size_str[size_count] = (char)key;
					size_str[size_count + 1] = '\0'; // Add null terminator at the end of the string.
					size_count++;
				}

				key = GetCharPressed(); // Check next character in the queue
			}

			if (IsKeyPressed(KEY_BACKSPACE))
			{
				size_count--;
				if (size_count < 0)
					size_count = 0;
				size_str[size_count] = '\0';
			}

			if (size_str[0] != '\0')
				state->board_size = atoi(size_str);
			else
				state->board_size = 0;
		}
		else
			SetMouseCursor(MOUSE_CURSOR_DEFAULT);

		if (mouseOnText)
			framesCounter++;
		else
			framesCounter = 0;

		if (mouseOnSize)
			framsCounterSize++;
		else
			framsCounterSize = 0;

		// Button Updates
		bool mouseOnAuto = false;
		bool mouseOnMan = false;

		if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), autoBox))
			cur_mode = AUTO;
		else if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), manBox))
			cur_mode = MANUAL;

		if(CheckCollisionPointRec(GetMousePosition(), autoBox))
			mouseOnAuto = true;
		else
			mouseOnAuto = false;

		if(CheckCollisionPointRec(GetMousePosition(), manBox))
			mouseOnMan = true;
		else
			mouseOnMan = false;

		// Draw the text box
		BeginDrawing();

		ClearBackground(RAYWHITE);

		if (mouseOnText)
		{
			DrawRectangleRec(textBox, CYAN);
			DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, BLACK);
			DrawText(puzzle_str, (int)textBox.x + 5, (int)textBox.y + 8, 35, RAYWHITE);

			DrawRectangleRec(sizeBox, LLGRAY);
			DrawRectangleLines((int)sizeBox.x, (int)sizeBox.y, (int)sizeBox.width, (int)sizeBox.height, DARKGRAY);
			DrawText(size_str, (int)sizeBox.x + 5, (int)sizeBox.y + 8, 40, CYAN);
		}
		else if (mouseOnSize)
		{
			DrawRectangleRec(sizeBox, CYAN);
			DrawRectangleLines((int)sizeBox.x, (int)sizeBox.y, (int)sizeBox.width, (int)sizeBox.height, BLACK);
			DrawText(size_str, (int)sizeBox.x + 5, (int)sizeBox.y + 8, 40, RAYWHITE);

			DrawRectangleRec(textBox, LLGRAY);
			DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, DARKGRAY);
			DrawText(puzzle_str, (int)textBox.x + 5, (int)textBox.y + 8, 35, CYAN);
		}
		else
		{
			DrawRectangleRec(textBox, LLGRAY);
			DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, DARKGRAY);
			DrawText(puzzle_str, (int)textBox.x + 5, (int)textBox.y + 8, 35, CYAN);

			DrawRectangleRec(sizeBox, LLGRAY);
			DrawRectangleLines((int)sizeBox.x, (int)sizeBox.y, (int)sizeBox.width, (int)sizeBox.height, DARKGRAY);
			DrawText(size_str, (int)sizeBox.x + 5, (int)sizeBox.y + 8, 40, CYAN);
		}

		if (MAX_CHAR == -1)
			MAX_CHAR = 0;

		DrawText(TextFormat("INPUT CHARS: %i/%i", letter_count, MAX_CHAR), SCREEN_WIDTH / 2 - MeasureText(TextFormat("INPUT CHARS: %i/%i", letter_count, MAX_CHAR), 20) / 2, 360, 20, DARKGRAY);
		// TODO
		// DrawText(TextFormat("PRESS ENTER TO START"), 315, 275, 20, DARKGRAY);
		// DrawText(TextFormat("PRESS ESC TO EXIT"), 315, 300, 20, DARKGRAY);
		// DrawText(TextFormat("PRESS A TO TOGGLE AUTOSOVLE"), 315, 325, 20, DARKGRAY);
		DrawText(TextFormat("N-Piece Puzzle"), SCREEN_WIDTH / 2 - MeasureText("N-Piece Puzzle", 70) / 2, 25, 70, CYAN);
		DrawText(TextFormat("Puzzle Size"), SCREEN_WIDTH / 2 - (MeasureText("Puzzle Size", 30) + 50 + 10) / 2, 200, 30, DARKGRAY);
		DrawText(TextFormat("Current Puzzle"), SCREEN_WIDTH / 2 - MeasureText("Current Puzzle", 30) / 2, 260, 30, DARKGRAY);
		DrawText(TextFormat("Made by GeorgeRG"), SCREEN_WIDTH / 2 - MeasureText("Made by GeorgeRG", 25) / 2, SCREEN_HEIGHT - 35, 25, CYAN);

		if (mouseOnText)
		{
			if (letter_count < MAX_CHAR)
			{
				// Draw blinking underscore char
				if (((framesCounter / 20) % 2) == 0)
					DrawText("_", (int)textBox.x + 8 + MeasureText(puzzle_str, 35), (int)textBox.y + 12, 35, RAYWHITE);
			}
			else if (MAX_CHAR != 0)
			{
				DrawText("Press BACKSPACE to delete chars...", 230, 300, 20, GRAY);
			}
		}

		if (mouseOnSize)
		{
			if (size_count < 2)
			{
				// Draw blinking underscore char
				if (((framsCounterSize / 20) % 2) == 0)
					DrawText("_", (int)sizeBox.x + 8 + MeasureText(size_str, 40), (int)sizeBox.y + 12, 40, RAYWHITE);
			}
		}
		
		// Draw the buttons

		if (cur_mode == AUTO)
		{
			DrawRectangleRec(autoBox, CYAN);
			DrawText("AUTO", (int)autoBox.x + ((int)autoBox.width)/2 - MeasureText("AUTO",40)/2, (int)autoBox.y + ((int)autoBox.height)/2 - 20, 40, RAYWHITE);

			DrawRectangleRec(manBox, LLGRAY);
			DrawText("MANUAL", (int)manBox.x + ((int)manBox.width)/2 - MeasureText("MANUAL",40)/2, (int)manBox.y + ((int)manBox.height)/2 - 20, 40, CYAN);
		}
		else if(cur_mode == MANUAL)
		{
			DrawRectangleRec(autoBox, LLGRAY);
			DrawText("AUTO", (int)autoBox.x + ((int)autoBox.width)/2 - MeasureText("AUTO",40)/2, (int)autoBox.y + ((int)autoBox.height)/2 - 20, 40, CYAN);

			DrawRectangleRec(manBox, CYAN);
			DrawText("MANUAL", (int)manBox.x + ((int)manBox.width)/2 - MeasureText("MANUAL",40)/2, (int)manBox.y + ((int)manBox.height)/2 - 20, 40, RAYWHITE);
		}
		else
		{
			DrawRectangleRec(autoBox, LLGRAY);
			DrawText("AUTO", (int)autoBox.x + ((int)autoBox.width)/2 - MeasureText("AUTO",40)/2, (int)autoBox.y + ((int)autoBox.height)/2 - 20, 40, CYAN);

			DrawRectangleRec(manBox, LLGRAY);
			DrawText("MANUAL", (int)manBox.x + ((int)manBox.width)/2 - MeasureText("MANUAL",40)/2, (int)manBox.y + ((int)manBox.height)/2 - 20, 40, CYAN);
		}

		if(mouseOnAuto)
		{
			DrawRectangleLines((int)autoBox.x, (int)autoBox.y, (int)autoBox.width, (int)autoBox.height, BLACK);

			DrawRectangleLines((int)manBox.x, (int)manBox.y, (int)manBox.width, (int)manBox.height, DARKGRAY);
		}
		else if(mouseOnMan)
		{
			DrawRectangleLines((int)autoBox.x, (int)autoBox.y, (int)autoBox.width, (int)autoBox.height, DARKGRAY);

			DrawRectangleLines((int)manBox.x, (int)manBox.y, (int)manBox.width, (int)manBox.height, BLACK);
		}
		else
		{
			DrawRectangleLines((int)autoBox.x, (int)autoBox.y, (int)autoBox.width, (int)autoBox.height, DARKGRAY);

			DrawRectangleLines((int)manBox.x, (int)manBox.y, (int)manBox.width, (int)manBox.height, DARKGRAY);	
		}

		DrawFPS(10, 10);

		EndDrawing();
	}
	else
	{
		State *prev = ListGetNth(state->move_list, 1);

		BeginDrawing();

		// Καθαρισμός, θα τα σχεδιάσουμε όλα από την αρχή
		ClearBackground(BLACK);

		if ((play == false && state->trans.in_transition == false) || isGoal(*prev) || first_time == true)
		{
			for (int i = 0; i < state->board_size; i++)
				for (int j = 0; j < state->board_size; j++)
					if (prev->board[i][j] != 0)
					{
						DrawRectangle(state->positions[i][j].x + 10, state->positions[i][j].y + 10, state->edge - 20, state->edge - 20, WHITE);
						// DrawTexture(textures[ListGetNth(state->move_list,1)->board[i][j]-1],state->positions[i][j].x,state->positions[i][j].y,WHITE);
						char *buf = int_to_ascii(ListGetNth(state->move_list, 1)->board[i][j]);
						DrawText(
							buf,
							state->positions[i][j].x + state->edge / 2 - MeasureText(buf, (int)(state->edge / 2)) / 2,
							state->positions[i][j].y + state->edge / 2 - 50,
							(int)(state->edge / 2), BLACK);
						free(buf);
					}

			if (isGoal(*prev))
			{
				DrawText(
					"PRESS [ESC] TO EXIT",
					GetScreenWidth() / 2 - MeasureText("PRESS [ESC] TO EXIT", 40) / 2,
					GetScreenHeight() / 2 - 70, 40, RED);

				if (first_end)
				{
					PlaySound(clap);
					first_end = false;
				}
			}
			else if (first_time)
			{
				DrawText(
					"PRESS [ENTER] TO START",
					GetScreenWidth() / 2 - MeasureText("PRESS [ENTER] TO START", 40) / 2,
					GetScreenHeight() / 2 - 70, 40, RED);

				if (IsKeyDown(KEY_ENTER))
					first_time = false;
			}
			else
			{
				DrawText(
					"PRESS [ENTER] TO PROCEED",
					20, 20,
					20, BLUE);

				DrawText(
					"PRESS [A] TO ENABLE AUTO-PLAY",
					GetScreenWidth() - MeasureText("PRESS [A] TO ENABLE AUTO-PLAY", 20) - 20,
					20, 20, BLUE);
			}
		}
		else
		{
			State *next = ListGetNth(state->move_list, 2);

			if (!state->trans.in_transition)
			{
				state->trans.in_transition = true;

				state->trans.offset_row = prev->blank_row - next->blank_row;
				state->trans.offset_col = prev->blank_col - next->blank_col;

				for (int i = 0; i < state->board_size; i++)
					for (int j = 0; j < state->board_size; j++)
						if (prev->board[i][j] != 0)
						{
							DrawRectangle(state->positions[i][j].x + 10, state->positions[i][j].y + 10, state->edge - 20, state->edge - 20, WHITE);
							// DrawTexture(textures[ListGetNth(state->move_list,1)->board[i][j]-1],state->positions[i][j].x,state->positions[i][j].y,WHITE);
							char *buf = int_to_ascii(ListGetNth(state->move_list, 1)->board[i][j]);
							DrawText(
								buf,
								state->positions[i][j].x + state->edge / 2 - MeasureText(buf, (int)(state->edge / 2)) / 2,
								state->positions[i][j].y + state->edge / 2 - 50,
								(int)(state->edge / 2), BLACK);
							free(buf);
						}
			}
			else
			{
				float off_row = state->trans.offset_row * (state->edge / ANIMATION_SPEED);
				float off_col = state->trans.offset_col * (state->edge / ANIMATION_SPEED);
				state->positions[next->blank_row][next->blank_col].x += off_col;
				state->positions[next->blank_row][next->blank_col].y += off_row;

				for (int i = 0; i < state->board_size; i++)
					for (int j = 0; j < state->board_size; j++)
						if (ListGetNth(state->move_list, 1)->board[i][j] != 0)
						{
							DrawRectangle(state->positions[i][j].x + 10, state->positions[i][j].y + 10, state->edge - 20, state->edge - 20, WHITE);
							// DrawTexture(textures[ListGetNth(state->move_list,1)->board[i][j]-1],state->positions[i][j].x,state->positions[i][j].y,WHITE);
							char *buf = int_to_ascii(ListGetNth(state->move_list, 1)->board[i][j]);
							DrawText(
								buf,
								state->positions[i][j].x + state->edge / 2 - MeasureText(buf, (int)(state->edge / 2)) / 2,
								state->positions[i][j].y + state->edge / 2 - 50,
								(int)(state->edge / 2), BLACK);
							free(buf);
						}

				point blank_next = state->positions[next->blank_row][next->blank_col];
				point blank_prev = state->positions[prev->blank_row][prev->blank_col];

				if (blank_next.x == blank_prev.x && blank_next.y == blank_prev.y)
				{
					ListRemove_nth(state->move_list, 1);
					state->trans.in_transition = false;
					off_row = state->trans.offset_row * state->edge;
					off_col = state->trans.offset_col * state->edge;
					state->positions[next->blank_row][next->blank_col].x -= off_col;
					state->positions[next->blank_row][next->blank_col].y -= off_row;
				}
			}

			if (autoplay)
			{
				DrawText(
					"PRESS [A] TO DISABLE AUTO-PLAY",
					GetScreenWidth() - MeasureText("PRESS [A] TO DISABLE AUTO-PLAY", 20) - 20,
					20, 20, BLUE);
			}
		}

		EndDrawing();
	}
}

void interface_close()
{
	free(textures);
	CloseAudioDevice();
	CloseWindow();
}

char *int_to_ascii(int num)
{
	char *buf;
	if (num <= 9)
	{
		buf = malloc(sizeof(char) * 2);
		buf[0] = num + 48;
		buf[1] = 0;
	}
	else
	{
		buf = malloc(sizeof(char) * 3);
		buf[0] = (num / 10) + 48;
		buf[1] = (num % 10) + 48;
		buf[2] = 0;
	}
	return buf;
}