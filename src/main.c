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