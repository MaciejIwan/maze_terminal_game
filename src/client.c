#include "client.h"

sem_t *sem_c_write;
int fd_c_write;
struct data2_t *pdata_c_write;

void send_key_data(int *c, int *my_id)
{
    *c = key_listener_get(); // fetch newest data
    connection_push(&pdata_c_write->cs, pdata_c_write->payload, c, sizeof(int));
    connection_push(&pdata_c_write->cs, &pdata_c_write->id, my_id, sizeof(int));
}

void client()
{
    disp_init();
    key_listener_init();
    srand(time(NULL));
    int my_id = rand() % 1000;

    mvprintw(1, 1, "Identyfikator sesji: %d; pdata=%p......\n", my_id, (void *)pdata_c_write);
    refresh();

    int terminate = 0, counter = 0,counter2 = 0, c = 0;
    while (!terminate)
    {
        int res;
        char msg[PAYLOAD_SIZE];

        mvprintw(2, 1, "Podaj tekst [c:%d s:%d]: ", counter, counter2); // draw game arena
        refresh();

        sem_wait(sem_s_write); // wait until server dont ask you

        send_key_data(&c, &my_id);

        sem_post(sem_c_write); // let server know data are ready

        sem_getvalue(sem_c_write, &counter); // debug only
        sem_getvalue(sem_s_write, &counter2); // debug only

        if (c == 'q')
            terminate = 1;
    }
    disp_close();
    fflush(stdout); // safe for output streams // https://man7.org/linux/man-pages/man3/fflush.3p.html
}
