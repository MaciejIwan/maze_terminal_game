#include "server.h"

sem_t *sem_s_write;
int fd_s_write;
struct SERVER_OUTPUT *pdata_s_write;
extern SCREEN_S G_SCR;

WORLD_T *world;
bool server_player_move(WORLD_T *world, PLAYER *player, DIRECTION dir);
bool server_player_create(WORLD_T *world, PLAYER *player, char id, PLAYER_TYPE type);
bool server_add_player_to_world(WORLD_T *world, PLAYER *player);
CORDS server_player_locate(WORLD_T *world, char player_id);
void generate_player_view(WORLD_T *world, struct SERVER_OUTPUT *dst);
int server_find_random_free_chunk(WORLD_T *world, CORDS *ptr);
void server_swap_file_generator(WORLD_T *world, PLAYER *player, struct SERVER_OUTPUT *output);
DIRECTION key_to_direction(int k);
void deal_with_input(int *key, int *terminate, int *round_number, int *cpid);
void send_data(WORLD_T *world, void *src, const int *round_number);
void deal_with_cmd(int cmd);
DIRECTION beast_pursuit(WORLD_T *world);
void *beast_move(void *data);

void server()
{
    struct SERVER_OUTPUT swap_online_player_copy;
    struct SERVER_OUTPUT swap_local_player;
    memset(&swap_online_player_copy, 0, sizeof(struct SERVER_OUTPUT));
    memset(&swap_local_player, 0, sizeof(struct SERVER_OUTPUT));
    PLAYER local_player;
    PLAYER online_player;
    BEAST beast;
    pthread_t beast_th;
    //DIRECTION beast_direction;

    connection_fetch(&pdata_s_write->cs, &swap_online_player_copy, pdata_s_write, sizeof(struct SERVER_OUTPUT));

    disp_init();
    draw_game_screen_layout();
    key_listener_init();

    world = server_world_generate();
    if (world == NULL)
        exit(2); // have to free all here }

    server_player_create(world, &local_player, '1', PLAYER_HUMAN);
    server_player_create(world, &online_player, '2', PLAYER_HUMAN);
    server_player_create(world, &beast, '*', PLAYER_BEAST);
    server_swap_file_generator(world, &online_player, &swap_online_player_copy);

    mvprintw(4, 4, "Gotowe, czekam na klienta");
    refresh();

    int terminate = 0, round_number = 0, cpid = 0;
    int player_input = 0, local_input = 0;
    send_data(world, &swap_online_player_copy, &round_number);

    struct timespec interval;
    // ==============================
    // SERVER LOOP
    // ==============================

    while (!terminate)
    {
        // wait for player / user data
        //sem_wait(sem_c_write);
        clock_gettime(CLOCK_REALTIME, &interval);
        interval.tv_sec += 15;
        if(sem_timedwait(sem_c_write, &interval) != 0){
            break;
        }
        
        int result = kill(cpid, 0);
        if((result == -1 && errno == ESRCH)){
            terminate = 1;
            break;
        }

        // get input from other thread in safe way
        deal_with_input(&player_input, &terminate, &round_number, &cpid);
        local_input = key_listener_get();

        if (local_input == 'q' || player_input == 'q')
        {
            terminate = 1;
            break;
        }
        // convert input to direction
        local_player.input = key_to_direction(local_input);
        online_player.input = key_to_direction(player_input);

        // deal with inputs
        deal_with_cmd(local_input);
        server_player_move(world, &local_player, local_player.input);
        server_player_move(world, &online_player, online_player.input);
        pthread_create(&beast_th, NULL, beast_move, &beast.input);
        pthread_join(beast_th, NULL);

        // generate player data in safe way, and send it
        server_swap_file_generator(world, &online_player, &swap_online_player_copy);
        send_data(world, &swap_online_player_copy, &round_number);

        // serwer tick and view
        usleep(250 * MS);
        draw_display(G_SCR.W[W_DISPLAY], &swap_online_player_copy,0);
        memcpy(&swap_local_player.player, &local_player, sizeof(PLAYER));
        mvwprintw(G_SCR.W[W_DISPLAY]->winptr, 1, 28, "CPID:  %14d", cpid);
        draw_display(G_SCR.W[W_DISPLAY], &swap_local_player,28);
        draw_server_map(G_SCR.W[W_ARENA], world);
        draw_input(local_player.input);

        // send singal to player "NEW DATA ARE WAITING"
        sem_post(sem_s_write);
    }

    // ==============================
    // SERVER LOOP
    // ==============================
    if(kill(cpid, 0))
        kill(cpid, SIGKILL);
    void key_listener_close();
    connection_close();
    server_world_destory(&world);
    screen_layout_close(&G_SCR);
    disp_close();
    fflush(stdout); // safe for output streams (remove mesh after ncurses) // https://man7.org/linux/man-pages/man3/fflush.3p.html
    exit_curses(3);
}

