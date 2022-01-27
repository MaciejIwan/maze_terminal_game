#include "server.h"

void server()
{
    printf("Gotowe, czekam na klienta; pdata=%p...\n", (void *)pdata);

    int terminate = 0, counter = 0;
    while (!terminate)
    {
        sem_wait(sem);
        sem_wait(&pdata->cs);
        printf("[%03d:%03d]: %c\n", counter++, pdata->id, (char)(*(int*)pdata->payload));
        terminate = *(int*)pdata->payload == 'q';
        sem_post(&pdata->cs);
        usleep(SEC);
    }
}