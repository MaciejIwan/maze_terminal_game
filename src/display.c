#include "display.h"

char arena_map[28][49] = {
    "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB",
    "B            B           BBB        B          B",
    "B BB BBBBBBB B BBBBBBBBB     BBBBBB B BBB BBBB B",
    "B BB B     B   BB        BBB BB   B B BB   BBB B",
    "B BB B B## B B BB BBBBBB BBB#BB B   B   B BB   B",
    "B BB B BBBBB B BB B           B BBBBB BBBBBBBB B",
    "B BB #       B ## B BBBBBBBBB B       B        B",
    "B BB BBBBBBBBB BB B           B B###BBB BBB BB B",
    "B BB           BB BBB B A B BBB B       BB   B B",
    "B BBBBBBBBB BB BB     B#B#B BBB B BBB###BBB BB B",
    "B        BB BB BBBBBB BBBBB                    B",
    "BBB BBBB BB BB         B     BBBBBBBBBBBBB BBB B",
    "BBB BBBB BB BBBBBBBBBB B B BB             B    B",
    "B                      B B BB BBBB BBBBBB B#BB B",
    "BBBBB####BBBB BBB BBBB B B BB BBBB B      BB   B",
    "BBBBB    BBBB BBB BBBB B B    #    B BBBBB   BBB",
    "B                      B B BB BBBB B   BBBBBBBBB",
    "B BBBBBBBBBBBBBB B BBB #   BB      BBBB        B",
    "B              B B BBB BBBBBB BB BBB BB BBBBBB B",
    "BBBBBBBBBBBBBB   B             B            BB B",
    "B              B BBBBB BBBBB BBB BB B B BBBBBB B",
    "B BBBBBBBBBBBBBB       B   B BBB BB BBB BB     B",
    "B              B BBBBB B B B            BB BBB B",
    "BBBBBBB####BBB B #     B B BBBBB BBBBBB    BBB B",
    "B              B BB BB B B           B  BBB    B",
    "B BBBBBBBBBBBBBB BB BB B B BBBB BBB BB BBBB BB B",
    "B                        B      BB   B         B",
    "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB",
};



void disp_init()
{
    system("resize -s 32 110"); // set deafult window size
    setlocale(LC_ALL, "");
    initscr();

    // cbreak();
    raw();                // dont stop on click
    noecho();             // dont print what user press
    keypad(stdscr, TRUE); // to detect arrow keys
}
void disp_close(){
    endwin();
}
void game_screen(){

}
window_s *create_window(int height, int width, int y, int x)
{
    window_s *window = calloc(1, sizeof(window_s));
    if (!window)
    {
        errno = ENOMEM;
        return NULL;
    }
    WINDOW *window_ptr = newwin(height, width, y, x);
    if (window_ptr == NULL)
    {
        errno = ENOMEM;
        return free(window), NULL;
    }

    window->winptr = window_ptr;
    window->height = height;
    window->width = width;
    window->start_y = y;
    window->start_x = x;

    box(window->winptr, 0, 0);

    return window;
}

void destroy_window_s(window_s *data)
{
    free(data);
}

void draw_map(window_s *win)
{
    for (int i = 1; i < win->height -1; i++)
    {
        //mvwprintw(win->winptr, i, 1, "%.*s", win->width - 2, arena_map[i - 1]);
        for (int j = 1; j < win->width-1; j++)
            if(arena_map[i-1][j-1] == BLOCK_FULL)
                mvwaddch(win->winptr, i, j, ACS_CKBOARD);
            else
                mvwaddch(win->winptr, i, j, arena_map[i-1][j-1]);
    }
    wrefresh(win->winptr);
}