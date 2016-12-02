//
// Created by ubuntu on 10/20/16.
//
#define _GNU_SOURCE

#include <allegro5/allegro_memfile.h>

#include <math.h>
#include <allegro5/allegro_primitives.h>
#include <ctype.h>
#include <stdio.h>
#include <bits/stdio.h>
#include "game.h"
#include "battleship.h"
#include "utils.h"
#include "client.h"
#include "resources/img/1.png.h"
#include "resources/img/2.png.h"
#include "resources/img/3.png.h"
#include "resources/img/4.png.h"
#include "resources/img/5.png.h"
#include "resources/img/6.png.h"
#include "resources/img/7.png.h"
#include "resources/img/8.png.h"
#include "resources/img/9.png.h"
#include "resources/img/10.png.h"
#include "resources/img/11.png.h"
#include "resources/img/12.png.h"
#include "resources/img/13.png.h"
#include "resources/img/14.png.h"
#include "resources/img/15.png.h"
#include "resources/img/16.png.h"

BATTLESHIP *host_ships[NUMBER_OF_SHIPS_PER_PLAYER];
BATTLESHIP *client_ships[NUMBER_OF_SHIPS_PER_PLAYER];
BATTLESHIP *host_mothership;
BATTLESHIP *client_mothership;
ALLEGRO_BITMAP *rsc_explosion[16];
char **dictionary;
int dictionary_len;
char host_target = -1, client_target = -1;
int host_ship_count = 0;
int client_ship_count = 0;
int next_host_ship_spawn = 0;
int next_client_ship_spawn = 0;

static int const MINIMUM_SPAWN_WAIT = 5 * 60; // 5 seconds
static int const SPAWN_WINDOW = 10 * 60; // 10 seconds

GAME_SNAPSHOT game;
int coef[3];

int game_bs_host_limit;
int game_bs_client_limit;

int frame_count;

int word_pool_start_pos;
int word_pool_end_pos;

void init_motherships();

void move_game_ships();

void draw_game_ships();

void spawn_ship(BATTLESHIP_OWNER owner, BATTLESHIP_CLASS class);

void update_battleship(BATTLESHIP *battleship, SERIAL_BATTLESHIP serial_battleship);

char *get_word_from_pool(BATTLESHIP_OWNER owner);

bool exist_ship_starting_with(char letter, BATTLESHIP_OWNER targets);

char get_index_of_ship_starting_with(char letter, BATTLESHIP_OWNER targets);

void update_word_pool(bool pump_word_index);

