#include "key_listener.h"

static void *key_listener(void *args);
static void ket_listener_set(int value);

int last_key;
pthread_t klistener_th;
pthread_mutex_t klistener_mutex;

int key_listener_get()
{
    int temp_key = 0;

    // if (pthread_mutex_trylock(&klistener_mutex) == 0){
    pthread_mutex_lock(&klistener_mutex);
    temp_key = last_key;
    last_key = 0;
    pthread_mutex_unlock(&klistener_mutex);

    return temp_key;
}

static void ket_listener_set(int value)
{
    // if (pthread_mutex_trylock(&klistener_mutex) == 0){
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
        if(temp_key == 'q')
            pthread_exit(NULL);
    }
}

void key_listener_init()
{
    pthread_create(&klistener_th, NULL, key_listener, NULL);
}
