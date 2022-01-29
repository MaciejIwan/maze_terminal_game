#include "server.h"

sem_t *sem_s_write;
int fd_s_write;
struct data2_t *pdata_s_write;

void deal_with_input(int *c, int *terminate, int *round_number)
{
    sem_wait(&pdata_c_write->cs);

    *c = pdata_c_write->payload;
    *round_number = *round_number + 1;
    *terminate = *c == 'q';
    if (*c)
        printf("[%03d:%03d]: %c\n", *round_number, pdata_c_write->id, (char)*c);

    sem_post(&pdata_c_write->cs);
}

void send_data(void *src, const int *round_number)
{
    struct data2_t *data = (struct data2_t *)src;

    size_t size = sizeof(char) * (ARENA_HEIGHT) * (ARENA_WIDTH);
    extern char arena_map[ARENA_HEIGHT][ARENA_WIDTH];

    memcpy(data->arena, arena_map, size);
    data->round = *round_number;

    connection_push(&pdata_s_write->cs, pdata_s_write, data, sizeof(struct data2_t));
}

void server()
{
    struct data2_t local_data;
    memset(&local_data, 0, sizeof(struct data2_t));
    connection_fetch(&pdata_s_write->cs, &local_data, pdata_s_write, sizeof(struct data2_t));

    printf("Gotowe, czekam na klienta; pdata=%p...\n", (void *)pdata_c_write);

    int terminate = 0, round_number = 0,
        player_input = 0;
    while (!terminate)
    {
        send_data(&local_data, &round_number);

        sem_wait(sem_c_write); // wait for player / user data
        deal_with_input(&player_input, &terminate, &round_number);
        usleep(500 * MS);

        sem_post(sem_s_write); // send singal to player "NEW DATA ARE WAITING"
    }
}

WORLD_T *server_world_generate()
{
    extern char arena_map[ARENA_HEIGHT][ARENA_WIDTH];
    WORLD_T *world = calloc(1, sizeof(world));
    if (!world)
    {
        errno = ENOMEM;
        return NULL;
    }
    world->max_players_on_server = 2;

    for (int i = 0; i < ARENA_HEIGHT; i++)
    {
        for (int j = 0; j < ARENA_WIDTH; j++)
        {
            CHUNK chunk = world->MAP[i][j];
            block_change_type(&chunk, arena_map[i][j],0);
        }
    }

    return world;
}
void server_world_destory(WORLD_T **world)
{
    free(*world);
    *world = NULL;
}