#include "game_beast.h"

BEAST* beast_spawn(){

    BEAST* beast = calloc(1, sizeof(BEAST));
    if(beast == NULL){
        errno = ENOMEM;
        return NULL;
    }

    beast->id = -1;
    beast->type = PLAYER_BEAST;
    beast->positon.x = 25;
    beast->positon.y = 5;
    beast->last_move = STAY;
    beast->kills = 0;

    
    return beast;
}

void beast_kill(BEAST * beast){
    free(beast);
}

DIRECTION beast_pursuit(struct SERVER_OUTPUT* local_data){
    // check if player is reachable, if yes try to catch him
    // if not go to random direction, but continue last direction until block (end of tunel or someting)
    return (rand() % STAY);
}