#include <stdio.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_memfile.h>
#include <getopt.h>
#include "main.h"
#include "menu_screen.h"
#include "resources/img/background.png.h"
#include "resources/font/VT323.ttf.h"
#include "battleship.h"
#include "game.h"
#include "rank.h"

// A tela do jogo
ALLEGRO_DISPLAY *display = NULL;

// O bitmap do background do jogo
ALLEGRO_BITMAP *bmp_background;

// A fonte utilizada no jogo em seus 3 tamanhos
ALLEGRO_FONT *main_font_size_60;
ALLEGRO_FONT *main_font_size_45;
ALLEGRO_FONT *main_font_size_25;

// O temporizador responsável pelos eventos de repintura de tela
ALLEGRO_TIMER *timer;

// A fila de eventos pelos quais o executável ira escutar
ALLEGRO_EVENT_QUEUE *queue;

// O tamanho em pixels utilizado por cada um dos tamanhos de fonte
// utilizado no jogo
int main_font_size_60_height;
int main_font_size_45_height;
int main_font_size_25_height;

// O estado atual do jogo
GAME_STATE current_game_state;

// O estado de fluxo atual do jogo
GAME_FLOW_STATE current_game_flow_state;

// As dimensões da tela do jogo
const int DISPLAY_H = 800, DISPLAY_W = 500;

// Variavel que contem informação se o jogo está
// encerrando ou não
bool exiting = false;

// Variavel que contem informação se o jogo está
// em modo de DEBUG
bool DEBUG = false;

// Variavel que altera de leve o comportamento do
// jogo quando `true` para este se enquadrar nas
// especificações do jogo
bool PITTHAN_MODE = true;

int the_game();

int change_game_state_step_remaining = 0;
GAME_STATE changing_game_state;

void init_display();

void destroy_display();

void load_resources();

void unload_resources();

void draw_background();

void on_changed_game_state();

void do_the_loop();

void on_key_press(ALLEGRO_KEYBOARD_EVENT keyboard_event);

void on_mouse_move(int x, int y);

void on_mouse_down(int x, int y);

void on_mouse_up(int x, int y);

void on_redraw();

void create_queue();

void destroy_queue();

int main(int argc, char **argv) {
    int c;

    // Os blocos a seguir verificam a existência de parametros de
    // execução do executavel e definem variaveis que alterarão o
    // comportamento do mesmo de acordo com estes parametros
    static struct option long_options[] = {
            {"pitthan", no_argument, 0, 0},
            {"debug",   no_argument, 0, 0},
            {0, 0,                   0, 0}
    };

    while (1) {
        int option_index = 0;
        c = getopt_long(argc, argv, "",
                        long_options, &option_index);

        if (c == -1)
            break;

        if (c == 0) {
            if (long_options[option_index].name == "pitthan") {
                PITTHAN_MODE = true;
            } else if (long_options[option_index].name == "debug") {
                DEBUG = true;
            }
        }

    }

    // chama o método the_game() e repassa para a saida do
    // executável o seu retorno
    return the_game();

}


int the_game() {

    // Inicializa os módulos necessários para a execução do jogo,
    // cria uma janela para o mesmo e inicializa os diversos addons
    init_display();

    // Carrega recurssos que serão necessários durante a execução do jogo
    load_resources();

    // Define o estado do jogo para o de menu inicial
    change_game_state(GAME_STATE_MAIN_MENU);

    // Cria a fila de eventos que disparará as ações
    create_queue();

    // Realiza o loop principal até o jogo ser encerrado
    do_the_loop();

    // Desinicializa a fila de eventos
    destroy_queue();

    // Descarrega de memória os diversos recurssos utilizados (ou não) em jogo
    unload_resources();

    // Destroi a janela do jogo
    destroy_display();

    // Se a execução cheou até aqui o jogo encerrou a execução com sucesso
    return EXIT_SUCCESS;

}