void load_resources_game() {

    // Abre arquivo "dictionary" para leitura apenas
    FILE *dictionary_file = fopen("dictionary","r");

    if (dictionary_file == NULL){
        fprintf(stderr,"Could not found dictionary file!");
        exit(EXIT_FAILURE);
    }

    int dic_size = 1000;
    char *pos;

    // aloca espaço para 1000 ponteiros de char em memória
    dictionary = malloc(sizeof(char*)*dic_size);

    size_t len = 0;

    dictionary_len = 0;
    while (((dictionary[dictionary_len] = NULL), // A cada iteração inicializa como NULL o endereço para a próxima palavra
            (getline(&dictionary[dictionary_len],&len,dictionary_file))) != -1) { // Lê uma linha inteira do arquivo e armazena em dictionary

        // Se posição (pos) de \n na palavra recem-lida for diferente de NULL, sobrescreve com \0
        if ((pos=strchr(dictionary[dictionary_len], '\n')) != NULL) *pos = '\0';

        // Se o numero de palavras lidas alcançou o numero de espaços no vetor dictionary, duplica-se o seu valor
        if (dictionary_len++ >= dic_size){
            dic_size *= 2;
            dictionary = realloc(dictionary,sizeof(char*)*dic_size);
        }
    }

    // Fecha o arquivo "dictionary"
    fclose(dictionary_file);

    ALLEGRO_FILE *explosion_1 = al_open_memfile(img_1_png, img_1_png_len, "r");
    load_bitmap(&rsc_explosion[0], &explosion_1, ".png");

    ALLEGRO_FILE *explosion_2 = al_open_memfile(img_2_png, img_2_png_len, "r");
    load_bitmap(&rsc_explosion[1], &explosion_2, ".png");

    ALLEGRO_FILE *explosion_3 = al_open_memfile(img_3_png, img_3_png_len, "r");
    load_bitmap(&rsc_explosion[2], &explosion_3, ".png");

    ALLEGRO_FILE *explosion_4 = al_open_memfile(img_4_png, img_4_png_len, "r");
    load_bitmap(&rsc_explosion[3], &explosion_4, ".png");

    ALLEGRO_FILE *explosion_5 = al_open_memfile(img_5_png, img_5_png_len, "r");
    load_bitmap(&rsc_explosion[4], &explosion_5, ".png");

    ALLEGRO_FILE *explosion_6 = al_open_memfile(img_6_png, img_6_png_len, "r");
    load_bitmap(&rsc_explosion[5], &explosion_6, ".png");

    ALLEGRO_FILE *explosion_7 = al_open_memfile(img_7_png, img_7_png_len, "r");
    load_bitmap(&rsc_explosion[6], &explosion_7, ".png");

    ALLEGRO_FILE *explosion_8 = al_open_memfile(img_8_png, img_8_png_len, "r");
    load_bitmap(&rsc_explosion[7], &explosion_8, ".png");

    ALLEGRO_FILE *explosion_9 = al_open_memfile(img_9_png, img_9_png_len, "r");
    load_bitmap(&rsc_explosion[8], &explosion_9, ".png");

    ALLEGRO_FILE *explosion_10 = al_open_memfile(img_10_png, img_10_png_len, "r");
    load_bitmap(&rsc_explosion[9], &explosion_10, ".png");

    ALLEGRO_FILE *explosion_11 = al_open_memfile(img_11_png, img_11_png_len, "r");
    load_bitmap(&rsc_explosion[10], &explosion_11, ".png");

    ALLEGRO_FILE *explosion_12 = al_open_memfile(img_12_png, img_12_png_len, "r");
    load_bitmap(&rsc_explosion[11], &explosion_12, ".png");

    ALLEGRO_FILE *explosion_13 = al_open_memfile(img_13_png, img_13_png_len, "r");
    load_bitmap(&rsc_explosion[12], &explosion_13, ".png");

    ALLEGRO_FILE *explosion_14 = al_open_memfile(img_14_png, img_14_png_len, "r");
    load_bitmap(&rsc_explosion[13], &explosion_14, ".png");

    ALLEGRO_FILE *explosion_15 = al_open_memfile(img_15_png, img_15_png_len, "r");
    load_bitmap(&rsc_explosion[14], &explosion_15, ".png");

    ALLEGRO_FILE *explosion_16 = al_open_memfile(img_16_png, img_16_png_len, "r");
    load_bitmap(&rsc_explosion[15], &explosion_16, ".png");
}

void unload_resources_game() {
    for (int i = 0; i < dictionary_len; i++){
        free(dictionary[i]);
    }
    free(dictionary);

    for(int i=0; i<16; i++){
        al_destroy_bitmap(rsc_explosion[i]);
    }
}

void init_game() {
    frame_count = 0;
    init_motherships();
}

void init_motherships() {

    int ship_height = get_battleship_height(BATTLESHIP_CLASS_SPACESHIP);

    host_mothership = init_battleship(BATTLESHIP_CLASS_SPACESHIP,
                                      BATTLESHIP_OWNER_PLAYER, DISPLAY_W / 2, DISPLAY_H - ship_height, 0);

    client_mothership = init_battleship(BATTLESHIP_CLASS_SPACESHIP,
                                        BATTLESHIP_OWNER_OPPONENT, DISPLAY_W / 2, ship_height, 0);


    change_battleship_state(host_mothership, BATTLESHIP_MOVE_STATE_IN_GAME);
    change_battleship_state(client_mothership, BATTLESHIP_MOVE_STATE_IN_GAME);

    if (current_game_state == GAME_STATE_IN_GAME_MULTIPLAYER_CLIENT){
        for (int i = 0; i < NUMBER_OF_SHIPS_PER_PLAYER; i++) {
            host_ships[i] = init_battleship(BATTLESHIP_CLASS_MISSILE,BATTLESHIP_OWNER_PLAYER,0,0,0);
            client_ships[i] = init_battleship(BATTLESHIP_CLASS_MISSILE,BATTLESHIP_OWNER_OPPONENT,0,0,0);
        }
    }

}

