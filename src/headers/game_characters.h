#if !defined(_GAME_CHARACTERS_H_)
#define _GAME_CHARACTERS_H_
#include "common.h"

typedef enum  {
	UP,
	LEFT,
	DOWN,
	RIGHT,
    STAY
} DIRECTION;

typedef enum player_type
{
    PLAYER_CPU,
    PLAYER_HUMAN,
    PLAYER_BEAST
} PLAYER_TYPE;

typedef struct economy
{
    int bank; // cash in bank
    int eq;   // cash in eq
} ECONOMY;

typedef struct
{
    int id;
    int x, y; // cordinates
    PLAYER_TYPE type;

    union
    {
        struct
        {
            // player
            int deaths;
            ECONOMY econonmy;
        };
        struct
        {
            // beast
            int kills;
            DIRECTION last_move;
        };
    };

} PLAYER, BOT, BEAST;

#endif // _GAME_CHARACTERS_H_