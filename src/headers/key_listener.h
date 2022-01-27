#if !defined(_CLIENT_H_)
#define _CLIENT_H_
#include "common.h"
#include <fcntl.h>

static void *key_listener(void *args);
void key_listener_init();
int key_listener_get();
static void ket_listener_set(int value);

#endif // _CLIENT_H_