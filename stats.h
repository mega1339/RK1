#pragma once

#include <stdbool.h>

struct Stats {
	unsigned int correctly_char;
	unsigned int incorrectly_char;
};

typedef struct Stats Stats_t;

void print_stats(int total_time, int word_count);
void set_char_stats(bool is_correct);
void set_time_stats(double time);
Stats_t get_stats();