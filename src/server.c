#include "common.h"

#include <signal.h> // kill
#include <errno.h> // errno, ESRCH

#define SEC 1000*1000
sem_t *sem_ping;
sem_t *sem_pong;


int CLIENT_TYPE = 1;

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
    printf("Hello world! %d\n",(int)sizeof(struct data2_t));
    connection_init(); // deal with 2 cases, server and client
    connection_close();
    printf("Server end\n");
    return 0;
}