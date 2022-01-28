#if !defined(_DISPLAY_H_)
#define _DISPLAY_H_
#include "common.h"


#ifndef ARENA_WIDTH
    #define ARENA_WIDTH 49
#endif

#ifndef ARENA_HEIGHT
    #define ARENA_HEIGHT 28
#endif
    
extern struct data2_t* data;

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

void draw_display(WINDOW_S *win, const struct data2_t* local_data);
 void draw_info();
void draw_input(char c);
void draw_map(WINDOW_S *win, struct data2_t* data); //void draw_map(WINDOW_S *win, char (*arena)[ARENA_WIDTH]);

#endif // _DISPLAY_H_