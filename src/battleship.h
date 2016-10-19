//
// Created by ubuntu on 10/19/16.
//

#ifndef BATTLE_TYPE_BATTLESHIP_H
#define BATTLE_TYPE_BATTLESHIP_H

#include <allegro5/bitmap.h>

typedef enum {
    BATTLESHIP_MOVE_STATE_INITAL_STATE,
    BATTLESHIP_MOVE_STATE_DEMO,
    BATTLESHIP_MOVE_STATE_DEMO_PUSHBACK,
} BATTLESHIP_MOVE_STATE;

typedef enum {
    TURNING_DIRECTION_NONE,
    TURNING_DIRECTION_LEFT,
    TURNING_DIRECTION_RIGHT
} TURNING_DIRECTION;

typedef struct {
    BATTLESHIP_MOVE_STATE state;
    TURNING_DIRECTION turning_direction;
    ALLEGRO_BITMAP *bmp;
    int turning_frame;
    float dx,dy;
    float vx,vy;
    bool push_back_done;
    bool push_back_set_speed;
    int push_back_frame;
    int push_back_k;
} BATTLESHIP;

BATTLESHIP* init_battleship(ALLEGRO_BITMAP *bmp, int dx, int dy, int vx, int vy);
void change_battleship_state(BATTLESHIP *battleship,BATTLESHIP_MOVE_STATE state);
void draw_ship(BATTLESHIP *battleship);


#endif //BATTLE_TYPE_BATTLESHIP_H