void spawn_ship(BATTLESHIP_OWNER owner, BATTLESHIP_CLASS class) {

    int ship_count = (owner == BATTLESHIP_OWNER_PLAYER) ? host_ship_count : client_ship_count;

    if (ship_count >= NUMBER_OF_SHIPS_PER_PLAYER) return;

    float dx = (owner == BATTLESHIP_OWNER_OPPONENT)? client_mothership->dx : host_mothership->dx;
    float dy = (owner == BATTLESHIP_OWNER_OPPONENT)? client_mothership->dy : host_mothership->dy;
    float x = (owner == BATTLESHIP_OWNER_OPPONENT)? host_mothership->dx : client_mothership->dx ;


    BATTLESHIP *battleship = init_battleship(class,owner,dx, dy, x);

    change_battleship_state(battleship, BATTLESHIP_MOVE_STATE_IN_GAME);
    battleship->word = get_word_from_pool(owner);

    for (int i = 0; i < NUMBER_OF_SHIPS_PER_PLAYER; i++) {
        if (owner == BATTLESHIP_OWNER_PLAYER) {
            if (!host_ships[i] || !host_ships[i]->active) {
                host_ships[i] = battleship;
                host_ship_count++;
                break;
            }
        } else {
            if (!client_ships[i] || !client_ships[i]->active) {
                client_ships[i] = battleship;
                client_ship_count++;
                break;
            }
        }
    }
}

void move_game_ships() {

    game_bs_host_limit = (int)get_bottom_dy(client_mothership);
    game_bs_client_limit = (int)get_top_dy(host_mothership);

//    int ship_bound;

    // Encontra limite para movimento dos battleships do host
    /*for (int i=0;i< MAX_SHIPS_FOR_PLAYERS;i++) {
        if (!client_ships[i]) continue;
        ship_bound = (int) client_ships[i]->dy + get_battleship_height(client_ships[i]->class) / 2;
        game_bs_host_limit = (ship_bound > game_bs_host_limit) ? ship_bound : game_bs_host_limit;
    }*/

    //Move os battleships do host
    move_ship(host_mothership,0);
    for (int i = 0; i < NUMBER_OF_SHIPS_PER_PLAYER; i++) {
        if (host_ships[i] && host_ships[i]->active) move_ship(host_ships[i],client_mothership->dx);
    }

    // Encontra limite para movimento dos battleships do client
    /*for (int i=0;i< MAX_SHIPS_FOR_PLAYERS;i++){
        if (!host_ships[i]) continue;
        ship_bound = (int)host_ships[i]->dy - get_battleship_height(host_ships[i]->class)/2;
        game_bs_client_limit = ( ship_bound < game_bs_client_limit ) ? ship_bound : game_bs_client_limit;
    }*/

    //Move os battleships do client
    move_ship(client_mothership,0);
    for (int i = 0; i < NUMBER_OF_SHIPS_PER_PLAYER; i++) {
        if (client_ships[i] && client_ships[i]->active) move_ship(client_ships[i],host_mothership->dx);
    }
}

void draw_game_ships() {
    for (int i = 0; i < NUMBER_OF_SHIPS_PER_PLAYER; i++) {
        if (current_game_state == GAME_STATE_IN_GAME_MULTIPLAYER_CLIENT) {
            if (client_ships[i] && client_ships[i]->active) draw_ship(client_ships[i]);
            if (host_ships[i] && host_ships[i]->active) {
                draw_ship(host_ships[i]);
                draw_ship_word(host_ships[i], false);
            }
        } else {
            if (host_ships[i] && host_ships[i]->active) draw_ship(host_ships[i]);
            if (client_ships[i] && client_ships[i]->active) {
                draw_ship(client_ships[i]);
                draw_ship_word(client_ships[i], false);
            }
        }
    }

    switch (current_game_state) {
        case GAME_STATE_IN_GAME_SINGLE_PLAYER:
        case GAME_STATE_IN_GAME_MULTIPLAYER_HOST:
            if (host_target != -1 && client_ships[host_target] &&
                client_ships[host_target]->active) {
                draw_target_lock(client_ships[host_target]);
                draw_ship_word(client_ships[host_target], true);
            }
            break;
        case GAME_STATE_IN_GAME_MULTIPLAYER_CLIENT:
            if (client_target != -1 && host_ships[client_target] &&
                host_ships[client_target]->active) {
                draw_target_lock(host_ships[client_target]);
                draw_ship_word(host_ships[client_target], true);
            }
            break;
        default:
            break;
    }

    draw_ship(host_mothership);
    draw_ship(client_mothership);
}

