//
// Created by ubuntu on 10/20/16.
//

#include <math.h>
#include <allegro5/allegro_primitives.h>
#include "game.h"
#include "resources/dictionary.h"
#include "battleship.h"

BATTLESHIP* host_ships[NUMBER_OF_SHIPS_PER_PLAYER];
BATTLESHIP* client_ships[NUMBER_OF_SHIPS_PER_PLAYER];
BATTLESHIP* host_mothership;
BATTLESHIP* client_mothership;
int host_ship_count = 0;
int client_ship_count = 0;
int next_host_ship_spawn = 0;
int next_client_ship_spawn = 0;

static int const MINIMUM_SPAWN_WAIT = 5 * 60; // 5 seconds
static int const SPAWN_WINDOW = 10 * 60; // 10 seconds

GAME_SNAPSHOT game;

              int game_bs_host_limit;
              int game_bs_client_limit;

              int frame_count;

              int word_pool_start_pos;
              int word_pool_end_pos;

              void init_motherships();
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
    init_motherships();
}

void init_motherships(){

    int ship_height = get_battleship_height(BATTLESHIP_CLASS_M);

    host_mothership = init_battleship(BATTLESHIP_CLASS_M,DISPLAY_W/2,DISPLAY_H - ship_height);
    host_mothership->owner = BATTLESHIP_OWNER_PLAYER;
    client_mothership = init_battleship(BATTLESHIP_CLASS_M,DISPLAY_W/2,ship_height);
    client_mothership->owner = BATTLESHIP_OWNER_OPPONENT;

}

void spawn_ship(BATTLESHIP_OWNER owner, BATTLESHIP_CLASS class, char* word){

    static bool p_l = false;

    int ship_count = (owner == BATTLESHIP_OWNER_PLAYER)? host_ship_count : client_ship_count;

    if (ship_count >= NUMBER_OF_SHIPS_PER_PLAYER) return;

    int max_rand = DISPLAY_W - get_battleship_width(class);
    int half_ship = get_battleship_width(class)/2;
    int ship_height = get_battleship_height(class);
    int y_pos = (owner == BATTLESHIP_OWNER_PLAYER) ? DISPLAY_H+ship_height/2 : -ship_height/2;

    BATTLESHIP* battleship = init_battleship(class,
                                             (rand()%max_rand)+half_ship, y_pos);
    change_battleship_state(battleship,BATTLESHIP_MOVE_STATE_IN_GAME);
    battleship->owner = owner;
    battleship->word = word;

    for (int i=0;i<NUMBER_OF_SHIPS_PER_PLAYER;i++){
        if (owner == BATTLESHIP_OWNER_PLAYER) {
            if (!host_ships[i] || !host_ships[i]->active){
                host_ships[i] = battleship;
                host_ship_count++;
                break;
            }
        } else {
            if (!client_ships[i] || !client_ships[i]->active){
                client_ships[i] = battleship;
                client_ship_count++;

                if (!p_l){
                    p_l = true;
                    client_ships[i]->locked = true;
                }
                break;
            }
        }
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
    draw_ship(host_mothership);
    draw_ship(client_mothership);

    bool draw_lock = false;
    bool ship_is_locked;
    int locked_ship = -1;

    for (int i =0; i < NUMBER_OF_SHIPS_PER_PLAYER; i++){
        if (host_ships[i] && host_ships[i]->active) draw_ship(host_ships[i]);
        if (client_ships[i] && client_ships[i]->active) draw_ship(client_ships[i]);

        ship_is_locked = (current_game_state == GAME_STATE_IN_GAME_MULTIPLAYER_HOST)?
                         host_ships[i] && host_ships[i]->active && host_ships[i]->locked :
                         client_ships[i] && client_ships[i]->active && client_ships[i]->locked;

        if (ship_is_locked) locked_ship = i;

        draw_lock = draw_lock || ship_is_locked;
    }

    if (draw_lock){
        draw_target_lock((current_game_state == GAME_STATE_IN_GAME_MULTIPLAYER_HOST)?
                         host_ships[locked_ship]:client_ships[locked_ship]);
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
        battleship->word   = serial_battleship.word;
        battleship->locked = serial_battleship.looked;

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
    serial.word   = battleship->word;
    serial.looked = battleship->locked;
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

    if (current_game_state == GAME_STATE_IN_GAME_MULTIPLAYER_HOST ||
            current_game_state == GAME_STATE_IN_GAME_SINGLE_PLAYER){

        if (frame_count++ % 30 == 0){
            update_word_pool(true);
        }

        if (next_host_ship_spawn-- == 0){
            spawn_ship(BATTLESHIP_OWNER_PLAYER,BATTLESHIP_CLASS_5,
                       get_word_from_pool(BATTLESHIP_OWNER_PLAYER));
            next_host_ship_spawn = (rand()%(SPAWN_WINDOW+1))+MINIMUM_SPAWN_WAIT;
        }

        if (next_client_ship_spawn-- == 0){
            spawn_ship(BATTLESHIP_OWNER_OPPONENT,BATTLESHIP_CLASS_5,
                       get_word_from_pool(BATTLESHIP_OWNER_OPPONENT));
            next_client_ship_spawn = (rand()%(SPAWN_WINDOW+1))+MINIMUM_SPAWN_WAIT;
        }


        if (DEBUG){
            al_draw_line(5,5,5,5+dictionary_len/2,al_map_rgb(255,255,153),2);
            al_draw_line(5,5+word_pool_start_pos/2,5,5+word_pool_end_pos/2,al_map_rgb(0,0,255),2);
        }

        move_game_ships();
        update_game_snapshot();
    } else if (current_game_state == GAME_STATE_IN_GAME_MULTIPLAYER_CLIENT){
        update_game_ships();
    }
    draw_game_ships();
}
