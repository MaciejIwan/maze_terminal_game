#if !defined(_SERVER_H_)
#define _SERVER_H_

#define MAX_SERVER_PLAYERS 3

#include "common.h"

#ifndef _CHUNK_BLOCK_TYPES_
typedef int (*block_action_ptr)(void *);

typedef struct
{
    char ID;
    int symbol;
    int treasure_value;
    int move_permission; // 0 - not allowed, 1 - bush semi allowed (slowness), 2 move normally
    block_action_ptr action;
} BLOCK;

typedef struct
{
    BLOCK block;
    CORDS location;
    bool is_free;
    int visitors_count;      // how many players beast on chunk;
    int visitors_max;        // how many players beast on chunk;
    CHARACTERS *visitors[3]; // to do on heap when more players
} CHUNK;

#define _CHUNK_BLOCK_TYPES_
#endif


typedef struct {
    CHUNK MAP[ARENA_HEIGHT][ARENA_WIDTH];
    PLAYER* online_player;
    PLAYER* local_player;
    BEAST* beast;

    int max_players_on_server;
} WORLD_T;

WORLD_T* server_world_generate();
void server_world_destory(WORLD_T** world);

void server();

#endif // _SERVER_H_
