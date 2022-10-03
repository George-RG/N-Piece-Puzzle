#include "interface.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <pthread.h>
#include <time.h>

#define CYAN \
	(Color) { 0, 145, 143, 255 } // Cyan

#define LIGHTCYAN \
	(Color) { 8, 119, 134, 255 } // Light Cyan

#define LLGRAY \
	(Color) { 220, 220, 220, 255 } // Light Light Gray

#define TRANSPARENT_BLACK \
	(Color) { 0, 0, 0, 200 } // Transparent Black

typedef enum play_mode
{
	NONE,
	AUTO,
	MANUAL,
	SHUFFLE
} play_mode;

char *int_to_ascii(int num);
char *remove_spaces(char *str);

// Interface Globals
bool first_time = true;
bool first_end = true;
Sound clap;
Texture paste;
Texture copy;
bool started = false;
State *cur_state = NULL;
float seconds = 0;

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

// Solver Globals
bool solver_running = false;
char *solver_str = NULL;

// Auto Globals
int moves = 0;
int total_moves = 0;
bool auto_play = false;
bool play = false;

// Manual Globals
State *manual_next_state = NULL;

// SHUFFLE globals
int rand_moves;
State *rand_next_state = NULL;
Move last = 10;

void interface_init()
{
	// Αρχικοποίηση του παραθύρου
	InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Puzzle");
	SetTargetFPS(60);

	InitAudioDevice();
	clap = LoadSound("assets/clap.wav");

	Image temp = LoadImage("assets/paste.png");
	ImageResize(&temp, 40, 40);

	paste = LoadTextureFromImage(temp);

	temp = LoadImage("assets/copy.png");
	ImageResize(&temp, 40, 40);

	copy = LoadTextureFromImage(temp);
}

