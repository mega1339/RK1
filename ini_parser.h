#pragma once

enum GameLevel {
	UNKNOWN = 0,
	EASY,
	MEDIUM,
	HARD
};

typedef enum GameLevel GameLevel_t;

const char* ini_game_level_as_string(GameLevel_t level);
int parse_game_ini(char* buffer, int size, GameLevel_t level);
GameLevel_t ini_game_level_from_string(const char* level);