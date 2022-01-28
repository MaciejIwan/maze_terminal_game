#include "common.h"

int CLIENT_TYPE = 1;

int main()
{
    printf("Hello world! %d\n", (int)sizeof(struct data2_t));
    connection_init(); // deal with 2 cases, server and client

    switch (CLIENT_TYPE)
    {
    case TYPE_HOST:
        server();
        break;
    case TYPE_CLIENT:
        client();
        break;
    default:
        break;
    }
    connection_close();
    printf("Server end\n");
    return 0;
}



// int main2()
// {
//     disp_init();
//     connection_init();
//     key_listener_init();

//     mvprintw(2, 1, "hello world");
//     refresh();

//     while (1)
//     {
//         sem_wait(sem_pong); // czekaj az go poprosza

//         pthread_mutex_lock(&klistener_mutex);

//         switch (last_key)
//         {
//         case KEY_RIGHT:
//             mvprintw(4, 1, "pong RIGHT");
//             break;
//         case KEY_LEFT:
//             mvprintw(4, 1, "pong LEFT");
//             break;
//         case KEY_UP:
//             mvprintw(4, 1, "pong UP");
//             break;
//         case KEY_DOWN:
//             mvprintw(4, 1, "pong DOWN");
//             break;

//         default:
//             mvprintw(4, 1, "pong %c", last_key);
//             last_key = 0;
//             break;
//         }
//         pthread_mutex_unlock(&klistener_mutex);
//         sem_post(sem_ping); // wysyla informacje o tym ze te dane co maja to czekaja
//         refresh();
//     }

//     printf("Client stop\n");
//     return 0;
// }