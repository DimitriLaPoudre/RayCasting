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
#include <SFML/Window.h>

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
    sfRectangleShape *rect;
} gamecore_t;

void display_ray(float cam[2], int ray, float pos[2], gamecore_t *gc)
{
    float side_x[2] = {(float)((cam[0] >= 180) ? -1 : 1), 0};
    float side_y[2] = {0, (float)((cam[0] < 90 || cam[0] >= 270) ? -1 : 1)};
    float point_x[2] = {0};
    float point_y[2] = {0};
    
    point_x[0] = (cam[0] >= 180) ? (int)pos[0] : (int)pos[0] + 1;
    point_x[1] = tanf((cam[0] >= 180) ? fabs(cam[0] - 270) * (M_PI / 180.0)
    : fabs(cam[0] - 90) * (M_PI / 180.0)) * fabs(point_x[0] - pos[0])
    * (float)((cam[0] < 90 || cam[0] >= 270) ? -1 : 1) + pos[1];
    point_y[1] = (cam[0] < 90 || cam[0] >= 270) ? (int)pos[1] : (int)pos[1] + 1;
    point_y[0] = fabs(point_y[1] - pos[1]) / tanf((cam[0] >= 180)
    ? fabs(cam[0] - 270) * (M_PI / 180.0)
    : fabs(cam[0] - 90) * (M_PI / 180.0)) * (float)((cam[0] >= 180) ? -1 : 1) + pos[0];
    side_x[1] = tanf((cam[0] >= 180) ? fabs(cam[0] - 270) * (M_PI / 180.0)
    : fabs(cam[0] - 90) * (M_PI / 180.0))
    * (float)((cam[0] < 90 || cam[0] >= 270) ? -1 : 1);
    side_y[0] = (float)((cam[0] >= 180) ? -1 : 1) / tanf((cam[0] >= 180)
    ? fabs(cam[0] - 270) * (M_PI / 180.0) : fabs(cam[0] - 90) * (M_PI / 180.0));
    while (1) {
        if (sqrt(powf(point_x[0] - pos[0], 2) + powf(point_x[1] - pos[1], 2))
        < sqrt(powf(point_y[0] - pos[0], 2) + powf(point_y[1] - pos[1], 2))) {
            if ((gc->map[(int)floor(point_x[1])][(int)point_x[0] - 1] == 1)
            || gc->map[(int)floor(point_x[1])][(int)point_x[0]] == 1)
                break;
            point_x[0] += side_x[0];
            point_x[1] += side_x[1];
        } else {
            if (gc->map[(int)point_y[1] - 1][(int)floor(point_y[0])] == 1
            || gc->map[(int)point_y[1]][(int)floor(point_y[0])] == 1)
                break;
            point_y[0] += side_y[0];
            point_y[1] += side_y[1];
        }
    }
}

void display(gamecore_t *gc, player_t *player)
{
    sfRenderWindow_clear(gc->window, sfBlack);
    sfRectangleShape_setSize(gc->rect, (sfVector2f){gc->window_size.x, gc->window_size.y});
    sfRectangleShape_setFillColor(gc->rect, sfBlue);
    sfRectangleShape_setPosition(gc->rect, (sfVector2f){0, 0});
    sfRenderWindow_drawRectangleShape(gc->window, gc->rect, NULL);
    sfRectangleShape_setFillColor(gc->rect, sfGreen);
    sfRectangleShape_setPosition(gc->rect, (sfVector2f){0, gc->window_size.y / (float)2 + player->cam_y / gc->window_size.y});
    sfRenderWindow_drawRectangleShape(gc->window, gc->rect, NULL);
    for (float i = 0; i < gc->fov; i += gc->fov / gc->window_size.x)
        display_ray((float [2]){fmod(player->cam_x - gc->fov / 2 + i + 360, 360), player->cam_y},
        i * gc->window_size.x, (float [2]){player->x, player->y}, gc);
    sfRenderWindow_display(gc->window);
}

void analyze_event(gamecore_t *gc, player_t *player, sfEvent event)
{
    switch (event.type) {
        case sfEvtClosed:
            sfRenderWindow_close(gc->window);
            break;
    }
}

int main(void)
{
    player_t player = {2.3, 6.78, 0, 0};
    gamecore_t gc = {sfRenderWindow_create((sfVideoMode){800, 600, 32}, "Wolf3D", sfClose | sfResize, NULL),
    (sfVector2f){800, 600}, 0, 90, NULL, sfRectangleShape_create()};

    gc.map = calloc(sizeof(char *), 10 + 1);
    for (int i = 0; i < 10; i++) {
        gc.map[i] = calloc(sizeof(char), (10 + 1));
        gc.map[i][0] = 1;
        gc.map[i][10 - 1] = 1;
        if (i == 0 || i == 10 - 1)
            memset(gc.map[i] + 1, 1, 10 - 2);
    }
    while (sfRenderWindow_isOpen(gc.window)) {
        while(sfRenderWindow_pollEvent(gc.window, &gc.event))
            analyze_event(&gc, &player, gc.event);
        display(&gc, &player);
    }
    return 0;
}