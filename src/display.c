#include "display.h"

char arena_map[ARENA_HEIGHT][ARENA_WIDTH] = {
    "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB",
    "B            B           BBB        B          B",
    "B BB BBBBBBB B BBBBBBBBB     BBBBBB B BBB BBBB B",
    "B BB B     B   BB        BBB BB   B B BB   BBB B",
    "B BB B B## B B BB BBBBBB BBB#BB B   B   B BB   B",
    "B BB B BBBBB B BB B           B BBBBB BBBBBBBB B",
    "B BB #       B ##   BBBBBBBBB B       B        B",
    "B BB BBBBBBBBB BB B           B B###BBB BBB BB B",
    "B BB           BB BBB B A B BBB B       BB   B B",
    "B BBBBBBBBB BB BB     B#B#B BBB B BBB###BBB BB B",
    "B        BB BB BBBBBB BBBBB                    B",
    "BBB BBBB BB BB         B    BBBBBBBBBBBBBB BBB B",
    "BBB BBBB BB BBBBBBBBBB B B BB             #    B",
    "B                      B B BB BBBB BBBBBB B#BB B",
    "BBBBB####BBBB BBB BBBB B B BB BBBB B      BB   B",
    "BBBBB    BBBB BBB BBBB B B    #    B BBBBB   BBB",
    "B                      B B BB BBBB B   BBBBB BBB",
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
    draw_display(G_SCR.W[W_DISPLAY], NULL);
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

void draw_display(WINDOW_S *win, const struct SERVER_OUTPUT* local_data)
{

    mvwprintw(G_SCR.W[W_DISPLAY]->winptr, 1, 2, " SPID:  %14d", local_data ? local_data->owner_pid : 0);
    mvwprintw(G_SCR.W[W_DISPLAY]->winptr, 2, 2, " CPID:  %14d", getpid());
    mvwprintw(G_SCR.W[W_DISPLAY]->winptr, 3, 2, "pdata:  %p", (void *)pdata_c_write);

    if (!sem_trywait(&pdata_s_write->cs))
    {
        mvwprintw(G_SCR.W[W_DISPLAY]->winptr, 4, 2, "round:	 %7ld", (long)pdata_s_write->round);
        sem_post(&pdata_s_write->cs);
        waddch(G_SCR.W[W_DISPLAY]->winptr, ACS_CKBOARD);
    }

    wrefresh(G_SCR.W[W_DISPLAY]->winptr);
}
 void draw_info(){
    WINDOW* win = G_SCR.W[W_INFO]->winptr;
    wclear(win);
    box(win, 0, 0);

    mvwprintw(win, 1, 2, "Use Arrows to move");
    wrefresh(win);
}

void draw_input(char c)
{
    WINDOW* win = G_SCR.W[W_INPUT]->winptr;

    //wclear(win);
    //box(win, 0, 0);
    mvwprintw(win, 1, 2, "Input: %c", c);
    wrefresh(win);
}

void draw_map(WINDOW_S *win, struct SERVER_OUTPUT *data)
{
    // int y,x;
    for (int i = 1; i < win->height - 1; i++)
    {
        // mvwprintw(win->winptr, i, 1, "%.*s", win->width - 2, arena_map[i - 1]);
        for (int j = 1; j < win->width - 1; j++)
        {
            if (data->arena[i - 1][j - 1] == BLOCK_FULL)
                mvwaddch(win->winptr, i, j, ACS_CKBOARD);
            else
                mvwaddch(win->winptr, i, j, data->arena[i - 1][j - 1]);
        }
    }
    wrefresh(win->winptr);
}
