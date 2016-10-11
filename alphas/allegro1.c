#include <stdio.h>
#include <math.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_memfile.h>
#include "allegro1.h"
#include "../resources/font.h"
#include "buttons.h"


ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_BITMAP *bmp_battleship, *bmp_background;
ALLEGRO_FONT *main_font;
bool start_sp = false;
bool exiting = false;
bool is_mouse_down = false;
bool is_mouse_down_on_button = false;
Button buttons[3];

int DISPLAY_H = 800;
int DISPLAY_W = 500;

void init_menu_buttons();
void load_font(ALLEGRO_FONT* *font, ALLEGRO_FILE* *file,int size, int flags);
void load_bitmap(ALLEGRO_BITMAP* *bitmap, char* filename);
void do_the_loop(ALLEGRO_DISPLAY *display);

int show_screen(){

    if(!al_init()) {
       fprintf(stderr, "failed to initialize allegro!\n");
       return EXIT_FAILURE;
    }

    display = al_create_display(DISPLAY_W, DISPLAY_H);
    if(!display) {
       fprintf(stderr, "failed to create display!\n");
       return EXIT_FAILURE;
    }    
    
    al_install_mouse();
    al_install_keyboard();
    al_init_image_addon();  
    al_init_primitives_addon();
    al_init_font_addon();
    if (!al_init_ttf_addon()) fprintf(stderr,"failed to load ttf addon!");
    

    al_clear_to_color(al_map_rgb(255,255,255));  
    al_flip_display();   

    load_bitmap(&bmp_background,"resources/alpha/background.jpg");
    load_bitmap(&bmp_battleship,"resources/alpha/battleship.png");
    
    ALLEGRO_FILE* main_font_file = al_open_memfile(VT323_ttf,VT323_ttf_len,"r");    
    load_font(&main_font,&main_font_file,40,ALLEGRO_TTF_MONOCHROME);
    
    init_menu_buttons();

    do_the_loop(display);
    
   
    al_destroy_bitmap(bmp_background);
    al_destroy_bitmap(bmp_battleship);
    al_destroy_font(main_font);
    //al_fclose(main_font_file); //crash

    
    al_shutdown_ttf_addon();
    al_shutdown_font_addon();
    al_shutdown_primitives_addon();
    al_shutdown_image_addon();
    al_uninstall_keyboard();
    al_uninstall_mouse();
    
    al_destroy_display(display);
    
    return EXIT_SUCCESS;
    
}

void init_menu_buttons(){  
    float cx, cy;
    
    init_button_colors();
    
    cx = (DISPLAY_W/2.0);
    cy = DISPLAY_H - 280;

    buttons[0] = init_button(main_font,"Single Player",cx,cy);
    buttons[1] = init_button(main_font,"Multi Player",cx,cy+80);
    buttons[2] = init_button(main_font,"Sair",cx,cy+160);    
    
}

void load_font(ALLEGRO_FONT* *font, ALLEGRO_FILE* *file,int size, int flags){
    *font = al_load_ttf_font_f(*file,NULL,size,flags);
    if (!font) {
        fprintf(stderr,"failed to load font resource!\n");
        //exit(EXIT_FAILURE);
    }   
}

void load_bitmap(ALLEGRO_BITMAP* *bitmap, char* filename){  
    *bitmap = al_load_bitmap(filename);
    if (!bitmap) {
        fprintf(stderr,"failed to load bitmap resource: %s!\n",filename);
        exit(EXIT_FAILURE);
    }
}

void draw_background(ALLEGRO_DISPLAY *display){
    static int x=1,y=1;
    int bgw = al_get_bitmap_width(bmp_background);
    int bgh = al_get_bitmap_height(bmp_background);
    int i,j;
    x=(x < -bgw ||x > DISPLAY_W)?1:x;
    y=(y < -bgh ||y > DISPLAY_H)?1:y;
    x--;
    y--;
    for(i=x; i<DISPLAY_W; i+=bgw){
        al_draw_bitmap(bmp_background,i,j,0);
        for(j=y; j<DISPLAY_H; j+=bgh){
            al_draw_bitmap(bmp_background,i,j,0);
        }                    
    }
}

