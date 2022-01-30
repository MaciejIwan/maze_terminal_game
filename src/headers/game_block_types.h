#if !defined(_BLOCK_TYPES_H_)
#define _BLOCK_TYPES_H_
#include "common.h"

#define BLOCK_BEAST '*'
#define BLOCK_FULL 'B'
#define BLOCK_BUSHES '#'
#define BLOCK_ONE_COIN 'c'
#define BLOCK_TREASURE 't'
#define BLOCK_LARGE_TREASURE 'T'
#define BLOCK_DROPED_TREASURE 'D'
#define BLOCK_BANK 'A'
#define BLOCK_BLANK ' '

#ifndef _CHUNK_BLOCK_TYPES_
typedef int (*block_action_ptr)(void *, void*);

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

int block_action_treasure(void *data, void* player);
int block_action_bank(void *data,void* player);
int block_action_bushes(void *data,void* player);
int block_action_player(void *data, void *player);
int block_action_beast(void *data, void *player);

char block_change_type(CHUNK *chunk, char new_block_type, int treasure_value);
bool block_is_player(char new_block_type);
#endif // _BLOCK_TYPES_H_