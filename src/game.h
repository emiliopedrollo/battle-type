//
// Created by ubuntu on 10/20/16.
//

#ifndef BATTLE_TYPE_GAME_H
#define BATTLE_TYPE_GAME_H

#include "battleship.h"
#include "main.h"

void init_game();
void on_redraw_game();
void load_resources_game();
void unload_resources_game();

typedef struct {
    BATTLESHIP_CLASS class;
    BATTLESHIP_OWNER owner;
    bool active;
    float dx,dy;
} SERIAL_BATTLESHIP;

typedef struct {
    SERIAL_BATTLESHIP host_ships[10];
    SERIAL_BATTLESHIP client_ships[10];
    int host_cash, client_cash;
} GAME_SNAPSHOT;

extern int game_bs_host_limit;
extern int game_bs_client_limit;
extern GAME_SNAPSHOT game;

#endif //BATTLE_TYPE_GAME_H
