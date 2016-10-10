#include <stdio.h>
#include <math.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "allegro1.h"

ALLEGRO_BITMAP *bmp_battleship, *bmp_background;
ALLEGRO_FONT *main_font;

int DISPLAY_H = 800;
int DISPLAY_W = 500;

void load_font(ALLEGRO_FONT* font, char* filename,int size, int flags);
void load_bitmap(ALLEGRO_BITMAP* *bitmap, char* filename);
void do_the_loop(ALLEGRO_DISPLAY *display);

int show_screen(){
    
    ALLEGRO_DISPLAY *display = NULL;

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
    load_font(main_font,"resources/alpha/VT23.ttf",32,ALLEGRO_TTF_MONOCHROME);

    do_the_loop(display);
    
    al_destroy_bitmap(bmp_background);
    al_destroy_bitmap(bmp_battleship);
    al_destroy_font(main_font);

    
    al_shutdown_ttf_addon();
    al_shutdown_font_addon();
    al_shutdown_primitives_addon();
    al_shutdown_image_addon();
    al_uninstall_keyboard();
    al_uninstall_mouse();
    
    al_destroy_display(display);
    
    return EXIT_SUCCESS;
    
}

void load_font(ALLEGRO_FONT* font, char* filename,int size, int flags){
    font = al_load_ttf_font(filename,size,flags);
    if (!font) {
        fprintf(stderr,"failed to load font resource: %s!\n",filename);
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
    static int dx,dy=40,vx=2,vy=1,temp=0;
    int bsw = al_get_bitmap_width(bmp_battleship);
    int bsh = al_get_bitmap_height(bmp_battleship);
    const float n = -89.03036879;
    float test,mod=((rand()%100)/100.0)+0.01;
    
    
    test=(vx>0)? (exp((DISPLAY_W-(dx+bsw))/n))/mod : (exp(dx/n))/mod;
    
    vy=((vy>0 && (bsh+dy)==DISPLAY_H-40)||(vy<0 && dy==40))?vy*(-1):vy;
    
    if(temp<=0){
        vx=(test>=1.0)?vx*(-1):vx;
        temp=(test>=1.0)?57:50;
    }else if(temp>0){
        temp--;
    }
    
    if(temp<50){
        dx+=vx;
        dy+=vy;
    }
    
    al_draw_bitmap(bmp_battleship,dx,dy, 0);
}

void draw_menu(ALLEGRO_DISPLAY *display){
    float bx, by, bw, bh, br, margin;
    //ALLEGRO_PATH *path = al_get_standard_path(ALLEGRO_RESOURCES_PATH);
    ALLEGRO_COLOR btn_color = al_map_rgba(63,81,181,230);
    ALLEGRO_COLOR font_color = al_map_rgb(158,158,158);
        
    
    bh = 60;
    bw = 300;
    bx = (DISPLAY_W/2.0)-(bw/2.0);
    br = 2;
    margin = 20;
    
    //draw single-player button
    by = DISPLAY_H - ((bh+margin) * 4);
    al_draw_filled_rounded_rectangle(bx,by,bx+bw,by+bh,br,br,btn_color);
    //al_draw_text(main_font,font_color,(DISPLAY_W/2),((bh+margin) * 4),ALLEGRO_ALIGN_CENTER,"SINGLE PLAYER");
    
    //draw multi-player button
    by = DISPLAY_H - ((bh+margin) * 3);
    al_draw_filled_rounded_rectangle(bx,by,bx+bw,by+bh,br,br,btn_color);
    
    //draw exit;
    by = DISPLAY_H - ((bh+margin) * 2);
    al_draw_filled_rounded_rectangle(bx,by,bx+bw,by+bh,br,br,btn_color);
    
}

void do_the_loop(ALLEGRO_DISPLAY *display){
    
    ALLEGRO_TIMER *timer;
    ALLEGRO_EVENT_QUEUE *queue;
    bool redraw = true;
    bool done = false;  
    
    al_set_window_title(display, "BattleType");
    
    timer = al_create_timer(1.0 / 60); 
    queue = al_create_event_queue();
    al_register_event_source(queue, al_get_keyboard_event_source()); 
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_start_timer(timer);
    
    while (!done) {
        ALLEGRO_EVENT event;
        al_wait_for_event(queue, &event); // Wait for and get an event.
        
        switch (event.type){
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                done = true;
                break;
            case ALLEGRO_EVENT_KEY_CHAR:
                if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                    done = true;                
                printf("Keycode: %d | Modifier %o\n",event.keyboard.keycode,event.keyboard.modifiers);
                break;
            case ALLEGRO_EVENT_TIMER:
                redraw = true;
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

    al_destroy_bitmap(bmp_background);
    al_destroy_bitmap(bmp_battleship);
    al_destroy_event_queue(queue);
    
}