void create_queue() {

    // Temporizador `timer` que será executado 60 vezes por segundo (60 FPS)
    timer = al_create_timer(1.0 / 60);

    // Criação da fila de eventos (ainda vazia)
    queue = al_create_event_queue();

    // Adiciona à fila eventos do teclado quando estes ocorrerem
    al_register_event_source(queue, al_get_keyboard_event_source());

    // Adiciona à fila eventos referentes à janela quando estes ocorrerem
    al_register_event_source(queue, al_get_display_event_source(display));

    // Adiciona à fila eventos do `timer` quando estes ocorrerem
    al_register_event_source(queue, al_get_timer_event_source(timer));

    // Adiciona à fila eventos do mouse quando estes ocorrerem
    al_register_event_source(queue, al_get_mouse_event_source());

    // Inicia o temporizador `timer`
    al_start_timer(timer);

}

void destroy_queue() {
    // Desinicializa o temporizador `timer`
    al_destroy_timer(timer);

    // Desinicializa a lista de eventos `queue`
    al_destroy_event_queue(queue);
}

void init_display() {

    // Carrega Allegro
    if (!al_init()) {
        // Ou escreve mensagem de erro e encerra o jogo em caso de falha
        fprintf(stderr, "failed to initialize allegro (error %d)\n", al_get_errno());
        printf("%0x\n%0x\n", ALLEGRO_VERSION_INT, al_get_allegro_version());
        exit(EXIT_FAILURE);
    }

    // Cria Tela
    display = al_create_display(DISPLAY_W, DISPLAY_H);
    if (!display) {
        // Ou escreve mensagem de erro e encerra o jogo em caso de falha
        fprintf(stderr, "failed to create display!\n");
        exit(EXIT_FAILURE);
    }

    // Define titulo da Tela
    al_set_window_title(display, "BattleType");

    // Instala os drivers das entradas
    al_install_mouse();
    al_install_keyboard();

    // Inicializa addons do Allegro
    al_init_image_addon();
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();

    // Limpa a tela
    al_clear_to_color(al_map_rgb(255, 255, 255));
    al_flip_display();

}

void load_resources() {

    // Carrega as imagens necessárias para a tela do menu
    ALLEGRO_FILE *background_png = al_open_memfile(img_background_png, img_background_png_len, "r");
    load_bitmap(&bmp_background, &background_png, ".png");

    // Carrega a fonte principal da aplicação nos tamanhos 25, 45 e 60
    ALLEGRO_FILE *vt323_ttf_60 = al_open_memfile(font_VT323_ttf, font_VT323_ttf_len, "r");
    ALLEGRO_FILE *vt323_ttf_45 = al_open_memfile(font_VT323_ttf, font_VT323_ttf_len, "r");
    ALLEGRO_FILE *vt323_ttf_25 = al_open_memfile(font_VT323_ttf, font_VT323_ttf_len, "r");
    load_font(&main_font_size_60, &vt323_ttf_60, 60, ALLEGRO_TTF_MONOCHROME);
    load_font(&main_font_size_45, &vt323_ttf_45, 45, ALLEGRO_TTF_MONOCHROME);
    load_font(&main_font_size_25, &vt323_ttf_25, 25, ALLEGRO_TTF_MONOCHROME);
    main_font_size_60_height = al_get_font_line_height(main_font_size_60);
    main_font_size_45_height = al_get_font_line_height(main_font_size_45);
    main_font_size_25_height = al_get_font_line_height(main_font_size_25);

    // Chama `load_resources_*` de cada modulo para que estes possam carregar recursos
    // que venham a precisar durante o jogo
    load_resources_menu_screen();
    load_resources_battleship();
    load_resources_game();

}

void load_font(ALLEGRO_FONT **font, ALLEGRO_FILE **file, int size, int flags) {
    // Carrega fonte ou imprime erro em `stderr` e encerra o programa caso não consiga
    *font = al_load_ttf_font_f(*file, NULL, size, flags);
    if (!*font) {
        fprintf(stderr, "failed to load font resource (error %d)\n", al_get_errno());
        exit(EXIT_FAILURE);
    }
}

