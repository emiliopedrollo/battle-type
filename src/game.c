//
// Created by ubuntu on 10/20/16.
//

#include <math.h>
#include "game.h"
#include "resources/dictionary.h"
#include "battleship.h"

BATTLESHIP* host_ships[NUMBER_OF_SHIPS_PER_PLAYER];
BATTLESHIP* client_ships[NUMBER_OF_SHIPS_PER_PLAYER];

GAME_SNAPSHOT game;

int game_bs_host_limit;
int game_bs_client_limit;

int frame_count;

int word_pool_start_pos;
int word_pool_end_pos;

void init_starter_battleships();
void move_game_ships();
void draw_game_ships();
void update_battleship(BATTLESHIP *battleship, SERIAL_BATTLESHIP serial_battleship);
char* get_word_from_pool(BATTLESHIP_OWNER owner);
void update_word_pool(bool pump_word_index);

void load_resources_game(){

}

void unload_resources_game(){

}

void init_game(){
    frame_count = 0;
    init_starter_battleships();
}

void init_starter_battleships(){
    update_word_pool(false);

    int max_rand = DISPLAY_W - get_battleship_width(BATTLESHIP_CLASS_5);
    int half_ship = get_battleship_width(BATTLESHIP_CLASS_5)/2;

    int ship_height = get_battleship_height(BATTLESHIP_CLASS_5);

    for (int i = 0; i < 4; i++){
        host_ships[i] = (i==3)? init_battleship(BATTLESHIP_CLASS_M,
                                        DISPLAY_W/2, DISPLAY_H - ship_height ):
                                init_battleship(BATTLESHIP_CLASS_5,
                                        (rand()%max_rand)+half_ship, DISPLAY_H + ship_height/2 );
        change_battleship_state(host_ships[i],BATTLESHIP_MOVE_STATE_IN_GAME);
        host_ships[i]->owner = BATTLESHIP_OWNER_PLAYER;
        host_ships[i]->limit = 800;//DISPLAY_H/2 + 10;
        host_ships[i]->word = get_word_from_pool(BATTLESHIP_OWNER_PLAYER);



        client_ships[i] = (i==3)? init_battleship(BATTLESHIP_CLASS_M,
                                                  DISPLAY_W/2, ship_height):
                                  init_battleship(BATTLESHIP_CLASS_5,
                                                  (rand()%max_rand)+half_ship,  -ship_height/2 );
        change_battleship_state(client_ships[i],BATTLESHIP_MOVE_STATE_IN_GAME);
        client_ships[i]->owner = BATTLESHIP_OWNER_OPPONENT;
        client_ships[i]->limit = 800;
        client_ships[i]->word = get_word_from_pool(BATTLESHIP_OWNER_OPPONENT);
    }


}

void move_game_ships(){

    game_bs_host_limit = 0;
    game_bs_client_limit = DISPLAY_H;

    int ship_bound;

    // Encontra limite para movimento dos battleships do host
    /*for (int i=0;i< MAX_SHIPS_FOR_PLAYERS;i++) {
        if (!client_ships[i]) continue;
        ship_bound = (int) client_ships[i]->dy + get_battleship_height(client_ships[i]->class) / 2;
        game_bs_host_limit = (ship_bound > game_bs_host_limit) ? ship_bound : game_bs_host_limit;
    }*/

    //Move os battleships do host
    for (int i =0; i < NUMBER_OF_SHIPS_PER_PLAYER; i++) {
        if (host_ships[i] && host_ships[i]->active) move_ship(host_ships[i]);
    }

    // Encontra limite para movimento dos battleships do client
    /*for (int i=0;i< MAX_SHIPS_FOR_PLAYERS;i++){
        if (!host_ships[i]) continue;
        ship_bound = (int)host_ships[i]->dy - get_battleship_height(host_ships[i]->class)/2;
        game_bs_client_limit = ( ship_bound < game_bs_client_limit ) ? ship_bound : game_bs_client_limit;
    }*/

    //Move os battleships do client
    for (int i =0; i < NUMBER_OF_SHIPS_PER_PLAYER; i++){
        if (client_ships[i] && client_ships[i]->active) move_ship(client_ships[i]);
    }
}

void draw_game_ships(){
    for (int i =0; i < NUMBER_OF_SHIPS_PER_PLAYER; i++){
        if (host_ships[i] && host_ships[i]->active) draw_ship(host_ships[i]);
        if (client_ships[i] && client_ships[i]->active) draw_ship(client_ships[i]);
    }
}

void update_game_ships(){
    for (int i =0; i < NUMBER_OF_SHIPS_PER_PLAYER; i++){
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
    for (int i =0; i < NUMBER_OF_SHIPS_PER_PLAYER; i++){
        if (host_ships[i] && host_ships[i]->active){
            game.host_ships[i] = convert_battleship_to_serial(host_ships[i]);
        } else game.host_ships[i].active = false;

        if (client_ships[i] && client_ships[i]->active){
            game.client_ships[i] = convert_battleship_to_serial(client_ships[i]);
        } else game.client_ships[i].active = false;
    }
}

void update_word_pool(bool pump_word_index){
    static int word_pool_index = 0;
    int r1 = (rand()% 50 ) - 20;
    int r2 = (rand()% MAXIMUM_WORD_POOL_SIZE ) + MINIMUM_WORD_POOL_SIZE;

    word_pool_start_pos = ((word_pool_index + r1)<0)?0:abs(word_pool_index + r1);
    if (word_pool_start_pos + r2 >= dictionary_len){
        word_pool_start_pos = dictionary_len - MINIMUM_WORD_POOL_SIZE;
        word_pool_end_pos = dictionary_len;
    } else {
        word_pool_end_pos = word_pool_start_pos + r2;
    }

    if (pump_word_index && (word_pool_index <= dictionary_len))
        word_pool_index++;
}

bool exist_ship_starting_with(char letter, BATTLESHIP_OWNER targets){
    bool exists = false;
    BATTLESHIP* ship;
    for (int i=0; i < NUMBER_OF_SHIPS_PER_PLAYER; i++){
        ship = (targets == BATTLESHIP_OWNER_OPPONENT)?client_ships[i]:host_ships[i];
        if (!ship || !ship->word) continue;
        if ((exists = (ship->word[0] == letter))) break;
    }
    return exists;
}

char* get_word_from_pool(BATTLESHIP_OWNER owner){
    int pool_size = word_pool_end_pos-word_pool_start_pos;
    char* word;
    int tries = 0;

    do {
        word = dictionary[rand()%(pool_size+1)];
        if (tries++ % 5 == 0) update_word_pool(false);
    } while ( exist_ship_starting_with(word[0],owner) );

    return word;
}

void on_redraw_game(){
    if (frame_count++ % 30){
        update_word_pool(true);
    }
    if (current_game_state == GAME_STATE_IN_GAME_MULTIPLAYER_HOST ||
            current_game_state == GAME_STATE_IN_GAME_SINGLE_PLAYER){
        move_game_ships();
        update_game_snapshot();
    } else if (current_game_state == GAME_STATE_IN_GAME_MULTIPLAYER_CLIENT){
        update_game_ships();
    }
    draw_game_ships();
}
