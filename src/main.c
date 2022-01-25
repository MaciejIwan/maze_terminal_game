#include "common.h"

int CLIENT_TYPE = 1;

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
    munmap(pdata, sizeof(struct data2_t));
    close(fd);
    shm_unlink("/msg_data");
    sem_close(sem);
    exit(0);
}

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

        sem_wait(&pdata->cs);
        strcpy(pdata->payload, msg);
        pdata->id = my_id;
        sem_post(&pdata->cs);

        sem_post(sem);
    }
    sem_close(sem);
    munmap(pdata, sizeof(struct data2_t));
    close(fd);
    exit(0);
}

int main()
{
    printf("Hello world! %d\n", (int)sizeof(struct data2_t));
    connection_init(); // deal with 2 cases, server and client

    switch (CLIENT_TYPE)
    {
    case TYPE_HOST:
        server();
        break;
    case TYPE_CLIENT:
        client();
        break;
    default:
        break;
    }
    connection_close();
    printf("Server end\n");
    return 0;
}