void load_bitmap(ALLEGRO_BITMAP **bitmap, ALLEGRO_FILE **file, char *ident) {
    // Carrega imagem ou imprime erro em `stderr` e encerra o programa caso não consiga
    *bitmap = al_load_bitmap_f(*file, ident);
    if (!*bitmap) {
        fprintf(stderr, "failed to load bitmap resource (error %d)\n", al_get_errno());
        exit(EXIT_FAILURE);
    }
}

void unload_resources() {
    // Chama `unload_resources_*` de cada módulo para que estes possam executar as
    // devidas chamadas e descarregar os recursos que haviam carregado
    unload_resources_menu_screen();
    unload_resources_battleship();
    unload_resources_game();

    // Descarrega da memória a imagem do background
    al_destroy_bitmap(bmp_background);

    // Descarrega da memória a fonte utilizada no jogo em seus diversos tamanhos
    al_destroy_font(main_font_size_60);
    al_destroy_font(main_font_size_45);
    al_destroy_font(main_font_size_25);
}

void destroy_display() {
    // Descarrega os addons do Allegro
    al_shutdown_ttf_addon();
    al_shutdown_font_addon();
    al_shutdown_primitives_addon();
    al_shutdown_image_addon();

    // Desinstala os drivers das entradas
    al_uninstall_keyboard();
    al_uninstall_mouse();

    // Fecha tela
    al_destroy_display(display);
}

void change_game_state(GAME_STATE state) {

    // Define para qual estado de jogo esta mudando
    changing_game_state = state;
    // Define quantas etapas faltam concluir antes de mudar de estado
    change_game_state_step_remaining = 0;

    // Dependendo do estado atual podem existir etapas a serem concluidas antes
    // de mudar para o próximo estado, nestes casos o modulo responsavel pelo estado
    // atual ira chamar `check_game_state_complete()` quando cada etapa for fninalizada
    switch (current_game_state) {
        case GAME_STATE_MAIN_MENU:
            change_game_state_step_remaining =
                    on_game_state_changing_count_steps_menu_screen(state);
            start_game_state_change_menu_screen(state);
            break;
        default:
            break;
    }

    // Caso o estado atual não exija nenhuma etapa antes da troca, será chamado
    // o método `check_game_state_complete()` automaticamente
    if (change_game_state_step_remaining < 1) {
        check_game_state_complete();
    }

}

void on_changed_game_state() {
    // Novos módulos precisam serem iniciados para poderem funcionar corretamente,
    // este método (que é chamado no momento oportudo) executa o método de
    // inicialização do módulo que acaba de ser alterado
    switch (current_game_state) {
        case GAME_STATE_MAIN_MENU:
            init_menu_screen();
            break;
        case GAME_STATE_IN_GAME_SINGLE_PLAYER:
        case GAME_STATE_IN_GAME_MULTIPLAYER_HOST:
        case GAME_STATE_IN_GAME_MULTIPLAYER_CLIENT:
            init_game();
            break;
        case GAME_STATE_VISUALIZING_RANK:
            init_rank();
            break;
        default:
            break;
    }
}

void check_game_state_complete() {
    // Verifica se existem etapas pendentes para a troca de estado de jogo
    if (--change_game_state_step_remaining <= 0) {
        if (changing_game_state != GAME_STATE_NONE) {
            // E se não houver altera o estado do jogo
            current_game_state = changing_game_state;
            // Chamando o inicializador do módulo responsável pelo estado
            on_changed_game_state();
            changing_game_state = GAME_STATE_NONE;
        }
    }
}

