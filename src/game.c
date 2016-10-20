//
// Created by ubuntu on 10/20/16.
//

#include "game.h"

int MAX_SHIPS_FOR_PLAYERS = 10;
BATTLESHIP* host_ships[10];
BATTLESHIP* client_ships[10];

GAME_SNAPSHOT game;

void init_starter_battleships();
void draw_game_ships();

void load_resources_game(){

}

void unload_resources_game(){

}

void init_game(){
    init_starter_battleships();
}

void init_starter_battleships(){

    int max_rand = DISPLAY_W - get_battleship_width(BATTLESHIP_CLASS_5);
    int half_ship = get_battleship_width(BATTLESHIP_CLASS_5)/2;

    int ship_height = get_battleship_height(BATTLESHIP_CLASS_5);

    for (int i = 0; i < 3; i++){
        host_ships[i] = init_battleship(BATTLESHIP_CLASS_5,
                                        (rand()%max_rand)+half_ship, DISPLAY_H + ship_height/2 );
        change_battleship_state(host_ships[i],BATTLESHIP_MOVE_STATE_IN_GAME);
        host_ships[i]->owner = BATTLESHIP_OWNER_PLAYER;


        client_ships[i] = init_battleship(BATTLESHIP_CLASS_5,
                                          (rand()%max_rand)+half_ship,  -ship_height/2 );
        change_battleship_state(client_ships[i],BATTLESHIP_MOVE_STATE_IN_GAME);
        client_ships[i]->owner = BATTLESHIP_OWNER_OPPONENT;
    }


}

void draw_game_ships(){
    for (int i =0; i < MAX_SHIPS_FOR_PLAYERS; i++){
        if (host_ships[i] && host_ships[i]->active) draw_ship(host_ships[i]);
        if (client_ships[i] && client_ships[i]->active) draw_ship(client_ships[i]);
    }
}

SERIAL_BATTLESHIP convert_battleship_to_serial(BATTLESHIP *battleship){
    SERIAL_BATTLESHIP serial;
    serial.owner  = battleship->owner;
    serial.active = battleship->active;
    serial.class  = battleship->class;
    serial.dx     = battleship->dx;
    serial.dy     = battleship->dy;
    return serial;
}

void update_game_snapshot(){

}

void on_redraw_game(){
    if (current_game_state == GAME_STATE_IN_GAME_MULTIPLAYER_HOST ||
            current_game_state == GAME_STATE_IN_GAME_SINGLE_PLAYER){
        update_game_snapshot();
    }
    draw_game_ships();
}
