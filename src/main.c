/*
** EPITECH PROJECT, 2023
** RayCasting
** File description:
** main.c
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <SFML/Graphics.h>
#include <SFML/System.h>

#define move_speed (float)2.5
#define mouse_sensibility (sfVector2f){(float)3 / (float)10, (float)3 / (float)10}
#define key_up sfKeyZ
#define key_down sfKeyS
#define key_left sfKeyQ
#define key_right sfKeyD

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
} gamecore_t;

static void place_wall(float point[2], int ray, player_t *player, gamecore_t *gc)
{
    float dist = sqrt(powf(point[0] - player->x, 2) + powf(point[1] - player->y, 2));
    int type = 0;

    dist *= cosf(fabs(player->cam_x - gc->fov / (float)2 + (fabs(ray) / (gc->window_size.x / gc->fov)) - player->cam_x) * (M_PI / 180.0));
    // if (ray > 0) {
    //     type = (sqrt(powf(floorf(point[0]) - floorf(player->x), 2) + powf(point[1] - floorf(player->y), 2)))
    // } else {

    // }
    ray = fabs(ray) - 1;
    sfRectangleShape_setSize(gc->rect, (sfVector2f){1, gc->window_size.y / dist});
    sfRectangleShape_setPosition(gc->rect, (sfVector2f){ray, -player->cam_y * (gc->window_size.y / (float)90) + (gc->window_size.y - (gc->window_size.y / dist)) / (float)2});
    sfRectangleShape_setFillColor(gc->rect, sfRed);
    sfRenderWindow_drawRectangleShape(gc->window, gc->rect, NULL);
}  

void display_ray(float cam[2], int ray, player_t *player, gamecore_t *gc)
{
    float side_x[2] = {(float)((cam[0] >= 180) ? -1 : 1), 0};
    float side_y[2] = {0, (float)((cam[0] < 90 || cam[0] >= 270) ? -1 : 1)};
    float point_x[2] = {0};
    float point_y[2] = {0};
    
    point_x[0] = (cam[0] >= 180) ? floorf(player->x) : floorf(player->x) + 1;
    point_x[1] = tanf((cam[0] >= 180) ? fabs(cam[0] - 270) * (M_PI / 180.0)
    : fabs(cam[0] - 90) * (M_PI / 180.0)) * fabs(point_x[0] - player->x)
    * (float)((cam[0] < 90 || cam[0] >= 270) ? -1 : 1) + player->y;
    point_y[1] = (cam[0] < 90 || cam[0] >= 270) ? floorf(player->y) : floorf(player->y) + 1;
    point_y[0] = fabs(point_y[1] - player->y) / tanf((cam[0] >= 180)
    ? fabs(cam[0] - 270) * (M_PI / 180.0)
    : fabs(cam[0] - 90) * (M_PI / 180.0)) * (float)((cam[0] >= 180) ? -1 : 1) + player->x;
    side_x[1] = tanf((cam[0] >= 180) ? fabs(cam[0] - 270) * (M_PI / 180.0)
    : fabs(cam[0] - 90) * (M_PI / 180.0))
    * (float)((cam[0] < 90 || cam[0] >= 270) ? -1 : 1);
    side_y[0] = (float)((cam[0] >= 180) ? -1 : 1) / tanf((cam[0] >= 180)
    ? fabs(cam[0] - 270) * (M_PI / 180.0) : fabs(cam[0] - 90) * (M_PI / 180.0));
    while (1) {
        if (sqrt(powf(point_x[0] - player->x, 2) + powf(point_x[1] - player->y, 2))
        < sqrt(powf(point_y[0] - player->x, 2) + powf(point_y[1] - player->y, 2))) {
            if ((gc->map[(int)floorf(point_x[1])][(int)point_x[0] - 1] == 1)
            || gc->map[(int)floorf(point_x[1])][(int)point_x[0]] == 1) {
                place_wall(point_x, ray + 1, player, gc);
                break;
            }
            point_x[0] += side_x[0];
            point_x[1] += side_x[1];
        } else {
            if (gc->map[(int)point_y[1] - 1][(int)floorf(point_y[0])] == 1
            || gc->map[(int)point_y[1]][(int)floorf(point_y[0])] == 1) {
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
    sfRectangleShape_setFillColor(gc->rect, sfBlue);
    sfRectangleShape_setPosition(gc->rect, (sfVector2f){0, 0});
    sfRenderWindow_drawRectangleShape(gc->window, gc->rect, NULL);
    sfRectangleShape_setFillColor(gc->rect, sfGreen);
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
    if (gc->key[sfKeyZ]) {
        player->x += sinf(player->cam_x * (M_PI / 180.0)) * move_speed * gc->delay;
        player->y -= cosf(player->cam_x * (M_PI / 180.0)) * move_speed * gc->delay;
        if (gc->map[(int)floorf(player->y)][(int)floorf(player->x)] != 0) {
            player->x -= sinf(player->cam_x * (M_PI / 180.0)) * move_speed * gc->delay;
            player->y += cosf(player->cam_x * (M_PI / 180.0)) * move_speed * gc->delay;
        }
    }
    if (gc->key[sfKeyS]) {
        player->x -= sinf(player->cam_x * (M_PI / 180.0)) * move_speed * gc->delay;
        player->y += cosf(player->cam_x * (M_PI / 180.0)) * move_speed * gc->delay;
        if (gc->map[(int)floorf(player->y)][(int)floorf(player->x)] != 0) {
            player->x += sinf(player->cam_x * (M_PI / 180.0)) * move_speed * gc->delay;
            player->y -= cosf(player->cam_x * (M_PI / 180.0)) * move_speed * gc->delay;
        }
    }
    if (gc->key[sfKeyQ]) {
        player->x -= sinf((player->cam_x + 90) * (M_PI / 180.0)) * move_speed * gc->delay;
        player->y += cosf((player->cam_x + 90) * (M_PI / 180.0)) * move_speed * gc->delay;
        if (gc->map[(int)floorf(player->y)][(int)floorf(player->x)] != 0) {
            player->x += sinf((player->cam_x + 90) * (M_PI / 180.0)) * move_speed * gc->delay;
            player->y -= cosf((player->cam_x + 90) * (M_PI / 180.0)) * move_speed * gc->delay;
        }
    }
    if (gc->key[sfKeyD]) {
        player->x += sinf((player->cam_x + 90) * (M_PI / 180.0)) * move_speed * gc->delay;
        player->y -= cosf((player->cam_x + 90) * (M_PI / 180.0)) * move_speed * gc->delay;
        if (gc->map[(int)floorf(player->y)][(int)floorf(player->x)] != 0) {
            player->x -= sinf((player->cam_x + 90) * (M_PI / 180.0)) * move_speed * gc->delay;
            player->y += cosf((player->cam_x + 90) * (M_PI / 180.0)) * move_speed * gc->delay;
        }
    }
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

int main(void)
{
    player_t player = {2.3, 6.78, 0, 0};
    gamecore_t gc = {sfRenderWindow_create((sfVideoMode){800, 600, 32}, "Wolf3D", sfClose, NULL),
    {800, 600}, 0, 90, NULL, {10, 10}, {0}, {0}, sfRectangleShape_create(), sfClock_create()};

    gc.map = calloc(sizeof(char *), gc.map_size.y + 1);
    for (int i = 0; i < 10; i++) {
        gc.map[i] = calloc(sizeof(char), gc.map_size.x + 1);
        gc.map[i][0] = 1;
        gc.map[i][gc.map_size.x - 1] = 1;
        if (i == 0 || i == gc.map_size.y - 1)
            memset(gc.map[i] + 1, 1, gc.map_size.x - 2);
    }
    gc.map[4][4] = 1;
    sfRenderWindow_setMouseCursorVisible(gc.window, sfFalse);
    gc.mouse_pos = sfMouse_getPositionRenderWindow(gc.window);
    while (sfRenderWindow_isOpen(gc.window)) {
        gc.delay = sfClock_restart(gc.clock).microseconds / 1000000.0;
        analyze_event(&gc, &player);
        display(&gc, &player);
    }
    return 0;
}