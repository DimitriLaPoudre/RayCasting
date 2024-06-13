/*
** EPITECH PROJECT, 2023
** RayCasting
** File description:
** display.c
*/

#include "Wolf3D.h"

static float place_wall(float point[2], int ray, player_t *player, gamecore_t *gc)
{
    float dist = sqrtf(powf(point[0] - player->x, 2) + powf(point[1] - player->y, 2));
    short type = 0;

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
    type = (ray > 0) ? type >> 8 & 0xFF : type & 0xFF;
    sfSprite_setTextureRect(gc->wall.sprite, (sfIntRect){(float)((type - 1) % 6) * (float)64 + (float)64 * ((ray > 0) ? point[1] - floorf(point[1]) : point[0] - floorf(point[0])), ((type - 1) / 6) * 64, 1, 64});
    ray = fabsf(ray) - 1;
    sfSprite_setScale(gc->wall.sprite, (sfVector2f){1, (gc->window_size.y / dist) / (float)64});
    sfSprite_setPosition(gc->wall.sprite, (sfVector2f){ray, -player->cam_y * (gc->window_size.y / (float)90) + (gc->window_size.y - (gc->window_size.y / dist)) / (float)2});
    sfSprite_setColor(gc->wall.sprite, sfWhite);
    if (gc->render_distance > 0)
        sfSprite_setColor(gc->wall.sprite, (sfColor){255, 255, 255,
        (gc->render_distance / sqrtf(powf(point[0] - player->x, 2) + powf(point[1] - player->y, 2)) > 255)
        ? 255 : gc->render_distance / sqrtf(powf(point[0] - player->x, 2) + powf(point[1] - player->y, 2))});
    sfRenderWindow_drawSprite(gc->window, gc->wall.sprite, NULL);
    return dist;
}

static void display_ray(float cam[2], int ray, player_t *player, gamecore_t *gc)
{
    float side_x[2] = {(float)((cam[0] >= 180) ? -1 : 1), 0};
    float side_y[2] = {0, (float)((cam[0] < 90 || cam[0] >= 270) ? -1 : 1)};
    float point_x[2] = {0};
    float point_y[2] = {0};
    float dist = 0;
    
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
                dist = place_wall(point_x, ray + 1, player, gc);
                break;
            }
            point_x[0] += side_x[0];
            point_x[1] += side_x[1];
        } else {
            if ((in_map((int)floorf(point_y[0]), (int)point_y[1] - 1, gc) && gc->map[(int)point_y[1] - 1][(int)floorf(point_y[0])])
            || (in_map((int)floorf(point_y[0]), (int)point_y[1], gc) && gc->map[(int)point_y[1]][(int)floorf(point_y[0])])) {
                dist = place_wall(point_y, -(ray + 1), player, gc);
                break;
            }
            point_y[0] += side_y[0];
            point_y[1] += side_y[1];
        }
    }
    for (float i = -player->cam_y * (gc->window_size.y / (float)90) + (gc->window_size.y - (gc->window_size.y / dist)) / (float)2
    ; i > 0; i--)
        if (i < gc->window_size.y) {
            gc->framebuffer[ray + (int)floorf(i) * (int)gc->window_size.x] = (sfColor){50, 50, 50, 255};
        }
    for (float i = -player->cam_y * (gc->window_size.y / (float)90) + (gc->window_size.y - (gc->window_size.y / dist)) / (float)2
    + (gc->window_size.y / dist); i < gc->window_size.y; i++)
        if (i >= 0) {
            gc->framebuffer[ray + (int)floorf(i) * (int)gc->window_size.x] = (sfColor){100, 100, 100, 255};
        }
}

void display_game(gamecore_t *gc, player_t *player)
{
    sfRenderWindow_clear(gc->window, sfBlack);
    for (float i = 0; i < gc->fov; i += gc->fov / gc->window_size.x) {
        display_ray((float [2]){fmod(player->cam_x - gc->fov / 2 + i + 360, 360), player->cam_y},
        roundf(i * (gc->window_size.x / gc->fov)), player, gc);
    }
    sfTexture_updateFromPixels(gc->floor.texture, (const sfUint8 *)gc->framebuffer, gc->window_size.x, gc->window_size.y, 0, 0);
    sfRenderWindow_drawSprite(gc->window, gc->floor.sprite, NULL);
    memset(gc->framebuffer, 0, 4 * gc->window_size.x * gc->window_size.y);
    sfRenderWindow_display(gc->window);
}