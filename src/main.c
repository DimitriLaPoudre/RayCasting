/*
** EPITECH PROJECT, 2023
** RayCasting
** File description:
** main.c
*/

#include "Wolf3D.h"

void analyze_event(gamecore_t *gc, player_t *player)
{
    void (*func[SCENE_COUNT])(gamecore_t *, player_t *) = {&analyze_event_game, &analyze_event_pause};

    while(sfRenderWindow_pollEvent(gc->window, &gc->event)) {
        if (gc->event.type == sfEvtClosed) {
            sfRenderWindow_close(gc->window);
            continue;
        }
        if ((gc->event.type == sfEvtKeyPressed || gc->event.type == sfEvtKeyReleased) && gc->event.key.code != -1) {
            gc->key[gc->event.key.code] = 6 - gc->event.type;
            continue;
        }
        if (gc->event.type == sfEvtMouseMoved)  {
            gc->diff_mouse_pos.x += gc->event.mouseMove.x - gc->mouse_pos.x;
            gc->diff_mouse_pos.y += gc->event.mouseMove.y - gc->mouse_pos.y;
            gc->mouse_pos = (sfVector2i){gc->event.mouseMove.x, gc->event.mouseMove.y};
            continue;
        }
        if (gc->event.type == sfEvtGainedFocus || gc->event.type == sfEvtLostFocus)
            gc->focus = gc->event.type - 2;
    }
    func[gc->state](gc, player);
}

void update(gamecore_t *gc, player_t *player)
{
    void (*func[SCENE_COUNT])(gamecore_t *, player_t *) = {&update_game, &update_pause};
    static float task = 0;

    task += gc->delay;
    if (task >= 1 / gc->ups) {
        func[gc->state](gc, player);
        task = fmodf(task, gc->ups);
    }
}

void display(gamecore_t *gc, player_t *player)
{
    void (*func[SCENE_COUNT])(gamecore_t *, player_t *) = {&display_game, &display_pause};
    static float task = 0;

    task += gc->delay;
    if (task >= 1 / gc->fps) {
        func[gc->state](gc, player);
        task = fmodf(task, gc->fps);
    }
}

int gameloop(short **map, sfVector2u map_size)
{
    player_t player = {1.2, 1.2, 135, 0};
    gamecore_t gc = {sfRenderWindow_create((sfVideoMode){800, 600, 32}, "Wolf3D", sfClose, NULL),
    {800, 600}, 0, FOV, map, map_size, {0}, {0}, sfRectangleShape_create(), sfClock_create(), 0, {0},
    {sfRed, (sfColor){200, 0, 0, 255}, sfBlue,  (sfColor){0, 0, 200, 255}, sfGreen,  (sfColor){0, 200, 0, 255}},
    {sfTexture_createFromFile("asset/wall.png", NULL), sfSprite_create()}, GAME, 1, 60, 60, render_dist,
    NULL, {sfTexture_create(gc.window_size.x, gc.window_size.y), sfSprite_create()}};

    gc.framebuffer = malloc(sizeof(sfColor) * gc.window_size.x * gc.window_size.y);
    sfSprite_setTexture(gc.floor.sprite, gc.floor.texture, sfTrue);
    sfRenderWindow_setPosition(gc.window,
    (sfVector2i){(sfVideoMode_getDesktopMode().width - gc.window_size.x) / 2,
    (sfVideoMode_getDesktopMode().height - 80 - gc.window_size.y) / 2});
    sfSprite_setTexture(gc.wall.sprite, gc.wall.texture, sfTrue);
    sfRenderWindow_setMouseCursorVisible(gc.window, sfFalse);
    gc.mouse_pos = (sfVector2i){gc.window_size.x / 2, gc.window_size.y / 2};
    sfMouse_setPositionRenderWindow(gc.mouse_pos, gc.window);
    while (sfRenderWindow_isOpen(gc.window)) {
        gc.delay = sfClock_restart(gc.clock).microseconds / 1000000.0;
        analyze_event(&gc, &player);
        update(&gc, &player);
        display(&gc, &player);
    }
    return 0;
}

static short **get_map(char *path, sfVector2u *map_size)
{
    struct stat info;
    short **map = NULL;
    char *data = NULL;
    char *line = NULL;
    char *token = NULL;
    int offset = 0;
    int len = 0;
    int fd = open("map/level1", O_RDONLY);

    if (fd == -1)
        return NULL;
    (*map_size).x = 0;
    (*map_size).y = 1;
    stat(path, &info);
    if (info.st_size == 0)
        return NULL;
    data = calloc(sizeof(char), info.st_size + 1);
    read(fd, data, info.st_size);
    for (int i = 0; data[i]; i++)
        if (data[i] == '\n') {
            (*map_size).y++;
            data[i] = '\0';
        }
    map = malloc(sizeof(short *) * (*map_size).y);
    for (int i = 0; i < (*map_size).y; i++) {
        len = 0;
        line = strdup(data + offset);
        token = strtok(line, "-");
        for (; token != NULL; token = strtok(NULL, "-"))
            len++;
        free(line);
        if ((*map_size).x == 0)
            (*map_size).x = len;
        if (len != (*map_size).x)
            return NULL;
        map[i] = malloc(sizeof(short) * len);
        line = strdup(data + offset);
        offset += strlen(line) + 1; 
        token = strtok(line, "-");
        for (int j = 0; token != NULL; j++) {
            map[i][j] = atoi(token);
            token = strtok(NULL, "-");
        }
        free(line);
    }
    return map;
}

int main(void)
{
    short **map = NULL;
    sfVector2u map_size = {0};

    map = get_map("map/level1", &map_size);
    if (map)
        gameloop(map, map_size);
    else
        return 84;
}