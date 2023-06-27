#pragma once

// Structs
typedef struct SyncQueue {
    struct SyncQueue* next;
    char character;
} SyncQueue_t;

typedef struct SyncQueue* SyncQueuePtr;

// Definition funcs

int create_sync_queue();
void destroy_sync_queue();
void stop_sync_queue();
int push_node(char ch);
SyncQueue_t* pop_node(int diff_time);
void delete_node(SyncQueue_t* node);