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

typedef struct {
    int x, y; // cordinates
} CORDS;

typedef struct
{
    int id;
    CORDS positon;
    PLAYER_TYPE type;
    bool blocked; // could be blocked in bush block
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

} PLAYER, BOT, BEAST, CHARACTERS;

#endif // _GAME_CHARACTERS_H_