void deal_with_input(int *key, int *terminate, int *round_number, int *cpid)
{
    sem_wait(&pdata_c_write->cs);

    *key = pdata_c_write->input;
    *cpid = pdata_c_write->client_pid;
    *round_number = *round_number + 1;
    *terminate = *key == 'q';

    if (*key)
    {
        WINDOW *win = G_SCR.W[W_INFO]->winptr;
        wclear(win);
        box(win, 0, 0);
        draw_info();
        mvwprintw(win, 1, 2, "Use Arrows to move");

        switch (*key)
        {
        case KEY_LEFT:
            mvwprintw(win, 2, 2, "LAST ONLINE USER INPUT: LEFT");
            break;
        case KEY_RIGHT:
            mvwprintw(win, 2, 2, "LAST ONLINE USER INPUT: RIGHT");
            break;
        case KEY_UP:
            mvwprintw(win, 2, 2, "LAST ONLINE USER INPUT: UP");
            break;
        case KEY_DOWN:
            mvwprintw(win, 2, 2, "LAST ONLINE USER INPUT: DOWN");
            break;

        default:
            mvwprintw(win, 2, 2, "LAST ONLINE USER INPUT: [%03d:%03d]: %c", *round_number, pdata_c_write->client_pid, (char)*key);
            break;
        }
        wrefresh(win);
    }

    sem_post(&pdata_c_write->cs);
}

void deal_with_cmd(int cmd)
{
    CORDS cords = {0, 0};
    CHUNK *chunk;

    server_find_random_free_chunk(world, &cords);
    chunk = &world->MAP[cords.y][cords.x];

    switch (cmd)
    {
    case BLOCK_ONE_COIN:
        block_change_type(chunk, BLOCK_ONE_COIN, 0);
        break;
    case BLOCK_LARGE_TREASURE:
        block_change_type(chunk, BLOCK_LARGE_TREASURE, 0);
        break;
    case BLOCK_TREASURE:
        block_change_type(chunk, BLOCK_TREASURE, 0);
        break;
    case BLOCK_BUSHES:
        block_change_type(chunk, BLOCK_BUSHES, 0);
        break;

    default:
        break;
    }
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
    WORLD_T *world = calloc(1, sizeof(WORLD_T));
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
            if (chunk->block.ID == BLOCK_BLANK)
                chunk->is_free = TRUE;
        }
    }

    return world;
}

