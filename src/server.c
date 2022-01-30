#include "server.h"

sem_t *sem_s_write;
int fd_s_write;
struct SERVER_OUTPUT *pdata_s_write;

bool server_player_move(WORLD_T *world, PLAYER *player, DIRECTION dir);
bool server_player_create(WORLD_T *world, PLAYER *player, char id);
bool server_add_player_to_world(WORLD_T *world, PLAYER *player);
CORDS server_player_locate(WORLD_T *world, char player_id);
void generate_player_view(WORLD_T *world, struct SERVER_OUTPUT *dst);
int server_find_random_free_chunk(WORLD_T *world, CORDS *ptr);
void server_swap_file_generator(WORLD_T *world, PLAYER *player, struct SERVER_OUTPUT *output);
DIRECTION key_to_direction(int k);
void deal_with_input(int *key, int *terminate, int *round_number);
void send_data(WORLD_T *world, void *src, const int *round_number);



void server()
{
    struct SERVER_OUTPUT swap_online_player_copy;
    struct SERVER_OUTPUT swap_local_player;
    memset(&swap_online_player_copy, 0, sizeof(struct SERVER_OUTPUT));
    memset(&swap_local_player, 0, sizeof(struct SERVER_OUTPUT));
    PLAYER local_player;
    PLAYER online_player;

    connection_fetch(&pdata_s_write->cs, &swap_online_player_copy, pdata_s_write, sizeof(struct SERVER_OUTPUT));

    WORLD_T *world = server_world_generate();
    if (world == NULL)
        exit(2); // have to free all here }

    server_player_create(world, &online_player, '1');
    server_swap_file_generator(world, &online_player, &swap_online_player_copy);
    printf("Gotowe, czekam na klienta; pdata=%p...\n", (void *)pdata_c_write);

    int terminate = 0, round_number = 0,
        player_input = 0;

    send_data(world, &swap_online_player_copy, &round_number);
    while (!terminate)
    {

        sem_wait(sem_c_write); // wait for player / user data

        deal_with_input(&player_input, &terminate, &round_number);
        server_player_move(world, &online_player, key_to_direction(player_input));
        server_swap_file_generator(world, &online_player, &swap_online_player_copy);
        send_data(world, &swap_online_player_copy, &round_number);

        usleep(250 * MS);
        sem_post(sem_s_write); // send singal to player "NEW DATA ARE WAITING"
    }

    server_world_destory(&world);
}

void deal_with_input(int *key, int *terminate, int *round_number)
{
    sem_wait(&pdata_c_write->cs);

    *key = pdata_c_write->input;
    *round_number = *round_number + 1;
    *terminate = *key == 'q';
    if (*key)
        printf("[%03d:%03d]: %c\n", *round_number, pdata_c_write->client_pid, (char)*key);

    sem_post(&pdata_c_write->cs);
}

void send_data(WORLD_T *world, void *src, const int *round_number)
{
    struct SERVER_OUTPUT *data = (struct SERVER_OUTPUT *)src;

    data->round = *round_number;

    connection_push(&pdata_s_write->cs, pdata_s_write, data, sizeof(struct SERVER_OUTPUT));
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
    world->max_players_on_server = MAX_SERVER_PLAYERS;

    // generate map structure from arena layput
    for (int i = 0; i < ARENA_HEIGHT; i++)
    {
        for (int j = 0; j < ARENA_WIDTH; j++)
        {
            CHUNK *chunk = &world->MAP[i][j];
            block_change_type(chunk, arena_map[i][j], 0);
            chunk->location.x = j;
            chunk->location.y = i;
            chunk->visitors_max = MAX_SERVER_PLAYERS;
            if (chunk->block.ID == BLOCK_BLANK)
                chunk->is_free = TRUE;
        }
    }

    return world;
}

void server_world_destory(WORLD_T **world)
{
    free((*world)->local_player);
    free((*world)->online_player);
    free((*world)->beast);

    free(*world);
    *world = NULL;
}

void server_swap_file_generator(WORLD_T *world, PLAYER *player, struct SERVER_OUTPUT *output)
{
    if (!world || !output)
    {
        return;
    }

    generate_player_view(world, output);
    memcpy(&output->player, player, sizeof(PLAYER));
}
bool server_player_create(WORLD_T *world, PLAYER *player, char id)
{
    if (player == NULL || world == NULL)
        return false;

    memset(player, 0, sizeof(PLAYER));

    player->id = id;
    player->type = PLAYER_HUMAN;
    //player->positon = server_player_locate(world, id); // if not world spawn player in random place
    server_find_random_free_chunk(world, &player->positon);
    server_add_player_to_world(world, player);
    return true;
}
DIRECTION key_to_direction(int k)
{
    switch (k)
    {
    case KEY_RIGHT:
        return RIGHT;
    case KEY_LEFT:
        return LEFT;
    case KEY_UP:
        return UP;
    case KEY_DOWN:
        return DOWN;

    default:
        return STAY;
    }
    return STAY;
}
void server_place_player_on_map(CHUNK *chunk, PLAYER *player)
{
    chunk->visitors_count = 1;
    chunk->visitors[player->id - '0'] = player;
    chunk->is_free = false;
    block_change_type(chunk, player->id, 0);
}
bool server_add_player_to_world(WORLD_T *world, PLAYER *player)
{

    if (world == NULL || player == NULL)
        return FALSE;

    if (world->local_player == NULL)
    {
        world->local_player = player;
        CHUNK *cur_chunk = &world->MAP[player->positon.y][player->positon.x];
        server_place_player_on_map(cur_chunk, player);
        return TRUE;
    }
    else if (world->online_player == NULL)
    {
        world->online_player = player;
        CHUNK *cur_chunk = &world->MAP[player->positon.y][player->positon.x];
        server_place_player_on_map(cur_chunk, player);
        return TRUE;
    }

    return FALSE;
}

