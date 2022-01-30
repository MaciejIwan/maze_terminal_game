#if !defined(_KEYLISTENER_H_)
#define _KEYLISTENER_H_
#include "common.h"
#include <fcntl.h>


void key_listener_init();

void key_listener_close();
int key_listener_get();

#endif // _KEYLISTENER_H_