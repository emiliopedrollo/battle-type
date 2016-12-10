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

BATTLESHIP *host_missiles[NUMBER_OF_SHIPS_PER_PLAYER];
BATTLESHIP *client_missiles[NUMBER_OF_SHIPS_PER_PLAYER];
BATTLESHIP *host_ship;
BATTLESHIP *client_ship;
Button purchase_buttons[2];

char **dictionary;
int dictionary_len;
char host_target = -1, client_target = -1;
int host_missile_count = 0;
int client_missile_count = 0;
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

void init_ships();

void init_purchase_buttons();

void update_game_missiles_frame_count();

void move_game_missiles_and_ships();

void draw_game_missiles_and_ships();

void spawn_missile(BATTLESHIP_OWNER owner);

void update_ship_or_missile(BATTLESHIP *battleship, SERIAL_BATTLESHIP serial_battleship);

char *get_word_from_pool(BATTLESHIP_OWNER owner);

bool exist_missile_starting_with(char letter, BATTLESHIP_OWNER targets);

char get_index_of_missile_starting_with(char letter, BATTLESHIP_OWNER targets);

char get_index_from_closest_missile(BATTLESHIP_OWNER targets);

void update_word_pool(bool pump_word_index);

void on_explosion_end(BATTLESHIP_OWNER *owner);

void draw_pause_overlay();

void draw_explosions();

void draw_game_level();

void draw_game_over();

void start_level_change();

void clear_missiles();

void on_new_level(short level);

void on_char_typed(PLAYER player, char key);

SERIAL_BATTLESHIP convert_ship_or_missile_to_serial(BATTLESHIP *battleship);

void update_snapshot_from_game();

void update_game_from_snapshot();

bool purchase_life(PLAYER player);

void try_auto_purchase_life(PLAYER player);

void update_score(PLAYER player, bool up);

void on_button_click_game(int i);

void take_a_life(PLAYER player);

void end_game();

long get_last_game_score() {
    // Retorna a pontuação do ultimo
    return rank_score;
}

void reset_last_game_score() {
    // Redefine para -1 a pontuação do último jogo
    rank_score = -1;
}

void load_resources_game() {

    // Abre arquivo "dictionary" para leitura apenas
    FILE *dictionary_file = fopen("dictionary", "r");

    if (dictionary_file == NULL) {
        // Exibe mensagem de erro em `stderr` e encerra a execução do
        // jogo caso não consiga
        fprintf(stderr, "Could not found dictionary file!");
        al_show_native_message_box(display, "Error", "File missing",
                                   "Could not found dictionary file!", NULL, ALLEGRO_MESSAGEBOX_ERROR);
        exit(EXIT_FAILURE);
    }

    // Tamanho inicial para a array de palavras utilizadas no jogo (aka `dictionary`)
    int dic_size = 1000;

    // Variavel auxiliar utilizada na remoção do char \n ao final de cada leitura de linha
    char *pos;

    // Aloca espaço para 1000 ponteiros de char em memória
    dictionary = malloc(sizeof(char *) * dic_size);

    // Variavel `len` com valor 0 indica a função getline a ir até o final de linha/arquivo
    size_t len = 0;

    // Quantidade inicial de palavras em `dictionary`
    dictionary_len = 0;

    // A cada iteração inicializa com NULL o endereço para a próxima palavra, lê
    // uma linha inteira do arquivo e armazena em seu novo indicie em `dictionary`
    while (((dictionary[dictionary_len] = NULL),
            (getline(&dictionary[dictionary_len], &len, dictionary_file))) != -1) {

        // Se posição (pos) de \n na palavra recem-lida for diferente de NULL, sobrescreve com \0
        if ((pos = strchr(dictionary[dictionary_len], '\n')) != NULL) *pos = '\0';

        // Se o numero de palavras lidas alcançou o limite de posições no vetor `dictionary`,
        // duplica-se o seu valor
        if (dictionary_len++ >= dic_size) {
            dic_size *= 2;
            dictionary = realloc(dictionary, sizeof(char *) * dic_size);
        }
    }

    // Fecha o arquivo `dictionary`
    fclose(dictionary_file);

    // Carrega cada um dos 16 bitmaps dos frames utilizados nas explosões

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
    // Libera memória utilizada para o `dictionary`
    for (int i = 0; i < dictionary_len; i++) {
        free(dictionary[i]);
    }
    free(dictionary);

    // Libera da memória os bitmaps dos frames de explosões
    for (int i = 0; i < 16; i++) {
        al_destroy_bitmap(rsc_explosion[i]);
    }
}

