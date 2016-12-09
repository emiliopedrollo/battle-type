#ifndef BATTLE_TYPE_GAME_H
#define BATTLE_TYPE_GAME_H

#include "battleship.h"
#include "main.h"

#define NUMBER_OF_SHIPS_PER_PLAYER 10

ALLEGRO_BITMAP *rsc_explosion[16];

void init_game();
void on_redraw_game();
void load_resources_game();
void unload_resources_game();
bool is_single_player();
bool is_multiplayer_host();
bool is_multiplayer_client();
bool is_multiplayer();
bool is_game_paused();
void on_timer_game();
bool is_game_ending();
long get_last_game_score();
void on_mouse_move_game(int x, int y);
void on_mouse_up_game(int x, int y);
void on_mouse_down_game(int x, int y);

void on_key_press_game(ALLEGRO_KEYBOARD_EVENT event);
void process_key_press(int keycode, PLAYER player);

typedef struct {
    BATTLESHIP_CLASS class;
    BATTLESHIP_OWNER owner;
    bool active;
    bool exploding;
    bool exploding_with_lasers;
    char explosion_frame;
    float dx,dy;
    char word[30];
} SERIAL_BATTLESHIP;

typedef struct {
    SERIAL_BATTLESHIP host_ships[NUMBER_OF_SHIPS_PER_PLAYER];
    SERIAL_BATTLESHIP client_ships[NUMBER_OF_SHIPS_PER_PLAYER];
    long host_score, client_score;
    char host_target, client_target;
    unsigned short host_ship_dx, client_ship_dx;
    short player_lives, client_lives;
    char game_winner;
    bool is_game_ending;
} GAME_SNAPSHOT;

extern int game_bs_host_limit;
extern int game_bs_client_limit;
extern GAME_SNAPSHOT game;

extern BATTLESHIP *host_mothership;
extern BATTLESHIP *client_mothership;

static int const MINIMUM_WORD_POOL_SIZE = 75;
static int const MAXIMUM_WORD_POOL_SIZE = 125;

#endif //BATTLE_TYPE_GAME_H
