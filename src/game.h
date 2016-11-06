//
// Created by ubuntu on 10/20/16.
//

#ifndef BATTLE_TYPE_GAME_H
#define BATTLE_TYPE_GAME_H

#include "battleship.h"
#include "main.h"

#define NUMBER_OF_SHIPS_PER_PLAYER 10

void init_game();
void on_redraw_game();
void load_resources_game();
void unload_resources_game();

typedef struct {
    BATTLESHIP_CLASS class;
    BATTLESHIP_OWNER owner;
    bool active;
    float dx,dy;
    char* word;
} SERIAL_BATTLESHIP;

typedef struct {
    SERIAL_BATTLESHIP host_ships[NUMBER_OF_SHIPS_PER_PLAYER];
    SERIAL_BATTLESHIP client_ships[NUMBER_OF_SHIPS_PER_PLAYER];
    int host_cash, client_cash;
} GAME_SNAPSHOT;

extern int game_bs_host_limit;
extern int game_bs_client_limit;
extern GAME_SNAPSHOT game;

static int const MINIMUM_WORD_POOL_SIZE = 75;
static int const MAXIMUM_WORD_POOL_SIZE = 125;

#endif //BATTLE_TYPE_GAME_H