void init_game() {
    // (Re)define o estado de fluxo de jogo como rodando
    current_game_flow_state = GAME_FLOW_STATE_RUNNING;

    // (Re)define veriaveis de controle de jogo
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

    // Inicializa botões de escolha de compra de vida
    init_purchase_buttons();

    // Inicializa naves
    init_ships();

    // Redefine todos os espaços de missies, removendo-os do jogo
    clear_missiles();

    // Define variáveis de controle de nivel de jogo para o nivel 1
    on_new_level(1);
}

void init_purchase_buttons() {
    // Inicializa botões de escolha de compra (ou não) de vida
    purchase_buttons[0] = init_button(main_font_size_45, "&Sim", (DISPLAY_W / 4) + 25, DISPLAY_H / 2 + 100, 180);
    purchase_buttons[1] = init_button(main_font_size_45, "&Nao", (DISPLAY_W / 4) * 3 - 25, DISPLAY_H / 2 + 100, 180);
}

void clear_missiles() {
    // Cada missil inicializado é marcado como inativo
    for (int i = 0; i < NUMBER_OF_SHIPS_PER_PLAYER; i++) {
        if (host_missiles[i]) {
            host_missiles[i]->active = false;
        }
        if (client_missiles[i]) {
            client_missiles[i]->active = false;
        }
    }

    // (Re)define a contagem de misseis para cada lado do jogo
    client_missile_count = 0;
    host_missile_count = 0;

    // (Re)define o alvo selecionado em cada lado do jogo para nada
    host_target = -1;
    client_target = -1;
}

void init_ships() {

    // Tamanho em pixels da imagem da nave utilizada no jogo
    int ship_height = get_battleship_height(BATTLESHIP_CLASS_SPACESHIP);

    // (Re)define as propriedades da nave do jogador
    host_ship = init_battleship(BATTLESHIP_CLASS_SPACESHIP,
                                BATTLESHIP_OWNER_PLAYER, DISPLAY_W / 2, DISPLAY_H - ship_height, 0, 1);

    // (Re)define as propriedades da nave do adversário (ou da CPU)
    client_ship = init_battleship(BATTLESHIP_CLASS_SPACESHIP,
                                  BATTLESHIP_OWNER_OPPONENT, DISPLAY_W / 2, ship_height, 0, 1);

    // (Re)define o tipo de movimento das naves para "in_game"
    change_battleship_state(host_ship, BATTLESHIP_MOVE_STATE_IN_GAME);
    change_battleship_state(client_ship, BATTLESHIP_MOVE_STATE_IN_GAME);

    // Se for jogador cliente de rede inicializa por padão todas os espaços
    // para misseis para evitar problemas no futuro
    if (is_multiplayer_client()) {
        for (int i = 0; i < NUMBER_OF_SHIPS_PER_PLAYER; i++) {
            host_missiles[i] = init_battleship(BATTLESHIP_CLASS_MISSILE, BATTLESHIP_OWNER_PLAYER,
                                               0, 0, client_ship->dx, 1);
            client_missiles[i] = init_battleship(BATTLESHIP_CLASS_MISSILE, BATTLESHIP_OWNER_OPPONENT,
                                                 0, 0, host_ship->dx, 1);
        }
    }

    // Define limites de deslocamento dos misseis, aonde eles explodem
    game_bs_host_limit = (int) get_bottom_dy(client_ship);
    game_bs_client_limit = (int) get_top_dy(host_ship);

}