void update_game_from_snapshot() {
    for (int i = 0; i < NUMBER_OF_SHIPS_PER_PLAYER; i++) {
        update_battleship(host_ships[i], game.host_ships[i]);
        update_battleship(client_ships[i], game.client_ships[i]);
    }
    host_target = game.host_target;
    client_target = game.client_target;

    host_mothership->dx = game.host_ship_dx;
    client_mothership->dx = game.client_ship_dx;
}

void update_battleship(BATTLESHIP *battleship, SERIAL_BATTLESHIP serial_battleship) {
    if (serial_battleship.active) {
        if (!battleship) battleship = init_battleship(serial_battleship.class,
                                                      serial_battleship.owner,0,0,0);

        battleship->dx = serial_battleship.dx;
        battleship->dy = serial_battleship.dy;
        battleship->active = serial_battleship.active;

        free(battleship->word);

        char *word = serial_battleship.word;
//        battleship->word = malloc(sizeof word);
        battleship->word = strdup(word);

//        strcpy(battleship->word,word);

    } else {
        if (battleship) battleship->active = false;
    }

}

SERIAL_BATTLESHIP convert_battleship_to_serial(BATTLESHIP *battleship) {
    SERIAL_BATTLESHIP serial;
    serial.owner = battleship->owner;
    serial.active = battleship->active;
    serial.class = battleship->class;
    serial.dx = battleship->dx;
    serial.dy = battleship->dy;
    strcpy(serial.word,battleship->word);
    return serial;
}

void update_snapshot_from_game() {
    for (int i = 0; i < NUMBER_OF_SHIPS_PER_PLAYER; i++) {
        if (host_ships[i] && host_ships[i]->active) {
            game.host_ships[i] = convert_battleship_to_serial(host_ships[i]);
        } else game.host_ships[i].active = false;

        if (client_ships[i] && client_ships[i]->active) {
            game.client_ships[i] = convert_battleship_to_serial(client_ships[i]);
        } else game.client_ships[i].active = false;
    }
    game.host_target = host_target;
    game.client_target = client_target;

    game.host_ship_dx = (unsigned short)host_mothership->dx;
    game.client_ship_dx = (unsigned short)client_mothership->dx;
}

void update_word_pool(bool pump_word_index) {
    static int word_pool_index = 0;
    int r1 = (rand() % 50) - 20;
    int r2 = (rand() % MAXIMUM_WORD_POOL_SIZE) + MINIMUM_WORD_POOL_SIZE;

    word_pool_start_pos = ((word_pool_index + r1) < 0) ? 0 : abs(word_pool_index + r1);
    if (word_pool_start_pos + r2 >= dictionary_len) {
        word_pool_start_pos = dictionary_len - MINIMUM_WORD_POOL_SIZE;
        word_pool_end_pos = dictionary_len;
    } else {
        word_pool_end_pos = word_pool_start_pos + r2;
    }

    if (pump_word_index && (word_pool_index <= dictionary_len))
        word_pool_index++;
}

bool exist_ship_starting_with(char letter, BATTLESHIP_OWNER targets) {
    return (get_index_of_ship_starting_with(letter, targets) != -1);
}

char get_index_of_ship_starting_with(char letter, BATTLESHIP_OWNER targets) {
    BATTLESHIP *ship;
    for (char i = 0; i < NUMBER_OF_SHIPS_PER_PLAYER; i++) {
        ship = (targets == BATTLESHIP_OWNER_OPPONENT) ? client_ships[i] : host_ships[i];
        if (!ship || !ship->active || !ship->word) continue;
        if (get_next_letter_from_battleship(ship) == letter) return i;
    }
    return -1;
}

