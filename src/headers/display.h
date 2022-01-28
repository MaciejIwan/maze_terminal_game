#if !defined(_DISPLAY_H_)
#define _DISPLAY_H_
#include "common.h"



typedef struct
{
    WINDOW *winptr;
    int height, width;
    int start_y, start_x;
} window_s;

void disp_init();
void disp_close();
window_s *create_window(int height, int width, int y, int x);
void destroy_window_s(window_s *data);

#endif // _DISPLAY_H_