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
#include <locale.h> /* for LC_ALL */
#include <ncurses.h>
#include <signal.h>


#include "client.h"
#include "server.h"
#include "connection.h"
#include "key_listener.h"
#include "block_types.h"
#include "display.h"


#define SEC 1000 * 1000
#define MS 1000
#define PAYLOAD_SIZE 2048

#define FILE_C_WRITE_TO "C_SAVE_SHM"
#define FILE_C_WRITE_TO_SEM "C_SAVE_SEM"

#define FILE_S_WRITE_TO "S_SAVE_SHM"
#define FILE_S_WRITE_TO_SEM "S_SAVE_SEM"

#define ARENA_WIDTH 49
#define ARENA_HEIGHT 28

enum CLIENT_TYPES {TYPE_HOST, TYPE_CLIENT};

extern sem_t *sem_c_write;
extern int fd_c_write;
extern struct data2_t *pdata_c_write;

struct data2_t {
    sem_t cs; // sekcja krytyczna
    pid_t owner_pid;
    int id;
    long long round;
    int payload;
    char arena[28][49];
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