/*
** EPITECH PROJECT, 2023
** RayCasting
** File description:
** analyze_event.c
*/

#include "Wolf3D.h"

static void check_key(gamecore_t *gc, player_t *player)
{
    float sin_h = sinf(player->cam_x * (M_PI / 180.0)) * move_speed * gc->delay;
    float sin_v = sinf((player->cam_x + 90) * (M_PI / 180.0)) * move_speed * gc->delay;
    float cos_h = cosf(player->cam_x * (M_PI / 180.0)) * move_speed * gc->delay;
    float cos_v = cosf((player->cam_x + 90) * (M_PI / 180.0)) * move_speed * gc->delay;
    sfVector2f move = {0, 0};
    
    if (gc->key[sfKeyEscape] || !gc->focus) {
        gc->state = PAUSE;
        sfRenderWindow_setMouseCursorVisible(gc->window, sfTrue);
    }
    if (gc->key[sfKeyZ] || gc->key[sfKeyUp]) {
        move.x += sin_h;
        move.y -= cos_h;
    }
    if (gc->key[sfKeyS] || gc->key[sfKeyDown]) {
        move.x -= sin_h;
        move.y += cos_h;
    }
    if (gc->key[sfKeyQ] || gc->key[sfKeyLeft]) {
        move.x -= sin_v;
        move.y += cos_v;
    }
    if (gc->key[sfKeyD] || gc->key[sfKeyRight]) {
        move.x += sin_v;
        move.y -= cos_v;
    }
    if (in_map((int)floorf(player->x + move.x), (int)floorf(player->y + move.y), gc) && gc->map[(int)floorf(player->y + move.y)][(int)floorf(player->x + move.x)] == 0) {
        player->x += move.x;
        player->y += move.y;
    }
    // printf("%f et %f et %f\n", player->x, player->y, player->cam_x);
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

void analyze_event_game(gamecore_t *gc, player_t *player)
{
    check_key(gc, player);
    check_cam(gc, player);
}