#include "display.h"

char arena_map[ARENA_HEIGHT][ARENA_WIDTH] = {
    "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB",
    "B            ##          BBB        B          B",
    "B BB BB#BBBB B BBBBBBBBB     BB#BBB B BBB BBBB B",
    "B BB B     B   BB        BBB BB   B B BB  cBBB B",
    "B BB B B##TB B BB BBBBBB BBB#BB B   B   BTBB   B",
    "B BB B BBBBB B BB B      c    B BBBBB B#BBBBBB B",
    "B BB #       B ##   BB#BBBBBB B       B        B",
    "B BB BBBBBBBBB BB B           B B###BBB BBB BB B",
    "B BB     c     BB BBB B A B BBB B       BB   B B",
    "B BBBBBBBBB BB BB     B#B#B BBB B BBB###BBB BB B",
    "B        BB BB BBBBBB BBBBB             c      B",
    "BBBTBBBB BB BB    c    B    BBBBBBBB BBBBB BBB B",
    "BBB BBBB BB BBBBBBBBBB B B BB            ##    B",
    "B         #            B B BB BBBB BBBBBB B#BB B",
    "BBBBB####BBBB BBB BBBB BtB BB BBBB B      B#   B",
    "BBBBB    BBBB BBB BBBB BcB    #    B BBBBB   BBB",
    "B  c        c          BcB BB BBBB B   BBBBB BBB",
    "B BBBBBBBBBBBBBB B BBB #   BB      BBBB        B",
    "B       c      B B BBB BBBBBB BB BBB BB BBBBBB B",
    "BBBBBBBBBBBBBB   B             B           tBB B",
    "B              B BBBBB BBBBB BBB BB B B BBBBBB B",
    "B BBBBBBBBBBBBBB       B   B BBB BB BBB BB     B",
    "B      T       B BBBBB B B B  c         BB BBB B",
    "BBBBBBB####BBB B #T    B B BBBBB BBBBBB   tBBB B",
    "B              B BB BB B B           B TBBBt   B",
    "B BBBBBBBBBBBBBB BB BB B B BBBB BBB BB BBBB BB B",
    "B      #               t B    t BBT TB         B",
    "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB",
};

SCREEN_S G_SCR;

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
void disp_close()
{
    endwin();
}
void draw_game_screen_layout()
{
    // 1st column
    G_SCR.W[W_ARENA] = create_window(30, 50, 1, 2);
    refresh();
    // 2nd column
    int col2_x = G_SCR.W[W_ARENA]->width + G_SCR.W[W_ARENA]->start_x + 2;
    G_SCR.W[W_DISPLAY] = create_window(15, 54, 1, col2_x);
    G_SCR.W[W_INFO] = create_window(12, 54, G_SCR.W[W_DISPLAY]->start_y + G_SCR.W[W_DISPLAY]->height, col2_x);
    G_SCR.W[W_INPUT] = create_window(3, 54, G_SCR.W[W_INFO]->start_y + G_SCR.W[W_INFO]->height, col2_x);

    for (size_t i = 0; i < W_MAX; i++)
        wrefresh(G_SCR.W[i]->winptr);

    // fill window with texts / map
    draw_display(G_SCR.W[W_DISPLAY], NULL, 0);
    draw_info();
}

