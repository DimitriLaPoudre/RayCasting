/*
** EPITECH PROJECT, 2023
** RayCasting
** File description:
** header.h
*/

#ifndef WOLF_3D_
#define WOLF_3D_
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
    #define wall_distance (float)0.03
    #define FOV 60
    #define render_dist 0
    #define mouse_sensibility (sfVector2f){(float)3 / (float)10, (float)3 / (float)10}
    #define in_map(px, py, gc) (px >= 0 && px < gc->map_size.x && py >= 0 && py < gc->map_size.y)

typedef struct player_s {
    float x;
    float y;
    float cam_x;
    float cam_y;
} player_t;

enum scene_e {
    GAME,
    PAUSE,
    SCENE_COUNT,
};

typedef struct {
    sfTexture *texture;
    sfSprite *sprite;
} sprite_t;

typedef struct gamecore_s {
    sfRenderWindow *window;
    sfVector2f window_size;
    sfEvent event;
    float fov;
    short **map;
    sfVector2u map_size;
    sfVector2i mouse_pos;
    sfVector2i diff_mouse_pos;
    sfRectangleShape *rect;
    sfClock *clock;
    float delay;
    char key[sfKeyCount];
    sfColor type[6];
    sprite_t wall;
    char state;
    char focus;
    short fps;
    short ups;
    float render_distance;
    sfColor *framebuffer;
    sprite_t floor;
} gamecore_t;

void analyze_event_game(gamecore_t *gc, player_t *player);
void analyze_event_pause(gamecore_t *gc, player_t *player);

void update_game(gamecore_t *gc, player_t *player);
void update_pause(gamecore_t *gc, player_t *player);

void display_game(gamecore_t *gc, player_t *player);
void display_pause(gamecore_t *gc, player_t *player);

#endif