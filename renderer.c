#include <stdio.h>
#include <Windows.h>
#include "renderer.h"

static HANDLE console;
static CONSOLE_SCREEN_BUFFER_INFO screen_info;
static int truth_size;
static int screen_w;
static int screen_h;
static int screen_size;
static const WORD fg_white = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;

void initialize_renderer() {
	console = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(console, &screen_info);
	screen_w = (screen_info.srWindow.Right - screen_info.srWindow.Left + 1);
	screen_h = (screen_info.srWindow.Bottom - screen_info.srWindow.Top + 1);
	screen_size = screen_w * (screen_h - 1);
	clear_console();
}

void clear_console() {
	DWORD written = 0;
	COORD pos = {0, 0};
	FillConsoleOutputCharacterA(console, ' ', screen_w * screen_h, pos, &written);
	FillConsoleOutputAttribute(console, fg_white, screen_w * screen_h, pos, &written);
    SetConsoleCursorPosition(console, pos);
	SetConsoleTextAttribute(console, fg_white);
}

int show_truth(char* truth_buffer, int truth_current_pos) {
	COORD pos = {0, 0};
	SetConsoleCursorPosition(console, pos);
	SetConsoleTextAttribute(console, fg_white);

	char* screen_buffer = malloc(screen_size + 1);
	if (!screen_buffer) return RENDERER_ALLOCATION_ERROR;
	memset(screen_buffer, 0, screen_size + 1);
	strncpy_s(screen_buffer, screen_size + 1, truth_buffer + truth_current_pos, screen_size);
	printf(screen_buffer);
	free(screen_buffer);

	SetConsoleCursorPosition(console, pos);
	return RENDERER_NO_ERROR;
}

void show_time(int diff_time) {
	COORD pos = {0, screen_h - 1};
	SetConsoleCursorPosition(console, pos);
	SetConsoleTextAttribute(console, fg_white);
	
	printf("Remaining time: %d.%d\r", diff_time/1000, diff_time%1000);
}

int renderer(char* truth_buffer, int truth_current_pos, Answer_t answer, int diff_time) {
	show_time(diff_time);
	if (truth_current_pos % screen_size == 0) {
		int status = show_truth(truth_buffer, truth_current_pos);
		if (status)
			return status;
	} else {
		SetConsoleCursorPosition(console, screen_info.dwCursorPosition);
	}
	if (answer != SKIP) {
		SetConsoleTextAttribute(console, (answer == RIGHT ? BACKGROUND_GREEN : BACKGROUND_RED) | fg_white);
		printf("%c", truth_buffer[truth_current_pos]);
	}
	if (screen_info.dwCursorPosition.X == screen_w - 1) {
		screen_info.dwCursorPosition.X = 0;
		screen_info.dwCursorPosition.Y++;
	}
	else {
		GetConsoleScreenBufferInfo(console, &screen_info);

		if (screen_w != (screen_info.srWindow.Right - screen_info.srWindow.Left + 1)
			|| screen_h != (screen_info.srWindow.Bottom - screen_info.srWindow.Top + 1))
			return RENDERER_SCREEN_SIZE_ERROR;
	}
	return RENDERER_NO_ERROR;
}