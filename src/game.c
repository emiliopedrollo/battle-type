//
// Created by ubuntu on 10/20/16.
//

#include <math.h>
#include "game.h"

int MAX_SHIPS_FOR_PLAYERS = 10;
BATTLESHIP* host_ships[10];
BATTLESHIP* client_ships[10];

GAME_SNAPSHOT game;

void init_starter_battleships();
void move_game_ships();
void draw_game_ships();
void update_battleship(BATTLESHIP *battleship, SERIAL_BATTLESHIP serial_battleship);

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
        host_ships[i]->limit = 800;//DISPLAY_H/2 + 10;


        client_ships[i] = init_battleship(BATTLESHIP_CLASS_5,
                                          (rand()%max_rand)+half_ship,  -ship_height/2 );
        change_battleship_state(client_ships[i],BATTLESHIP_MOVE_STATE_IN_GAME);
        client_ships[i]->owner = BATTLESHIP_OWNER_OPPONENT;
        client_ships[i]->limit = 800;//DISPLAY_H/2 - 10;
    }


}

void move_game_ships(){
    for (int i =0; i < MAX_SHIPS_FOR_PLAYERS; i++){
        if (host_ships[i] && host_ships[i]->active) move_ship(host_ships[i]);
        if (client_ships[i] && client_ships[i]->active) move_ship(client_ships[i]);

        //int bsh = get_battleship_height(client_ships[j]);

        for (int i =0; i < MAX_SHIPS_FOR_PLAYERS; i++){
            for(int j =0; j < MAX_SHIPS_FOR_PLAYERS; j++){

                host_ships[i]->limit =
                        (fabs(host_ships[i]->dy - (client_ships[j]->dy + get_battleship_height(client_ships[j]->class))) < host_ships[i]->limit)
                        ?fabs(host_ships[i]->dy - (client_ships[j]->dy + get_battleship_height(client_ships[j]->class))):host_ships[i]->limit;
                client_ships[j]->limit =
                        (fabs(host_ships[i]->dy - (client_ships[j]->dy + get_battleship_height(client_ships[j]->class))) < client_ships[j]->limit)
                        ?fabs(host_ships[i]->dy - (client_ships[j]->dy + get_battleship_height(client_ships[j]->class))):client_ships[j]->limit;
            }
        }
    }
}

void draw_game_ships(){
    for (int i =0; i < MAX_SHIPS_FOR_PLAYERS; i++){
        if (host_ships[i] && host_ships[i]->active) draw_ship(host_ships[i]);
        if (client_ships[i] && client_ships[i]->active) draw_ship(client_ships[i]);
    }
}

void update_game_ships(){
    for (int i =0; i < MAX_SHIPS_FOR_PLAYERS; i++){
        update_battleship(host_ships[i],game.host_ships[i]);
        update_battleship(client_ships[i],game.client_ships[i]);
    }
}

void update_battleship(BATTLESHIP *battleship, SERIAL_BATTLESHIP serial_battleship){
    if (serial_battleship.active){
        if (!battleship) battleship = init_battleship(serial_battleship.class, 0,0);

        battleship->dx     = serial_battleship.dx;
        battleship->dy     = serial_battleship.dy;
        battleship->owner  = serial_battleship.owner;
        battleship->active = serial_battleship.active;

    } else {
        if (battleship) battleship->active = false;
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
    for (int i =0; i < MAX_SHIPS_FOR_PLAYERS; i++){
        if (host_ships[i] && host_ships[i]->active){
            game.host_ships[i] = convert_battleship_to_serial(host_ships[i]);
        } else game.host_ships[i].active = false;

        if (client_ships[i] && client_ships[i]->active){
            game.client_ships[i] = convert_battleship_to_serial(client_ships[i]);
        } else game.client_ships[i].active = false;
    }
}

void on_redraw_game(){
    if (current_game_state == GAME_STATE_IN_GAME_MULTIPLAYER_HOST ||
            current_game_state == GAME_STATE_IN_GAME_SINGLE_PLAYER){
        move_game_ships();
        update_game_snapshot();
    } else if (current_game_state == GAME_STATE_IN_GAME_MULTIPLAYER_CLIENT){
        update_game_ships();
    }
    draw_game_ships();
}
