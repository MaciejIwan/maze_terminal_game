#if !defined(_GAME_BEAST_H_)
#define _GAME_BEAST_H_

#include "common.h"

BEAST* beast_spawn();
void beast_kill(BEAST * beast);
DIRECTION beast_pursuit(struct data2_t* local_data);

#endif // _GAME_BEAST_H_