void server_world_destory(WORLD_T **world)
{

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
bool server_player_create(WORLD_T *world, PLAYER *player, char id, enum player_type type)
{
    if (player == NULL || world == NULL)
        return false;

    memset(player, 0, sizeof(PLAYER));

    player->id = id;
    player->type = type;
    if (player->type == PLAYER_BEAST)
    {
        player->kills = 0;
        player->last_move = STAY;
    }
    // player->positon = server_player_locate(world, id); // if not world spawn player in random place
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
    chunk->visitor = player;
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
    else if (world->beast == NULL)
    {
        world->beast = player;
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
    player_pos = server_player_locate(world, '2'); // for debug and player 1 set in arena_map declaration

    int x = player_pos.x >= 2 ? player_pos.x - 2 : 0,
        y = player_pos.y >= 2 ? player_pos.y - 2 : 0;

    int width = player_pos.x >= 2 ? 5 : 3,
        height = player_pos.y >= 2 ? 5 : (player_pos.y == 1 ? 4 : 3);

    while (x + width > ARENA_WIDTH - 1)
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

void server_respawn_player(CHUNK *player_chunk)
{
    if (player_chunk == NULL)
        return;
    CORDS new_cords;
    CHUNK *new_chunk;

    server_find_random_free_chunk(world, &new_cords);
    new_chunk = &world->MAP[new_cords.y][new_cords.x];
    player_chunk->visitor->positon.x = new_cords.x;
    player_chunk->visitor->positon.y = new_cords.y;

    server_place_player_on_map(new_chunk, player_chunk->visitor);
    block_change_type(player_chunk, BLOCK_BLANK, 0);
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

    if (player->blocked == TRUE)
    {
        player->blocked = FALSE;
        return FALSE;
    }

    CHUNK *curr_chunk = &world->MAP[player->positon.y][player->positon.x];
    CHUNK *dest_chunk = player_move_desitnation_chunk(world, player->positon, dir);

    if (dest_chunk == NULL)
        return FALSE;

    if (player_move_possible(dest_chunk) == FALSE)
        return FALSE;

    CHUNK temp_chunk; // old properties, need for action fun
    memcpy(&temp_chunk, dest_chunk, sizeof(CHUNK));

    extern char arena_map[ARENA_HEIGHT][ARENA_WIDTH];
    block_action_ptr action = dest_chunk->block.action;
    if (action == block_action_player || action == block_action_beast)
    {
        int D = (*action)(curr_chunk, dest_chunk);

        server_respawn_player(curr_chunk);
        server_respawn_player(dest_chunk);
        block_change_type(curr_chunk, BLOCK_DROPED_TREASURE, D);
        block_change_type(dest_chunk, BLOCK_BLANK, 0);
    }
    // else if (action == block_action_beast){
    //     int D = (*action)(curr_chunk, dest_chunk);
    // }
    else
    {
        char block = arena_map[player->positon.y][player->positon.x];
        if (block == BLOCK_BUSHES)
            block = BLOCK_BUSHES;
        else
            block = BLOCK_BLANK;
        block_change_type(curr_chunk, block, 0);
        server_player_move_change_p_pos(player, dir);
        server_place_player_on_map(dest_chunk, player);

        if (action != NULL) // block has some action to do
        {
            (*action)(&temp_chunk, player);
        }
    }

    return TRUE;
}

void *beast_move(void *data)
{
    //return NULL;
    PLAYER* player = world->beast;
    DIRECTION dir = beast_pursuit(world);
    memcpy(data, &dir, sizeof(DIRECTION));

    CHUNK *curr_chunk = &world->MAP[player->positon.y][player->positon.x];
    CHUNK *dest_chunk = player_move_desitnation_chunk(world, player->positon, dir);

    if (dest_chunk == NULL)
        return FALSE;

    if (player_move_possible(dest_chunk) == FALSE)
        return FALSE;

    CHUNK temp_chunk; // old properties, need for action fun
    memcpy(&temp_chunk, dest_chunk, sizeof(CHUNK));

    extern char arena_map[ARENA_HEIGHT][ARENA_WIDTH];
    block_action_ptr action = dest_chunk->block.action;
    if (action == block_action_player)
    {
        int D = (*action)(curr_chunk, dest_chunk);

        server_respawn_player(curr_chunk);
        server_respawn_player(dest_chunk);
        block_change_type(curr_chunk, BLOCK_DROPED_TREASURE, D);
        block_change_type(dest_chunk, BLOCK_BLANK, 0);
    }

    else
    {
        char block = arena_map[player->positon.y][player->positon.x];

        block_change_type(curr_chunk, block, 0);
        server_player_move_change_p_pos(player, dir);
        server_place_player_on_map(dest_chunk, player);

        if (action != NULL) // block has some action to do
        {
            (*action)(&temp_chunk, player);
        }
    }

    return NULL;
}
DIRECTION beast_pursuit(WORLD_T *world)
{
    // check if player is reachable, if yes then try to catch him
    // if not just stop)
    CORDS beast_pos = world->beast->positon;
    CHUNK *chunk = &world->MAP[beast_pos.y][beast_pos.x];

    int BEAST_REACH = 2;

    // vertical
    for (int i = -2; i <= BEAST_REACH; i++)
    {
        if (i == 0)
            continue;
        if (beast_pos.y + i < 1 || beast_pos.y + i > ARENA_HEIGHT - 1)
            continue;

        chunk = &world->MAP[beast_pos.y+i][beast_pos.x + i];

        if (chunk->visitor && chunk->visitor->type == PLAYER_HUMAN)
        {
            if (i < 0)
                return UP;
            else
                return DOWN;
        }
    }

    // horizontal
    for (int i = -2; i <= BEAST_REACH; i++)
    {
        if (i == 0)
            continue;
        if (beast_pos.x + i < 1 || beast_pos.x + i > ARENA_WIDTH - 1)
            continue;

        chunk = &world->MAP[beast_pos.y][beast_pos.x + i];

        if (chunk->visitor && chunk->visitor->type == PLAYER_HUMAN)
        {
            if (i < 0)
                return LEFT;
            else
                return RIGHT;
        }
    }

    return (rand() % STAY);
}
