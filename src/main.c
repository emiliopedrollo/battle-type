//#include <stdio.h>
//#include <stdlib.h>
//#include <getopt.h>
//#include <stdbool.h>
#include <stdio.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_memfile.h>
#include <getopt.h>
#include <allegro5/bitmap.h>
#include <allegro5/file.h>
#include "main.h"
#include "menu_screen.h"
#include "resources/img/background.png.h"
#include "resources/font/VT323.ttf.h"
#include "battleship.h"
//#include "server.h"
#include "../alphas/allegro_samples/ex_enet_server.h"
#include "../alphas/allegro_samples/ex_enet_client.h"
#include "game.h"
#include "rank.h"
//#include "alphas/allegro_samples/ex_enet_server.h"
//#include "alphas/allegro_samples/ex_enet_client.h"

/*
 * 
 */
ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_BITMAP *bmp_background;
ALLEGRO_FONT *main_font_size_60;
ALLEGRO_FONT *main_font_size_45;
ALLEGRO_FONT *main_font_size_25;
ALLEGRO_TIMER *timer;
int main_font_size_60_height;
int main_font_size_45_height;
int main_font_size_25_height;
GAME_STATE current_game_state;
GAME_FLOW_STATE current_game_flow_state;
const int DISPLAY_H = 800, DISPLAY_W = 500;
bool exiting = false;
bool DEBUG = true;
bool PITTHAN_MODE = true;

int change_game_state_step_remaining = 0;
GAME_STATE changing_game_state;

void init_display();
void destroy_display();
void load_resources();
void unload_resources();
void draw_background();
void on_changed_game_state();
//bool on_game_state_change(GAME_STATE old_state, GAME_STATE new_state);
void do_the_loop(ALLEGRO_EVENT_QUEUE *queue);
void on_key_press(ALLEGRO_KEYBOARD_EVENT keyboard_event);
void on_mouse_move(int x, int y);
void on_mouse_down(int x, int y);
void on_mouse_up(int x, int y);
void on_redraw();

ALLEGRO_EVENT_QUEUE* create_queue();

int main(int argc, char** argv) {
    int c;
    static struct option long_options[] = {
        {"pitthan", no_argument,       0,  0 },
        {"debug",   no_argument,       0,  0 },
        {0,         0,                 0,  0 }
    };

    while (1){
        int option_index = 0;
        c = getopt_long(argc, argv, "",
                 long_options, &option_index);

        if (c == -1)
            break;

        if (c == 0){
            if (long_options[option_index].name == "pitthan") {
                PITTHAN_MODE = true;
            } else if (long_options[option_index].name == "debug") {
                DEBUG = true;
            }
        }

    }

    return show_screen();

}


int show_screen(){
    ALLEGRO_EVENT_QUEUE *queue;

    init_display();

    load_resources();

    change_game_state(GAME_STATE_MAIN_MENU);

    queue = create_queue();

    do_the_loop(queue);

    al_destroy_event_queue(queue);

    unload_resources();
    destroy_display();

    return EXIT_SUCCESS;

}

ALLEGRO_EVENT_QUEUE* create_queue(){
    ALLEGRO_EVENT_QUEUE *queue;

    timer = al_create_timer(1.0 / 60);
    queue = al_create_event_queue();
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_mouse_event_source());
    al_start_timer(timer);

    return queue;
}



