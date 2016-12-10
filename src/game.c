#include <allegro5/allegro_memfile.h>

#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>
#include <stdio.h>
#include "game.h"
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
#include "server.h"
#include "buttons.h"

BATTLESHIP *host_ships[NUMBER_OF_SHIPS_PER_PLAYER];
BATTLESHIP *client_ships[NUMBER_OF_SHIPS_PER_PLAYER];
BATTLESHIP *host_mothership;
BATTLESHIP *client_mothership;
Button purchase_buttons[2];

char **dictionary;
int dictionary_len;
char host_target = -1, client_target = -1;
int host_ship_count = 0;
int client_ship_count = 0;
int next_host_ship_spawn = 0;
int next_client_ship_spawn = 0;
char game_winner = -1;
short game_level = 1;
long opponent_score = 0;
long player_score = 0;
long rank_score = -1;
short player_lives;
short opponent_lives;
int life_price_for_player;
int life_price_for_opponent;
bool need_to_show_purchase_life = false;
int consecutive_right_key_player = 0;
int consecutive_right_key_opponent = 0;
int remaining_words_to_next_level = -1;
int word_pool_index = 0;
bool need_to_show_game_level = false;
bool wait_new_level = false;
int shortcut_key_pressed_game = -1;
bool is_mouse_down_game;
bool is_mouse_down_on_button_game;
bool draw_explosions_on_game_end = true;

static int const GAME_WINNER_PLAYER = 0;
static int const GAME_WINNER_OPPONENT = 1;

static int const MINIMUM_SPAWN_WAIT = 1 * 30;
static int const SPAWN_WINDOW = 1 * 60;

int const BTN_PURCHASE_YES = 0;
int const BTN_PURCHASE_NO = 1;

GAME_SNAPSHOT game;

int game_bs_host_limit;
int game_bs_client_limit;

int word_pool_start_pos;
int word_pool_end_pos;

void init_motherships();

void init_purchase_buttons();

void update_game_ships_frame_count();

void move_game_ships();

void draw_game_ships();

void spawn_ship(BATTLESHIP_OWNER owner, BATTLESHIP_CLASS class);

void update_battleship(BATTLESHIP *battleship, SERIAL_BATTLESHIP serial_battleship);

char *get_word_from_pool(BATTLESHIP_OWNER owner);

bool exist_ship_starting_with(char letter, BATTLESHIP_OWNER targets);

char get_index_of_ship_starting_with(char letter, BATTLESHIP_OWNER targets);

char get_index_from_closest_ship(BATTLESHIP_OWNER targets);

void update_word_pool(bool pump_word_index);

void on_explosion_end(BATTLESHIP_OWNER *owner);

void draw_pause_overlay();

void draw_explosions();

void draw_game_level();

void draw_game_over();

void start_level_change();

void clear_ships();

void on_new_level(short level);

void on_char_typed(PLAYER player, char key);

SERIAL_BATTLESHIP convert_battleship_to_serial(BATTLESHIP *battleship);

void update_snapshot_from_game();

void update_game_from_snapshot();

bool purchase_life(PLAYER player);

void try_auto_purchase_life(PLAYER player);

void update_score(PLAYER player, bool up);

void on_button_click_game(int i);

void take_a_life(PLAYER player);

void end_game();

long get_last_game_score() {
    long score = rank_score;
    rank_score = -1;
    return score;
}