char *get_word_from_pool(BATTLESHIP_OWNER owner) {
    int pool_size = word_pool_end_pos - word_pool_start_pos;
    char *word = malloc(strlen(dictionary[dictionary_len]) + 1);
    int tries = 0;

    do {
        strcpy(word, dictionary[rand() % (pool_size + 1)]);
        if (tries++ % 5 == 0) update_word_pool(false);
    } while (exist_ship_starting_with(get_next_ascii_char(word), owner));

    return word;
}

void on_key_press_game(ALLEGRO_KEYBOARD_EVENT event) {


    switch (event.keycode) {
        case ALLEGRO_KEY_ESCAPE:
            if (current_game_state == GAME_STATE_IN_GAME_SINGLE_PLAYER) {
                current_game_flow_state = (current_game_flow_state == GAME_FLOW_STATE_PAUSE) ?
                                          GAME_FLOW_STATE_RUNNING : GAME_FLOW_STATE_PAUSE;
            }
            break;
        default:
            break;
    }

    if (current_game_state == GAME_FLOW_STATE_PAUSE) return;


    switch (current_game_state) {
        case GAME_STATE_IN_GAME_SINGLE_PLAYER:
            process_key_press(event.keycode, PLAYER_SINGLE);
            break;
        case GAME_STATE_IN_GAME_MULTIPLAYER_HOST:
            process_key_press(event.keycode, PLAYER_HOST);
            break;
        case GAME_STATE_IN_GAME_MULTIPLAYER_CLIENT:
            send_key_press((unsigned char)event.keycode);
            break;
        default:
            break;
    }
}

void process_key_press(int keycode, PLAYER player){

    char key;

    switch (keycode) {
        case ALLEGRO_KEY_A:
            key = 'A';
            break;
        case ALLEGRO_KEY_B:
            key = 'B';
            break;
        case ALLEGRO_KEY_C:
        case ALLEGRO_KEY_BACKSLASH2: // Ç
            key = 'C';
            break;
        case ALLEGRO_KEY_D:
            key = 'D';
            break;
        case ALLEGRO_KEY_E:
            key = 'E';
            break;
        case ALLEGRO_KEY_F:
            key = 'F';
            break;
        case ALLEGRO_KEY_G:
            key = 'G';
            break;
        case ALLEGRO_KEY_H:
            key = 'H';
            break;
        case ALLEGRO_KEY_I:
            key = 'I';
            break;
        case ALLEGRO_KEY_J:
            key = 'J';
            break;
        case ALLEGRO_KEY_K:
            key = 'K';
            break;
        case ALLEGRO_KEY_L:
            key = 'L';
            break;
        case ALLEGRO_KEY_M:
            key = 'M';
            break;
        case ALLEGRO_KEY_N:
            key = 'N';
            break;
        case ALLEGRO_KEY_O:
            key = 'O';
            break;
        case ALLEGRO_KEY_P:
            key = 'P';
            break;
        case ALLEGRO_KEY_Q:
            key = 'Q';
            break;
        case ALLEGRO_KEY_R:
            key = 'R';
            break;
        case ALLEGRO_KEY_S:
            key = 'S';
            break;
        case ALLEGRO_KEY_T:
            key = 'T';
            break;
        case ALLEGRO_KEY_U:
            key = 'U';
            break;
        case ALLEGRO_KEY_V:
            key = 'V';
            break;
        case ALLEGRO_KEY_W:
            key = 'W';
            break;
        case ALLEGRO_KEY_X:
            key = 'X';
            break;
        case ALLEGRO_KEY_Y:
            key = 'Y';
            break;
        case ALLEGRO_KEY_Z:
            key = 'Z';
            break;
        case ALLEGRO_KEY_MINUS:
            key = '-';
            break;
        default:
            key = 0;
            break;
    }


    BATTLESHIP *battleship = NULL;
    if (key != 0) {
        char next_letter = 0;
        switch (player) {
            case PLAYER_SINGLE:
            case PLAYER_HOST:
                if (host_target == -1) {
                    host_target = get_index_of_ship_starting_with(key, BATTLESHIP_OWNER_OPPONENT);
                }
                if (host_target != -1) {
                    battleship = client_ships[host_target];
                }
                break;
            case PLAYER_CLIENT:
                if (client_target == -1) {
                    client_target = get_index_of_ship_starting_with(key, BATTLESHIP_OWNER_PLAYER);
                }
                if (client_target != -1) {
                    battleship = host_ships[client_target];
                }
            default:
                break;
        }
        if (battleship != NULL) {
            next_letter = get_next_letter_from_battleship(battleship);
            if (key == next_letter) {
                if (remove_next_letter_from_battleship(battleship) == 0) {
                    battleship->active = false;
                    switch (player) {
                        case PLAYER_SINGLE:
                        case PLAYER_HOST:
                            host_target = -1;
                            client_ship_count--;
                            break;
                        case PLAYER_CLIENT:
                            client_target = -1;
                            host_ship_count--;
                        default:
                            break;
                    }
                }
            }
        }
    }
}

