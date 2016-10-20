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
    BATTLESHIP_MOVE_STATE_IN_GAME,
} BATTLESHIP_MOVE_STATE;

typedef enum {
    BATTLESHIP_CLASS_1,
    BATTLESHIP_CLASS_2,
    BATTLESHIP_CLASS_3,
    BATTLESHIP_CLASS_4,
    BATTLESHIP_CLASS_5,
} BATTLESHIP_CLASS;

typedef enum {
    TURNING_DIRECTION_NONE,
    TURNING_DIRECTION_LEFT,
    TURNING_DIRECTION_RIGHT
} TURNING_DIRECTION;

typedef struct {
    BATTLESHIP_MOVE_STATE state;
    TURNING_DIRECTION turning_direction;
    BATTLESHIP_CLASS class;
    ALLEGRO_BITMAP *bmp;
    int turning_frame;
    float dx,dy;
    float vx,vy;
    float vxi,vyi;
    bool push_back_ended;
    bool push_back_done;
    bool push_back_set_speed;
    int push_back_frame;
    int push_back_k;
    void (*push_back_callback)(void);
} BATTLESHIP;

BATTLESHIP* init_battleship(BATTLESHIP_CLASS class, int dx, int dy);
void change_battleship_state(BATTLESHIP *battleship,BATTLESHIP_MOVE_STATE state);
void draw_ship(BATTLESHIP *battleship);
void load_resources_battleship();
void unload_resources_battleship();
int get_battleship_height(BATTLESHIP_CLASS class);
int get_battleship_width(BATTLESHIP_CLASS class);


#endif //BATTLE_TYPE_BATTLESHIP_H
