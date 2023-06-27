#include <Windows.h>
#include <stdio.h>
#include "sync_queue.h"

static SyncQueue_t* sync_queue_write = NULL;
static SyncQueue_t* sync_queue_read = NULL;
static CRITICAL_SECTION sync_queue_critical_sec;
static HANDLE sync_event_handle;

int create_sync_queue() {
    sync_event_handle = CreateEvent(NULL, TRUE, FALSE, TEXT("InputLoopEvent"));
    if (!sync_event_handle) {
        return 1;
    }
    InitializeCriticalSection(&sync_queue_critical_sec);
    return 0;
}

void destroy_sync_queue() {
    DeleteCriticalSection(&sync_queue_critical_sec);
}

void stop_sync_queue() {
    EnterCriticalSection(&sync_queue_critical_sec);
    sync_queue_read = -1;
    sync_queue_write = -1;
    LeaveCriticalSection(&sync_queue_critical_sec);
}

int push_node(char ch) {
    SyncQueue_t* node = malloc(sizeof(SyncQueue_t));
    if (!node) {
        return 1;
    }

    memset(node, 0, sizeof(SyncQueue_t));
    node->character = ch;

    EnterCriticalSection(&sync_queue_critical_sec);
    if (sync_queue_read == -1)
        return 1;
    if (!sync_queue_read) {
        sync_queue_read = node;
    }
    if (sync_queue_write) {
        sync_queue_write->next = node;
    }
    sync_queue_write = node;
    LeaveCriticalSection(&sync_queue_critical_sec);

    SetEvent(sync_event_handle);
    return 0;
}

SyncQueue_t* pop_node(int diff_time) {
    SyncQueue_t* node = NULL;
    WaitForSingleObject(sync_event_handle, diff_time);
    EnterCriticalSection(&sync_queue_critical_sec);
    if (sync_queue_read) {
        node = sync_queue_read;
        if (node->next) {
            sync_queue_read = sync_queue_read->next;
        }
        else {
            sync_queue_read = NULL;
            sync_queue_write = NULL;
        }
    }
    LeaveCriticalSection(&sync_queue_critical_sec);
    return node;
}


void delete_node(SyncQueue_t* node) {
    //CloseHandle(sync_event_handle);
    free(node);
}