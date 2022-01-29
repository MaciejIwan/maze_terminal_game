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
typedef int (*block_action_ptr)(void *);

typedef struct
{
    char ID;
    int treasure_value;
    block_action_ptr action;
} BLOCK;

typedef struct
{
    BLOCK block;
    CORDS location;
    int visitors_count;      // how many players beast on chunk;
    int visitors_max;        // how many players beast on chunk;
    CHARACTERS *visitors[3]; // to do on heap when more players
} CHUNK;

#define _CHUNK_BLOCK_TYPES_
#endif

int block_action_treasure(void *data);
int block_action_bank(void *data);
int block_action_bushes(void *data);

char block_change_type(CHUNK *chunk, char new_block_type, int treasure_value);

#endif // _BLOCK_TYPES_H_