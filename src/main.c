//#include <stdio.h>
//#include <stdlib.h>
//#include <getopt.h>
//#include <stdbool.h>
#include <stdio.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_memfile.h>
#include "main.h"
#include "menu_screen.h"
#include "resources/img/background.jpg.h"
#include "resources/font/VT323.ttf.h"
#include "battleship.h"
//#include "alphas/allegro_samples/ex_enet_server.h"
//#include "alphas/allegro_samples/ex_enet_client.h"

/*
 * 
 */
ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_BITMAP *bmp_background;
ALLEGRO_FONT *main_font;
GAME_STATE current_game_state;
GAME_FLOW_STATE current_game_flow_state;
const int DISPLAY_H = 800, DISPLAY_W = 500;
bool exiting = false;
bool DEBUG = true;

int change_game_state_step_remaining = 0;
GAME_STATE changing_game_state;

void init_display();
void destroy_display();
void load_resources();
void unload_resources();
void draw_background();
//bool on_game_state_change(GAME_STATE old_state, GAME_STATE new_state);
void do_the_loop(ALLEGRO_EVENT_QUEUE *queue);
void on_key_press(ALLEGRO_KEYBOARD_EVENT keyboard_event);
void on_mouse_move(int x, int y);
void on_mouse_down(int x, int y);
void on_mouse_up(int x, int y);
void on_redraw();

ALLEGRO_EVENT_QUEUE* create_queue();

int main(int argc, char** argv) {
        
    return show_screen();
}


int show_screen(){
    ALLEGRO_EVENT_QUEUE *queue;

    init_display();

    change_game_state(GAME_STATE_MAIN_MENU);

    load_resources();

    init_menu_screen();

    queue = create_queue();

    do_the_loop(queue);

    al_destroy_event_queue(queue);

    unload_resources();
    destroy_display();

    return EXIT_SUCCESS;

}

ALLEGRO_EVENT_QUEUE* create_queue(){
    ALLEGRO_TIMER *timer;
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
        fprintf(stderr, "failed to initialize allegro!\n");
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
    ALLEGRO_FILE* background_jpg = al_open_memfile(img_background_jpg,img_background_jpg_len,"r");
    load_bitmap(&bmp_background,&background_jpg,".jpg");

    // Carrega a fonte principal da aplicação
    ALLEGRO_FILE* vt323_ttf = al_open_memfile(font_VT323_ttf,font_VT323_ttf_len,"r");
    load_font(&main_font,&vt323_ttf,45,ALLEGRO_TTF_MONOCHROME);

    load_resources_menu_screen();
    load_resources_battleship();

}

void load_font(ALLEGRO_FONT* *font, ALLEGRO_FILE* *file,int size, int flags){
    *font = al_load_ttf_font_f(*file,NULL,size,flags);
    if (!*font) {
        fprintf(stderr,"failed to load font resource!\n");
        exit(EXIT_FAILURE);
    }
}

void load_bitmap(ALLEGRO_BITMAP* *bitmap, ALLEGRO_FILE* *file, char* ident){
    *bitmap = al_load_bitmap_f(*file,ident);
    if (!*bitmap) {
        fprintf(stderr,"failed to load bitmap resource!\n");
        exit(EXIT_FAILURE);
    }
}

void unload_resources(){
    unload_resources_menu_screen();
    unload_resources_battleship();

    al_destroy_bitmap(bmp_background);
    al_destroy_font(main_font);
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
            break;
    }

    if (change_game_state_step_remaining < 1){
        check_game_state_complete();
    }

}

void check_game_state_complete(){
    if (--change_game_state_step_remaining == 0){
        if (changing_game_state != GAME_STATE_NONE){
            current_game_state = changing_game_state;
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
    if(current_game_state == GAME_STATE_MAIN_MENU){
        on_key_press_menu_screen(keyboard_event);
    }   else if (current_game_state == GAME_STATE_IN_GAME){

        current_game_flow_state = (current_game_flow_state == GAME_FLOW_STATE_PAUSE)?
                                  GAME_FLOW_STATE_RUNNING:GAME_FLOW_STATE_PAUSE;
    }
}

void on_mouse_move(int x, int y){
    on_mouse_move_menu_screen(x,y);
}

void on_mouse_down(int x, int y){
    on_mouse_down_menu_screen(x,y);
}

void on_mouse_up(int x, int y){
    on_mouse_up_menu_screen(x,y);
}

void on_redraw(){
    al_clear_to_color(al_map_rgb_f(0, 0, 0));
    draw_background();

    on_redraw_menu_screen();

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