void on_explosion_end(BATTLESHIP_OWNER *owner) {

    // Decrementa o contador de misseis em jogo
    if (*owner == BATTLESHIP_OWNER_OPPONENT) {
        client_missile_count--;
    } else {
        host_missile_count--;
    }

    if (is_single_player()) {
        // Se estiver rodando em single player, não faltarem naves para serem
        // carregadas neste nivel e não houverem mais naves em jogo então muda de nivel
        if ((remaining_words_to_next_level <= 0) &&
            (client_missile_count == 0) && (host_missile_count == 0)) {
            start_level_change();
        }
    } else {
        // Em modo multiplayer a transação de nivel não exige que não haja mais naves na tela
        if (remaining_words_to_next_level <= 0) {
            start_level_change();
        }
    }

}

void spawn_missile(BATTLESHIP_OWNER owner) {
    // Este método inicializa um missil logo abaixo da nava que o está disparando

    // Quando `PITTHAN_MODE` estiver ativo e o jogo estiver em modo single player
    // então o jogador não disparaŕa missies
    if (PITTHAN_MODE && (owner == BATTLESHIP_OWNER_PLAYER) && is_single_player()) return;

    // Variavel com contagem de misseis em jogo do jogador que está disparando missel
    int missile_count = (owner == BATTLESHIP_OWNER_PLAYER) ? host_missile_count : client_missile_count;

    // Caso o numero de missies do jogador/adversário tenha em tela estiver alcançado
    // a quantidade máxima este método não faz nada
    if (missile_count >= NUMBER_OF_SHIPS_PER_PLAYER) return;

    // Decramenta o numero de palavras restantes para terminar o nivel e caso não
    // faltem palavras a serem carregadas no atual nivel de jogo então este método é finalizado
    if (remaining_words_to_next_level-- <= 0) return;

    // Variaveis de posicionamento inicial para o missil
    float dx = (owner == BATTLESHIP_OWNER_OPPONENT) ? client_ship->dx : host_ship->dx;
    float dy = (owner == BATTLESHIP_OWNER_OPPONENT) ? client_ship->dy : host_ship->dy;

    // Variavel da posição lateral do alvo do missil
    float x = (owner == BATTLESHIP_OWNER_OPPONENT) ? host_ship->dx : client_ship->dx;

    // Inicializa o missil
    BATTLESHIP *missile = init_battleship(BATTLESHIP_CLASS_MISSILE, owner, dx, dy, x, game_level);

    // Define callback para ser executado depois que acabar a animação de explosão do missil
    missile->on_explosion_end = on_explosion_end;

    // Define o estado de movimento do missil para "in_game"
    change_battleship_state(missile, BATTLESHIP_MOVE_STATE_IN_GAME);

    // Carrega uma palavra do pool de palavras para o missil
    missile->word = get_word_from_pool(owner);

    // Encontra uma entrada vaga na array de misseis do jogador correto e aloca o
    // novo missil nesta posição
    for (int i = 0; i < NUMBER_OF_SHIPS_PER_PLAYER; i++) {
        if (owner == BATTLESHIP_OWNER_PLAYER) {
            if (!host_missiles[i] || !host_missiles[i]->active) {
                host_missiles[i] = missile;
                host_missile_count++;
                break;
            }
        } else {
            if (!client_missiles[i] || !client_missiles[i]->active) {
                client_missiles[i] = missile;
                client_missile_count++;
                break;
            }
        }
    }
}

void update_game_missiles_frame_count() {
    // Atualiza o contador de frames da explosão de cada
    // um dos misseis em jogo
    for (int i = 0; i < NUMBER_OF_SHIPS_PER_PLAYER; i++) {
        if (host_missiles[i] && host_missiles[i]->active) {
            update_ship_frame_count(host_missiles[i]);
        }
        if (client_missiles[i] && client_missiles[i]->active) {
            update_ship_frame_count(client_missiles[i]);
        }
    }
}