void interface_draw_frame(Graphics *gr_state_ptr, bool *in_menu)
{
	Graphics gr_state = *gr_state_ptr;

	if (*in_menu)
	{
		if (started == true && gr_state->move_list == NULL)
		{
			int N = atoi(size_str);

			if (N < 2 || N > 5)
			{
				printf("Invalid size.\n");
				started = false;
				return;
			}

			free_gra_state(gr_state);

			*gr_state_ptr = create_gra_state(N);
			gr_state = *gr_state_ptr;

			cur_state = malloc(sizeof(State));
			cur_state->board = malloc(sizeof(int *) * N);
			for (int i = 0; i < N; i++)
				cur_state->board[i] = malloc(sizeof(int) * N);
			cur_state->representation = NULL;
			cur_state->moves = 0;
			cur_state->parent = NULL;
			cur_state->size = N;

			int col = 0, row = 0;
			int i = 0, j;
			char buff[100];
			int cur;
			bool exists[N * N];
			while (puzzle_str[i] != '\0')
			{
				j = 0;
				buff[j] = '\0';

				while (puzzle_str[i] != ' ' && puzzle_str[i] != '\0')
				{
					buff[j] = puzzle_str[i];
					buff[j + 1] = '\0';
					j++;
					i++;
				}
				if (puzzle_str[i] == ' ')
					i++;

				cur = atoi(buff);

				if (cur > N * N - 1 || cur < 0)
				{
					printf("Invalid puzzle.\n");
					started = false;
					destroyfunc(cur_state);
					free_gra_state(gr_state);
					*gr_state_ptr = create_dumy_state();
					(*gr_state_ptr)->board_size = -1;
					letter_count = 0;
					puzzle_str[0] = '\0';
					return;
				}

				cur_state->board[row][col] = cur;
				exists[cur] = true;
				if (cur == 0)
				{
					cur_state->blank_row = row;
					cur_state->blank_col = col;
				}

				col++;
				if (col == N)
				{
					col = 0;
					row++;
				}
			}

			for (int i = 0; i < N * N; i++)
			{
				if (!exists[i])
				{
					printf("Missing Number.\n");
					started = false;
					destroyfunc(cur_state);
					int temp_size = (*gr_state_ptr)->board_size;
					free_gra_state(gr_state);
					*gr_state_ptr = create_dumy_state();
					(*gr_state_ptr)->board_size = temp_size;
					letter_count = 0;
					puzzle_str[0] = '\0';
					return;
				}
			}

			// if (!IsSolveable(cur_state))
			// {
			// 	printf("Puzzle is not solvable\n");
			// 	started = false;
			// 	destroyfunc(cur_state);
			// 	int temp_size = (*gr_state_ptr)->board_size;
			// 	free_gra_state(gr_state);
			// 	*gr_state_ptr = create_dumy_state();
			// 	(*gr_state_ptr)->board_size = temp_size;
			// 	letter_count = 0;
			// 	puzzle_str[0] = '\0';
			// 	return;
			// }

			if (isGoal(*cur_state) == 1)
			{
				printf("This puzzle is already solved\n");
				started = false;
				destroyfunc(cur_state);
				int temp_size = (*gr_state_ptr)->board_size;
				free_gra_state(gr_state);
				*gr_state_ptr = create_dumy_state();
				(*gr_state_ptr)->board_size = temp_size;
				return;
			}

			if (cur_mode == NONE)
			{
				printf("No mode selected\n");
				started = false;
				destroyfunc(cur_state);
				free_gra_state(gr_state);
				*gr_state_ptr = create_dumy_state();
				(*gr_state_ptr)->board_size = -1;
				return;
			}

			if (cur_mode == AUTO)
			{
				gr_state->move_list = NULL;

				thread_data *arg = malloc(sizeof(thread_data));
				arg->input = cur_state;
				ListPtr *temp = &gr_state->move_list;
				arg->result = temp;

				pthread_t thread_id;

				pthread_create(&thread_id, NULL, solve_new, arg);

				solver_running = true;
				started = false;
			}

			if (cur_mode == MANUAL)
			{
				*in_menu = false;
				moves = 0;
			}
		}

		int MAX_CHAR = -1;

		// Update the string
		if (gr_state->board_size != -1)
		{
			MAX_CHAR = gr_state->board_size * gr_state->board_size;

			int temp = MAX_CHAR;
			if (temp > 9)
			{
				temp -= 9;
				temp *= 2;
				temp += 8;
			}
			MAX_CHAR--;
			MAX_CHAR += temp;
		}

		if (MAX_CHAR != OLD_MAX && MAX_CHAR > 1)
		{
			if (puzzle_str != NULL)
				free(puzzle_str);

			puzzle_str = malloc(sizeof(char) * (MAX_CHAR + 1));
			puzzle_str[0] = '\0';
			letter_count = 0;
		}

		if (MAX_CHAR != OLD_MAX)
			OLD_MAX = MAX_CHAR;

		int min = 300;

		if (MAX_CHAR != -1)
		{
			char *max_string;
			int temp = 0;

			max_string = malloc(sizeof(char) * MAX_CHAR + 1);
			for (int i = 0; i < MAX_CHAR; i++)
			{
				if (i < 9 + 9) // 9 chars and 9 spaces
				{
					if (i % 2 == 0)
						max_string[i] = '0';
					else
						max_string[i] = ' ';
				}
				else
				{
					if ((i - 20) % 3 == 0)
						max_string[i] = ' ';
					else if (i < 19 + 9) // 19 chars and 9 spaces
					{
						if (temp % 2 == 0)
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

			int max = SCREEN_WIDTH - 250;
			if (min > max)
				min = max;

			free(max_string);
		}

		// Rectangle positions
		Rectangle textBox = (Rectangle){SCREEN_WIDTH / 2 - min / 2 - ((paste.width / 2) * 2) - 20 / 2, 300, min, 50};
		Rectangle pasteBox = (Rectangle){textBox.x + textBox.width + 10, 300, textBox.height, textBox.height};
		Rectangle copyBox = (Rectangle){pasteBox.x + pasteBox.width + 10, 300, textBox.height, textBox.height};
		Rectangle sizeBox = (Rectangle){SCREEN_WIDTH / 2 + (MeasureText("Puzzle Size", 30) + 50 + 10) / 2 - 50, 190, 50, 50};
		Rectangle enterBox = (Rectangle){SCREEN_WIDTH / 2 - 110, 650, 220, 80};
		Rectangle autoBox = (Rectangle){SCREEN_WIDTH / 2 - (200 + 200 + 100) / 2, 450, 200, 140};
		Rectangle manBox = (Rectangle){SCREEN_WIDTH / 2 + (200 + 200 + 100) / 2 - 200, 450, 200, 140};
		Rectangle exitBox = (Rectangle){SCREEN_WIDTH - 30 - 170, SCREEN_HEIGHT - 30 - 50, 170, 60};
		Rectangle shuffleBox = (Rectangle){GetScreenWidth() / 2 - MeasureText("Shuffle", 30) / 2 - 10, textBox.y + textBox.height + 40, MeasureText("Shuffle", 30) + 20, 40};

		// Text box triggers
		bool mouseOnText = false;
		bool mouseOnSize = false;

		// Button triggers
		bool mouseOnAuto = false;
		bool mouseOnMan = false;
		bool mouseOnExit = false;
		bool mouseOnStart = false;
		bool mouseOnShuffle = false;
		bool mouseOnPaste = false;
		bool clickedPaste = false;
		bool mouseOnCopy = false;
		bool clickedCopy = false;

		//
		if (solver_running == false)
		{
			// Text Boxes Update

			// Update the mouse position
			if (CheckCollisionPointRec(GetMousePosition(), textBox))
				mouseOnText = true;
			else
				mouseOnText = false;

			if (CheckCollisionPointRec(GetMousePosition(), sizeBox))
				mouseOnSize = true;
			else
				mouseOnSize = false;

			mouseOnPaste = CheckCollisionPointRec(GetMousePosition(), pasteBox);

			if (mouseOnPaste && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
				clickedPaste = true;

			mouseOnCopy = CheckCollisionPointRec(GetMousePosition(), copyBox);

			if (mouseOnCopy && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
				clickedCopy = true;

			if ((mouseOnText || clickedPaste || clickedCopy) && puzzle_str != NULL)
			{
				if (mouseOnText)
					// Set the window's cursor to the I-Beam
					SetMouseCursor(MOUSE_CURSOR_IBEAM);

				// Update the puzzle string
				int key = GetCharPressed();

				// Check if more characters have been pressed on the same frame
				while (key > 0 && !clickedPaste && !clickedCopy)
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

				if (IsKeyPressed(KEY_BACKSPACE) && !clickedPaste && !clickedCopy)
				{
					letter_count--;
					if (letter_count < 0)
						letter_count = 0;
					puzzle_str[letter_count] = '\0';
				}

				if (((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyPressed(KEY_V)) || clickedPaste)
				{
					const char *clipboard = GetClipboardText();
					if (clipboard != NULL)
					{
						for (int i = 0; i < strlen(clipboard); i++)
						{
							if ((((clipboard[i] >= '0') && (clipboard[i] <= '9')) || (clipboard[i] == ' ')) && (letter_count < MAX_CHAR))
							{
								puzzle_str[letter_count] = clipboard[i];
								puzzle_str[letter_count + 1] = '\0'; // Add null terminator at the end of the string.
								letter_count++;
							}
						}
					}
				}

				if (((IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL)) && IsKeyPressed(KEY_C)) || clickedCopy)
				{
					SetClipboardText(puzzle_str);
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
					gr_state->board_size = atoi(size_str);
				else
				{
					gr_state->board_size = 0;
					if (puzzle_str != NULL)
					{
						free(puzzle_str);
						puzzle_str = NULL;
						letter_count = 0;
					}
				}
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

			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), autoBox))
				cur_mode = AUTO;
			else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(GetMousePosition(), manBox))
				cur_mode = MANUAL;

			if (CheckCollisionPointRec(GetMousePosition(), autoBox))
				mouseOnAuto = true;
			else
				mouseOnAuto = false;

			if (CheckCollisionPointRec(GetMousePosition(), manBox))
				mouseOnMan = true;
			else
				mouseOnMan = false;

			if (CheckCollisionPointRec(GetMousePosition(), enterBox))
				mouseOnStart = true;
			else
				mouseOnStart = false;

			if (CheckCollisionPointRec(GetMousePosition(), exitBox))
				mouseOnExit = true;
			else
				mouseOnExit = false;

			mouseOnShuffle = CheckCollisionPointRec(GetMousePosition(), shuffleBox);

			if ((CheckCollisionPointRec(GetMousePosition(), shuffleBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) && size_str[0] != '\0')
			{
				cur_mode = SHUFFLE;
				*in_menu = false;
				
				int N = atoi(size_str);

				if (N < 2 || N > 5)
				{
					printf("Invalid size.\n");
					cur_mode = NONE;
					*in_menu = true;
					return;
				}

				// Create PUzzle to shuffle
				if (puzzle_str != NULL)
				{
					free(puzzle_str);
					puzzle_str = NULL;
					letter_count = 0;
				}

				free_gra_state(gr_state);

				*gr_state_ptr = create_gra_state(N);
				gr_state = *gr_state_ptr;

				cur_state = malloc(sizeof(State));
				cur_state->board = malloc(sizeof(int *) * N);
				for (int i = 0; i < N; i++)
					cur_state->board[i] = malloc(sizeof(int) * N);
				cur_state->representation = NULL;
				cur_state->moves = 0;
				cur_state->parent = NULL;
				cur_state->size = N;

				int i = 1;
				for(int row = 0; row < N; row++)
				{
					for(int col = 0; col < N; col++)
					{
						cur_state->board[row][col] = i;
						i++;
					}
				}
				cur_state->board[N - 1][N - 1] = 0;

				cur_state->blank_col = N - 1;
				cur_state->blank_row = N - 1;

				// Shuffle the puzzle
				rand_moves = 0;
				*in_menu = false;
				cur_mode = SHUFFLE;
				srand(time(NULL));
			}

			if ((CheckCollisionPointRec(GetMousePosition(), enterBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) || IsKeyPressed(KEY_ENTER))
			{
				started = true;
			}

			if ((CheckCollisionPointRec(GetMousePosition(), exitBox) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)))
			{
				// Exit the game
				if (puzzle_str != NULL)
					free(puzzle_str);

				interface_close();

				exit(0);
			}
		}

		// Draw the text box
		BeginDrawing();

		ClearBackground(RAYWHITE);

		char *shown_str = puzzle_str;

		int guard = -10;

		if (letter_count < MAX_CHAR)
			guard -= MeasureText("_", 35);

		while (MeasureText(shown_str, 35) > textBox.width + guard)
		{
			shown_str++;
		}

		if (mouseOnText)
		{
			DrawRectangleRec(textBox, CYAN);
			DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, BLACK);
			DrawText(shown_str, (int)textBox.x + 5, (int)textBox.y + 8, 35, RAYWHITE);

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
			DrawText(shown_str, (int)textBox.x + 5, (int)textBox.y + 8, 35, CYAN);
		}
		else
		{
			DrawRectangleRec(textBox, LLGRAY);
			DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, DARKGRAY);
			DrawText(shown_str, (int)textBox.x + 5, (int)textBox.y + 8, 35, CYAN);

			DrawRectangleRec(sizeBox, LLGRAY);
			DrawRectangleLines((int)sizeBox.x, (int)sizeBox.y, (int)sizeBox.width, (int)sizeBox.height, DARKGRAY);
			DrawText(size_str, (int)sizeBox.x + 5, (int)sizeBox.y + 8, 40, CYAN);
		}

		if (MAX_CHAR == -1)
			MAX_CHAR = 0;

		if (letter_count != MAX_CHAR)
			DrawText(TextFormat("INPUT CHARS: %i/%i", letter_count, MAX_CHAR), SCREEN_WIDTH / 2 - MeasureText(TextFormat("INPUT CHARS: %i/%i", letter_count, MAX_CHAR), 20) / 2, textBox.y + textBox.height + 10, 20, DARKGRAY);
		else
			DrawText(TextFormat("INPUT CHARS: %i/%i", letter_count, MAX_CHAR), SCREEN_WIDTH / 2 - MeasureText(TextFormat("INPUT CHARS: %i/%i", letter_count, MAX_CHAR), 20) / 2, textBox.y + textBox.height + 10, 20, MAROON);

		DrawText(TextFormat("N-Piece Puzzle"), SCREEN_WIDTH / 2 - MeasureText("N-Piece Puzzle", 70) / 2, 25, 70, CYAN);
		DrawText(TextFormat("Puzzle Size"), SCREEN_WIDTH / 2 - (MeasureText("Puzzle Size", 30) + 50 + 10) / 2, sizeBox.y + 15, 30, DARKGRAY);
		DrawText(TextFormat("Current Puzzle"), SCREEN_WIDTH / 2 - MeasureText("Current Puzzle", 30) / 2, textBox.y - 30, 30, DARKGRAY);
		DrawText(TextFormat("Made by GeorgeRG"), SCREEN_WIDTH / 2 - MeasureText("Made by GeorgeRG", 25) / 2, SCREEN_HEIGHT - 35, 25, CYAN);

		if (mouseOnText)
		{
			if (letter_count < MAX_CHAR)
			{
				// Draw blinking underscore char
				if (((framesCounter / 20) % 2) == 0)
					DrawText("_", (int)textBox.x + 8 + MeasureText(shown_str, 35), (int)textBox.y + 12, 35, RAYWHITE);
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
			DrawText("AUTO", (int)autoBox.x + ((int)autoBox.width) / 2 - MeasureText("AUTO", 40) / 2, (int)autoBox.y + ((int)autoBox.height) / 2 - 20, 40, RAYWHITE);

			DrawRectangleRec(manBox, LLGRAY);
			DrawText("MANUAL", (int)manBox.x + ((int)manBox.width) / 2 - MeasureText("MANUAL", 40) / 2, (int)manBox.y + ((int)manBox.height) / 2 - 20, 40, CYAN);
		}
		else if (cur_mode == MANUAL)
		{
			DrawRectangleRec(autoBox, LLGRAY);
			DrawText("AUTO", (int)autoBox.x + ((int)autoBox.width) / 2 - MeasureText("AUTO", 40) / 2, (int)autoBox.y + ((int)autoBox.height) / 2 - 20, 40, CYAN);

			DrawRectangleRec(manBox, CYAN);
			DrawText("MANUAL", (int)manBox.x + ((int)manBox.width) / 2 - MeasureText("MANUAL", 40) / 2, (int)manBox.y + ((int)manBox.height) / 2 - 20, 40, RAYWHITE);
		}
		else
		{
			DrawRectangleRec(autoBox, LLGRAY);
			DrawText("AUTO", (int)autoBox.x + ((int)autoBox.width) / 2 - MeasureText("AUTO", 40) / 2, (int)autoBox.y + ((int)autoBox.height) / 2 - 20, 40, CYAN);

			DrawRectangleRec(manBox, LLGRAY);
			DrawText("MANUAL", (int)manBox.x + ((int)manBox.width) / 2 - MeasureText("MANUAL", 40) / 2, (int)manBox.y + ((int)manBox.height) / 2 - 20, 40, CYAN);
		}

		if (mouseOnAuto)
		{
			DrawRectangleLines((int)autoBox.x, (int)autoBox.y, (int)autoBox.width, (int)autoBox.height, BLACK);

			DrawRectangleLines((int)manBox.x, (int)manBox.y, (int)manBox.width, (int)manBox.height, DARKGRAY);
		}
		else if (mouseOnMan)
		{
			DrawRectangleLines((int)autoBox.x, (int)autoBox.y, (int)autoBox.width, (int)autoBox.height, DARKGRAY);

			DrawRectangleLines((int)manBox.x, (int)manBox.y, (int)manBox.width, (int)manBox.height, BLACK);
		}
		else
		{
			DrawRectangleLines((int)autoBox.x, (int)autoBox.y, (int)autoBox.width, (int)autoBox.height, DARKGRAY);

			DrawRectangleLines((int)manBox.x, (int)manBox.y, (int)manBox.width, (int)manBox.height, DARKGRAY);
		}

		if (mouseOnStart)
			DrawRectangleRec(enterBox, LIGHTCYAN);
		else
			DrawRectangleRec(enterBox, CYAN);

		DrawRectangleLines((int)enterBox.x, (int)enterBox.y, (int)enterBox.width, (int)enterBox.height, DARKGRAY);
		DrawText("START", (int)enterBox.x + ((int)enterBox.width) / 2 - MeasureText("START", 40) / 2, (int)enterBox.y + ((int)enterBox.height) / 2 - 20, 40, RAYWHITE);

		DrawText("Press ENTER to start", SCREEN_WIDTH / 2 - MeasureText("Press ENTER to start", 20) / 2, (int)enterBox.y + (int)enterBox.height + 10, 20, DARKGRAY);

		if (mouseOnExit)
			DrawRectangleRec(exitBox, DARKGRAY);
		else
			DrawRectangleRec(exitBox, GRAY);

		DrawRectangleLines((int)exitBox.x, (int)exitBox.y, (int)exitBox.width, (int)exitBox.height, DARKGRAY);
		DrawText("EXIT", (int)exitBox.x + ((int)exitBox.width) / 2 - MeasureText("EXIT", 40) / 2, (int)exitBox.y + ((int)exitBox.height) / 2 - 20, 40, RAYWHITE);

		if (mouseOnPaste)
			DrawRectangleRec(pasteBox, LIGHTCYAN);
		else
			DrawRectangleRec(pasteBox, CYAN);

		DrawRectangleLines((int)pasteBox.x, (int)pasteBox.y, (int)pasteBox.width, (int)pasteBox.height, DARKGRAY);
		DrawTexture(paste, pasteBox.x + pasteBox.width / 2 - paste.width / 2, pasteBox.y + pasteBox.height / 2 - paste.height / 2, CYAN);

		if (mouseOnCopy)
			DrawRectangleRec(copyBox, LIGHTCYAN);
		else
			DrawRectangleRec(copyBox, CYAN);

		DrawRectangleLines((int)copyBox.x, (int)copyBox.y, (int)copyBox.width, (int)copyBox.height, DARKGRAY);
		DrawTexture(copy, copyBox.x + copyBox.width / 2 - paste.width / 2, copyBox.y + copyBox.height / 2 - paste.height / 2, CYAN);

		if (mouseOnShuffle)
			DrawRectangleRec(shuffleBox, LIGHTCYAN);
		else
			DrawRectangleRec(shuffleBox, CYAN);
		
		DrawRectangleLines((int)shuffleBox.x, (int)shuffleBox.y, (int)shuffleBox.width, (int)shuffleBox.height, DARKGRAY);
		DrawText("Shuffle", (int)shuffleBox.x + ((int)shuffleBox.width) / 2 - MeasureText("Shuffle", 30) / 2, (int)shuffleBox.y + ((int)shuffleBox.height) / 2 - 15, 30, RAYWHITE);

		if (solver_running)
		{
			float delta = GetFrameTime();
			seconds += delta;

			char final_str[] = "Solving...";

			if (solver_str == NULL)
			{
				solver_str = (char *)malloc(sizeof(char) * (strlen(final_str) + 1));
				solver_str[0] = '\0';
			}

			if (seconds > 0.15)
			{
				if (strlen(solver_str) == strlen(final_str))
				{
					solver_str[7] = '\0';
				}
				else
				{
					size_t temp = strlen(solver_str);
					solver_str[temp] = final_str[temp];
					solver_str[temp + 1] = '\0';
				}
				seconds = 0;
			}

			Rectangle rec = (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()};
			DrawRectangleRec(rec, TRANSPARENT_BLACK);

			DrawText(solver_str, SCREEN_WIDTH / 2 - MeasureText(final_str, 30) / 2, SCREEN_HEIGHT / 2 - 30, 30, RAYWHITE);

			if (gr_state->move_list != NULL)
			{
				solver_running = false;
				*in_menu = false;
				moves = 0;
				total_moves = ListSize(gr_state->move_list);
				cur_state = ListGetNth(gr_state->move_list, 1);
			}
		}

		EndDrawing();
	}
	else if (cur_mode == AUTO)
	{
		Rectangle back_button = (Rectangle){(SCREEN_WIDTH - PUZZLE_WIDTH) / 2 - (170 / 2), SCREEN_HEIGHT - 30 - 50, 170, 60};
		Rectangle procced_button = (Rectangle){(SCREEN_WIDTH - PUZZLE_WIDTH) / 2 - (210 / 2), SCREEN_HEIGHT / 2 - (70 + 20) / 2, 210, 70};

		// Update
		// Update moves progress bar
		int MAX_BAR_WIDTH = 300;
		Rectangle max_bar = (Rectangle){(GetScreenWidth() - PUZZLE_WIDTH) / 2 - MAX_BAR_WIDTH / 2, 100, MAX_BAR_WIDTH, 20};
		float progress = (float)moves / ((float)total_moves - 1.0);
		int bar_width = (int)(progress * MAX_BAR_WIDTH);
		if (bar_width > MAX_BAR_WIDTH - 4)
			bar_width = MAX_BAR_WIDTH - 4;

		// Update buttons
		bool mouseOnBack = CheckCollisionPointRec(GetMousePosition(), back_button);
		bool mouseOnProcced = CheckCollisionPointRec(GetMousePosition(), procced_button);

		if ((CheckCollisionPointRec(GetMousePosition(), back_button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)))
		{
			int MAX_CHAR = gr_state->board_size * gr_state->board_size;

			int temp = MAX_CHAR;
			if (temp > 9)
			{
				temp -= 9;
				temp *= 2;
				temp += 8;
			}
			MAX_CHAR--;
			MAX_CHAR += temp;

			if (puzzle_str != NULL)
				free(puzzle_str);

			puzzle_str = malloc(sizeof(char) * (MAX_CHAR + 1));
			letter_count = MAX_CHAR;

			int index = 0;
			for (int i = 0; i < gr_state->board_size; i++)
			{
				for (int j = 0; j < gr_state->board_size; j++)
				{
					if (cur_state->board[i][j] == 0)
						puzzle_str[index++] = '0';
					else
					{
						char *buf = int_to_ascii(cur_state->board[i][j]);

						int k;
						for (k = 0; k < strlen(buf); k++)
							puzzle_str[index + k] = buf[k];

						index += k;
					}
					puzzle_str[index++] = ' ';
				}
			}
			puzzle_str[MAX_CHAR] = '\0';

			destroyfunc(cur_state);

			temp = gr_state->board_size;

			gr_state->move_list = NULL;
			free_gra_state(gr_state);
			*gr_state_ptr = create_dumy_state();

			(*gr_state_ptr)->board_size = temp;

			cur_state = NULL;
			*in_menu = true;
			started = false;
			auto_play = 0;
			first_time = true;
			first_end = true;
			return;
		}

		bool procced = false;
		if (((CheckCollisionPointRec(GetMousePosition(), procced_button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))) || IsKeyPressed(KEY_ENTER))
		{
			procced = true;
		}

		// Get Move
		if (cur_state == NULL || !isGoal(*cur_state))
			cur_state = ListGetNth(gr_state->move_list, 1);
		else if (gr_state->move_list != NULL)
		{
			ListRemove_nth(gr_state->move_list, 1);
			freeList(gr_state->move_list);
			gr_state->move_list = NULL;
		}

		if (IsKeyPressed(KEY_A) && !isGoal(*cur_state))
			auto_play = !auto_play;

		if (first_time)
			play = procced;
		else
			play = (procced || auto_play);

		// Start Drawing
		BeginDrawing();

		// Καθαρισμός, θα τα σχεδιάσουμε όλα από την αρχή
		ClearBackground(DARKGRAY);

		if ((play == false && gr_state->trans.in_transition == false) || isGoal(*cur_state) || first_time == true)
		{
			for (int i = 0; i < gr_state->board_size; i++)
				for (int j = 0; j < gr_state->board_size; j++)
					if (cur_state->board[i][j] != 0)
					{
						DrawRectangle(gr_state->positions[i][j].x + 10, gr_state->positions[i][j].y + 10, gr_state->edge - 20, gr_state->edge - 20, LLGRAY);
						Rectangle temp = {gr_state->positions[i][j].x + 10, gr_state->positions[i][j].y + 10, gr_state->edge - 20, gr_state->edge - 20};
						DrawRectangleLinesEx(temp, 5, CYAN);

						char *buf = int_to_ascii(cur_state->board[i][j]);
						DrawText(
							buf,
							gr_state->positions[i][j].x + gr_state->edge / 2 - MeasureText(buf, (int)(gr_state->edge / 2)) / 2,
							gr_state->positions[i][j].y + gr_state->edge / 2 - 50,
							(int)(gr_state->edge / 2), CYAN);
						free(buf);
					}

			if (isGoal(*cur_state))
			{
				if (first_end)
				{
					PlaySound(clap);
					first_end = false;
				}
			}
			else if (first_time)
			{
				if (procced)
					first_time = false;
			}
		}
		else
		{
			State *next = ListGetNth(gr_state->move_list, 2);

			if (!gr_state->trans.in_transition)
			{
				gr_state->trans.in_transition = true;

				gr_state->trans.offset_row = cur_state->blank_row - next->blank_row;
				gr_state->trans.offset_col = cur_state->blank_col - next->blank_col;

				for (int i = 0; i < gr_state->board_size; i++)
					for (int j = 0; j < gr_state->board_size; j++)
						if (cur_state->board[i][j] != 0)
						{
							DrawRectangle(gr_state->positions[i][j].x + 10, gr_state->positions[i][j].y + 10, gr_state->edge - 20, gr_state->edge - 20, LLGRAY);
							Rectangle temp = {gr_state->positions[i][j].x + 10, gr_state->positions[i][j].y + 10, gr_state->edge - 20, gr_state->edge - 20};
							DrawRectangleLinesEx(temp, 5, CYAN);

							char *buf = int_to_ascii(cur_state->board[i][j]);
							DrawText(
								buf,
								gr_state->positions[i][j].x + gr_state->edge / 2 - MeasureText(buf, (int)(gr_state->edge / 2)) / 2,
								gr_state->positions[i][j].y + gr_state->edge / 2 - 50,
								(int)(gr_state->edge / 2), CYAN);
							free(buf);
						}
			}
			else
			{
				float off_row = gr_state->trans.offset_row * (gr_state->edge / ANIMATION_SPEED);
				float off_col = gr_state->trans.offset_col * (gr_state->edge / ANIMATION_SPEED);
				gr_state->positions[next->blank_row][next->blank_col].x += off_col;
				gr_state->positions[next->blank_row][next->blank_col].y += off_row;

				for (int i = 0; i < gr_state->board_size; i++)
					for (int j = 0; j < gr_state->board_size; j++)
						if (cur_state->board[i][j] != 0)
						{
							DrawRectangle(gr_state->positions[i][j].x + 10, gr_state->positions[i][j].y + 10, gr_state->edge - 20, gr_state->edge - 20, LLGRAY);
							Rectangle temp = {gr_state->positions[i][j].x + 10, gr_state->positions[i][j].y + 10, gr_state->edge - 20, gr_state->edge - 20};
							DrawRectangleLinesEx(temp, 5, CYAN);

							char *buf = int_to_ascii(cur_state->board[i][j]);
							DrawText(
								buf,
								gr_state->positions[i][j].x + gr_state->edge / 2 - MeasureText(buf, (int)(gr_state->edge / 2)) / 2,
								gr_state->positions[i][j].y + gr_state->edge / 2 - 50,
								(int)(gr_state->edge / 2), CYAN);
							free(buf);
						}

				point blank_next = gr_state->positions[next->blank_row][next->blank_col];
				point blank_prev = gr_state->positions[cur_state->blank_row][cur_state->blank_col];

				if (blank_next.x == blank_prev.x && blank_next.y == blank_prev.y)
				{
					moves++;
					destroyfunc(ListRemove_nth(gr_state->move_list, 1));
					cur_state = next;
					gr_state->trans.in_transition = false;
					off_row = gr_state->trans.offset_row * gr_state->edge;
					off_col = gr_state->trans.offset_col * gr_state->edge;
					gr_state->positions[next->blank_row][next->blank_col].x -= off_col;
					gr_state->positions[next->blank_row][next->blank_col].y -= off_row;
				}
			}
		}

		// Draw the side panel
		Rectangle side_panel = {0, 0, SCREEN_WIDTH - PUZZLE_WIDTH, SCREEN_HEIGHT};
		DrawRectangleRec(side_panel, LLGRAY);
		DrawRectangleLinesEx(side_panel, 15, CYAN);
		DrawRectangleLinesEx(side_panel, 7, DARKGRAY);

		// Show the number of moves
		DrawText(TextFormat("MOVES: %2d", moves), side_panel.x + side_panel.width / 2 - MeasureText(TextFormat("MOVES: %2d", moves), 40) / 2, side_panel.y + 50, 40, CYAN);

		// Progress bar
		DrawRectangleLines(max_bar.x, max_bar.y, max_bar.width, max_bar.height, BLACK);
		DrawRectangle(max_bar.x + 2, max_bar.y + 2, bar_width, max_bar.height - 4, CYAN);

		if (mouseOnBack)
			DrawRectangleRec(back_button, DARKGRAY);
		else
			DrawRectangleRec(back_button, GRAY);

		DrawRectangleLines((int)back_button.x, (int)back_button.y, (int)back_button.width, (int)back_button.height, DARKGRAY);
		DrawText("BACK", (int)back_button.x + ((int)back_button.width) / 2 - MeasureText("BACK", 40) / 2, (int)back_button.y + ((int)back_button.height) / 2 - 20, 40, RAYWHITE);

		if (first_end == true && !mouseOnProcced && (!auto_play || first_time))
			DrawRectangleRec(procced_button, CYAN);
		else if (first_end == true && mouseOnProcced && (!auto_play || first_time))
			DrawRectangleRec(procced_button, LIGHTCYAN);
		else
			DrawRectangleRec(procced_button, GRAY);

		DrawRectangleLines((int)procced_button.x, (int)procced_button.y, (int)procced_button.width, (int)procced_button.height, DARKGRAY);

		char *procced_text = "PROCCED";
		char *start_text = "START";
		char *buf;
		if (first_time)
			buf = start_text;
		else
			buf = procced_text;

		DrawText(buf, (int)procced_button.x + ((int)procced_button.width) / 2 - MeasureText(buf, 40) / 2, (int)procced_button.y + ((int)procced_button.height) / 2 - 20, 40, RAYWHITE);

		if (!auto_play && !first_time)
			DrawText("Press Enter to Procced", (int)procced_button.x + ((int)procced_button.width) / 2 - MeasureText("Press Enter to Procced", 20) / 2, (int)procced_button.y + ((int)procced_button.height) + 5, 20, DARKGRAY);

		char *auto_trigger = "AUTO";
		char *man_trigger = "MANUAL";
		Color auto_color = GREEN;
		Color man_color = RED;
		Color buf_color;

		if (auto_play)
		{
			buf = auto_trigger;
			buf_color = auto_color;
		}
		else
		{
			buf = man_trigger;
			buf_color = man_color;
		}

		float trigger_text_y = max_bar.y + max_bar.height + 60;
		float trigger_text_x = (GetScreenWidth() - PUZZLE_WIDTH) / 2 - MeasureText("TRIGGER: ", 35) / 2 - MeasureText(buf, 35) / 2;

		DrawText("TRIGGER: ", trigger_text_x, trigger_text_y, 35, CYAN);
		DrawText(buf, trigger_text_x + MeasureText("TRIGGER: ", 35), trigger_text_y, 35, buf_color);
		DrawText("Press \"A\" to toggle Auto Play", (GetScreenWidth() - PUZZLE_WIDTH) / 2 - MeasureText("Press \"A\" to toggle Auto Play", 20) / 2, trigger_text_y + 35 + 5, 20, GRAY);

		EndDrawing();
	}
	else if (cur_mode == MANUAL)
	{
		Rectangle back_button = (Rectangle){(SCREEN_WIDTH - PUZZLE_WIDTH) / 2 - (170 / 2), SCREEN_HEIGHT - 30 - 50, 170, 60};

		// Updating
		if (gr_state->trans.in_transition == false && !isGoal(*cur_state))
		{
			int next_blank_row = cur_state->blank_row;
			int next_blank_col = cur_state->blank_col;

			if (IsKeyPressed(KEY_UP) && cur_state->blank_row + 1 < gr_state->board_size)
				next_blank_row++;
			else if (IsKeyPressed(KEY_DOWN) && cur_state->blank_row - 1 >= 0)
				next_blank_row--;
			else if (IsKeyPressed(KEY_LEFT) && cur_state->blank_col + 1 < gr_state->board_size)
				next_blank_col++;
			else if (IsKeyPressed(KEY_RIGHT) && cur_state->blank_col - 1 >= 0)
				next_blank_col--;

			if (next_blank_col != cur_state->blank_col || next_blank_row != cur_state->blank_row)
			{
				manual_next_state = CopyState(cur_state);

				manual_next_state->board[cur_state->blank_row][cur_state->blank_col] = manual_next_state->board[next_blank_row][next_blank_col];
				manual_next_state->board[next_blank_row][next_blank_col] = 0;

				manual_next_state->blank_row = next_blank_row;
				manual_next_state->blank_col = next_blank_col;

				gr_state->trans.in_transition = true;
				gr_state->trans.offset_row = cur_state->blank_row - manual_next_state->blank_row;
				gr_state->trans.offset_col = cur_state->blank_col - manual_next_state->blank_col;
			}
		}

		// Update buttons
		bool mouseOnBack = CheckCollisionPointRec(GetMousePosition(), back_button);

		if ((CheckCollisionPointRec(GetMousePosition(), back_button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)))
		{
			int MAX_CHAR = gr_state->board_size * gr_state->board_size;

			int temp = MAX_CHAR;
			if (temp > 9)
			{
				temp -= 9;
				temp *= 2;
				temp += 8;
			}
			MAX_CHAR--;
			MAX_CHAR += temp;

			if (puzzle_str != NULL)
				free(puzzle_str);

			puzzle_str = malloc(sizeof(char) * (MAX_CHAR + 1));
			letter_count = MAX_CHAR;

			int index = 0;
			for (int i = 0; i < gr_state->board_size; i++)
			{
				for (int j = 0; j < gr_state->board_size; j++)
				{
					if (cur_state->board[i][j] == 0)
						puzzle_str[index++] = '0';
					else
					{
						char *buf = int_to_ascii(cur_state->board[i][j]);

						int k;
						for (k = 0; k < strlen(buf); k++)
							puzzle_str[index + k] = buf[k];

						index += k;
					}
					puzzle_str[index++] = ' ';
				}
			}
			puzzle_str[MAX_CHAR] = '\0';

			destroyfunc(cur_state);

			temp = gr_state->board_size;

			gr_state->move_list = NULL;
			free_gra_state(gr_state);
			*gr_state_ptr = create_dumy_state();

			(*gr_state_ptr)->board_size = temp;

			cur_state = NULL;
			*in_menu = true;
			started = false;
			return;
		}

		State *drawn_state = cur_state;

		// Drawing move
		BeginDrawing();

		ClearBackground(DARKGRAY);

		if (gr_state->trans.in_transition == false || isGoal(*drawn_state))
		{
			for (int i = 0; i < gr_state->board_size; i++)
				for (int j = 0; j < gr_state->board_size; j++)
					if (drawn_state->board[i][j] != 0)
					{
						DrawRectangle(gr_state->positions[i][j].x + 10, gr_state->positions[i][j].y + 10, gr_state->edge - 20, gr_state->edge - 20, LLGRAY);
						Rectangle temp = {gr_state->positions[i][j].x + 10, gr_state->positions[i][j].y + 10, gr_state->edge - 20, gr_state->edge - 20};
						DrawRectangleLinesEx(temp, 5, CYAN);

						char *buf = int_to_ascii(drawn_state->board[i][j]);
						DrawText(
							buf,
							gr_state->positions[i][j].x + gr_state->edge / 2 - MeasureText(buf, (int)(gr_state->edge / 2)) / 2,
							gr_state->positions[i][j].y + gr_state->edge / 2 - 50,
							(int)(gr_state->edge / 2), CYAN);
						free(buf);
					}
		}
		else if (gr_state->trans.in_transition == true)
		{
			float off_row = gr_state->trans.offset_row * (gr_state->edge / ANIMATION_SPEED);
			float off_col = gr_state->trans.offset_col * (gr_state->edge / ANIMATION_SPEED);
			gr_state->positions[manual_next_state->blank_row][manual_next_state->blank_col].x += off_col;
			gr_state->positions[manual_next_state->blank_row][manual_next_state->blank_col].y += off_row;

			for (int i = 0; i < gr_state->board_size; i++)
				for (int j = 0; j < gr_state->board_size; j++)
					if (drawn_state->board[i][j] != 0)
					{
						DrawRectangle(gr_state->positions[i][j].x + 10, gr_state->positions[i][j].y + 10, gr_state->edge - 20, gr_state->edge - 20, LLGRAY);
						Rectangle temp = {gr_state->positions[i][j].x + 10, gr_state->positions[i][j].y + 10, gr_state->edge - 20, gr_state->edge - 20};
						DrawRectangleLinesEx(temp, 5, CYAN);

						char *buf = int_to_ascii(drawn_state->board[i][j]);
						DrawText(
							buf,
							gr_state->positions[i][j].x + gr_state->edge / 2 - MeasureText(buf, (int)(gr_state->edge / 2)) / 2,
							gr_state->positions[i][j].y + gr_state->edge / 2 - 50,
							(int)(gr_state->edge / 2), CYAN);
						free(buf);
					}

			point blank_next = gr_state->positions[manual_next_state->blank_row][manual_next_state->blank_col];
			point blank_prev = gr_state->positions[cur_state->blank_row][cur_state->blank_col];

			if (blank_next.x == blank_prev.x && blank_next.y == blank_prev.y)
			{
				moves++;
				gr_state->trans.in_transition = false;

				off_row = gr_state->trans.offset_row * gr_state->edge;
				off_col = gr_state->trans.offset_col * gr_state->edge;
				gr_state->positions[manual_next_state->blank_row][manual_next_state->blank_col].x -= off_col;
				gr_state->positions[manual_next_state->blank_row][manual_next_state->blank_col].y -= off_row;

				free(cur_state);
				cur_state = manual_next_state;
				manual_next_state = NULL;
			}
		}

		// Draw the side panel
		Rectangle side_panel = {0, 0, SCREEN_WIDTH - PUZZLE_WIDTH, SCREEN_HEIGHT};
		DrawRectangleRec(side_panel, LLGRAY);
		DrawRectangleLinesEx(side_panel, 15, CYAN);
		DrawRectangleLinesEx(side_panel, 7, DARKGRAY);

		// Show the number of moves
		DrawText(TextFormat("MOVES: %2d", moves), side_panel.x + side_panel.width / 2 - MeasureText(TextFormat("MOVES: %2d", moves), 40) / 2, side_panel.y + 50, 40, CYAN);

		DrawText("Use the Arrow keys", side_panel.x + side_panel.width / 2 - MeasureText("Use the Arrow keys", 30) / 2, side_panel.height / 2 - 35, 30, DARKGRAY);
		DrawText("to solve the puzzle.", side_panel.x + side_panel.width / 2 - MeasureText("to solve the puzzle.", 30) / 2, side_panel.height / 2 + 5, 30, DARKGRAY);

		if (mouseOnBack)
			DrawRectangleRec(back_button, DARKGRAY);
		else
			DrawRectangleRec(back_button, GRAY);

		DrawRectangleLines((int)back_button.x, (int)back_button.y, (int)back_button.width, (int)back_button.height, DARKGRAY);
		DrawText("BACK", (int)back_button.x + ((int)back_button.width) / 2 - MeasureText("BACK", 40) / 2, (int)back_button.y + ((int)back_button.height) / 2 - 20, 40, RAYWHITE);

		EndDrawing();
	}
	else if (cur_mode == SHUFFLE)
	{
		int max_moves = gr_state->board_size * gr_state->board_size * 3;
		if (rand_moves == max_moves)
		{
			int MAX_CHAR = gr_state->board_size * gr_state->board_size;

			int temp = MAX_CHAR;
			if (temp > 9)
			{
				temp -= 9;
				temp *= 2;
				temp += 8;
			}
			MAX_CHAR--;
			MAX_CHAR += temp;

			if (puzzle_str != NULL)
				free(puzzle_str);

			puzzle_str = malloc(sizeof(char) * (MAX_CHAR + 1));
			letter_count = MAX_CHAR;

			int index = 0;
			for (int i = 0; i < gr_state->board_size; i++)
			{
				for (int j = 0; j < gr_state->board_size; j++)
				{
					if (cur_state->board[i][j] == 0)
						puzzle_str[index++] = '0';
					else
					{
						char *buf = int_to_ascii(cur_state->board[i][j]);

						int k;
						for (k = 0; k < strlen(buf); k++)
							puzzle_str[index + k] = buf[k];

						index += k;
					}
					puzzle_str[index++] = ' ';
				}
			}
			puzzle_str[MAX_CHAR] = '\0';

			destroyfunc(cur_state);

			temp = gr_state->board_size;

			gr_state->move_list = NULL;
			free_gra_state(gr_state);
			*gr_state_ptr = create_dumy_state();

			(*gr_state_ptr)->board_size = temp;

			cur_state = NULL;
			*in_menu = true;
			cur_mode = NONE;
			return;
		}

		if (gr_state->trans.in_transition == false && rand_moves < max_moves)
		{
			int next_blank_row = cur_state->blank_row;
			int next_blank_col = cur_state->blank_col;

			int N = gr_state->board_size;

			int direction = rand() % 4;

			switch (direction)
			{
				case UP:
					if(next_blank_row - 1 >= 0 && last != DOWN)
					{	
						next_blank_row--;
						last = UP;	
					}
					break;

				case DOWN:
					if(next_blank_row + 1 < N && last != UP)
					{
						next_blank_row++;
						last = DOWN;
					}
					break;

				case LEFT:
					if(next_blank_col - 1 >= 0 && last != RIGHT)
					{
						next_blank_col--;
						last = LEFT;
					}
					break;

				case RIGHT:
					if(next_blank_col + 1 < N && last != LEFT)
					{
						next_blank_col++;
						last = RIGHT;
					}
					break;

				default:
					break;
			}

			if (next_blank_col != cur_state->blank_col || next_blank_row != cur_state->blank_row)
			{
				rand_next_state = CopyState(cur_state);

				rand_next_state->board[cur_state->blank_row][cur_state->blank_col] = rand_next_state->board[next_blank_row][next_blank_col];
				rand_next_state->board[next_blank_row][next_blank_col] = 0;

				rand_next_state->blank_row = next_blank_row;
				rand_next_state->blank_col = next_blank_col;

				gr_state->trans.in_transition = true;
				gr_state->trans.offset_row = cur_state->blank_row - rand_next_state->blank_row;
				gr_state->trans.offset_col = cur_state->blank_col - rand_next_state->blank_col;

				rand_moves++;
			}
		}

		// Drawing move
		BeginDrawing();

		ClearBackground(DARKGRAY);

		if (gr_state->trans.in_transition == false)
		{
			for (int i = 0; i < gr_state->board_size; i++)
				for (int j = 0; j < gr_state->board_size; j++)
					if (cur_state->board[i][j] != 0)
					{
						DrawRectangle(gr_state->positions[i][j].x + 10, gr_state->positions[i][j].y + 10, gr_state->edge - 20, gr_state->edge - 20, LLGRAY);
						Rectangle temp = {gr_state->positions[i][j].x + 10, gr_state->positions[i][j].y + 10, gr_state->edge - 20, gr_state->edge - 20};
						DrawRectangleLinesEx(temp, 5, CYAN);

						char *buf = int_to_ascii(cur_state->board[i][j]);
						DrawText(
							buf,
							gr_state->positions[i][j].x + gr_state->edge / 2 - MeasureText(buf, (int)(gr_state->edge / 2)) / 2,
							gr_state->positions[i][j].y + gr_state->edge / 2 - 50,
							(int)(gr_state->edge / 2), CYAN);
						free(buf);
					}
		}
		else if (gr_state->trans.in_transition == true)
		{
			float off_row = gr_state->trans.offset_row * (gr_state->edge / (ANIMATION_SPEED / 2));
			float off_col = gr_state->trans.offset_col * (gr_state->edge / (ANIMATION_SPEED / 2));
			gr_state->positions[rand_next_state->blank_row][rand_next_state->blank_col].x += off_col;
			gr_state->positions[rand_next_state->blank_row][rand_next_state->blank_col].y += off_row;

			for (int i = 0; i < gr_state->board_size; i++)
				for (int j = 0; j < gr_state->board_size; j++)
					if (cur_state->board[i][j] != 0)
					{
						DrawRectangle(gr_state->positions[i][j].x + 10, gr_state->positions[i][j].y + 10, gr_state->edge - 20, gr_state->edge - 20, LLGRAY);
						Rectangle temp = {gr_state->positions[i][j].x + 10, gr_state->positions[i][j].y + 10, gr_state->edge - 20, gr_state->edge - 20};
						DrawRectangleLinesEx(temp, 5, CYAN);

						char *buf = int_to_ascii(cur_state->board[i][j]);
						DrawText(
							buf,
							gr_state->positions[i][j].x + gr_state->edge / 2 - MeasureText(buf, (int)(gr_state->edge / 2)) / 2,
							gr_state->positions[i][j].y + gr_state->edge / 2 - 50,
							(int)(gr_state->edge / 2), CYAN);
						free(buf);
					}

			point blank_next = gr_state->positions[rand_next_state->blank_row][rand_next_state->blank_col];
			point blank_prev = gr_state->positions[cur_state->blank_row][cur_state->blank_col];

			if (blank_next.x == blank_prev.x && blank_next.y == blank_prev.y)
			{
				moves++;
				gr_state->trans.in_transition = false;

				off_row = gr_state->trans.offset_row * gr_state->edge;
				off_col = gr_state->trans.offset_col * gr_state->edge;
				gr_state->positions[rand_next_state->blank_row][rand_next_state->blank_col].x -= off_col;
				gr_state->positions[rand_next_state->blank_row][rand_next_state->blank_col].y -= off_row;

				free(cur_state);
				cur_state = rand_next_state;
				rand_next_state = NULL;
			}
		}

		// Draw the side panel
		Rectangle side_panel = {0, 0, SCREEN_WIDTH - PUZZLE_WIDTH, SCREEN_HEIGHT};
		DrawRectangleRec(side_panel, LLGRAY);
		DrawRectangleLinesEx(side_panel, 15, CYAN);
		DrawRectangleLinesEx(side_panel, 7, DARKGRAY);
	
		EndDrawing();
	}
}

void interface_close()
{
	// Unload all loaded data (textures, sounds, models...)
	UnloadSound(clap);
	UnloadTexture(paste);
	UnloadTexture(copy);

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

char *remove_spaces(char *str)
{
	char *temp = malloc(sizeof(char) * strlen(str));
	int i = 0;
	int j = 0;
	while (str[i] != '\0')
	{
		if (str[i] != ' ')
		{
			temp[j] = str[i];
			j++;
		}
		i++;
	}
	temp[j] = '\0';

	char *result = malloc(sizeof(char) * strlen(temp));
	strcpy(result, temp);
	free(temp);
	return result;
}

Graphics create_gra_state(int size)
{
	Graphics gr = malloc(sizeof(gra_state));

	gr->board_size = size;
	gr->trans.in_transition = false;

	// Figuring out where the blocks are going to be
	gr->edge = (float)PUZZLE_HEIGHT / (float)size;

	gr->positions = malloc(sizeof(point *) * size);
	for (int i = 0; i < size; i++)
		gr->positions[i] = malloc(sizeof(point) * size);

	point offset = {SCREEN_WIDTH - PUZZLE_WIDTH, 0};
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			gr->positions[i][j] = offset;
			offset.x += gr->edge;
		}
		offset.x = SCREEN_WIDTH - PUZZLE_WIDTH;
		offset.y += gr->edge;
	}

	return gr;
}

void free_gra_state(Graphics gr)
{
	if (gr != NULL)
	{
		if (gr->move_list != NULL)
			freeList(gr->move_list);

		if (gr->positions != NULL)
		{
			for (int i = 0; i < gr->board_size; i++)
				free(gr->positions[i]);
			free(gr->positions);
		}

		free(gr);
	}
}

Graphics create_dumy_state(void)
{
	Graphics gr_state = malloc(sizeof(gra_state));

	gr_state->board_size = -1;
	gr_state->move_list = NULL;
	gr_state->positions = NULL;

	return gr_state;
}