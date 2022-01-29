#include "game_block_types.h"


static block_action_ptr block_match_action(char block_type);
static int block_match_treasure_value(char block_type);

int block_action_bank(void  *data)
{   
    CHUNK *chunk = (CHUNK*)data;
    if (chunk == NULL)
        return 1;

    for (int i = 0; i < chunk->visitors_max; i++)
    {
        CHARACTERS *character = chunk->visitors[i];
        if (character != NULL && character->type != PLAYER_BEAST)
        {
            int temp = character->econonmy.eq;
            character->econonmy.bank += temp;
            character->econonmy.eq = 0;
        }
    }

    return 0;
}

int block_action_treasure(void  *data)
{   
    CHUNK *chunk = (CHUNK*)data;
    if (chunk == NULL)
        return 1;

    for (int i = 0; i < chunk->visitors_max; i++)
    {
        CHARACTERS *character = chunk->visitors[i];
        if (character != NULL && character->type != PLAYER_BEAST)
        {
            character->econonmy.eq += chunk->block.treasure_value;
            block_change_type(chunk, BLOCK_BLANK, 0);
        }
    }

    return 0;
}

int block_action_bushes(void  *data)
{   
    CHUNK *chunk = (CHUNK*)data;
    if (chunk == NULL)
        return 1;

    for (int i = 0; i < chunk->visitors_max; i++)
    {
        CHARACTERS *character = chunk->visitors[i];
        if (character != NULL && character->type != PLAYER_BEAST && character->blocked == FALSE)
        {
            character->blocked = TRUE;
        }
    }

    return 0;
}

static int block_match_treasure_value(char block_type)
{
    if (block_type == BLOCK_LARGE_TREASURE)
        return 50;
    else if (block_type == BLOCK_TREASURE)
        return 10;
    else if (block_type == BLOCK_ONE_COIN)
        return 1;
    else
        return 0;
}
static block_action_ptr block_match_action(char block_type){
    if(block_type == BLOCK_TREASURE || block_type == BLOCK_DROPED_TREASURE || block_type == BLOCK_LARGE_TREASURE || block_type == BLOCK_ONE_COIN)
        return block_action_treasure;
    else if (block_type == BLOCK_BUSHES)
        return block_action_bushes;
    else if (block_type == BLOCK_BANK)
        return block_action_bank;
    else
        return NULL;
}

char block_change_type(CHUNK *chunk, char new_block_type, int treasure_value)
{
    if (chunk == NULL)
        return -1;

    char old_block_type = chunk->block.ID;
    if(treasure_value == 0){
        treasure_value = block_match_treasure_value(new_block_type);
    }

    chunk->block.ID = new_block_type;
    chunk->block.treasure_value = treasure_value;
    chunk->block.action = block_match_action(new_block_type);
    return old_block_type;
}