void move_game_missiles_and_ships() {

    bool kill_opponent = false;
    bool kill_player = false;

    //Move os misseis do jogador
    move_ship(host_ship, 0);
    for (int i = 0; i < NUMBER_OF_SHIPS_PER_PLAYER; i++) {
        if (host_missiles[i] && host_missiles[i]->active) {
            // Se o missil atinge o limite de deslocamento vertical (retorno
            // true) então mata o oponente
            kill_opponent = move_ship(host_missiles[i], client_ship->dx) || kill_opponent;
        }
    }

    if (kill_opponent) {
        take_a_life(PLAYER_CLIENT);
    }

    //Move os misseis do oponente
    move_ship(client_ship, 0);
    for (int i = 0; i < NUMBER_OF_SHIPS_PER_PLAYER; i++) {
        if (client_missiles[i] && client_missiles[i]->active) {
            // Se o missil atinge o limite de deslocamento vertical (retorno
            // true) então mata o jogador
            kill_player = move_ship(client_missiles[i], host_ship->dx) || kill_player;
        }
    }

    if (kill_player) {
        take_a_life(PLAYER_HOST);
    }
}

void take_a_life(PLAYER player) {
    if (player == PLAYER_CLIENT) {
        // Se o jogador que perdeu uma vida for o cliente de uma jogo
        // multiplayer e ele ficar sem vidas então o host é definido como vencedor
        if (--opponent_lives <= 0) game_winner = GAME_WINNER_PLAYER;
    } else {
        if (--player_lives <= 0) {
            if (is_multiplayer()) {
                // Se estiver em modo multiplayer quem ganha é o adversário
                // se o host ficar sem vidas
                game_winner = GAME_WINNER_OPPONENT;
            } else if (player_score < life_price_for_player) {
                // Se for single player e o jogador não tiver pontos para comprar
                // nova vida então ele perde o jogo (o oponente CPU ganha)
                game_winner = GAME_WINNER_OPPONENT;
            } else {
                // Define flag para mostrar tela de compra de vida
                need_to_show_purchase_life = true;
            }
        }
    }

    // Se alguem ganhou (ou perdeu) o jogo muda de estado de fluxo
    if (game_winner != -1) {
        current_game_flow_state = GAME_FLOW_STATE_ENDING;
    }
}

void draw_game_missiles_and_ships() {
    // Desenha cada uma dos misseis do jogo, alternando a ordem de desenho
    // dependendo do jogador (cliente em jogo multiplayer ou não)
    for (int i = 0; i < NUMBER_OF_SHIPS_PER_PLAYER; i++) {
        if (is_multiplayer_client()) {
            if (client_missiles[i] && client_missiles[i]->active) draw_ship(client_missiles[i]);
            if (host_missiles[i] && host_missiles[i]->active) {
                draw_ship(host_missiles[i]);
                if (!is_game_paused())
                    draw_ship_word(host_missiles[i], false);
            }
        } else {
            if (host_missiles[i] && host_missiles[i]->active) draw_ship(host_missiles[i]);
            if (client_missiles[i] && client_missiles[i]->active) {
                draw_ship(client_missiles[i]);
                if (!is_game_paused())
                    draw_ship_word(client_missiles[i], false);
            }
        }
    }

    // Desenha o alvo no missil adequado e redesenha a palavra
    // correspondente em cor diferente
    switch (current_game_state) {
        case GAME_STATE_IN_GAME_SINGLE_PLAYER:
        case GAME_STATE_IN_GAME_MULTIPLAYER_HOST:
            if (host_target != -1 && client_missiles[host_target] &&
                client_missiles[host_target]->active) {
                draw_target_lock(client_missiles[host_target]);
                if (!is_game_paused())
                    draw_ship_word(client_missiles[host_target], true);
            }
            break;
        case GAME_STATE_IN_GAME_MULTIPLAYER_CLIENT:
            if (client_target != -1 && host_missiles[client_target] &&
                host_missiles[client_target]->active) {
                draw_target_lock(host_missiles[client_target]);
                if (!is_game_paused())
                    draw_ship_word(host_missiles[client_target], true);
            }
            break;
        default:
            break;
    }

    // Desenha as naves
    draw_ship(host_ship);
    draw_ship(client_ship);


    for (int i = 0; i < NUMBER_OF_SHIPS_PER_PLAYER; i++) {
        if (client_missiles[i] && client_missiles[i]->active && client_missiles[i]->exploding &&
            !client_missiles[i]->exploding_with_lasers)
            draw_ship(client_missiles[i]);
        if (host_missiles[i] && host_missiles[i]->active && host_missiles[i]->exploding &&
            !host_missiles[i]->exploding_with_lasers)
            draw_ship(host_missiles[i]);
    }
}