void on_key_press(ALLEGRO_KEYBOARD_EVENT keyboard_event) {
    // Repassa os eventos do teclado para o módulo responsável
    // pelo estado de jogo atual
    switch (current_game_state) {
        case GAME_STATE_MAIN_MENU:
            on_key_press_menu_screen(keyboard_event);
            break;
        case GAME_STATE_IN_GAME_SINGLE_PLAYER:
        case GAME_STATE_IN_GAME_MULTIPLAYER_CLIENT:
        case GAME_STATE_IN_GAME_MULTIPLAYER_HOST:
            on_key_press_game(keyboard_event);
            break;
        case GAME_STATE_VISUALIZING_RANK:
            on_key_press_rank(keyboard_event);
            break;
        default:
            break;
    }

    // Se o jogo estiver executando em modo de `DEBUG` o jogador
    // pode aumetar ou diminuir o framerate do jogo
    if (DEBUG) {
        double timer_speed;
        if (keyboard_event.keycode == ALLEGRO_KEY_PAD_PLUS) {
            timer_speed = al_get_timer_speed(timer);
            al_set_timer_speed(timer, timer_speed * 0.5f);
        } else if (keyboard_event.keycode == ALLEGRO_KEY_PAD_MINUS) {
            timer_speed = al_get_timer_speed(timer);
            al_set_timer_speed(timer, timer_speed * 2.0f);
        }
    }
}

void on_mouse_move(int x, int y) {
    // Repassa os eventos de movimento do mouse para o módulo
    // responsável pelo estado de jogo atual
    switch (current_game_state) {
        case GAME_STATE_MAIN_MENU:
            on_mouse_move_menu_screen(x, y);
            break;
        case GAME_STATE_IN_GAME_SINGLE_PLAYER:
        case GAME_STATE_IN_GAME_MULTIPLAYER_CLIENT:
        case GAME_STATE_IN_GAME_MULTIPLAYER_HOST:
            on_mouse_move_game(x, y);
            break;
        case GAME_STATE_VISUALIZING_RANK:
            on_mouse_move_rank(x, y);
            break;
        default:
            break;
    }
}

void on_mouse_down(int x, int y) {
    // Repassa os eventos do mouse para o módulo
    // responsável pelo estado de jogo atual
    switch (current_game_state) {
        case GAME_STATE_MAIN_MENU:
            on_mouse_down_menu_screen(x, y);
            break;
        case GAME_STATE_IN_GAME_SINGLE_PLAYER:
        case GAME_STATE_IN_GAME_MULTIPLAYER_CLIENT:
        case GAME_STATE_IN_GAME_MULTIPLAYER_HOST:
            on_mouse_down_game(x, y);
            break;
        case GAME_STATE_VISUALIZING_RANK:
            on_mouse_down_rank(x, y);
            break;
        default:
            break;
    }
}

void on_mouse_up(int x, int y) {
    // Repassa os eventos do mouse para o módulo
    // responsável pelo estado de jogo atual
    switch (current_game_state) {
        case GAME_STATE_MAIN_MENU:
            on_mouse_up_menu_screen(x, y);
            break;
        case GAME_STATE_IN_GAME_SINGLE_PLAYER:
        case GAME_STATE_IN_GAME_MULTIPLAYER_CLIENT:
        case GAME_STATE_IN_GAME_MULTIPLAYER_HOST:
            on_mouse_up_game(x, y);
            break;
        case GAME_STATE_VISUALIZING_RANK:
            on_mouse_up_rank(x, y);
            break;
        default:
            break;
    }
}

void on_timer() {
    // Avisa o módulo responsável pelo estado de jogo atual
    // que um intervalo de frame está se passando (mesmo que este
    // frame acabe não sendo desenhado)
    switch (current_game_state) {
        case GAME_STATE_MAIN_MENU:
            on_timer_menu_screen();
            break;
        case GAME_STATE_IN_GAME_SINGLE_PLAYER:
        case GAME_STATE_IN_GAME_MULTIPLAYER_CLIENT:
        case GAME_STATE_IN_GAME_MULTIPLAYER_HOST:
            on_timer_game();
            break;
        case GAME_STATE_VISUALIZING_RANK:
            on_timer_rank();
        default:
            break;
    }
}