int server_find_random_free_chunk(WORLD_T *world, CORDS *ptr)
{
    int size = 0;
    CORDS free_blocks[ARENA_HEIGHT * ARENA_WIDTH];

    for (int i = 0; i < ARENA_HEIGHT; i++)
    {
        for (int j = 0; j < ARENA_WIDTH; j++)
        {
            CHUNK *chunk = &world->MAP[i][j];
            if (chunk->is_free)
            {
                free_blocks[size] = chunk->location;
                size++;
            }
        }
    }

    if (size == 0)
        return 1;

    *ptr = free_blocks[rand() % size];
    return 0;
}

CORDS server_player_locate(WORLD_T *world, char player_id)
{
    for (int i = 0; i < ARENA_HEIGHT; i++)
    {
        for (int j = 0; j < ARENA_WIDTH; j++)
        {
            if (world->MAP[i][j].block.ID == player_id)
            {
                return world->MAP[i][j].location;
            }
        }
    }

    return (CORDS){-1, -1}; // error
}
void generate_player_view(WORLD_T *world, struct SERVER_OUTPUT *dst)
{
    if (!world || !dst)
        return;

    struct SERVER_OUTPUT data;
    memcpy(&data, dst, sizeof(struct SERVER_OUTPUT));

    CORDS player_pos = data.player.positon;
    player_pos = server_player_locate(world, '1'); // for debug and player 1 set in arena_map declaration

    int x = player_pos.x >= 2 ? player_pos.x - 2 : 0,
        y = player_pos.y >= 2 ? player_pos.y - 2 : 0;

    int width = player_pos.x >= 2 ? 5 : 3,
        height = player_pos.y >= 2 ? 5 : (player_pos.y == 1 ? 4 : 3);

    while (x + width > ARENA_WIDTH-1)
        width--;

    while (y + height > ARENA_HEIGHT)
        height--;

    data.viewport_h = height;
    data.viewport_w = width;
    data.viewport_start = (CORDS){.x = x, .y = y};
    memset(&data.arena, 0, 6 * 6);
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
            memcpy(&data.arena[i][j], &world->MAP[y + i][x + j].block.ID, sizeof(char));
    }

    // // debug only
    // setbuf(stdout, 0);
    // for (int i = 0; i < height; i++)
    //     printf("%s\n", data.arena[i]);
    // // debug only

    memcpy(dst, &data, sizeof(struct SERVER_OUTPUT));
}

bool player_move_possible(CHUNK *target_chunk)
{
    if (target_chunk->block.move_permission == 0)
        return FALSE;
    else
        return TRUE;
}

CHUNK *player_move_desitnation_chunk(WORLD_T *world, CORDS pos, DIRECTION dir)
{
    CHUNK *desitnation = NULL;

    switch (dir)
    {
    case UP:
        desitnation = &world->MAP[pos.y - 1][pos.x];
        break;
    case DOWN:
        desitnation = &world->MAP[pos.y + 1][pos.x];
        break;
    case LEFT:
        desitnation = &world->MAP[pos.y][pos.x - 1];
        break;
    case RIGHT:
        desitnation = &world->MAP[pos.y][pos.x + 1];
        break;
    default:
        desitnation = &world->MAP[pos.y][pos.x];
        break;
    }

    return desitnation;
}

static void server_player_move_change_p_pos(PLAYER *player, DIRECTION dir)
{

    switch (dir)
    {
    case UP:
        player->positon.y = player->positon.y - 1;
        break;
    case DOWN:
        player->positon.y = player->positon.y + 1;
        break;
    case LEFT:
        player->positon.x = player->positon.x - 1;
        break;
    case RIGHT:
        player->positon.x = player->positon.x + 1;
        break;
    default:
        break;
    }
}
bool server_player_move(WORLD_T *world, PLAYER *player, DIRECTION dir)
{
    if (!world || !player || dir == STAY)
        return FALSE;
    CHUNK *curr_chunk = &world->MAP[player->positon.y][player->positon.x];
    CHUNK *dest_chunk = player_move_desitnation_chunk(world, player->positon, dir);
    if (dest_chunk == NULL)
        return FALSE;

    if (player_move_possible(dest_chunk) == FALSE)
        return FALSE;

    block_action_ptr action = dest_chunk->block.action;

    extern char arena_map[ARENA_HEIGHT][ARENA_WIDTH];
    block_change_type(curr_chunk, arena_map[player->positon.y][player->positon.x], 0);
    server_player_move_change_p_pos(player, dir);
    server_place_player_on_map(dest_chunk, player);

    if (action != NULL) // block has some action to do
        (*action)(dest_chunk);

    return TRUE;
}