void update_game_from_snapshot() {
    // Carrega estado de jogo recebido via rede

    for (int i = 0; i < NUMBER_OF_SHIPS_PER_PLAYER; i++) {
        update_ship_or_missile(host_missiles[i], game.host_ships[i]);
        update_ship_or_missile(client_missiles[i], game.client_ships[i]);
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

    host_ship->dx = game.host_ship_dx;
    client_ship->dx = game.client_ship_dx;
}

void update_ship_or_missile(BATTLESHIP *battleship, SERIAL_BATTLESHIP serial_battleship) {
    // Atualiza propriedades de um missil/nave com as propriedades recebidas via rede

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
//        battleship->word = malloc(sizeof word);
        battleship->word = strdup(word);

//        strcpy(battleship->word,word);

    } else {
        if (battleship) battleship->active = false;
    }

}

SERIAL_BATTLESHIP convert_ship_or_missile_to_serial(BATTLESHIP *battleship) {
    // Prepara propriedades de um missil/nave para serem enviados via rede

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
    // Prepara estado do jogo para ser enviado pela rede

    for (int i = 0; i < NUMBER_OF_SHIPS_PER_PLAYER; i++) {
        if (host_missiles[i] && host_missiles[i]->active) {
            game.host_ships[i] = convert_ship_or_missile_to_serial(host_missiles[i]);
        } else game.host_ships[i].active = false;

        if (client_missiles[i] && client_missiles[i]->active) {
            game.client_ships[i] = convert_ship_or_missile_to_serial(client_missiles[i]);
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

    game.host_ship_dx = (unsigned short) host_ship->dx;
    game.client_ship_dx = (unsigned short) client_ship->dx;
}

void update_word_pool(bool pump_word_index) {
    // Atualiza intervalo de posições de palavras que podem ser
    // escolhidas do `dictionary`

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

bool exist_missile_starting_with(char letter, BATTLESHIP_OWNER targets) {
    // Retorna true se existe um missil começando com um determinado caractere
    // entre os misseis de um determinado jogador
    return (get_index_of_missile_starting_with(letter, targets) != -1);
}

char get_index_of_missile_starting_with(char letter, BATTLESHIP_OWNER targets) {
    // Retorna o indicie do mais pŕoximo missil começando com um determinado
    // caractere entre os misseis de um determinado jogador
    BATTLESHIP *ship;
    char index = -1;

    for (char i = 0; i < NUMBER_OF_SHIPS_PER_PLAYER; i++) {
        ship = (targets == BATTLESHIP_OWNER_OPPONENT) ? client_missiles[i] : host_missiles[i];
        if (!ship || !ship->active || !ship->word) continue;
        if (get_next_letter_from_battleship(ship) == letter) {
            if (index == -1) {
                index = i;
            } else {
                if (targets == BATTLESHIP_OWNER_OPPONENT) {
                    // maior Y | mais abaixo
                    index = (client_missiles[i]->dy > client_missiles[index]->dy) ? i : index;
                } else {
                    // menor Y | mais acima
                    index = (host_missiles[i]->dy < host_missiles[index]->dy) ? i : index;
                }
            }
        };
    }
    return index;
}

char get_index_from_closest_missile(BATTLESHIP_OWNER targets) {
    // Retorna o indicie do missil mais pŕoximo de um determinado jogador

    BATTLESHIP *ship;
    char index = -1;

    for (char i = 0; i < NUMBER_OF_SHIPS_PER_PLAYER; i++) {
        ship = (targets == BATTLESHIP_OWNER_OPPONENT) ? client_missiles[i] : host_missiles[i];
        if (!ship || !ship->active || !ship->word) continue;
        if (index == -1) {
            index = i;
        } else {
            if (targets == BATTLESHIP_OWNER_OPPONENT) {
                // maior Y | mais abaixo
                index = (client_missiles[i]->dy > client_missiles[index]->dy) ? i : index;
            } else {
                // menor Y | mais acima
                index = (host_missiles[i]->dy < host_missiles[index]->dy) ? i : index;
            }
        }
    }
    return index;
}

char *get_word_from_pool(BATTLESHIP_OWNER owner) {
    // Pega uma palavra aleatoria entre os limites do pool do `dictionary`
    int pool_size = word_pool_end_pos - word_pool_start_pos;
    char *word = malloc(strlen(dictionary[dictionary_len]) + 1);
    int tries = 0;

    do {
        strcpy(word, dictionary[word_pool_start_pos + (rand() % (pool_size + 1))]);
        if (tries++ % 5 == 0) update_word_pool(false);
    } while (exist_missile_starting_with(get_next_ascii_char(word), owner) && (tries < 50));

    return word;
}

void on_key_press_game(ALLEGRO_KEYBOARD_EVENT event) {
    // Processa evento do teclado

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
    // Normaliza o char pressionado pelo jogador e o envia para processamento
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
    // Processa o pressionar de uma letra por um determinado jogador

    if (is_game_ending()) return;
    BATTLESHIP *battleship = NULL;
    if (key != 0) {
        char next_letter = 0;
        switch (player) {
            case PLAYER_SINGLE:
            case PLAYER_HOST:
                if (host_target == -1) {
                    host_target = get_index_of_missile_starting_with(key, BATTLESHIP_OWNER_OPPONENT);
                }
                if (host_target != -1) {
                    battleship = client_missiles[host_target];
                }
                break;
            case PLAYER_CLIENT:
                if (client_target == -1) {
                    client_target = get_index_of_missile_starting_with(key, BATTLESHIP_OWNER_PLAYER);
                }
                if (client_target != -1) {
                    battleship = host_missiles[client_target];
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
    // Compra uma vida para um determinado jogador

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
    // Dependendo das condições de jogo compra-se uma vida automaticamente

    if (player == PLAYER_CLIENT) {
        if (is_multiplayer() || (is_single_player() && opponent_score > player_score))
            purchase_life(player);
    } else {
        if (is_multiplayer())
            purchase_life(player);
    }
}

void update_score(PLAYER player, bool up) {
    // Atualiza a pontuação do(s) jogador(es)

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
    // Retorna true se o jogo estiver executando em modo single player
    return current_game_state == GAME_STATE_IN_GAME_SINGLE_PLAYER;
}

bool is_multiplayer_host() {
    // Retorna true se o jogo estiver executando em modo
    // multiplayer e o jogador for o host
    return current_game_state == GAME_STATE_IN_GAME_MULTIPLAYER_HOST;
}

bool is_multiplayer_client() {
    // Retorna true se o jogo estiver executando em modo
    // multiplayer e o jogador for o client
    return current_game_state == GAME_STATE_IN_GAME_MULTIPLAYER_CLIENT;
}

bool is_multiplayer() {
    // Retorna true se o jogo estiver executando em modo multiplayer
    return (current_game_state == GAME_STATE_IN_GAME_MULTIPLAYER_HOST) ||
           (current_game_state == GAME_STATE_IN_GAME_MULTIPLAYER_CLIENT);
}

bool is_game_paused() {
    // Retorna true se o estado de fluxo do jogo for pause
    if (is_game_ending()) {
        return false;
    } else
        return is_single_player() && current_game_flow_state == GAME_FLOW_STATE_PAUSE;
}

bool is_game_ending() {
    // Retorna true se o estado de fluxo do jogo indicar que o jogo está terminado
    return current_game_flow_state == GAME_FLOW_STATE_ENDING;
}

void on_mouse_move_game(int x, int y) {
    // Processa movimento do mouse (para o pressionar dos botões
    // da escolha de compra de vida)

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
    // Processa clique de um botão, para determinar se
    // compra-se ou não uma vida par ao jogador
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
    // Processa clique do mouse (para o pressionar dos botões
    // da escolha de compra de vida)
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
    // Processa clique do mouse (para o pressionar dos botões
    // da escolha de compra de vida)
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
        // Se o jogo está esperando uma resposta para a
        // compra de vida (ou não) pelo jogador, mas ele ja escolheu pressionando
        // uma tecla de atalho, executa o clique naquele botão automaticamente

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
        // Dependendo das condições de jogo executa a IA do oponente

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
            client_target = get_index_from_closest_missile(BATTLESHIP_OWNER_PLAYER);
            retarget_action_cont = game_level / 3 - 1;
        }

        if (client_target != -1) {
            on_char_typed(PLAYER_CLIENT, get_next_letter_from_battleship(host_missiles[client_target]));
        }

    }


}

void draw_pause_overlay() {
    // Desenha o overlay da tela de pause

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
    // Desenha as explosões sobre uma das naves

    float dx, dy;

    if (current_game_state == GAME_STATE_IN_GAME_MULTIPLAYER_HOST ||
        current_game_state == GAME_STATE_IN_GAME_SINGLE_PLAYER) {
        dx = (game_winner == GAME_WINNER_OPPONENT) ? host_ship->dx : client_ship->dx;
        dy = (game_winner == GAME_WINNER_OPPONENT) ? host_ship->dy : client_ship->dy;
    } else if (current_game_state == GAME_STATE_IN_GAME_MULTIPLAYER_CLIENT) {
        dx = (game_winner == GAME_WINNER_OPPONENT) ? DISPLAY_W - host_ship->dx : DISPLAY_W -
                                                                                 client_ship->dx;
        dy = (game_winner == GAME_WINNER_OPPONENT) ? DISPLAY_H - host_ship->dy : DISPLAY_H -
                                                                                 client_ship->dy;
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
    // Desenha tela de mudança de nivel

    al_draw_filled_rectangle(0, 0, DISPLAY_W, DISPLAY_H, al_map_rgb(0, 0, 0));
    al_draw_textf(main_font_size_45, al_map_rgb(255, 255, 255), DISPLAY_W / 2, DISPLAY_H / 2,
                  ALLEGRO_ALIGN_CENTER, "LEVEL %d", game_level);
}

void draw_game_over() {
    // Desenha tela de fim de jogo

    al_draw_filled_rectangle(0, 0, DISPLAY_W, DISPLAY_H, al_map_rgb(0, 0, 0));

    switch (current_game_state) {
        case GAME_STATE_IN_GAME_MULTIPLAYER_HOST:
        case GAME_STATE_IN_GAME_SINGLE_PLAYER:
            if (is_single_player()) {
                al_draw_text(main_font_size_45, al_map_rgb(255, 255, 255), DISPLAY_W / 2,
                             DISPLAY_H / 2 - +main_font_size_45_height - 10,
                             ALLEGRO_ALIGN_CENTER, "FIM DE JOGO");
                al_draw_textf(main_font_size_45, al_map_rgb(255, 255, 255), DISPLAY_W / 2,
                              DISPLAY_H / 2,
                              ALLEGRO_ALIGN_CENTER, "SEUS PONTOS: %li", player_score);
            } else if (game_winner == GAME_WINNER_PLAYER) {
                al_draw_text(main_font_size_45, al_map_rgb(255, 0, 0), DISPLAY_W / 2, DISPLAY_H / 2,
                             ALLEGRO_ALIGN_CENTER, "VOCÊ GANHOU");
            } else {
                al_draw_text(main_font_size_45, al_map_rgb(255, 0, 0), DISPLAY_W / 2, DISPLAY_H / 2,
                             ALLEGRO_ALIGN_CENTER, "VOCÊ PERDEU");
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
    // Prepara diversas viriaveis para o próximo nivel

    game_level = level;

    int state_mod = (is_single_player()) ? 1 : 2;

    remaining_words_to_next_level = game_level * state_mod * 10;

    wait_new_level = false;

    word_pool_index = ((level - 1) * 50);

    if (is_single_player()) {
        consecutive_right_key_player = 0;
        consecutive_right_key_opponent = 0;
        clear_missiles();
        need_to_show_game_level = true;
    }
}

void start_level_change() {
    // Define variavel que fará com que o jogo espere
    // antes de passar de nivel
    wait_new_level = true;
}

void draw_score() {
    // Desenha pontuação no canto da tela
    long score = (is_multiplayer_client()) ? opponent_score : player_score;

    al_draw_textf(main_font_size_25, al_map_rgb(255, 255, 255), 10,
                  DISPLAY_H - al_get_font_line_height(main_font_size_25) - 10,
                  ALLEGRO_ALIGN_LEFT, "PONTOS %06li", score);
}

void draw_purchase_life() {
    // Desenha tela de compra de vida
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
    // Redesenha a tela do jogo

    static int frame_count = 0;
    static int game_level_display_frame = 0;
    static int game_ending_frame = 0;
    static int wait_new_level_frame = 0;
    static int pre_purchase_life_frame = 0;

    // Se é cliente em jogo multiplayer e ainda não recebeu primeiro pacote não faz nada
    if (is_multiplayer_client() && !received_first_snapshot) return;

    if (need_to_show_game_level) {
        // Desenha tela de novo nivel por 120 frames
        draw_game_level();
        if (game_level_display_frame++ > 120) {
            need_to_show_game_level = false;
            game_level_display_frame = 0;
        }
        return;
    }

    if (need_to_show_purchase_life) {
        // Espera 192 para mostrar explosões e então
        // desenha tela de compra de vida
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
        // Se for single player ou host em multiplayer processa
        // criação de misseis e movimento dos objetos na tela

        if (!is_game_paused() && !is_game_ending()) {
            // Apenas se o jogo não estiver pausado ou acabando

            if (frame_count++ == 30) {
                frame_count = 0;
                update_word_pool(true);
            }

            if (next_host_ship_spawn-- == 0) {
                spawn_missile(BATTLESHIP_OWNER_PLAYER);
                next_host_ship_spawn = (rand() % (SPAWN_WINDOW + 1)) + MINIMUM_SPAWN_WAIT;
            }

            if (next_client_ship_spawn-- == 0) {
                spawn_missile(BATTLESHIP_OWNER_OPPONENT);
                next_client_ship_spawn = (rand() % (SPAWN_WINDOW + 1)) + MINIMUM_SPAWN_WAIT;
            }


            if (DEBUG) {
                al_draw_line(5, 5, 5, 5 + dictionary_len / 2, al_map_rgb(255, 255, 153), 2);
                al_draw_line(5, 5 + word_pool_start_pos / 2, 5, 5 + word_pool_end_pos / 2, al_map_rgb(0, 0, 255), 2);
            }

            if (!is_game_ending()) {

                update_game_missiles_frame_count();
                if (!need_to_show_purchase_life) {
                    move_game_missiles_and_ships();
                }

            }

        }

        if (is_multiplayer_host()) {
            // Se o jogo for em multiplayer prepara para enviar estado de jogo pela rede
            update_snapshot_from_game();
            ready_to_send = true;
        }

    } else if (is_multiplayer_client()) {
        // Se o jogo for em multiplayer e o jogador for client atualiza estado de
        // jogo com informações recebidas via rede
        update_game_from_snapshot();
    }

    // Desenha misseis e naves
    draw_game_missiles_and_ships();

    // Desenha pontuação
    draw_score();

    if (is_game_paused()) {
        // Se o jogo estiver pausado desenha overlay do pause
        draw_pause_overlay();
    }

    if (need_to_show_purchase_life && pre_purchase_life_frame < 192) {
        // Desenha explosões
        char tmp_gw = game_winner;
        game_winner = GAME_WINNER_OPPONENT;
        draw_explosions();
        game_winner = tmp_gw;
    }

    if (is_game_ending()) {
        // Desenha fim de jogo
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
        // Espera e exibe próximo nivel
        if (is_multiplayer() || wait_new_level_frame++ == 60) {
            wait_new_level_frame = 0;
            on_new_level(++game_level);
        }
    }


}

void end_game() {
    // Processa final de jogo

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
