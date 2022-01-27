#include "client.h"

void client()
{
    srand(time(NULL));
    int my_id = rand() % 1000;
    
    printf("Identyfikator sesji: %d; pdata=%p......\n", my_id, (void *)pdata);
    while (1)
    {
        char msg[PAYLOAD_SIZE];
        printf("Podaj tekst: ");
        char *p = fgets(msg, PAYLOAD_SIZE, stdin);
        if (!p)
            break;
        if (*msg)
            msg[strlen(msg) - 1] = '\x0';

        int res = sem_wait(&pdata->cs);
        strcpy(pdata->payload, msg);
        pdata->id = my_id;
        res = sem_post(&pdata->cs);

        res = sem_post(sem);
    }
}