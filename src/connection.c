#include "connection.h"

sem_t *sem;
int fd;
struct data2_t *pdata;

extern int CLIENT_TYPE;

static int connection_client_setup();
static int connection_server_setup();
static int connection_server_close();
static int connection_client_close();

void connection_init()
{
    int res;

    // try to open connection as client
    res = connection_client_setup();

    // if any error, clear all data and setup proccess as server
    if (res)
    {
        res = connection_server_setup();
        CLIENT_TYPE = TYPE_HOST;
    }

    // error
    err(res != 0, "Connection error");
}

void connection_close()
{
    switch (CLIENT_TYPE)
    {
    case TYPE_HOST:
        connection_server_close();
        break;
    case TYPE_CLIENT:
        connection_client_close();
        break;
    }
}

static int connection_server_setup()
{
    printf("Server starting...\n");
    sem = sem_open(FILE_C_SAVE_TO_SEM, O_CREAT, 0600, 0);
    if(sem == SEM_FAILED){
        return 1;
    }

    fd = shm_open(FILE_C_SAVE_TO, O_CREAT | O_RDWR, 0600);
    if (fd == -1)
    {
        connection_server_close();
        return 2;
    }

    ftruncate(fd, sizeof(struct data2_t));
    pdata = (struct data2_t *)mmap(NULL, sizeof(struct data2_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(sem == NULL){
        connection_server_close();
        return 3;
    }

    sem_init(&pdata->cs, 1, 1); // shared, signaled
    pdata->server_pid = getpid();

    return 0;
}

static int connection_client_setup()
{
    sem = sem_open(FILE_C_SAVE_TO_SEM, 0);
    if (sem == SEM_FAILED)
    {
        return 1;
    }

    fd = shm_open(FILE_C_SAVE_TO, O_RDWR, 0600);
    if (fd == -1)
    {
        connection_client_close();
        return 2;
    }
    
    pdata = (struct data2_t *)mmap(NULL, sizeof(struct data2_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (pdata == NULL)
    {
        connection_client_close();
        return 3;
    }

    int result = kill(pdata->server_pid, 0);
    if (result == -1 && errno == ESRCH)
    {
        printf("Server proccess doesnt exits (%d)\n", pdata->server_pid);
        munmap(pdata, sizeof(struct data2_t));
        close(fd);
        sem_close(sem);
        sem_unlink(FILE_C_SAVE_TO_SEM);
        shm_unlink(FILE_C_SAVE_TO);
        return 4;
    }

    return 0;
}

static int connection_client_close()
{
    sem_close(sem);
    munmap(pdata, sizeof(struct data2_t));
    close(fd);
}
static int connection_server_close()
{
    munmap(pdata, sizeof(struct data2_t));
    close(fd);
    shm_unlink(FILE_C_SAVE_TO);
    sem_close(sem);
    sem_unlink(FILE_C_SAVE_TO_SEM);
}