/*
** EPITECH PROJECT, 2023
** RayCasting
** File description:
** display.c
*/

#include "Wolf3D.h"

void display_pause(gamecore_t *gc, player_t *player)
{
    sfRenderWindow_clear(gc->window, sfBlack);
    sfRenderWindow_display(gc->window);
}