/*
** EPITECH PROJECT, 2023
** RayCasting
** File description:
** main.c
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <SFML/Graphics.h>
#include <SFML/System.h>

#define move_speed (float)2.5
#define mouse_sensibility (sfVector2f){(float)3 / (float)10, (float)3 / (float)10}
#define in_map(px, py, gc) (px >= 0 && px < gc->map_size.x && py >= 0 && py < gc->map_size.y)

typedef struct player_s {
    float x;
    float y;
    float cam_x;
    float cam_y;
} player_t;

typedef struct gamecore_s {
    sfRenderWindow *window;
    sfVector2f window_size;
    sfEvent event;
    float fov;
    char **map;
    sfVector2u map_size;
    sfVector2i mouse_pos;
    sfVector2i diff_mouse_pos;
    sfRectangleShape *rect;
    sfClock *clock;
    float delay;
    char key[sfKeyCount];
    sfColor type[3];
} gamecore_t;

static void place_wall(float point[2], int ray, player_t *player, gamecore_t *gc)
{
    float dist = sqrtf(powf(point[0] - player->x, 2) + powf(point[1] - player->y, 2));
    int type = 0;

    dist *= cosf(fabsf(player->cam_x - gc->fov / (float)2 + (fabsf(ray) / (gc->window_size.x / gc->fov)) - player->cam_x) * (M_PI / 180.0));
    if (ray > 0) {
        if ((in_map((int)floorf(point[0] - 1), (int)point[1], gc) && gc->map[(int)floorf(point[1])][(int)point[0] - 1])
        && (in_map((int)floorf(point[0]), (int)point[1], gc) && gc->map[(int)floorf(point[1])][(int)point[0]]))
            type = (sqrtf(powf((int)floorf(point[1]), 2) + powf((int)point[0] - 1, 2)) < sqrtf(powf((int)floorf(point[1]), 2) + powf((int)point[0], 2)))
            ? gc->map[(int)floorf(point[1])][(int)point[0] - 1] :  gc->map[(int)floorf(point[1])][(int)point[0]];
        else
            type = (in_map((int)floorf(point[0] - 1), (int)point[1], gc) && gc->map[(int)floorf(point[1])][(int)point[0] - 1])
            ? gc->map[(int)floorf(point[1])][(int)point[0] - 1] :  gc->map[(int)floorf(point[1])][(int)point[0]];
    } else {
        if ((in_map((int)floorf(point[0]), (int)point[1] - 1, gc) && gc->map[(int)point[1] - 1][(int)floorf(point[0])])
        && (in_map((int)floorf(point[0]), (int)point[1], gc) && gc->map[(int)point[1]][(int)floorf(point[0])]))
            type = (sqrtf(powf((int)floorf(point[0]), 2) + powf((int)point[1] - 1, 2)) < sqrtf(powf((int)floorf(point[0]), 2) + powf((int)point[1], 2)))
            ? gc->map[(int)point[1] - 1][(int)floorf(point[0])] :  gc->map[(int)point[1]][(int)floorf(point[0])];
        else
            type = (in_map((int)floorf(point[0]), (int)point[1] - 1, gc) && gc->map[(int)point[1] - 1][(int)floorf(point[0])])
            ? gc->map[(int)point[1] - 1][(int)floorf(point[0])] :  gc->map[(int)point[1]][(int)floorf(point[0])];
    }
    ray = fabsf(ray) - 1;
    sfRectangleShape_setSize(gc->rect, (sfVector2f){1, gc->window_size.y / dist});
    sfRectangleShape_setPosition(gc->rect, (sfVector2f){ray, -player->cam_y * (gc->window_size.y / (float)90) + (gc->window_size.y - (gc->window_size.y / dist)) / (float)2});
    sfRectangleShape_setFillColor(gc->rect, gc->type[type - 1]);
    sfRenderWindow_drawRectangleShape(gc->window, gc->rect, NULL);
}

static void display_ray(float cam[2], int ray, player_t *player, gamecore_t *gc)
{
    float side_x[2] = {(float)((cam[0] >= 180) ? -1 : 1), 0};
    float side_y[2] = {0, (float)((cam[0] < 90 || cam[0] >= 270) ? -1 : 1)};
    float point_x[2] = {0};
    float point_y[2] = {0};
    
    point_x[0] = (cam[0] >= 180) ? floorf(player->x) : floorf(player->x) + 1;
    point_x[1] = tanf((cam[0] >= 180) ? fabsf(cam[0] - 270) * (M_PI / 180.0)
    : fabsf(cam[0] - 90) * (M_PI / 180.0)) * fabsf(point_x[0] - player->x)
    * (float)((cam[0] < 90 || cam[0] >= 270) ? -1 : 1) + player->y;
    point_y[1] = (cam[0] < 90 || cam[0] >= 270) ? floorf(player->y) : floorf(player->y) + 1;
    point_y[0] = fabsf(point_y[1] - player->y) / tanf((cam[0] >= 180)
    ? fabsf(cam[0] - 270) * (M_PI / 180.0)
    : fabsf(cam[0] - 90) * (M_PI / 180.0)) * (float)((cam[0] >= 180) ? -1 : 1) + player->x;
    side_x[1] = tanf((cam[0] >= 180) ? fabsf(cam[0] - 270) * (M_PI / 180.0)
    : fabsf(cam[0] - 90) * (M_PI / 180.0))
    * (float)((cam[0] < 90 || cam[0] >= 270) ? -1 : 1);
    side_y[0] = (float)((cam[0] >= 180) ? -1 : 1) / tanf((cam[0] >= 180)
    ? fabsf(cam[0] - 270) * (M_PI / 180.0) : fabsf(cam[0] - 90) * (M_PI / 180.0));
    while (1) {
        if (!in_map((int)floorf(point_x[0] - 1), (int)point_x[1], gc) && !in_map((int)floorf(point_x[0]), (int)point_x[1], gc)
        && !in_map((int)floorf(point_y[0]), (int)point_y[1] - 1, gc) && !in_map((int)floorf(point_y[0]), (int)point_y[1], gc))
            break;
        if (sqrtf(powf(point_x[0] - player->x, 2) + powf(point_x[1] - player->y, 2))
        < sqrtf(powf(point_y[0] - player->x, 2) + powf(point_y[1] - player->y, 2))) {
            if ((in_map((int)(point_x[0] - 1), (int)floorf(point_x[1]), gc) && gc->map[(int)floorf(point_x[1])][(int)point_x[0] - 1])
            || (in_map((int)(point_x[0]), (int)floorf(point_x[1]), gc) && gc->map[(int)floorf(point_x[1])][(int)point_x[0]])) {
                place_wall(point_x, ray + 1, player, gc);
                break;
            }
            point_x[0] += side_x[0];
            point_x[1] += side_x[1];
        } else {
            if ((in_map((int)floorf(point_y[0]), (int)point_y[1] - 1, gc) && gc->map[(int)point_y[1] - 1][(int)floorf(point_y[0])])
            || (in_map((int)floorf(point_y[0]), (int)point_y[1], gc) && gc->map[(int)point_y[1]][(int)floorf(point_y[0])])) {
                place_wall(point_y, -(ray + 1), player, gc);
                break;
            }
            point_y[0] += side_y[0];
            point_y[1] += side_y[1];
        }
    }
}

void display(gamecore_t *gc, player_t *player)
{
    sfRenderWindow_clear(gc->window, sfBlack);
    sfRectangleShape_setSize(gc->rect, (sfVector2f){gc->window_size.x, gc->window_size.y * 2});
    sfRectangleShape_setFillColor(gc->rect, (sfColor){0, 200, 240, 255});
    sfRectangleShape_setPosition(gc->rect, (sfVector2f){0, 0});
    sfRenderWindow_drawRectangleShape(gc->window, gc->rect, NULL);
    sfRectangleShape_setFillColor(gc->rect, (sfColor){150, 150, 150, 255});
    sfRectangleShape_setPosition(gc->rect, (sfVector2f){0, -player->cam_y * (gc->window_size.y / (float)90) + gc->window_size.y / (float)2});
    sfRenderWindow_drawRectangleShape(gc->window, gc->rect, NULL);
    for (float i = 0; i < gc->fov; i += gc->fov / gc->window_size.x) {
        display_ray((float [2]){fmod(player->cam_x - gc->fov / 2 + i + 360, 360), player->cam_y},
        roundf(i * (gc->window_size.x / gc->fov)), player, gc);
    }
    sfRenderWindow_display(gc->window);
}

static void check_key(gamecore_t *gc, player_t *player)
{
    if (gc->key[sfKeyZ] || gc->key[sfKeyUp]) {
        player->x += sinf(player->cam_x * (M_PI / 180.0)) * move_speed * gc->delay;
        player->y -= cosf(player->cam_x * (M_PI / 180.0)) * move_speed * gc->delay;
        if (!in_map((int)floorf(player->x), (int)floorf(player->y), gc) || gc->map[(int)floorf(player->y)][(int)floorf(player->x)] != 0) {
            player->x -= sinf(player->cam_x * (M_PI / 180.0)) * move_speed * gc->delay;
            player->y += cosf(player->cam_x * (M_PI / 180.0)) * move_speed * gc->delay;
        }
    }
    if (gc->key[sfKeyS] || gc->key[sfKeyDown]) {
        player->x -= sinf(player->cam_x * (M_PI / 180.0)) * move_speed * gc->delay;
        player->y += cosf(player->cam_x * (M_PI / 180.0)) * move_speed * gc->delay;
        if (!in_map((int)floorf(player->x), (int)floorf(player->y), gc) || gc->map[(int)floorf(player->y)][(int)floorf(player->x)] != 0) {
            player->x += sinf(player->cam_x * (M_PI / 180.0)) * move_speed * gc->delay;
            player->y -= cosf(player->cam_x * (M_PI / 180.0)) * move_speed * gc->delay;
        }
    }
    if (gc->key[sfKeyQ] || gc->key[sfKeyLeft]) {
        player->x -= sinf((player->cam_x + 90) * (M_PI / 180.0)) * move_speed * gc->delay;
        player->y += cosf((player->cam_x + 90) * (M_PI / 180.0)) * move_speed * gc->delay;
        if (!in_map((int)floorf(player->x), (int)floorf(player->y), gc) || gc->map[(int)floorf(player->y)][(int)floorf(player->x)] != 0) {
            player->x += sinf((player->cam_x + 90) * (M_PI / 180.0)) * move_speed * gc->delay;
            player->y -= cosf((player->cam_x + 90) * (M_PI / 180.0)) * move_speed * gc->delay;
        }
    }
    if (gc->key[sfKeyD] || gc->key[sfKeyRight]) {
        player->x += sinf((player->cam_x + 90) * (M_PI / 180.0)) * move_speed * gc->delay;
        player->y -= cosf((player->cam_x + 90) * (M_PI / 180.0)) * move_speed * gc->delay;
        if (!in_map((int)floorf(player->x), (int)floorf(player->y), gc) || gc->map[(int)floorf(player->y)][(int)floorf(player->x)] != 0) {
            player->x -= sinf((player->cam_x + 90) * (M_PI / 180.0)) * move_speed * gc->delay;
            player->y += cosf((player->cam_x + 90) * (M_PI / 180.0)) * move_speed * gc->delay;
        }
    }
    printf("%f et %f et %f\n", player->x, player->y, player->cam_x);
}

static void check_cam(gamecore_t *gc, player_t *player)
{
    player->cam_x = fmodf(player->cam_x + gc->diff_mouse_pos.x * mouse_sensibility.x, 360);
    for (; player->cam_x < 0; player->cam_x += 360);
    player->cam_y += gc->diff_mouse_pos.y * mouse_sensibility.y;
    if (player->cam_y > 90 || player->cam_y < -90)
        player->cam_y = (player->cam_y > 90) ? 90 : -90;
    gc->diff_mouse_pos = (sfVector2i){0, 0};
    gc->mouse_pos = (sfVector2i){gc->window_size.x / 2, gc->window_size.y / 2};
    sfMouse_setPositionRenderWindow(gc->mouse_pos, gc->window);
}

void analyze_event(gamecore_t *gc, player_t *player)
{
    while(sfRenderWindow_pollEvent(gc->window, &gc->event))
        switch (gc->event.type) {
            case sfEvtClosed:
                sfRenderWindow_close(gc->window);
                break;
            case sfEvtKeyPressed:
                gc->key[gc->event.key.code] = 1;
                break;
            case sfEvtKeyReleased:
                gc->key[gc->event.key.code] = 0;
                break;
            case sfEvtMouseMoved:
                gc->diff_mouse_pos.x += gc->event.mouseMove.x - gc->mouse_pos.x;
                gc->diff_mouse_pos.y += gc->event.mouseMove.y - gc->mouse_pos.y;
                gc->mouse_pos = (sfVector2i){gc->event.mouseMove.x, gc->event.mouseMove.y};
        }
    check_key(gc, player);
    check_cam(gc, player);
}

int gameloop(char **map, sfVector2u map_size)
{
    player_t player = {1.2, 1.2, 180, 0};
    gamecore_t gc = {sfRenderWindow_create((sfVideoMode){800, 600, 32}, "Wolf3D", sfClose, NULL),
    {800, 600}, 0, 90, map, map_size, {0}, {0}, sfRectangleShape_create(), sfClock_create(), 0, {0}, {sfRed, sfBlue, sfGreen}};

    sfRenderWindow_setMouseCursorVisible(gc.window, sfFalse);
    gc.mouse_pos = sfMouse_getPositionRenderWindow(gc.window);
    while (sfRenderWindow_isOpen(gc.window)) {
        gc.delay = sfClock_restart(gc.clock).microseconds / 1000000.0;
        analyze_event(&gc, &player);
        display(&gc, &player);
    }
    return 0;
}

static char **get_map(int fd, char *path, sfVector2u *map_size)
{
    struct stat info;
    char **map = NULL;
    char *data = NULL;
    char *line = NULL;
    char *token = NULL;
    int offset = 0;
    int len = 0;

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
    map = malloc(sizeof(char *) * (*map_size).y);
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
        map[i] = malloc(sizeof(char) * len);
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

int main(int ac, char **av)
{
    int fd = 0;
    char **map = NULL;
    sfVector2u map_size = {0};

    if (ac != 2)
        return 84;
    fd = open(av[1], O_RDONLY);
    if (fd == -1)
        return 84;
    map = get_map(fd, av[1], &map_size);
    if (map)
        gameloop(map, map_size);
    else
        return 84;
}