void load_resources_game() {

    // Abre arquivo "dictionary" para leitura apenas
    FILE *dictionary_file = fopen("dictionary", "r");

    if (dictionary_file == NULL) {
        fprintf(stderr, "Could not found dictionary file!");
        al_show_native_message_box(display, "Error", "File missing",
                                   "Could not found dictionary file!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
        exit(EXIT_FAILURE);
    }

    int dic_size = 1000;
    char *pos;

    // aloca espaço para 1000 ponteiros de char em memória
    dictionary = malloc(sizeof(char *) * dic_size);

    size_t len = 0;

    dictionary_len = 0;
    while (((dictionary[dictionary_len] = NULL), // A cada iteração inicializa como NULL o endereço para a próxima palavra
            (getline(&dictionary[dictionary_len], &len, dictionary_file))) !=
           -1) { // Lê uma linha inteira do arquivo e armazena em dictionary

        // Se posição (pos) de \n na palavra recem-lida for diferente de NULL, sobrescreve com \0
        if ((pos = strchr(dictionary[dictionary_len], '\n')) != NULL) *pos = '\0';

        // Se o numero de palavras lidas alcançou o numero de espaços no vetor dictionary, duplica-se o seu valor
        if (dictionary_len++ >= dic_size) {
            dic_size *= 2;
            dictionary = realloc(dictionary, sizeof(char *) * dic_size);
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
    for (int i = 0; i < dictionary_len; i++) {
        free(dictionary[i]);
    }
    free(dictionary);

    for (int i = 0; i < 16; i++) {
        al_destroy_bitmap(rsc_explosion[i]);
    }
}

void init_game() {
    current_game_flow_state = GAME_FLOW_STATE_RUNNING;
    need_to_show_purchase_life = false;
    draw_explosions_on_game_end = true;
    received_first_snapshot = false;
    ready_to_send = false;
    is_mouse_down_game = false;
    is_mouse_down_on_button_game = false;
    game_winner = -1;
    opponent_score = 0;
    player_score = 0;
    life_price_for_player = 100;
    life_price_for_opponent = 100;
    rank_score = -1;
    player_lives = 1;
    opponent_lives = 1;
    word_pool_index = 0;
    init_purchase_buttons();
    init_motherships();
    clear_ships();
    on_new_level(1);
}

void init_purchase_buttons() {
    purchase_buttons[0] = init_button(main_font_size_45, "&Sim", (DISPLAY_W / 4) + 25, DISPLAY_H / 2 + 100, 180);
    purchase_buttons[1] = init_button(main_font_size_45, "&Nao", (DISPLAY_W / 4) * 3 - 25, DISPLAY_H / 2 + 100, 180);
}

void clear_ships() {
    for (int i = 0; i < NUMBER_OF_SHIPS_PER_PLAYER; i++) {
        if (host_ships[i]) {
            host_ships[i]->active = false;
        }
        if (client_ships[i]) {
            client_ships[i]->active = false;
        }
    }
    client_ship_count = 0;
    host_ship_count = 0;
    host_target = -1;
    client_target = -1;
}

void init_motherships() {

    int ship_height = get_battleship_height(BATTLESHIP_CLASS_SPACESHIP);

    host_mothership = init_battleship(BATTLESHIP_CLASS_SPACESHIP,
                                      BATTLESHIP_OWNER_PLAYER, DISPLAY_W / 2, DISPLAY_H - ship_height, 0, game_level);

    client_mothership = init_battleship(BATTLESHIP_CLASS_SPACESHIP,
                                        BATTLESHIP_OWNER_OPPONENT, DISPLAY_W / 2, ship_height, 0, game_level);


    change_battleship_state(host_mothership, BATTLESHIP_MOVE_STATE_IN_GAME);
    change_battleship_state(client_mothership, BATTLESHIP_MOVE_STATE_IN_GAME);

    if (is_multiplayer_client()) {
        for (int i = 0; i < NUMBER_OF_SHIPS_PER_PLAYER; i++) {
            host_ships[i] = init_battleship(BATTLESHIP_CLASS_MISSILE, BATTLESHIP_OWNER_PLAYER,
                                            0, 0, client_mothership->dx, game_level);
            client_ships[i] = init_battleship(BATTLESHIP_CLASS_MISSILE, BATTLESHIP_OWNER_OPPONENT,
                                              0, 0, host_mothership->dx, game_level);
        }
    }
    game_bs_host_limit = (int) get_bottom_dy(client_mothership);
    game_bs_client_limit = (int) get_top_dy(host_mothership);
}

void on_explosion_end(BATTLESHIP_OWNER *owner) {

    if (*owner == BATTLESHIP_OWNER_OPPONENT) {
        client_ship_count--;
    } else {
        host_ship_count--;
    }

    if (is_single_player()) {
        if ((remaining_words_to_next_level <= 0) &&
            (client_ship_count == 0) && (host_ship_count == 0)) {
            start_level_change();
        }
    } else {
        if (remaining_words_to_next_level <= 0) {
            start_level_change();
        }
    }

}

void spawn_ship(BATTLESHIP_OWNER owner, BATTLESHIP_CLASS class) {

    if (PITTHAN_MODE && (owner == BATTLESHIP_OWNER_PLAYER) &&
        (current_game_state == GAME_STATE_IN_GAME_SINGLE_PLAYER))
        return;

    int ship_count = (owner == BATTLESHIP_OWNER_PLAYER) ? host_ship_count : client_ship_count;

    if (ship_count >= NUMBER_OF_SHIPS_PER_PLAYER) return;

    if (remaining_words_to_next_level-- <= 0) return;

    float dx = (owner == BATTLESHIP_OWNER_OPPONENT) ? client_mothership->dx : host_mothership->dx;
    float dy = (owner == BATTLESHIP_OWNER_OPPONENT) ? client_mothership->dy : host_mothership->dy;
    float x = (owner == BATTLESHIP_OWNER_OPPONENT) ? host_mothership->dx : client_mothership->dx;


    BATTLESHIP *battleship = init_battleship(class, owner, dx, dy, x, game_level);

    battleship->on_explosion_end = on_explosion_end;

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

void update_game_ships_frame_count() {
    for (int i = 0; i < NUMBER_OF_SHIPS_PER_PLAYER; i++) {
        if (host_ships[i] && host_ships[i]->active) {
            update_ship_frame_count(host_ships[i]);
        }
    }
    for (int i = 0; i < NUMBER_OF_SHIPS_PER_PLAYER; i++) {
        if (client_ships[i] && client_ships[i]->active) {
            update_ship_frame_count(client_ships[i]);
        }
    }
}

void move_game_ships() {

    bool kill_opponent = false;
    bool kill_player = false;

    //Move os battleships do host
    move_ship(host_mothership, 0);
    for (int i = 0; i < NUMBER_OF_SHIPS_PER_PLAYER; i++) {
        if (host_ships[i] && host_ships[i]->active) {
            update_ship_frame_count(host_ships[i]);
            kill_opponent = move_ship(host_ships[i], client_mothership->dx) || kill_opponent;
        }
    }

    if (kill_opponent) {
        take_a_life(PLAYER_CLIENT);
    }

    //Move os battleships do client
    move_ship(client_mothership, 0);
    for (int i = 0; i < NUMBER_OF_SHIPS_PER_PLAYER; i++) {
        if (client_ships[i] && client_ships[i]->active) {
            update_ship_frame_count(client_ships[i]);
            kill_player = move_ship(client_ships[i], host_mothership->dx) || kill_player;
        }
    }

    if (kill_player) {
        take_a_life(PLAYER_HOST);
    }
}

void take_a_life(PLAYER player) {
    if (player == PLAYER_CLIENT) {
        if (--opponent_lives <= 0) game_winner = GAME_WINNER_PLAYER;
    } else {
        if (--player_lives <= 0) {
            if (is_multiplayer()) {
                game_winner = GAME_WINNER_OPPONENT;
            } else if (player_score < life_price_for_player) {
                game_winner = GAME_WINNER_OPPONENT;
            } else {
                need_to_show_purchase_life = true;
            }
        }
    }

    if (game_winner != -1) {
        current_game_flow_state = GAME_FLOW_STATE_ENDING;
    }
}

void draw_game_ships() {
    for (int i = 0; i < NUMBER_OF_SHIPS_PER_PLAYER; i++) {
        if (is_multiplayer_client()) {
            if (client_ships[i] && client_ships[i]->active) draw_ship(client_ships[i]);
            if (host_ships[i] && host_ships[i]->active) {
                draw_ship(host_ships[i]);
                if (!is_game_paused())
                    draw_ship_word(host_ships[i], false);
            }
        } else {
            if (host_ships[i] && host_ships[i]->active) draw_ship(host_ships[i]);
            if (client_ships[i] && client_ships[i]->active) {
                draw_ship(client_ships[i]);
                if (!is_game_paused())
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
                if (!is_game_paused())
                    draw_ship_word(client_ships[host_target], true);
            }
            break;
        case GAME_STATE_IN_GAME_MULTIPLAYER_CLIENT:
            if (client_target != -1 && host_ships[client_target] &&
                host_ships[client_target]->active) {
                draw_target_lock(host_ships[client_target]);
                if (!is_game_paused())
                    draw_ship_word(host_ships[client_target], true);
            }
            break;
        default:
            break;
    }

    draw_ship(host_mothership);
    draw_ship(client_mothership);

    for (int i = 0; i < NUMBER_OF_SHIPS_PER_PLAYER; i++) {
        if (client_ships[i] && client_ships[i]->active && client_ships[i]->exploding &&
            !client_ships[i]->exploding_with_lasers)
            draw_ship(client_ships[i]);
        if (host_ships[i] && host_ships[i]->active && host_ships[i]->exploding &&
            !host_ships[i]->exploding_with_lasers)
            draw_ship(host_ships[i]);
    }
}

void update_game_from_snapshot() {
    for (int i = 0; i < NUMBER_OF_SHIPS_PER_PLAYER; i++) {
        update_battleship(host_ships[i], game.host_ships[i]);
        update_battleship(client_ships[i], game.client_ships[i]);
    }
    host_target = game.host_target;
    client_target = game.client_target;

    player_score = game.host_score;
    opponent_score = game.client_score;

    player_lives = game.player_lives;
    opponent_lives = game.client_lives;

    game_winner = game.game_winner;
    if (game.is_game_ending) {
        current_game_flow_state = GAME_FLOW_STATE_ENDING;
    }

    host_mothership->dx = game.host_ship_dx;
    client_mothership->dx = game.client_ship_dx;
}

void update_battleship(BATTLESHIP *battleship, SERIAL_BATTLESHIP serial_battleship) {
    if (serial_battleship.active) {
        if (!battleship)
            battleship = init_battleship(serial_battleship.class,
                                         serial_battleship.owner, 0, 0, 0, game_level);

        battleship->dx = serial_battleship.dx;
        battleship->dy = serial_battleship.dy;
        battleship->active = serial_battleship.active;

        battleship->exploding = serial_battleship.exploding;
        battleship->exploding_with_lasers = serial_battleship.exploding_with_lasers;
        battleship->explosion_frame = serial_battleship.explosion_frame;

        free(battleship->word);

        char *word = serial_battleship.word;
        battleship->word = strdup(word);

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
    serial.exploding = battleship->exploding;
    serial.exploding_with_lasers = battleship->exploding_with_lasers;
    serial.explosion_frame = battleship->explosion_frame;
    strcpy(serial.word, battleship->word);
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

    game.client_score = opponent_score;
    game.host_score = player_score;

    game.player_lives = player_lives;
    game.client_lives = opponent_lives;

    game.game_winner = game_winner;
    game.is_game_ending = is_game_ending();

    game.host_ship_dx = (unsigned short) host_mothership->dx;
    game.client_ship_dx = (unsigned short) client_mothership->dx;
}

void update_word_pool(bool pump_word_index) {
    int r1 = (rand() % 50) - 20;
    int r2 = (rand() % MAXIMUM_WORD_POOL_SIZE) + MINIMUM_WORD_POOL_SIZE;

    word_pool_start_pos = ((word_pool_index + r1) < 0) ? 0 : abs(word_pool_index + r1);
    if (word_pool_start_pos + r2 >= dictionary_len - 1) {
        word_pool_start_pos = dictionary_len - 1 - MINIMUM_WORD_POOL_SIZE;
        word_pool_end_pos = dictionary_len - 1;
    } else {
        word_pool_end_pos = word_pool_start_pos + r2;
    }

    int game_level_pool_cap = game_level * 50;

    if (pump_word_index && (word_pool_index <= dictionary_len) &&
        word_pool_index < game_level_pool_cap)
        word_pool_index++;
}

bool exist_ship_starting_with(char letter, BATTLESHIP_OWNER targets) {
    return (get_index_of_ship_starting_with(letter, targets) != -1);
}

char get_index_of_ship_starting_with(char letter, BATTLESHIP_OWNER targets) {
    BATTLESHIP *ship;
    char index = -1;

    for (char i = 0; i < NUMBER_OF_SHIPS_PER_PLAYER; i++) {
        ship = (targets == BATTLESHIP_OWNER_OPPONENT) ? client_ships[i] : host_ships[i];
        if (!ship || !ship->active || !ship->word) continue;
        if (get_next_letter_from_battleship(ship) == letter) {
            if (index == -1) {
                index = i;
            } else {
                if (targets == BATTLESHIP_OWNER_OPPONENT) {
                    // maior Y | mais abaixo
                    index = (client_ships[i]->dy > client_ships[index]->dy) ? i : index;
                } else {
                    // menor Y | mais acima
                    index = (host_ships[i]->dy < host_ships[index]->dy) ? i : index;
                }
            }
        };
    }
    return index;
}

char get_index_from_closest_ship(BATTLESHIP_OWNER targets) {
    BATTLESHIP *ship;
    char index = -1;

    for (char i = 0; i < NUMBER_OF_SHIPS_PER_PLAYER; i++) {
        ship = (targets == BATTLESHIP_OWNER_OPPONENT) ? client_ships[i] : host_ships[i];
        if (!ship || !ship->active || !ship->word) continue;
        if (index == -1) {
            index = i;
        } else {
            if (targets == BATTLESHIP_OWNER_OPPONENT) {
                // maior Y | mais abaixo
                index = (client_ships[i]->dy > client_ships[index]->dy) ? i : index;
            } else {
                // menor Y | mais acima
                index = (host_ships[i]->dy < host_ships[index]->dy) ? i : index;
            }
        }
    }
    return index;
}

char *get_word_from_pool(BATTLESHIP_OWNER owner) {
    int pool_size = word_pool_end_pos - word_pool_start_pos;
    char *word = malloc(strlen(dictionary[dictionary_len]) + 1);
    int tries = 0;

    do {
        strcpy(word, dictionary[word_pool_start_pos + (rand() % (pool_size + 1))]);
        if (tries++ % 5 == 0) update_word_pool(false);
    } while (exist_ship_starting_with(get_next_ascii_char(word), owner) && (tries < 50));

    return word;
}

void on_key_press_game(ALLEGRO_KEYBOARD_EVENT event) {


    switch (event.keycode) {
        case ALLEGRO_KEY_ESCAPE:
            if (is_single_player()) {
                if (current_game_flow_state != GAME_FLOW_STATE_ENDING) {
                    current_game_flow_state = (current_game_flow_state == GAME_FLOW_STATE_PAUSE) ?
                                              GAME_FLOW_STATE_RUNNING : GAME_FLOW_STATE_PAUSE;
                }
            }
            break;
        default:
            break;
    }

    if (current_game_flow_state == GAME_FLOW_STATE_PAUSE) return;

    if (current_game_flow_state == GAME_FLOW_STATE_PURCHASING_LIFE) {
        if (event.keycode == ALLEGRO_KEY_S) {
            shortcut_key_pressed_game = BTN_PURCHASE_YES;
        } else if (event.keycode == ALLEGRO_KEY_N) {
            shortcut_key_pressed_game = BTN_PURCHASE_NO;
        }
        return;
    }

    switch (current_game_state) {
        case GAME_STATE_IN_GAME_SINGLE_PLAYER:
            process_key_press(event.keycode, PLAYER_SINGLE);
            break;
        case GAME_STATE_IN_GAME_MULTIPLAYER_HOST:
            process_key_press(event.keycode, PLAYER_HOST);
            break;
        case GAME_STATE_IN_GAME_MULTIPLAYER_CLIENT:
            send_key_press((unsigned char) event.keycode);
            break;
        default:
            break;
    }


}

void process_key_press(int keycode, PLAYER player) {

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

    on_char_typed(player, key);
}

void on_char_typed(PLAYER player, char key) {
    if (is_game_ending()) return;
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

                update_score(player, true);

                if (remove_next_letter_from_battleship(battleship) == 0) {
                    battleship->exploding = true;
                    battleship->exploding_with_lasers = true;
                    switch (player) {
                        case PLAYER_SINGLE:
                        case PLAYER_HOST:
                            host_target = -1;
                            break;
                        case PLAYER_CLIENT:
                            client_target = -1;
                        default:
                            break;
                    }
                }
            } else {
                update_score(player, false);
            }
        } else {
            update_score(player, false);
        }
    }
}

bool purchase_life(PLAYER player) {

    if (player == PLAYER_CLIENT) {
        if (opponent_score >= life_price_for_opponent) {
            opponent_score -= life_price_for_opponent;
            life_price_for_opponent *= 2;
            opponent_lives++;
            return true;
        } else return false;
    } else {
        if (player_score > life_price_for_player) {
            player_score -= life_price_for_player;
            life_price_for_player *= 2;
            player_lives++;
            return true;
        } else return false;
    }
}

void try_auto_purchase_life(PLAYER player) {

    if (player == PLAYER_CLIENT) {
        if (is_multiplayer() || (is_single_player() && opponent_score > player_score))
            purchase_life(player);
    } else {
        if (is_multiplayer())
            purchase_life(player);
    }
}

void update_score(PLAYER player, bool up) {
    if (up) {
        if (player == PLAYER_CLIENT) {
            opponent_score += (game_level * 10) + consecutive_right_key_opponent++;
            if (is_multiplayer() || !PITTHAN_MODE) try_auto_purchase_life(player);
        } else {
            player_score += (game_level * 10) + consecutive_right_key_player++;
            if (is_multiplayer()) try_auto_purchase_life(player);
        }
    } else {
        if (player == PLAYER_CLIENT) {
            consecutive_right_key_opponent = 0;
            opponent_score -= (game_level * 10);
            if (opponent_score < 0) opponent_score = 0;
        } else {
            consecutive_right_key_player = 0;
            player_score -= (game_level * 10);
            if (player_score < 0) player_score = 0;
        }
    }
}

bool is_single_player() {
    return current_game_state == GAME_STATE_IN_GAME_SINGLE_PLAYER;
}

bool is_multiplayer_host() {
    return current_game_state == GAME_STATE_IN_GAME_MULTIPLAYER_HOST;
}

bool is_multiplayer_client() {
    return current_game_state == GAME_STATE_IN_GAME_MULTIPLAYER_CLIENT;
}

bool is_multiplayer() {
    return (current_game_state == GAME_STATE_IN_GAME_MULTIPLAYER_HOST) ||
           (current_game_state == GAME_STATE_IN_GAME_MULTIPLAYER_CLIENT);
}

bool is_game_paused() {
    if (is_game_ending()) {
        return false;
    } else
        return is_single_player() && current_game_flow_state == GAME_FLOW_STATE_PAUSE;
}

bool is_game_ending() {
    return current_game_flow_state == GAME_FLOW_STATE_ENDING;
}

void on_mouse_move_game(int x, int y) {

    if (current_game_flow_state == GAME_FLOW_STATE_PURCHASING_LIFE) {
        bool is_over_button = false;

        if (!is_mouse_down_game) {
            int total_buttons = sizeof(purchase_buttons) / sizeof(purchase_buttons[0]);
            for (int i = 0; i < total_buttons; i++) {
                if (!purchase_buttons[i].visible) continue;
                if (is_coordinate_inside_button(purchase_buttons[i], x, y)) {
                    is_over_button = true;
                    purchase_buttons[i].state = (purchase_buttons[i].state != BUTTON_STATE_ACTIVE) ?
                                                BUTTON_STATE_HOVER : purchase_buttons[i].state;
                } else
                    purchase_buttons[i].state = (purchase_buttons[i].state == BUTTON_STATE_HOVER) ?
                                                BUTTON_STATE_NORMAL : purchase_buttons[i].state;
            }
        }

        if (is_over_button || is_mouse_down_on_button_game)
            al_set_system_mouse_cursor(display,
                                       ALLEGRO_SYSTEM_MOUSE_CURSOR_ALT_SELECT);
        else
            al_set_system_mouse_cursor(display,
                                       ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
    }

}

void on_button_click_game(int i) {
    if (i == BTN_PURCHASE_YES) {
        if (purchase_life(PLAYER_SINGLE)) {
            on_new_level(game_level);
            need_to_show_purchase_life = false;
            current_game_flow_state = GAME_FLOW_STATE_RUNNING;
        } else {
            need_to_show_purchase_life = false;
            draw_explosions_on_game_end = false;
            current_game_flow_state = GAME_FLOW_STATE_ENDING;
        }
    } else if (i == BTN_PURCHASE_NO) {
        need_to_show_purchase_life = false;
        draw_explosions_on_game_end = false;
        current_game_flow_state = GAME_FLOW_STATE_ENDING;
    }
}

void on_mouse_down_game(int x, int y) {
    int total_buttons = sizeof(purchase_buttons) / sizeof(purchase_buttons[0]);
    for (int i = 0; i < total_buttons; i++) {
        if (purchase_buttons[i].visible && is_coordinate_inside_button(purchase_buttons[i], x, y)) {
            is_mouse_down_on_button_game = true;
            purchase_buttons[i].state = BUTTON_STATE_ACTIVE;
            break;
        }
    }
    is_mouse_down_game = true;
}

void on_mouse_up_game(int x, int y) {
    int total_buttons = sizeof(purchase_buttons) / sizeof(purchase_buttons[0]);
    for (int i = 0; i < total_buttons; i++) {
        if (purchase_buttons[i].visible && purchase_buttons[i].state == BUTTON_STATE_ACTIVE) {
            if (is_coordinate_inside_button(purchase_buttons[i], x, y)) {
                purchase_buttons[i].state = BUTTON_STATE_HOVER;
                on_button_click_game(i);
            } else {
                purchase_buttons[i].state = BUTTON_STATE_NORMAL;
            }
            break;
        }
    }
    is_mouse_down_game = false;
    is_mouse_down_on_button_game = false;

    on_mouse_move_game(x, y);
}

void on_timer_game() {

    if (current_game_flow_state == GAME_FLOW_STATE_PURCHASING_LIFE) {
        int static frame_count = 0;

        if (shortcut_key_pressed_game == -1) return;

        switch (frame_count++) {
            case 0:
                on_mouse_down_game(
                        purchase_buttons[shortcut_key_pressed_game].x + 1,
                        purchase_buttons[shortcut_key_pressed_game].y + 1);
                break;
            case 10:
                on_mouse_up_game(
                        purchase_buttons[shortcut_key_pressed_game].x + 1,
                        purchase_buttons[shortcut_key_pressed_game].y + 1);
                frame_count = 0;
                shortcut_key_pressed_game = -1;
                break;
            default:
                break;
        }

    } else if (current_game_flow_state == GAME_FLOW_STATE_RUNNING) {

        if (is_multiplayer() || is_game_ending() || PITTHAN_MODE) return;

        static int frame_count = 0;
        static int retarget_action_cont = 0;

        int frame_wait = (40 - (game_level) * 5);

        if (frame_wait < 10) {
            frame_wait = 10;
        }

        if (frame_count++ < frame_wait) return;

        frame_count = 0;

        if (client_target == -1 && retarget_action_cont-- <= 0) {
            client_target = get_index_from_closest_ship(BATTLESHIP_OWNER_PLAYER);
            retarget_action_cont = game_level / 3 - 1;
        }

        if (client_target != -1) {
            on_char_typed(PLAYER_CLIENT, get_next_letter_from_battleship(host_ships[client_target]));
        }

    }


}

void draw_pause_overlay() {
    al_draw_filled_rectangle(0, 0, DISPLAY_W, DISPLAY_H, al_map_rgba(20, 20, 20, 100));


    int x1 = DISPLAY_W / 2 - 50;
    int x2 = DISPLAY_W / 2 - 10;
    int x3 = DISPLAY_W / 2 + 10;
    int x4 = DISPLAY_W / 2 + 50;

    int y1 = DISPLAY_H / 2 - 50;
    int y2 = DISPLAY_H / 2 + 50;

    al_draw_filled_rectangle(x1, y1, x2, y2, al_map_rgba(50, 50, 50, 200));
    al_draw_filled_rectangle(x3, y1, x4, y2, al_map_rgba(50, 50, 50, 200));
}

void draw_explosions() {

    float dx, dy;

    if (current_game_state == GAME_STATE_IN_GAME_MULTIPLAYER_HOST ||
        current_game_state == GAME_STATE_IN_GAME_SINGLE_PLAYER) {
        dx = (game_winner == GAME_WINNER_OPPONENT) ? host_mothership->dx : client_mothership->dx;
        dy = (game_winner == GAME_WINNER_OPPONENT) ? host_mothership->dy : client_mothership->dy;
    } else if (current_game_state == GAME_STATE_IN_GAME_MULTIPLAYER_CLIENT) {
        dx = (game_winner == GAME_WINNER_OPPONENT) ? DISPLAY_W - host_mothership->dx : DISPLAY_W -
                                                                                       client_mothership->dx;
        dy = (game_winner == GAME_WINNER_OPPONENT) ? DISPLAY_H - host_mothership->dy : DISPLAY_H -
                                                                                       client_mothership->dy;
    } else return;

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

void draw_game_level() {
    al_draw_filled_rectangle(0, 0, DISPLAY_W, DISPLAY_H, al_map_rgb(0, 0, 0));
    al_draw_textf(main_font_size_45, al_map_rgb(255, 255, 255), DISPLAY_W / 2, DISPLAY_H / 2,
                  ALLEGRO_ALIGN_CENTER, "LEVEL %d", game_level);
}

void draw_game_over() {
    al_draw_filled_rectangle(0, 0, DISPLAY_W, DISPLAY_H, al_map_rgb(0, 0, 0));

    switch (current_game_state) {
        case GAME_STATE_IN_GAME_MULTIPLAYER_HOST:
        case GAME_STATE_IN_GAME_SINGLE_PLAYER:
            if (game_winner == GAME_WINNER_PLAYER) {
                al_draw_text(main_font_size_45, al_map_rgb(0, 255, 0), DISPLAY_W / 2, DISPLAY_H / 2,
                             ALLEGRO_ALIGN_CENTER, "VOCÊ GANHOU");
            } else {
                if (is_single_player()) {
                    al_draw_text(main_font_size_45, al_map_rgb(255, 255, 255), DISPLAY_W / 2,
                                 DISPLAY_H / 2 - +main_font_size_45_height - 10,
                                 ALLEGRO_ALIGN_CENTER, "FIM DE JOGO");
                    al_draw_textf(main_font_size_45, al_map_rgb(255, 255, 255), DISPLAY_W / 2,
                                  DISPLAY_H / 2,
                                  ALLEGRO_ALIGN_CENTER, "SEUS PONTOS: %li", player_score);
                } else {
                    al_draw_text(main_font_size_45, al_map_rgb(255, 0, 0), DISPLAY_W / 2, DISPLAY_H / 2,
                                 ALLEGRO_ALIGN_CENTER, "VOCÊ PERDEU");
                }
            }
            break;
        case GAME_STATE_IN_GAME_MULTIPLAYER_CLIENT:
            if (game_winner == GAME_WINNER_OPPONENT) {
                al_draw_text(main_font_size_45, al_map_rgb(0, 255, 0), DISPLAY_W / 2, DISPLAY_H / 2,
                             ALLEGRO_ALIGN_CENTER, "VOCÊ GANHOU");
            } else {
                al_draw_text(main_font_size_45, al_map_rgb(255, 0, 0), DISPLAY_W / 2, DISPLAY_H / 2,
                             ALLEGRO_ALIGN_CENTER, "VOCÊ PERDEU");
            }
            break;
        default:
            break;
    }
}

void on_new_level(short level) {
    game_level = level;

    int state_mod = (is_single_player()) ? 1 : 2;

    remaining_words_to_next_level = game_level * state_mod * 10;

    wait_new_level = false;

    word_pool_index = ((level - 1) * 50);

    if (is_single_player()) {
        consecutive_right_key_player = 0;
        consecutive_right_key_opponent = 0;
        clear_ships();
        need_to_show_game_level = true;
    }
}

void start_level_change() {
    wait_new_level = true;
}

void draw_score() {
    long score = (is_multiplayer_client()) ? opponent_score : player_score;

    al_draw_textf(main_font_size_25, al_map_rgb(255, 255, 255), 10,
                  DISPLAY_H - al_get_font_line_height(main_font_size_25) - 10,
                  ALLEGRO_ALIGN_LEFT, "PONTOS %06li", score);
}

void draw_purchase_life() {
    al_draw_filled_rectangle(0, 0, DISPLAY_W, DISPLAY_H, al_map_rgb(0, 0, 0));

    al_draw_text(main_font_size_45, al_map_rgb(255, 255, 255), DISPLAY_W / 2, 300,
                 ALLEGRO_ALIGN_CENTER, "DESEJA CONTINUAR?");
    al_draw_textf(main_font_size_25, al_map_rgb(255, 255, 255),
                  DISPLAY_W / 2, 300 + main_font_size_45_height + 20,
                  ALLEGRO_ALIGN_CENTER, "PREÇO: %d PONTOS", life_price_for_player);

    draw_button(purchase_buttons[0]);
    draw_button(purchase_buttons[1]);
}

void on_redraw_game() {

    static int frame_count = 0;
    static int game_level_display_frame = 0;
    static int game_ending_frame = 0;
    static int wait_new_level_frame = 0;
    static int pre_purchase_life_frame = 0;

    if (is_multiplayer_client() && !received_first_snapshot) return;

    if (need_to_show_game_level) {
        draw_game_level();
        if (game_level_display_frame++ > 120) {
            need_to_show_game_level = false;
            game_level_display_frame = 0;
        }
        return;
    }

    if (need_to_show_purchase_life) {
        if (pre_purchase_life_frame >= 192) {
            if (pre_purchase_life_frame == 192) {
                current_game_flow_state = GAME_FLOW_STATE_PURCHASING_LIFE;
                pre_purchase_life_frame++;
            }
            draw_purchase_life();
            draw_score();
            return;
        } else {
            pre_purchase_life_frame++;
        }
    } else {
        pre_purchase_life_frame = 0;
    }


    if (is_multiplayer_host() || is_single_player()) {

        if (!is_game_paused() && !is_game_ending()) {

            if (frame_count++ == 30) {
                frame_count = 0;
                update_word_pool(true);
            }

            if (next_host_ship_spawn-- == 0) {
                spawn_ship(BATTLESHIP_OWNER_PLAYER, BATTLESHIP_CLASS_MISSILE);
                next_host_ship_spawn = (rand() % (SPAWN_WINDOW + 1)) + MINIMUM_SPAWN_WAIT;
            }

            if (next_client_ship_spawn-- == 0) {
                spawn_ship(BATTLESHIP_OWNER_OPPONENT, BATTLESHIP_CLASS_MISSILE);
                next_client_ship_spawn = (rand() % (SPAWN_WINDOW + 1)) + MINIMUM_SPAWN_WAIT;
            }


            if (DEBUG) {
                al_draw_line(5, 5, 5, 5 + dictionary_len / 2, al_map_rgb(255, 255, 153), 2);
                al_draw_line(5, 5 + word_pool_start_pos / 2, 5, 5 + word_pool_end_pos / 2, al_map_rgb(0, 0, 255), 2);
            }

            if (!is_game_ending()) {

                update_game_ships_frame_count();
                if (!need_to_show_purchase_life) {
                    move_game_ships();
                }

            }

        }

        if (is_multiplayer_host()) {
            update_snapshot_from_game();
            ready_to_send = true;
        }

    } else if (is_multiplayer_client()) {
        update_game_from_snapshot();
    }

    draw_game_ships();

    draw_score();

    if (is_game_paused()) {
        draw_pause_overlay();
    }

    if (need_to_show_purchase_life && pre_purchase_life_frame < 192) {
        char tmp_gw = game_winner;
        game_winner = GAME_WINNER_OPPONENT;
        draw_explosions();
        game_winner = tmp_gw;
    }

    if (is_game_ending()) {
        game_ending_frame++;
        if (!draw_explosions_on_game_end) {
            game_ending_frame = (game_ending_frame < 120) ? 120 : game_ending_frame;
        }
        if (game_ending_frame < 120) {
            draw_explosions();
        } else {
            draw_game_over();
            if (game_ending_frame >= 300) {
                game_ending_frame = 0;
                end_game();
            }

        }
    }

    if (wait_new_level) {
        if (is_multiplayer() || wait_new_level_frame++ == 60) {
            wait_new_level_frame = 0;
            on_new_level(++game_level);
        }
    }


}

void end_game() {
    if (is_single_player()) {
        rank_score = player_score;
        change_game_state(GAME_STATE_VISUALIZING_RANK);
    } else {
        change_game_state(GAME_STATE_MAIN_MENU);
        if (is_multiplayer_client()) {
            disconnect_client();
        } else if (is_multiplayer_host()) {
            stop_server();
        }
    }
}
