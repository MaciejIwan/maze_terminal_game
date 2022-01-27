#include "server.h"

void server()
{
    printf("Gotowe, czekam na klienta; pdata=%p...\n", (void *)pdata);

    int terminate = 0, counter = 0;
    while (!terminate)
    {
        sem_wait(sem);
        sem_wait(&pdata->cs);
        printf("[%03d:%03d]: %s\n", counter++, pdata->id, pdata->payload);
        terminate = strcasecmp(pdata->payload, "quit") == 0;
        sem_post(&pdata->cs);
    }
}