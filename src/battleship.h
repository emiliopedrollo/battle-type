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
    BATTLESHIP_OWNER_NONE,
    BATTLESHIP_OWNER_PLAYER,
    BATTLESHIP_OWNER_OPPONENT,
    BATTLESHIP_OWNER_SPECIAL,
} BATTLESHIP_OWNER;

typedef enum {
    PLAYER_SINGLE,
    PLAYER_HOST,
    PLAYER_CLIENT
} PLAYER;

typedef enum {
    BATTLESHIP_CLASS_MISSILE,
    BATTLESHIP_CLASS_SPACESHIP,
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
    BATTLESHIP_OWNER owner;
    bool active;
    int turning_frame;
    float dx,dy;
    float dxi,dyi;
    float vx,vy;
    float vxi,vyi;
    float mr,ml;
    float lr,ll;
    float limit;
    bool push_back_ended;
    bool push_back_done;
    bool push_back_set_speed;
    int push_back_frame;
    int push_back_k;
    void (*push_back_callback)(void);
    char *word;
    char explosion_frames;
    bool exploding;
    void (*on_explosion_end)(BATTLESHIP_OWNER *owner);
} BATTLESHIP;

BATTLESHIP* init_battleship(BATTLESHIP_CLASS class, BATTLESHIP_OWNER owner, float dx, float dy, float x, int game_level);
void change_battleship_state(BATTLESHIP *battleship,BATTLESHIP_MOVE_STATE state);
void draw_target_lock(BATTLESHIP *battleship);
bool move_ship(BATTLESHIP *battleship, float target_dx);
void draw_ship(BATTLESHIP *battleship, float target_dx);
void draw_ship_word(BATTLESHIP *battleship,bool is_target);
void load_resources_battleship();
void unload_resources_battleship();
int get_battleship_height(BATTLESHIP_CLASS class);
int get_battleship_width(BATTLESHIP_CLASS class);
unsigned short remove_next_letter_from_battleship(BATTLESHIP *battleship);
char get_next_letter_from_battleship(BATTLESHIP *battleship);
float get_left_dx(BATTLESHIP *battleship);
float get_top_dy(BATTLESHIP *battleship);
float get_righ_dx(BATTLESHIP *battleship);
float get_bottom_dy(BATTLESHIP *battleship);


#endif //BATTLE_TYPE_BATTLESHIP_H
