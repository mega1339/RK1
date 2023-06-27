#pragma once

enum Answer {
	SKIP = 0,
	RIGHT,
	WRONG
};

typedef enum Answer Answer_t;

#define RENDERER_NO_ERROR 0
#define RENDERER_ALLOCATION_ERROR 1
#define RENDERER_SCREEN_SIZE_ERROR 2

void initialize_renderer();
int renderer(char* truth_buffer, int truth_current_pos, Answer_t answer, int diff_time);
void clear_console();