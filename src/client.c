#include "client.h"

void client()
{
    win_init();
    key_listener_init();
    srand(time(NULL));
    int my_id = rand() % 1000;
    
    mvprintw(1, 1, "Identyfikator sesji: %d; pdata=%p......\n", my_id, (void *)pdata);
    refresh();

    while (1)
    {
        int res, p;
        char msg[PAYLOAD_SIZE];

        mvprintw(2, 1,"Podaj tekst: ");
        refresh();


        p = key_listener_get();

        res = sem_wait(&pdata->cs); // critique session start
        memcpy(pdata->payload,&p,sizeof(int));
        pdata->id = my_id;
        res = sem_post(&pdata->cs); // critique session stop

        res = sem_post(sem); // let server know data is ready

        if (p == 'q')
            break;
    }
}

