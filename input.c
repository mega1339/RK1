#include <Windows.h>
#include "input.h"
#include "sync_queue.h"

DWORD WINAPI input_loop(LPVOID lpParameter) {
    for (;;) {
        char ch = (char)getch();
        if (push_node(ch))
            break;
    }
    destroy_sync_queue();
    return 0;
}