/*
** EPITECH PROJECT, 2023
** RayCasting
** File description:
** analyze_event.c
*/

#include "Wolf3D.h"

static void check_key(gamecore_t *gc, player_t *player)
{
    if (gc->key[sfKeySpace]) {
        gc->state = GAME;
        gc->diff_mouse_pos = (sfVector2i){0, 0};
        sfRenderWindow_setMouseCursorVisible(gc->window, sfFalse);
    }
}

void analyze_event_pause(gamecore_t *gc, player_t *player)
{
    check_key(gc, player);
}