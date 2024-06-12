/*
** EPITECH PROJECT, 2023
** RayCasting
** File description:
** analyze_event.c
*/

#include "Wolf3D.h"
#define movex (fabsf(player->x + move.x - point_x[0]) < wall_distance \
              && ((in_map((int)(point_x[0] - 1), (int)floorf(point_x[1]), gc) && gc->map[(int)floorf(point_x[1])][(int)point_x[0] - 1]) \
              || (in_map((int)(point_x[0]), (int)floorf(point_x[1]), gc) && gc->map[(int)floorf(point_x[1])][(int)point_x[0]]))) \
              ? point_x[0] + wall_distance - wall_distance * 2 * (player->x - point_x[0] < 0) : player->x + move.x
#define movey (fabsf(player->y + move.y - point_y[1]) < wall_distance \
              && ((in_map((int)floorf(point_y[0]), (int)point_y[1] - 1, gc) && gc->map[(int)point_y[1] - 1][(int)floorf(point_y[0])]) \
              || (in_map((int)floorf(point_y[0]), (int)point_y[1], gc) && gc->map[(int)point_y[1]][(int)floorf(point_y[0])]))) \
              ? point_y[1] + wall_distance - wall_distance * 2 * (player->y - point_y[1] < 0) : player->y + move.y

static void can_move(gamecore_t *gc, player_t *player, sfVector2f move, float cam)
{
    float side_x[2] = {(float)((cam >= 180) ? -1 : 1), 0};
    float side_y[2] = {0, (float)((cam < 90 || cam >= 270) ? -1 : 1)};
    float point_x[2] = {0};
    float point_y[2] = {0};
    float dist = sqrtf(powf((player->x + move.x) - player->x, 2) + powf((player->y + move.y) - player->y, 2));
    
    point_x[0] = (cam >= 180) ? floorf(player->x) : floorf(player->x) + 1;
    point_x[1] = tanf((cam >= 180) ? fabsf(cam - 270) * (M_PI / 180.0)
    : fabsf(cam - 90) * (M_PI / 180.0)) * fabsf(point_x[0] - player->x)
    * (float)((cam < 90 || cam >= 270) ? -1 : 1) + player->y;
    point_y[1] = (cam < 90 || cam >= 270) ? floorf(player->y) : floorf(player->y) + 1;
    point_y[0] = fabsf(point_y[1] - player->y) / tanf((cam >= 180)
    ? fabsf(cam - 270) * (M_PI / 180.0)
    : fabsf(cam - 90) * (M_PI / 180.0)) * (float)((cam >= 180) ? -1 : 1) + player->x;
    side_x[1] = tanf((cam >= 180) ? fabsf(cam - 270) * (M_PI / 180.0)
    : fabsf(cam - 90) * (M_PI / 180.0))
    * (float)((cam < 90 || cam >= 270) ? -1 : 1);
    side_y[0] = (float)((cam >= 180) ? -1 : 1) / tanf((cam >= 180)
    ? fabsf(cam - 270) * (M_PI / 180.0) : fabsf(cam - 90) * (M_PI / 180.0));
    while (1) {
        if (dist < sqrtf(powf(point_x[0] - player->x, 2) + powf(point_x[1] - player->y, 2))
        && dist < sqrtf(powf(point_y[0] - player->x, 2) + powf(point_y[1] - player->y, 2))) {
            if ((in_map((int)floorf(movex), (int)floorf(movey), gc)
            && gc->map[(int)floorf(movey)][(int)floorf(movex)]) == 0) {
                player->x = movex;
                player->y = movey;
            }
            break;
        }
        if (!in_map((int)floorf(point_x[0] - 1), (int)point_x[1], gc) && !in_map((int)floorf(point_x[0]), (int)point_x[1], gc)
        && !in_map((int)floorf(point_y[0]), (int)point_y[1] - 1, gc) && !in_map((int)floorf(point_y[0]), (int)point_y[1], gc)) {
            break;
        }
        if (sqrtf(powf(point_x[0] - player->x, 2) + powf(point_x[1] - player->y, 2))
        < sqrtf(powf(point_y[0] - player->x, 2) + powf(point_y[1] - player->y, 2))) {
            if ((in_map((int)(point_x[0] - 1), (int)floorf(point_x[1]), gc) && gc->map[(int)floorf(point_x[1])][(int)point_x[0] - 1])
            || (in_map((int)(point_x[0]), (int)floorf(point_x[1]), gc) && gc->map[(int)floorf(point_x[1])][(int)point_x[0]])) {
                player->x = (point_x[0] <= player->x) ? point_x[0] + (float)wall_distance: point_x[0] - (float)wall_distance;
                player->y = point_x[1];
                break;
            }
            point_x[0] += side_x[0];
            point_x[1] += side_x[1];
        } else {
            if ((in_map((int)floorf(point_y[0]), (int)point_y[1] - 1, gc) && gc->map[(int)point_y[1] - 1][(int)floorf(point_y[0])])
            || (in_map((int)floorf(point_y[0]), (int)point_y[1], gc) && gc->map[(int)point_y[1]][(int)floorf(point_y[0])])) {
                player->y = (point_y[1] <= player->y) ? point_y[1] + (float)wall_distance : point_y[1] - (float)wall_distance;
                player->x = point_y[0];
                break;
            }
            point_y[0] += side_y[0];
            point_y[1] += side_y[1];
        }
    }
}

static void check_key(gamecore_t *gc, player_t *player)
{
    float sin_h = sinf(player->cam_x * (M_PI / 180.0)) * move_speed * gc->delay;
    float sin_v = sinf((player->cam_x + 90) * (M_PI / 180.0)) * move_speed * gc->delay;
    float cos_h = cosf(player->cam_x * (M_PI / 180.0)) * move_speed * gc->delay;
    float cos_v = cosf((player->cam_x + 90) * (M_PI / 180.0)) * move_speed * gc->delay;
    sfVector2f move = {0, 0};
    float cam = player->cam_x;
    
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
    if (move.x || move.y) {
        if ((gc->key[sfKeyQ] || gc->key[sfKeyLeft]) != (gc->key[sfKeyD] || gc->key[sfKeyRight])) {
            cam += 90 - (gc->key[sfKeyQ] || gc->key[sfKeyLeft]) * 180;
            cam += ((gc->key[sfKeyQ] || gc->key[sfKeyLeft]) && (gc->key[sfKeyS] || gc->key[sfKeyDown])) ? -45 : 0;
            cam += ((gc->key[sfKeyD] || gc->key[sfKeyRight]) && (gc->key[sfKeyS] || gc->key[sfKeyDown])) ? 45 : 0;
            cam += ((gc->key[sfKeyQ] || gc->key[sfKeyLeft]) && (gc->key[sfKeyZ] || gc->key[sfKeyUp])) ? 45 : 0;
            cam += ((gc->key[sfKeyD] || gc->key[sfKeyRight]) && (gc->key[sfKeyZ] || gc->key[sfKeyUp])) ? -45 : 0;
        } else
            cam += (gc->key[sfKeyS] || gc->key[sfKeyDown]) * 180;
        cam = fmodf(cam, 360);
        for (; cam < 0; cam += 360);
        can_move(gc, player, move, cam);
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

void analyze_event_game(gamecore_t *gc, player_t *player)
{
    check_cam(gc, player);
    check_key(gc, player);
}