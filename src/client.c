#include "client.h"

sem_t *sem_c_write;
int fd_c_write;
struct data2_t *pdata_c_write;

void send_key_data(int *c, int *my_id)
{
    *c = key_listener_get(); // fetch newest data
    connection_push(&pdata_c_write->cs, &pdata_c_write->payload, c, sizeof(int));
    connection_push(&pdata_c_write->cs, &pdata_c_write->id, my_id, sizeof(int));
}

void client()
{
    srand(time(NULL));
    extern SCREEN_S G_SCR;
    struct data2_t local_data;
    memset(&local_data, 0, sizeof(struct data2_t));

    disp_init();
    draw_game_screen_layout();
    key_listener_init();

    int my_id = rand() % 1000;

    mvwprintw(G_SCR.W[W_DISPLAY]->winptr, 4, 2, "Identyfikator sesji: %d", my_id);
    mvwprintw(G_SCR.W[W_DISPLAY]->winptr, 5, 2, "pdata=%p", (void *)pdata_c_write);
    wrefresh(G_SCR.W[W_DISPLAY]->winptr);

    int terminate = 0, counter = 0, counter2 = 0, c = 0;
    while (!terminate)
    {
        int res;
        char msg[PAYLOAD_SIZE];

        sem_wait(sem_s_write); // wait until server dont ask you

        connection_fetch(&pdata_s_write->cs, &local_data, pdata_s_write, sizeof(struct data2_t));
        send_key_data(&c, &my_id);

        sem_post(sem_c_write); // let server know data are ready

        sem_getvalue(sem_c_write, &counter);  // debug only
        sem_getvalue(sem_s_write, &counter2); // debug only

        draw_map(G_SCR.W[W_ARENA], &local_data);
        draw_display(G_SCR.W[W_DISPLAY]);
        draw_input(c);

        if (c == 'q')
            terminate = 1;
    }

    screen_layout_close(&G_SCR);
    disp_close();
    //_nc_freeall(); // free all memory allocated by ncurses
    exit_curses(3);
    fflush(stdout); // safe for output streams (remove mesh after ncurses) // https://man7.org/linux/man-pages/man3/fflush.3p.html
}
