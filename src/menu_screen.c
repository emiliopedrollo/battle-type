#include <stdio.h>
#include <math.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_memfile.h>
#include "resources/font/VT323.ttf.h"
#include "resources/img/background.jpg.h"
#include "resources/img/battleship.png.h"
#include "buttons.h"
#include "menu_screen.h"
#include "network_utils.h"

ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_BITMAP *bmp_battleship, *bmp_background;
ALLEGRO_FONT *main_font;
bool start_sp = false;
bool exiting = false;
bool is_mouse_down = false;
bool is_mouse_down_on_button = false;
Button buttons[9];

int DISPLAY_H = 800;
int DISPLAY_W = 500;

ALLEGRO_EVENT_QUEUE* create_queue();

void init_display();
void destroy_display();
void load_resources();
void unload_resources();
void init_menu_buttons();
void load_font(ALLEGRO_FONT* *font, ALLEGRO_FILE* *file,int size, int flags);
void load_bitmap(ALLEGRO_BITMAP* *bitmap, ALLEGRO_FILE* *file, char* ident);
void draw_background();
void draw_ship();
void draw_menu();
void on_button_click(int index);
void on_mouse_move(int x, int y);
void on_mouse_down(int x, int y);
void on_mouse_up(int x, int y);
void on_redraw();
void do_the_loop(ALLEGRO_EVENT_QUEUE *queue);

