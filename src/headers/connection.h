#if !defined(_CONNECTION_H_)
#define _CONNECTION_H_
#include "common.h"

extern sem_t *sem_c_write;
extern int fd_c_write;
extern struct data2_t *pdata_c_write;

extern sem_t *sem_s_write;
extern int fd_s_write;
extern struct data2_t *pdata_s_write;

extern int CLIENT_TYPE;

void connection_init(); // deal with server and host
void connection_close();// deal with server and host



// thread safe, using sem
int connection_push(sem_t *sem, void* dest, void* data, size_t size);
int connection_fetch(sem_t *sem, void* dest, void* src, size_t size);
#endif // _COMMON_H_