void on_redraw() {

    // Limpa a tela
    al_clear_to_color(al_map_rgb_f(0, 0, 0));

    // Indeppendente de tudo mais que estiver acontecendo no jogo,
    // redesenha o fundo (semelhante a como um jogo de Arcade funciona)
    draw_background();

    // Avisa o módulo responsável pelo estado de jogo atual que é necessário
    // repintar a tela
    switch (current_game_state) {
        case GAME_STATE_MAIN_MENU:
            on_redraw_menu_screen();
            break;
        case GAME_STATE_IN_GAME_SINGLE_PLAYER:
        case GAME_STATE_IN_GAME_MULTIPLAYER_HOST:
        case GAME_STATE_IN_GAME_MULTIPLAYER_CLIENT:
            on_redraw_game();
            break;
        case GAME_STATE_VISUALIZING_RANK:
            on_redraw_rank();
        default:
            break;
    }

    // Executa a pintura de tela com as novas instruções (flush)
    al_flip_display();
}

void do_the_loop() {

    // Evento responsável pela parada
    ALLEGRO_EVENT event;

    // Por padrão não será executada uma repintura de tela
    static bool redraw = false;

    // Este loop ficara executando até o jogo receber uma mensagem de
    // fechamento do sistema ou o usuário fechar o jogo pelo botão
    // interno
    while (!exiting) {
        // Para a execução até que ocorra algum dos eventos que a
        // variavel `queue` está escutando. Adicionalmente, quando um
        // evento occore ele é armazenado na variável `event`
        al_wait_for_event(queue, &event);

        switch (event.type) {
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                // Foi recebida uma mensagem de fechamento do executável
                // a partir de agora o loop se encerra, mas o executavel
                // continuará até estar pronto para finalizar
                exiting = true;
                break;
            case ALLEGRO_EVENT_KEY_CHAR:
                // Chama método que irá tratar e repassar o evento para o
                // módulo responsável pelo estado de jogo atual
                on_key_press(event.keyboard);
                break;
            case ALLEGRO_EVENT_TIMER:
                // Quando o evento é do tipo timer é necessário desenhar
                // um frame novo na tela
                redraw = true;
                // Além disso é chamado um método que irá tratar e repassar
                // o evento para o módulo responsável pelo estado de jogo atual
                on_timer();
                break;
            case ALLEGRO_EVENT_MOUSE_AXES:
                // Chama método que irá tratar e repassar o evento para o
                // módulo responsável pelo estado de jogo atual
                on_mouse_move(event.mouse.x, event.mouse.y);
                break;
            case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
                // Chama método que irá tratar e repassar o evento para o
                // módulo responsável pelo estado de jogo atual
                on_mouse_down(event.mouse.x, event.mouse.y);
                break;
            case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
                // Chama método que irá tratar e repassar o evento para o
                // módulo responsável pelo estado de jogo atual
                on_mouse_up(event.mouse.x, event.mouse.y);
                break;
            default:
                break;
        }

        // Verifica se é necessário redesenhar e se não há nenhum
        // evento esperando para ser processado. Como `redraw` é uma variável
        // estática, mesmo que a fila não esteja vazia agora ele continuará
        // como `true` até que a fila esteja vazia e o teste abaixo passe
        if (redraw && al_is_event_queue_empty(queue)) {
            // Como a variável `redraw` é estática se não atribuir-mos `false`
            // para ela agora quando o teste acima ocorrer novamente ele
            // pode passar mesmo que o timer não tenha disparado ainda
            redraw = false;
            // Chama método que fará a pintura do background e repassará
            // a chamada para módulo responsável pelo estado de jogo atual
            on_redraw();
        }
    }
}

void draw_background() {
    // Este método desenha o background sempre com efeito
    // paralaxe, dando a impressão de movimento
    static int x = 1, y = 1;
    int bgw = al_get_bitmap_width(bmp_background);
    int bgh = al_get_bitmap_height(bmp_background);
    int i, j = y - bgh;
    x = (x < -bgw) ? 1 : x;
    y = (y > bgh) ? 1 : y;
    x--;
    y++;
    for (i = x; i < DISPLAY_W; i += bgw) {
        al_draw_bitmap(bmp_background, i, j, 0);
        for (j = y - bgh; j < DISPLAY_H; j += bgh) {
            al_draw_bitmap(bmp_background, i, j, 0);
        }
    }
}