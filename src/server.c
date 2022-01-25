#include "common.h"

#include <signal.h> // kill
#include <errno.h> // errno, ESRCH

#define SEC 1000*1000
sem_t *sem_ping;
sem_t *sem_pong;

/*
// semaphore mozna znalexc w /dev/shm
    S_IRUSR; // creator can read
    S_IWUSR; // creator can wrtie
    S_IRGRP; // group can read
    S_IWGRP; // group can wrtie
    S_IWOTH; // other can write
    S_IROTH; // other can read

    S_IRUSR; // creator can read
*/

int main()
{
    printf("Hello world!\n");
    connection_init(); // deal with 2 cases, server and client

    while (1)
    {
        
        sem_post(sem_pong);         // popros dane
        sem_wait(sem_ping);        // poczekaj na dane
        printf("odebralem dane\n"); // zrob akcje z danymi
        usleep(SEC);                // poczekaj sobie
       
    }

    sem_close(sem_ping);
    sem_close(sem_pong);

    printf("Server end\n");
}