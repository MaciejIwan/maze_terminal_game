#include "server.h"

sem_t *sem_s_write;
int fd_s_write;
struct data2_t *pdata_s_write;

void deal_with_input(int *c, int *terminate, int *counter)
{
    sem_wait(&pdata_c_write->cs);

    *c = (*(int *)pdata_c_write->payload);
    *counter = *counter + 1;
    *terminate = *(int *)pdata_c_write->payload == 'q';
    if(*c)
        printf("[%03d:%03d]: %c\n", *counter, pdata_c_write->id, (char)*c);

    sem_post(&pdata_c_write->cs);
}
void server()
{
    printf("Gotowe, czekam na klienta; pdata=%p...\n", (void *)pdata_c_write);

    int terminate = 0, counter = 0, c = 0;
    while (!terminate)
    {
        sem_wait(sem_c_write);

        deal_with_input(&c, &terminate, &counter);

        usleep(500 * MS);
        sem_post(sem_s_write);
    }
}