int show_screen(){
    ALLEGRO_EVENT_QUEUE *queue;
    
    init_display();

    current_game_state = GAME_STATE_MAIN_MENU;

    load_resources();

    current_menu_screen = MENU_SCREEN_MAIN;

    init_menu_buttons();

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

void on_menu_change(){

    int total_buttons = sizeof(buttons)/sizeof(buttons[0]);
    for (int i = 0; i < total_buttons; i++) buttons[i].visible = false;

    switch (current_menu_screen){
        case MENU_SCREEN_MAIN:
            buttons[BTN_SINGLE_PLAYER].visible = true;
            buttons[BTN_MULTI_PLAYER].visible = true;
            buttons[BTN_EXIT].visible = true;
            break;
        case MENU_SCREEN_MULTIPLAYER_SELECT:
            buttons[BTN_MULTIPLAYER_JOIN].visible = true;
            buttons[BTN_MULTIPLAYER_HOST].visible = true;
            buttons[BTN_MULTIPLAYER_BACK].visible = true;
            break;
        case MENU_SCREEN_MULTIPLAYER_JOIN:
            buttons[BTN_MULTIPLAYER_JOIN_ENTER].visible = true;
            buttons[BTN_MULTIPLAYER_JOIN_CANCEL].visible = true;
            break;
        case MENU_SCREEN_MULTIPLAYER_HOST:
            printf("ip address found: %s\n",(char*) get_ip_address());
            buttons[BTN_MULTIPLAYER_HOST_CANCEL].visible = true;
            break;
        default:
            break;
    }

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

void load_resources(){
    
    // Carrega as imagens necessárias para a tela do menu
    ALLEGRO_FILE* background_jpg = al_open_memfile(img_background_jpg,img_background_jpg_len,"r");
    load_bitmap(&bmp_background,&background_jpg,".jpg");

    ALLEGRO_FILE* battleship_png = al_open_memfile(img_battleship_png,img_battleship_png_len,"r");
    load_bitmap(&bmp_battleship,&battleship_png,".png");
    
    // Carrega a fonte principal da aplicação
    ALLEGRO_FILE* vt323_ttf = al_open_memfile(font_VT323_ttf,font_VT323_ttf_len,"r");
    load_font(&main_font,&vt323_ttf,45,ALLEGRO_TTF_MONOCHROME);
    
}

void unload_resources(){    
    al_destroy_bitmap(bmp_background);
    al_destroy_bitmap(bmp_battleship);
    al_destroy_font(main_font);
}

void init_menu_buttons(){  
    int margin = 20;
    RectangleCoordinate coord;
    int top_position = (DISPLAY_H - 280);
    int middle_position, bottom_position;

    init_button_colors();

    //Main Menu
    buttons[BTN_SINGLE_PLAYER] = init_button(main_font,"Single Player",DISPLAY_W/2,top_position);

    coord = get_button_coordinate(buttons[0]);
    middle_position = coord.y2+buttons[0].h/2+margin;
    buttons[BTN_MULTI_PLAYER] = init_button(main_font,"Multi Player",DISPLAY_W/2,middle_position);

    coord = get_button_coordinate(buttons[1]);
    bottom_position = coord.y2+buttons[1].h/2+margin;
    buttons[BTN_EXIT] = init_button(main_font,"Sair",DISPLAY_W/2,bottom_position);


    // Multiplayer Menu
    buttons[BTN_MULTIPLAYER_JOIN] = init_button(main_font,"Join",DISPLAY_W/2,top_position);
    buttons[BTN_MULTIPLAYER_HOST] = init_button(main_font,"Host",DISPLAY_W/2,middle_position);
    buttons[BTN_MULTIPLAYER_BACK] = init_button(main_font,"Back",DISPLAY_W/2,bottom_position);

    // Multiplayer Join Menu
    buttons[BTN_MULTIPLAYER_JOIN_ENTER] = init_button(main_font,"Enter",DISPLAY_W/2,middle_position);
    buttons[BTN_MULTIPLAYER_JOIN_CANCEL] = init_button(main_font,"Cancel",DISPLAY_W/2,bottom_position);

    // Multiplayer Host Menu
    buttons[BTN_MULTIPLAYER_HOST_CANCEL] = init_button(main_font,"Cancel",DISPLAY_W/2,bottom_position);

    on_menu_change();
    
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

void draw_ship(){
    static int dx,dy,vx=4,vy=1,fps=0;
    int bsw = al_get_bitmap_width(bmp_battleship);
    int bsh = al_get_bitmap_height(bmp_battleship);
    const double n = -89.03036879;
    double prob,mod=((rand()%100)/100.0)+0.01;
    static bool position_center = false;
    
    if(!start_sp){
        
        prob=(vx>0)? (exp((DISPLAY_W-(dx+bsw))/n))/mod : (exp(dx/n))/mod;
        vy=((vy>0 && (bsh+dy)==DISPLAY_H-20)||(vy<0 && dy==20))?vy*(-1):vy;
        
        if( fps<=0 || dx<20 && fps<10 || DISPLAY_W-(dx+bsw)<20 && fps<10 ){
            vx=(prob>=1.0)?vx*(-1):vx;
            fps=(prob>=1.0)?30:10;
        }else if( fps>0 ){
            fps--;
        }
        
        if( fps<10 ){
            dx+=vx;
            dy+=vy;
        }else if( fps>=10 && fps<12 ){
            dx+=vx-(abs(vx)/-vx);
            dy+=vy;
        }else if( fps>=28 && fps<30 ){
            dx+=-vx-(abs(vx)/-vx);
            dy+=vy;
        }
        
    }else{
        if((dx!=(DISPLAY_W-bsw)/2 || dy!=(DISPLAY_H-bsh)/2) && position_center == false){
           dx=(dx < (DISPLAY_W-bsw)/2)?dx+1:dx;
           dx=(dx > (DISPLAY_W-bsw)/2)?dx-1:dx;
           dy=(dy < (DISPLAY_H-bsh)/2)?dy+1:dy;
           dy=(dy > (DISPLAY_H-bsh)/2)?dy-1:dy; 
        }else{
            position_center = true;
            if(dy!=-bsh)
                dy-=pow(1.2,++vy);
        }
        
    }
    //Bagunçado mas funcional :)
    al_draw_bitmap(bmp_battleship,dx,dy, 0);
}

void draw_menu(){    
    
    int total_buttons = sizeof(buttons)/sizeof(buttons[0]);
    for (int i = 0; i < total_buttons; i++){
        draw_button(buttons[i]);        
    }

    if (current_menu_screen == MENU_SCREEN_MULTIPLAYER_HOST){

    }
    
}

void change_menu_state(MENU_SCREEN state){
    current_menu_screen = state;
    on_menu_change();
}

void on_button_click(int index){    
    switch (index){
        case BTN_SINGLE_PLAYER:
            current_game_state = GAME_STATE_IN_GAME;
            //menu_buttons[1].visible = false;
            //menu_buttons[2].visible = false;
            //start_sp = true;
            break;
        case BTN_MULTI_PLAYER:
            change_menu_state(MENU_SCREEN_MULTIPLAYER_SELECT);
            break;
        case BTN_EXIT:
            exiting = true;
            break;
        case BTN_MULTIPLAYER_JOIN:
            change_menu_state(MENU_SCREEN_MULTIPLAYER_JOIN);
            break;
        case BTN_MULTIPLAYER_JOIN_ENTER:
            break;
        case BTN_MULTIPLAYER_HOST:
            change_menu_state(MENU_SCREEN_MULTIPLAYER_HOST);
            break;
        case BTN_MULTIPLAYER_JOIN_CANCEL:
        case BTN_MULTIPLAYER_HOST_CANCEL:
            change_menu_state(MENU_SCREEN_MULTIPLAYER_SELECT);
            break;
        case BTN_MULTIPLAYER_BACK:
            change_menu_state(MENU_SCREEN_MAIN);
            break;
        default:
            break;
    }
}

void on_mouse_move(int x, int y){
    bool is_over_button = false;
    
    if (!is_mouse_down){
        int total_buttons = sizeof(buttons)/sizeof(buttons[0]);
        for (int i = 0; i < total_buttons; i++){
            if (!buttons[i].visible) continue;
            if (is_coordinate_inside_button(buttons[i], x, y)){
                is_over_button = true;
                buttons[i].state = (buttons[i].state != BUTTON_STATE_ACTIVE)?
                    BUTTON_STATE_HOVER:buttons[i].state;
            } else 
                buttons[i].state = (buttons[i].state == BUTTON_STATE_HOVER)?
                    BUTTON_STATE_NORMAL:buttons[i].state;
        }
    }
    
    if (is_over_button || is_mouse_down_on_button)
        al_set_system_mouse_cursor(display,
                ALLEGRO_SYSTEM_MOUSE_CURSOR_ALT_SELECT);
    else 
        al_set_system_mouse_cursor(display,
                ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
}

void on_mouse_down(int x, int y){
    
    int total_buttons = sizeof(buttons)/sizeof(buttons[0]);
    for (int i = 0; i < total_buttons; i++){
        if (buttons[i].visible && is_coordinate_inside_button(buttons[i], x, y)){
            is_mouse_down_on_button = true;
            buttons[i].state = BUTTON_STATE_ACTIVE;
            break;
        }
    }
    is_mouse_down = true;
    
}

void on_mouse_up(int x, int y){
    
    int total_buttons = sizeof(buttons)/sizeof(buttons[0]);
    for (int i = 0; i < total_buttons; i++){
        if (buttons[i].visible && buttons[i].state == BUTTON_STATE_ACTIVE){
            if (is_coordinate_inside_button(buttons[i], x, y)){
                buttons[i].state = BUTTON_STATE_HOVER;
                on_button_click(i);
            } else {
                buttons[i].state = BUTTON_STATE_NORMAL;
            }
            break;
        }
    }    
    is_mouse_down = false;
    is_mouse_down_on_button = false;
    
    on_mouse_move(x,y);
}

void on_key_press(ALLEGRO_KEYBOARD_EVENT event){

    if (event.keycode == ALLEGRO_KEY_ESCAPE){
        if(current_game_state == GAME_STATE_MAIN_MENU){
            switch (current_menu_screen){
                case MENU_SCREEN_MAIN:
                    exiting = true;
                    break;
                case MENU_SCREEN_MULTIPLAYER_SELECT:
                    change_menu_state(MENU_SCREEN_MAIN);
                    break;
                case MENU_SCREEN_MULTIPLAYER_JOIN:
                case MENU_SCREEN_MULTIPLAYER_HOST:
                    change_menu_state(MENU_SCREEN_MULTIPLAYER_SELECT);
                    break;
            }
        } else if (current_game_state == GAME_STATE_IN_GAME){
            current_game_flow_state = (current_game_flow_state == GAME_FLOW_STATE_PAUSE)?
                                       GAME_FLOW_STATE_RUNNING:GAME_FLOW_STATE_PAUSE;
        }
    }




    printf("Keycode: %d | Modifier %o\n",
           event.keycode,event.modifiers);
}

void on_redraw(){
    al_clear_to_color(al_map_rgb_f(0, 0, 0));  

    draw_background();
    draw_ship();

    if (current_game_state == GAME_STATE_MAIN_MENU) draw_menu();
    
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