void on_redraw_game() {

    if (current_game_state == GAME_STATE_IN_GAME_MULTIPLAYER_HOST ||
        current_game_state == GAME_STATE_IN_GAME_SINGLE_PLAYER) {

        if (frame_count++ % 30 == 0) {
            update_word_pool(true);
        }

        if (current_game_flow_state == GAME_FLOW_STATE_RUNNING) {
            if (next_host_ship_spawn-- == 0) {
                spawn_ship(BATTLESHIP_OWNER_PLAYER, BATTLESHIP_CLASS_MISSILE);
                next_host_ship_spawn = (rand() % (SPAWN_WINDOW + 1)) + MINIMUM_SPAWN_WAIT;
            }

            if (next_client_ship_spawn-- == 0) {
                spawn_ship(BATTLESHIP_OWNER_OPPONENT, BATTLESHIP_CLASS_MISSILE);
                next_client_ship_spawn = (rand() % (SPAWN_WINDOW + 1)) + MINIMUM_SPAWN_WAIT;
            }
        }

        if (DEBUG) {
            al_draw_line(5, 5, 5, 5 + dictionary_len / 2, al_map_rgb(255, 255, 153), 2);
            al_draw_line(5, 5 + word_pool_start_pos / 2, 5, 5 + word_pool_end_pos / 2, al_map_rgb(0, 0, 255), 2);
        }

        if(current_game_flow_state == GAME_FLOW_STATE_RUNNING) {
            move_game_ships();
        }
        update_snapshot_from_game();
    } else if (current_game_state == GAME_STATE_IN_GAME_MULTIPLAYER_CLIENT) {
        update_game_from_snapshot();
    }
    draw_game_ships();
    if(current_game_flow_state == GAME_FLOW_STATE_HOST_BOOM || current_game_flow_state == GAME_FLOW_STATE_CLIENT_BOOM) {
        float dx = (current_game_flow_state == GAME_FLOW_STATE_HOST_BOOM)? host_mothership->dx : client_mothership->dx ;
        float dy = (current_game_flow_state == GAME_FLOW_STATE_HOST_BOOM)? host_mothership->dy : client_mothership->dy ;

        static int i = 0, j = 2, k = 4, l = 6, cont = 0;
        static int modi = 0, modj = 0, modk = 0, modl = 0;

        al_draw_bitmap(rsc_explosion[i], (dx - 30) - modi, (dy - 30) - modi, 0);
        al_draw_bitmap(rsc_explosion[j], (dx - 30) - modj, (dy - 30) + modj, 0);
        al_draw_bitmap(rsc_explosion[k], (dx - 30) + modk, (dy - 30) - modk, 0);
        al_draw_bitmap(rsc_explosion[l], (dx - 30) + modl, (dy - 30) + modl, 0);

        if (cont > 5) {
            i++;
            j++;
            k++;
            l++;
            cont = 0;
        }
        cont++;

        if (i > 15) {
            i = 0;
            modi = rand() % 45;
        }
        if (j > 15) {
            j = 0;
            modj = rand() % 45;
        }
        if (k > 15) {
            k = 0;
            modk = rand() % 45;
        }
        if (l > 15) {
            l = 0;
            modl = rand() % 45;
        }
    }
}
