#include "game_block_types.h"

static block_action_ptr block_match_action(char block_type);
static int block_match_treasure_value(char block_type);

int block_action_bank(void *data, void *player)
{
    if (data == NULL || player == NULL)
        return 1;

    // CHUNK *chunk = (CHUNK *)data;
    CHARACTERS *character = (CHARACTERS *)player;

    if (character != NULL && character->type != PLAYER_BEAST)
    {
        int temp = character->econonmy.eq;
        character->econonmy.bank += temp;
        character->econonmy.eq = 0;
    }

    return 0;
}

int block_action_treasure(void *data, void *player)
{
    if (data == NULL || player == NULL)
        return 1;

    CHUNK *chunk = (CHUNK *)data;
    CHARACTERS *character = (CHARACTERS *)player;

    if (character != NULL && character->type != PLAYER_BEAST)
    {
        character->econonmy.eq += chunk->block.treasure_value;
        block_change_type(chunk, BLOCK_BLANK, 0);
    }

    return 0;
}

int block_action_bushes(void *data, void *player)
{
    if (data == NULL || player == NULL)
        return 1;

    CHARACTERS *character = (CHARACTERS *)player;

    if (character != NULL && character->type != PLAYER_BEAST && character->blocked == FALSE)
    {
        character->blocked = TRUE;
    }

    return 0;
}

int block_action_beast(void *data, void *player)
{
    if (data == NULL || player == NULL)
        return 1;

    return block_action_player(data, player);
    // CHUNK *chunk = (CHUNK *)data;
    // CHARACTERS *character = (CHARACTERS *)player;

    //return 0;
}
int block_action_player(void *data, void *player)
{
    if (data == NULL || player == NULL)
        return 1;
    CHUNK *chunk = (CHUNK *)data;
    CHUNK *chunk2 = (CHUNK *)player;

    CHARACTERS *chara = chunk2->visitor;
    CHARACTERS *chara2 = chunk->visitor; // visitor

    chara->deaths += 1;
    chara2->deaths += 1;

    int D = chara->econonmy.eq + chara2->econonmy.eq;

    chara->econonmy.eq = 0;
    chara2->econonmy.eq = 0;

    return D;
}

static block_action_ptr block_match_action(char block_type)
{
    if (block_type == BLOCK_TREASURE || block_type == BLOCK_DROPED_TREASURE || block_type == BLOCK_LARGE_TREASURE || block_type == BLOCK_ONE_COIN)
        return block_action_treasure;
    else if (block_type == BLOCK_BUSHES)
        return block_action_bushes;
    else if (block_type == BLOCK_BANK)
        return block_action_bank;
    else if (block_type == BLOCK_BEAST)
        return block_action_beast;
    else if (block_is_player(block_type))
        return block_action_player;
    else
        return NULL;
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
int block_match_symbol(char blocktype)
{
    int symbol = (int)blocktype;

    if (blocktype == BLOCK_FULL)
        symbol = ACS_CKBOARD;

    return symbol;
}
int block_match_move_type(char blocktype)
{

    if (blocktype == BLOCK_FULL)
        return 0; // cant move here
    else if (blocktype == BLOCK_BUSHES)
        return 1; // semi, bushes
    else
        return 2; // normal move
}

bool block_is_player(char new_block_type)
{
    if ((new_block_type >= '1' && new_block_type <= '2') || new_block_type == BLOCK_BEAST)
        return TRUE;
    else
        return FALSE;
}
char block_change_type(CHUNK *chunk, char new_block_type, int treasure_value)
{
    if (chunk == NULL)
        return -1;

    char old_block_type = chunk->block.ID;
    if (treasure_value == 0)
    {
        if (new_block_type == BLOCK_DROPED_TREASURE)
            new_block_type = BLOCK_BLANK;
        treasure_value = block_match_treasure_value(new_block_type);
    }

    chunk->block.ID = new_block_type;
    chunk->block.treasure_value = treasure_value;
    chunk->block.symbol = block_match_symbol(new_block_type);
    chunk->block.action = block_match_action(new_block_type);
    chunk->block.move_permission = block_match_move_type(new_block_type);

    if (block_is_player(new_block_type) == FALSE)
    {

        chunk->visitors_count = 0;
        chunk->visitor = NULL;
    }

    if (new_block_type == BLOCK_BLANK)
    {
        chunk->is_free = TRUE;
    }

    return old_block_type;
}