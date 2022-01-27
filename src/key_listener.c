#include "key_listener.h"

int last_key;
pthread_t klistener_th;
pthread_mutex_t klistener_mutex;


int key_listener_get()
{
    int temp_key;

    pthread_mutex_lock(&klistener_mutex);
    temp_key = last_key;
    pthread_mutex_unlock(&klistener_mutex);

    return temp_key;
}

static void ket_listener_set(int value)
{
    pthread_mutex_lock(&klistener_mutex);
    last_key = value;
    pthread_mutex_unlock(&klistener_mutex);
}

static void *key_listener(void *args)
{
    pthread_mutex_init(&klistener_mutex, NULL);
    int temp_key;

    while (1)
    {
        temp_key = getch();
        ket_listener_set(temp_key);
    }
}

void key_listener_init()
{
    pthread_create(&klistener_th, NULL, key_listener, NULL);
}