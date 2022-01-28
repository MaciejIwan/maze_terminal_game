#include "connection.h"


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
    connection_server_close(); // remove data after old server (if was crashed);

    sem_c_write = sem_open(FILE_C_WRITE_TO_SEM, O_CREAT, 0600, 0);
    sem_s_write = sem_open(FILE_S_WRITE_TO_SEM, O_CREAT, 0600, 1);
    if(sem_c_write == SEM_FAILED || sem_s_write == SEM_FAILED)
        return connection_server_close(), 1;


    fd_c_write = shm_open(FILE_C_WRITE_TO, O_CREAT | O_RDWR, 0600);
    fd_s_write = shm_open(FILE_S_WRITE_TO, O_CREAT | O_RDWR, 0600);
    if (fd_c_write == -1 || fd_s_write == -1)
        return connection_server_close(), 2;

    ftruncate(fd_c_write, sizeof(struct data2_t));
    ftruncate(fd_s_write, sizeof(struct data2_t));
    pdata_c_write = (struct data2_t *)mmap(NULL, sizeof(struct data2_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd_c_write, 0);
    pdata_s_write = (struct data2_t *)mmap(NULL, sizeof(struct data2_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd_s_write, 0);
    if(sem_c_write == NULL || sem_s_write == NULL)
        return  connection_server_close(), 3;


    sem_init(&pdata_c_write->cs, 1, 1); // shared, signaled
    sem_init(&pdata_s_write->cs, 1, 1); // shared, signaled
    pdata_s_write->owner_pid = getpid();

    return 0;
}

static int connection_client_setup()
{
        sem_c_write = sem_open(FILE_C_WRITE_TO_SEM, 0);
        sem_s_write = sem_open(FILE_S_WRITE_TO_SEM, 0);
    if(sem_c_write == SEM_FAILED || sem_s_write == SEM_FAILED)
        return connection_server_close(), 1;


    fd_c_write = shm_open(FILE_C_WRITE_TO, O_RDWR, 0600); // cant create new
    fd_s_write = shm_open(FILE_S_WRITE_TO, O_RDWR, 0600); // cant create new
    if (fd_c_write == -1 || fd_s_write == -1)
        return connection_server_close(), 2;

    pdata_c_write = (struct data2_t *)mmap(NULL, sizeof(struct data2_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd_c_write, 0);
    pdata_s_write = (struct data2_t *)mmap(NULL, sizeof(struct data2_t), PROT_READ | PROT_WRITE, MAP_SHARED, fd_s_write, 0);
    if(sem_c_write == NULL || sem_s_write == NULL)
        return  connection_server_close(), 3;

    int result = kill(pdata_s_write->owner_pid, 0);
    if (result == -1 && errno == ESRCH)
    {
        printf("Server proccess doesnt exits (%d)\n", pdata_c_write->owner_pid);
        return 4;
    }

    pdata_c_write->owner_pid = getpid();

    return 0;
}

static int connection_client_close()
{
    sem_close(sem_c_write);
    sem_close(sem_s_write);
    munmap(pdata_c_write, sizeof(struct data2_t));
    munmap(pdata_s_write, sizeof(struct data2_t));
    close(fd_c_write);
    close(fd_s_write);
}
static int connection_server_close()
{
    munmap(pdata_c_write, sizeof(struct data2_t));
    munmap(pdata_s_write, sizeof(struct data2_t));

    close(fd_c_write);
    close(fd_s_write);

    shm_unlink(FILE_C_WRITE_TO);
    shm_unlink(FILE_S_WRITE_TO);

    sem_close(sem_c_write);
    sem_close(sem_s_write);

    sem_unlink(FILE_C_WRITE_TO_SEM);
    sem_unlink(FILE_S_WRITE_TO_SEM);
}

int connection_push(sem_t *sem, void* dest, void* data, size_t size){
    sem_wait(sem); // critique session start
    memcpy(dest, data, size);
    sem_post(sem); // critique session stop
}
//static int connection_fetch(){}