void screen_layout_close(SCREEN_S *sc)
{
    for (size_t i = 0; i < W_MAX; i++)
        destroy_window_s(sc->W[i]);
}
WINDOW_S *create_window(int height, int width, int y, int x)
{
    WINDOW_S *window = calloc(1, sizeof(WINDOW_S));
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

void destroy_window_s(WINDOW_S *data)
{
    delwin(data->winptr);
    free(data);
}

void draw_display(WINDOW_S *win, const struct SERVER_OUTPUT *local_data, int Offset)
{
    if (!win || !local_data)
        return;

    if(Offset == 0){
    mvwprintw(G_SCR.W[W_DISPLAY]->winptr, 1, 2, "SPID:  %14d", local_data->owner_pid);
    mvwprintw(G_SCR.W[W_DISPLAY]->winptr, 2, 2, "round:	 %7ld", (long)local_data->round);
    }

    int x = 2 + Offset;
    mvwprintw(G_SCR.W[W_DISPLAY]->winptr, 7, x, "PLAYER: %c", local_data->player.id);

    mvwprintw(G_SCR.W[W_DISPLAY]->winptr, 9, x,  "Bank  :%4d Y :%2d  ", local_data->player.econonmy.bank, local_data->player.positon.y);
    mvwprintw(G_SCR.W[W_DISPLAY]->winptr, 10, x, "Cash  :%4d X :%2d", local_data->player.econonmy.eq, local_data->player.positon.x);
    mvwprintw(G_SCR.W[W_DISPLAY]->winptr, 11, x, "Daeths:%4d", local_data->player.deaths);

    wrefresh(G_SCR.W[W_DISPLAY]->winptr);
}
void draw_info()
{
    WINDOW *win = G_SCR.W[W_INFO]->winptr;
    wclear(win);
    box(win, 0, 0);

    mvwprintw(win, 1, 2, "Use Arrows to move");
    mvwprintw(win, 4, 2, "#     bushes (slow down");
    mvwprintw(win, 5, 2, "12    players");
    mvwprintw(win, 6, 2, "*     enemy beast");
    mvwprintw(win, 7, 2, "c/C/T tresure (1, 10. 50 coins");
    mvwprintw(win, 8, 2, "A     Bank");
    wrefresh(win);
}

void draw_input(int c)
{
    WINDOW *win = G_SCR.W[W_INPUT]->winptr;

    wclear(win);
    box(win, 0, 0);
    switch (c)
    {
    case KEY_LEFT:
        mvwprintw(win, 1, 2, "Input: LEFT");
        break;
    case KEY_RIGHT:
        mvwprintw(win, 1, 2, "Input: RIGHT");
        break;
    case KEY_UP:
        mvwprintw(win, 1, 2, "Input: UP");
        break;
    case KEY_DOWN:
        mvwprintw(win, 1, 2, "Input: DOWN");
        break;

    default:
        mvwprintw(win, 1, 2, "Input: %c", (char)c);
        break;
    }

    wrefresh(win);
}

void draw_map(WINDOW_S *win, struct SERVER_OUTPUT *data)
{
    // int y,x;
    wclear(win->winptr);
    box(win->winptr, 0, 0);

    for (int i = 1; i <= data->viewport_h; i++)
    {

        int j = 1;
        wmove(win->winptr, data->viewport_start.y + i, data->viewport_start.x + j);
        for (; j <= data->viewport_w; j++)
        {
            if (data->arena[i - 1][j - 1] == BLOCK_FULL)
                waddch(win->winptr, ACS_CKBOARD);
            else
                waddch(win->winptr, data->arena[i - 1][j - 1]);
        }
    }
    wrefresh(win->winptr);
}

void draw_server_map(WINDOW_S *win, WORLD_T *data)
{
    // int y,x;
    wclear(win->winptr);
    box(win->winptr, 0, 0);

    int viewport_h = ARENA_HEIGHT;
    int viewport_w = ARENA_WIDTH - 1;
    int start_y = 0, start_x = 0;

    for (int i = 1; i <= viewport_h; i++)
    {

        int j = 1;
        wmove(win->winptr, start_y + i, start_x + j);
        for (; j <= viewport_w; j++)
        {

            if (data->MAP[i - 1][j - 1].block.ID == BLOCK_FULL)
                waddch(win->winptr, ACS_CKBOARD);
            else
                waddch(win->winptr, data->MAP[i - 1][j - 1].block.ID);
        }
    }

    wrefresh(win->winptr);
}
