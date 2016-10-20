//
// Created by ubuntu on 10/20/16.
//

#include "game.h"
#include "battleship.h"
#include "main.h"

BATTLESHIP* host_ships[10];
BATTLESHIP* client_ships[10];

void init_starter_battleships();

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
        client_ships[i]->owner = BATTLESHIP_OWNER_PLAYER;
    }

    for (int i = 0; i < 3; i++){
        client_ships[i] = init_battleship(BATTLESHIP_CLASS_5,
                                        (rand()%max_rand)+half_ship, DISPLAY_H - ship_height/2 );
        change_battleship_state(client_ships[i],BATTLESHIP_MOVE_STATE_IN_GAME);
        client_ships[i]->owner = BATTLESHIP_OWNER_OPPONENT;
    }


}

void on_redraw_game(){

}
