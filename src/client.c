#include "client.h"

sem_t *sem_c_write;
int fd_c_write;
struct USER_INPUT *pdata_c_write;

void send_key_data(int *c)
{
    *c = key_listener_get(); // fetch newest data
    int pid = getpid();
    connection_push(&pdata_c_write->cs, &pdata_c_write->input, c, sizeof(int));
    connection_push(&pdata_c_write->cs, &pdata_c_write->client_pid, &pid, sizeof(int));
}

void client()
{

    extern SCREEN_S G_SCR;
    struct SERVER_OUTPUT local_data;
    memset(&local_data, 0, sizeof(struct SERVER_OUTPUT));

    disp_init();
    draw_game_screen_layout();
    key_listener_init();


    int terminate = 0, c = 0;
    while (!terminate)
    {

        sem_wait(sem_s_write); // wait until server dont ask you

        connection_fetch(&pdata_s_write->cs, &local_data, pdata_s_write, sizeof(struct SERVER_OUTPUT));
        send_key_data(&c);

        sem_post(sem_c_write); // let server know data are ready

        draw_map(G_SCR.W[W_ARENA], &local_data);
        draw_display(G_SCR.W[W_DISPLAY], &local_data, 0);
        draw_input(c);

        if (c == 'q')
            terminate = 1;
    }

    key_listener_close();
    screen_layout_close(&G_SCR);
    disp_close();
    //_nc_freeall(); // free all memory allocated by ncurses
    exit_curses(3);
    fflush(stdout); // safe for output streams (remove mesh after ncurses) // https://man7.org/linux/man-pages/man3/fflush.3p.html
}
