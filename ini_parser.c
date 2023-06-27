#include <Windows.h>
#include "ini_parser.h"

GameLevel_t ini_game_level_from_string(const char* level) {
	if (!_stricmp(level, "easy")) {
		return EASY;
	} else if (!_stricmp(level, "medium")) {
		return MEDIUM;
	} else if (!_stricmp(level, "hard")) {
		return HARD;
	} else {
		return UNKNOWN;
	}
}

const char* ini_game_level_as_string(GameLevel_t level)  {
	switch (level) {
	case EASY: return "easy";
	case MEDIUM: return "medium";
	case HARD: return "hard";
	default: return NULL;
	}
}

int parse_game_ini(char* buffer, int size, GameLevel_t level) {
	const char* lvl_str = ini_game_level_as_string(level);
    if (!lvl_str)
        return 1;

    char pcFullPath[MAX_PATH];
	GetFullPathNameA("dictionary.ini", MAX_PATH, pcFullPath, NULL);

	if (!GetPrivateProfileStringA(lvl_str, "text", NULL, buffer, size, pcFullPath)) {
        return GetLastError();
    }
    return 0;
}