void draw_ship(ALLEGRO_DISPLAY *display){
    static int dx,dy=40,vx=4,vy=1,temp=0;
    int bsw = al_get_bitmap_width(bmp_battleship);
    int bsh = al_get_bitmap_height(bmp_battleship);
    const float n = -89.03036879;
    float test,mod=((rand()%100)/100.0)+0.01;
    static bool center=false;
    
    if(!start_sp){
        test=(vx>0)? (exp((DISPLAY_W-(dx+bsw))/n))/mod : (exp(dx/n))/mod;
        vy=((vy>0 && (bsh+dy)==DISPLAY_H-40)||(vy<0 && dy==40))?vy*(-1):vy;
    
        if(temp<=0 || dx<20 && temp<10 || (DISPLAY_W-(dx+bsw)<20 && temp<10)){
            vx=(test>=1.0)?vx*(-1):vx;
            temp=(test>=1.0)?30:10;
        }else if(temp>0){
            temp--;
        }
    
        if(temp<10){
            dx+=vx;
            dy+=vy;
        }else if(temp>=10 && temp<14){
            dx+=vx-(abs(vx)/-vx);
            dy+=vy;
        }else if(temp>=26 && temp<30){
            dx+=-vx-(abs(vx)/-vx);
            dy+=vy;
        }
    }else if((dx!=205 || 358!=dy) && center == false){
        dx=(dx < 205)?dx+1:dx;
        dx=(dx > 205)?dx-1:dx;
        dy=(dy < 358)?dy+1:dy;
        dy=(dy > 358)?dy-1:dy;
    }else if((dx==205 && dy==358) || center == true){
        center = true;
        if(dy!=-bsh)
            dy-=pow(1.1,++vy);
    }
    
    //Bagunçado mas funcional :)
    
    al_draw_bitmap(bmp_battleship,dx,dy, 0);
}

void draw_menu(ALLEGRO_DISPLAY *display){    
    
    int total_buttons = sizeof(buttons)/sizeof(buttons[0]);
    for (int i = 0; i < total_buttons; i++){
        draw_button(buttons[i]);        
    }
    
}

void on_button_click(int index){    
    switch (index){
        case (0):
            buttons[0].visible = false;
            //menu_buttons[1].visible = false;
            //menu_buttons[2].visible = false;
            start_sp = true;
            break;
        case 1:
            buttons[0].visible = true;
            break;                    
        case (2): //Exit
            exiting = true;
            break;
    }
}

void on_mouse_move(int x, int y){
    int i;
    bool is_over_button = false;
    
    if (!is_mouse_down){
        int total_buttons = sizeof(buttons)/sizeof(buttons[0]);
        for (int i = 0; i < total_buttons; i++){
            if (!buttons[i].visible) continue;
            if (is_coordenate_inside_button(buttons[i],x,y)){
                is_over_button = true;
                buttons[i].state = (buttons[i].state != BUTTON_STATE_ACTIVE)?BUTTON_STATE_HOVER:buttons[i].state;
            } else 
                buttons[i].state = (buttons[i].state == BUTTON_STATE_HOVER)?BUTTON_STATE_NORMAL:buttons[i].state;
        }
    }
    
    if (is_over_button || is_mouse_down_on_button)
        al_set_system_mouse_cursor(display,ALLEGRO_SYSTEM_MOUSE_CURSOR_ALT_SELECT);
    else 
        al_set_system_mouse_cursor(display,ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
}

void on_mouse_down(int x, int y){
    
    int total_buttons = sizeof(buttons)/sizeof(buttons[0]);
    for (int i = 0; i < total_buttons; i++){
        if (is_coordenate_inside_button(buttons[i],x,y)){
            is_mouse_down_on_button = true;
            buttons[i].state = BUTTON_STATE_ACTIVE;
            break;
        }
    }
    is_mouse_down = true;
    
}

void on_mouse_up(int x, int y){    
    int i;   
    
    int total_buttons = sizeof(buttons)/sizeof(buttons[0]);
    for (int i = 0; i < total_buttons; i++){
        if (buttons[i].state == BUTTON_STATE_ACTIVE){
            if (is_coordenate_inside_button(buttons[i],x,y)){
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

void do_the_loop(ALLEGRO_DISPLAY *display){
    
    ALLEGRO_TIMER *timer;
    ALLEGRO_EVENT_QUEUE *queue;
    bool redraw = true;
    
    al_set_window_title(display, "BattleType");
    
    timer = al_create_timer(1.0 / 60); 
    queue = al_create_event_queue();
    al_register_event_source(queue, al_get_keyboard_event_source()); 
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_mouse_event_source());
    al_start_timer(timer);
    
    while (!exiting) {
        ALLEGRO_EVENT event;
        al_wait_for_event(queue, &event); // Wait for and get an event.
        
        switch (event.type){
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                exiting = true;
                break;
            case ALLEGRO_EVENT_KEY_CHAR:
                if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                    exiting = true;                
                printf("Keycode: %d | Modifier %o\n",event.keyboard.keycode,event.keyboard.modifiers);
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
        }
            
        // Redraw, but only if the event queue is empty
        if (redraw && al_is_event_queue_empty(queue)) {
            redraw = false;
            al_clear_to_color(al_map_rgb_f(0, 0, 0));
            
            draw_background(display);
            draw_ship(display);
            draw_menu(display);

            al_flip_display();
        }
    }

    al_destroy_event_queue(queue);
    
}