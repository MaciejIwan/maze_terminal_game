// common.h został zaczerpnięty z przykładu wykładowego SO2 Pł Dr. inż Tomasz Jaworski upgrade/shm2a
// a następnie zmodyfikowany

#if !defined(_COMMON_H_)
#define _COMMON_H_
 // common file 
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h> //sem*
#include <sys/mman.h> // mmap, munmap, shm_open, shm_unlink
#include <sys/stat.h> /* For mode constants */
#include <fcntl.h> // O_*
#include <stdlib.h> // exit
#include <unistd.h> // close, ftruncate
#include <string.h> // strcasecmp
#include <time.h> // time
#include <ncurses.h> 

//#include "connection.h"

#define COMMON_FILE_NAME "/msg_data2"
#define COMMON_SEMAPHORE_NAME "/msg_signal2"
#define SEC 1000 * 1000

enum CLIENT_TYPES {TYPE_HOST, TYPE_CLIENT};


extern sem_t *sem;
extern int fd;
extern struct data2_t *pdata;

#define PAYLOAD_SIZE 1024
struct data2_t {
    sem_t cs; // sekcja krytyczna
    pid_t server_pid;
    int id;
    char payload[PAYLOAD_SIZE];
};

static void err(int c, const char* msg) {
    if (!c)
        return;
    perror(msg);
    exit(1);
}

#endif // _COMMON_H_



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