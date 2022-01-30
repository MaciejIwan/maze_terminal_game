#if !defined(_DISPLAY_H_)
#define _DISPLAY_H_
#include "server.h"
#include "common.h"


#ifndef ARENA_WIDTH
    #define ARENA_WIDTH 49
#endif

#ifndef ARENA_HEIGHT
    #define ARENA_HEIGHT 28
#endif


#ifndef _WORLD_T_TYPE_
typedef struct {
    CHUNK MAP[ARENA_HEIGHT][ARENA_WIDTH];
    PLAYER* online_player;
    PLAYER* local_player;
    BEAST* beast;

    int counter_c;
    int counter_t;
    int counter_T;
    int counter_D;

    int max_players_on_server;
} WORLD_T;

#define _WORLD_T_TYPE_
#endif

extern struct SERVER_OUTPUT* data;

enum windows_id
{
    W_ARENA,
    W_DISPLAY,
    W_INFO,
    W_INPUT,
    W_MAX
};

typedef struct
{
    WINDOW *winptr;
    int height, width;
    int start_y, start_x;
} WINDOW_S;

typedef struct
{
    WINDOW_S *W[W_MAX];
} SCREEN_S;

void disp_init();
void disp_close();
WINDOW_S *create_window(int height, int width, int y, int x);
void destroy_window_s(WINDOW_S *data);

void draw_game_screen_layout();
void screen_layout_close(SCREEN_S *sc);

void draw_display(WINDOW_S *win, const struct SERVER_OUTPUT* local_data);
void draw_info();
void draw_input(int c);

void draw_map(WINDOW_S *win, struct SERVER_OUTPUT* data); //void draw_map(WINDOW_S *win, char (*arena)[ARENA_WIDTH]);
void draw_server_map(WINDOW_S *win, WORLD_T *data);
#endif // _DISPLAY_H_