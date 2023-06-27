#include "stats.h"
	
static Stats_t stats;

void set_char_stats(bool is_correct) {
	stats.correctly_char += is_correct == true;
	stats.incorrectly_char += is_correct == false;
}

void print_stats(int total_time, int word_count) {
	printf("Correct characters count: %d\n", stats.correctly_char);
	printf("Wrong characters count: %d\n", stats.incorrectly_char);
	int total_chars = stats.correctly_char + stats.incorrectly_char;
	printf("Total characters count: %d\n", total_chars);
	printf("Avg character writing speed: %f/min\n", total_chars / (total_time / 60000.0));
	printf("Avg word writing speed: %f/min\n", word_count / (total_time / 60000.0));
}

Stats_t get_stats() {
	return stats;
}