void init_display(){

    // Carrega Allegro
    if(!al_init()) {
        fprintf(stderr, "failed to initialize allegro (error %d)\n",al_get_errno());
        printf("%0x\n%0x\n", ALLEGRO_VERSION_INT, al_get_allegro_version());
        exit(EXIT_FAILURE);
    }

    // Cria Tela
    display = al_create_display(DISPLAY_W, DISPLAY_H);
    if(!display) {
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
    al_clear_to_color(al_map_rgb(255,255,255));
    al_flip_display();

}

void load_resources(){

    // Carrega as imagens necessárias para a tela do menu
    ALLEGRO_FILE* background_png = al_open_memfile(img_background_png,img_background_png_len,"r");
    load_bitmap(&bmp_background,&background_png,".png");

    // Carrega a fonte principal da aplicação
    ALLEGRO_FILE* vt323_ttf_60 = al_open_memfile(font_VT323_ttf,font_VT323_ttf_len,"r");
    ALLEGRO_FILE* vt323_ttf_45 = al_open_memfile(font_VT323_ttf,font_VT323_ttf_len,"r");
    ALLEGRO_FILE* vt323_ttf_25 = al_open_memfile(font_VT323_ttf,font_VT323_ttf_len,"r");
    load_font(&main_font_size_60,&vt323_ttf_60,60,ALLEGRO_TTF_MONOCHROME);
    load_font(&main_font_size_45,&vt323_ttf_45,45,ALLEGRO_TTF_MONOCHROME);
    load_font(&main_font_size_25,&vt323_ttf_25,25,ALLEGRO_TTF_MONOCHROME);
    main_font_size_60_height = al_get_font_line_height(main_font_size_60);
    main_font_size_45_height = al_get_font_line_height(main_font_size_45);
    main_font_size_25_height = al_get_font_line_height(main_font_size_25);

    load_resources_menu_screen();
    load_resources_battleship();
    load_resources_game();

}

void load_font(ALLEGRO_FONT* *font, ALLEGRO_FILE* *file,int size, int flags){
    *font = al_load_ttf_font_f(*file,NULL,size,flags);
    if (!*font) {
        fprintf(stderr,"failed to load font resource (error %d)\n",al_get_errno());
        exit(EXIT_FAILURE);
    }
}

void load_bitmap(ALLEGRO_BITMAP* *bitmap, ALLEGRO_FILE* *file, char* ident){
    *bitmap = al_load_bitmap_f(*file,ident);
    if (!*bitmap) {
        fprintf(stderr,"failed to load bitmap resource (error %d)\n",al_get_errno());
        exit(EXIT_FAILURE);
    }
}

void unload_resources(){
    unload_resources_menu_screen();
    unload_resources_battleship();
    unload_resources_game();

    al_destroy_bitmap(bmp_background);
    al_destroy_font(main_font_size_60);
    al_destroy_font(main_font_size_45);
    al_destroy_font(main_font_size_25);
}

void destroy_display(){
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

// Change Events
void change_game_state(GAME_STATE state){

    changing_game_state = state;

    switch (current_game_state){
        case GAME_STATE_MAIN_MENU:
            change_game_state_step_remaining =
                    on_game_state_changing_count_steps_menu_screen(state);
            start_game_state_change_menu_screen(state);
            break;
        default:
            current_game_state = state;
            on_changed_game_state();
            break;
    }

    if (change_game_state_step_remaining < 1){
        check_game_state_complete();
    }

}

void on_changed_game_state(){
    switch (current_game_state){
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

void check_game_state_complete(){
    if (--change_game_state_step_remaining <= 0){
        if (changing_game_state != GAME_STATE_NONE){
            current_game_state = changing_game_state;
            on_changed_game_state();
            changing_game_state = GAME_STATE_NONE;
        }
    }
}

//bool on_game_state_change(GAME_STATE old_state, GAME_STATE new_state){
//
//
//    return false;
//    //return on_game_state_change_menu_screen(old_state,new_state);
//}

// OnEvents
void on_key_press(ALLEGRO_KEYBOARD_EVENT keyboard_event){
    switch (current_game_state){
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


    double timer_speed;
    if (keyboard_event.keycode == ALLEGRO_KEY_PAD_PLUS){
        timer_speed = al_get_timer_speed(timer);
        al_set_timer_speed(timer,timer_speed*0.5f);
    } else if (keyboard_event.keycode == ALLEGRO_KEY_PAD_MINUS){
        timer_speed = al_get_timer_speed(timer);
        al_set_timer_speed(timer,timer_speed*2.0f);
    }
}

void on_mouse_move(int x, int y){
    switch (current_game_state) {
        case GAME_STATE_MAIN_MENU:
            on_mouse_move_menu_screen(x,y);
            break;
        case GAME_STATE_VISUALIZING_RANK:
            on_mouse_move_rank(x,y);
            break;
        default:
            break;
    }
}

void on_mouse_down(int x, int y){
    switch (current_game_state) {
        case GAME_STATE_MAIN_MENU:
            on_mouse_down_menu_screen(x,y);
            break;
        case GAME_STATE_VISUALIZING_RANK:
            on_mouse_down_rank(x,y);
            break;
        default:
            break;
    }
}

void on_mouse_up(int x, int y){
    switch (current_game_state) {
        case GAME_STATE_MAIN_MENU:
            on_mouse_up_menu_screen(x,y);
            break;
        case GAME_STATE_VISUALIZING_RANK:
            on_mouse_up_rank(x,y);
            break;
        default:
            break;
    }
}

void on_timer(){
    switch (current_game_state) {
        case GAME_STATE_MAIN_MENU:
            on_timer_menu_screen();
            break;
        case GAME_STATE_IN_GAME_SINGLE_PLAYER:
            on_timer_game();
            break;
        case GAME_STATE_VISUALIZING_RANK:
            on_timer_rank();
        default:
            break;
    }
}

void on_redraw(){
    al_clear_to_color(al_map_rgb_f(0, 0, 0));
    draw_background();

    switch (current_game_state){
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

    al_flip_display();
}

void do_the_loop(ALLEGRO_EVENT_QUEUE *queue){
    ALLEGRO_EVENT event;
    bool redraw = true;

    while (!exiting) {
        al_wait_for_event(queue, &event); // Wait for and get an event.

        switch (event.type){
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                exiting = true;
                break;
            case ALLEGRO_EVENT_KEY_CHAR:
                on_key_press(event.keyboard);
                break;
            case ALLEGRO_EVENT_TIMER:
                redraw = true;
                on_timer();
                break;
            case ALLEGRO_EVENT_MOUSE_AXES:
                on_mouse_move(event.mouse.x,event.mouse.y);
                break;
            case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
                on_mouse_down(event.mouse.x,event.mouse.y);
                break;
            case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
                on_mouse_up(event.mouse.x,event.mouse.y);
                break;
            default:
                break;
        }

        // Redraw, but only if the event queue is empty
        if (redraw && al_is_event_queue_empty(queue)) {
            redraw = false;
            on_redraw();
        }
    }
}

void draw_background(){
    static int x=1,y=1;
    int bgw = al_get_bitmap_width(bmp_background);
    int bgh = al_get_bitmap_height(bmp_background);
    int i,j=y-bgh;
    x=(x < -bgw)?1:x;
    y=(y >  bgh)?1:y;
    x--;
    y++;
    for(i=x; i<DISPLAY_W; i+=bgw){
        al_draw_bitmap(bmp_background,i,j,0);
        for(j=y-bgh; j<DISPLAY_H; j+=bgh){
            al_draw_bitmap(bmp_background,i,j,0);
        }
    }
}