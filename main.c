#include <Windows.h>
#include <stdio.h>
#include <time.h>
#include "sync_queue.h"
#include "ini_parser.h"
#include "gopt.h"
#include "input.h"
#include "renderer.h"
#include "stats.h"


int main(int argc, char* argv[]) {
    void* options = gopt_sort(&argc, argv, gopt_start(
        gopt_option('l', GOPT_ARG, gopt_shorts('l'), gopt_longs("level")),
        gopt_option('t', GOPT_ARG, gopt_shorts('t'), gopt_longs("time-limit")),
        gopt_option('e', GOPT_ARG, gopt_shorts('e'), gopt_longs("allowed-err-count"))
    ));

    const char* help_texts = "Usage:\n\
    keyboard-game options [param]\n\n\
General Options:\n\n\
\t-l, --level                  Level name (easy, medium, hard)\n\
\t-t  --time-limit             Game time limit (in seconds)\n\
\t-e  --allowed-err-count      Number of allowed errors";


    char* level_name;
    char* time_lim_as_str;
    char* allowed_err_count_as_str;

 
    if (!gopt_arg(options, 'l', &level_name)) {
        printf(help_texts);
        return 0;
    }

    if (!gopt_arg(options, 't', &time_lim_as_str)) {
        printf(help_texts);
        return 0;
    }

    if (!gopt_arg(options, 'e', &allowed_err_count_as_str)) {
        printf(help_texts);
        return 0;
    }

    char truth[4096];
    int truth_current_pos = 0;
    int count_input_words = 1;
    
    
    GameLevel_t lvl = ini_game_level_from_string(level_name);
    if (lvl == UNKNOWN) {
        printf(help_texts);
        return 0;
    }

    // Parsing ini file
    int ret = parse_game_ini(truth, sizeof(truth), lvl);
    if (ret) {
        fprintf(stderr, "Error: parse dictionary from file (error code: %d)\n", ret);
        return 1;
    }

    int time_limit = atoi(time_lim_as_str);
    if (time_limit < 30 || time_limit > 300) {
        printf(help_texts);
        return 0;
    }

    unsigned int allowed_err_count = atoi(allowed_err_count_as_str);
    if (allowed_err_count < 5 || allowed_err_count > 10) {
        printf(help_texts);
        return 0;
    }

    LARGE_INTEGER qpf;
    QueryPerformanceFrequency(&qpf);

    LARGE_INTEGER time_begin;
    QueryPerformanceCounter(&time_begin);

    LARGE_INTEGER time_end;
    time_end.QuadPart = time_begin.QuadPart + time_limit * qpf.QuadPart;
    
    // Creating a queue for synchronization
    if (create_sync_queue()) {
        fprintf(stderr, "create_sync_queue() failed"); return 1;
    }
    // Creating a character renderer
    initialize_renderer();

    // Creating a separate thread for IO operation 
    HANDLE hThread = CreateThread(
        NULL,
        0,
        input_loop,
        NULL,
        0,
        NULL); 

    // If the thread could not be created, we will end the program with error code 1
    if (!hThread) {
        return 1;
    }

    // Starting an infinite loop in the main thread
    for (;;) {
        LARGE_INTEGER current_time;
        QueryPerformanceCounter(&current_time);
        int remaining_time = (time_end.QuadPart - current_time.QuadPart) * 1000 / qpf.QuadPart;
        if (remaining_time <= 0) {
            break;
        }

        // Get node and check that it is valid (not null)
        SyncQueue_t* sc = pop_node(100);
        // check correct character input
        Answer_t answer = !sc ? SKIP : sc->character == truth[truth_current_pos] ? RIGHT : WRONG;
        if (get_stats().incorrectly_char > allowed_err_count) {
            // end game because there are a lot of mistakes
            break;
        }
        // Call the function for rendering
        int status = renderer(truth, truth_current_pos, answer, remaining_time);
        if (status != RENDERER_NO_ERROR) {
            switch (status) {
            case RENDERER_ALLOCATION_ERROR: fprintf(stderr, "Failed to allocate screen buffer\n"); return 1;
            case RENDERER_SCREEN_SIZE_ERROR: fprintf(stderr, "Screen size changed during rendering\n"); return 1;
            default: return 1;
            }
        }
        // Increase the current position in the dictionary
        if (truth_current_pos == strlen(truth)) {
            // end game, we went through the whole dictionary
            break;
        }
        if (sc) {
            if (truth_current_pos > 0 && truth[truth_current_pos] == 32 && truth[truth_current_pos - 1] != 32) { // space
                count_input_words++;
            }
            truth_current_pos++;
            set_char_stats(answer == RIGHT);
            // delete node
            delete_node(sc);
        }
    }

    // Removing queue and the renderer
    stop_sync_queue();
    clear_console();

    LARGE_INTEGER current_time;
    QueryPerformanceCounter(&current_time);
    int total_time = (current_time.QuadPart - time_begin.QuadPart) * 1000. / qpf.QuadPart;
    print_stats(total_time, count_input_words);

    gopt_free(options);
    return 0;
}

