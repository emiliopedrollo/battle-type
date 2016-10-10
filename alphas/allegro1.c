#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include "allegro1.h"

ALLEGRO_BITMAP *bmp_battleship, *bmp_background;

int DISPLAY_H = 800;
int DISPLAY_W = 500;

void show_screen(){
    
    ALLEGRO_DISPLAY *display = NULL;

    if(!al_init()) {
       fprintf(stderr, "failed to initialize allegro!\n");
       return -1;
    }

    display = al_create_display(DISPLAY_W, DISPLAY_H);
    if(!display) {
       fprintf(stderr, "failed to create display!\n");
       return -1;
    }    
    
    al_install_mouse();
    al_install_keyboard();
    al_init_image_addon();  

    al_clear_to_color(al_map_rgb(255,255,255));  
    al_flip_display();   

    bmp_background = load_bitmap("resources/alpha/background.jpg");
    bmp_battleship = load_bitmap("resources/alpha/battleship.png");

    do_the_loop(display);

    al_rest(10.0);
    al_destroy_display(display);
    return 0;
    
}

void load_bitmap(char* filename){  
    ALLEGRO_BITMAP *bitmap;
    bitmap=al_load_bitmap(filename);
    if (!bitmap) {
        fprintf(stderr,"failed to load resource: %s!\n",filename);
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
    float dx,dy;
    for(int i=(int)dx; i <= al_get_display_width(display)-(al_get_bitmap_width(bmp_battleship)+5); i++){
        al_draw_bitmap(bmp_battleship, i, dy, 0);
    }
    al_draw_scaled_rotated_bitmap(bmp_battleship, 0, 0, 0, 0, 1, 1, 0, 0);
}

void do_the_loop(ALLEGRO_DISPLAY *display){
    
    ALLEGRO_TIMER *timer;
    ALLEGRO_EVENT_QUEUE *queue;
    bool redraw = true;
    bool done = false;  
    
    al_set_window_title(display, "BattleType");
    
    timer = al_create_timer(1.0 / 30); 
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
                printf("Keycode: %d | Modifier %d\n",event.keyboard.keycode,event.keyboard.modifiers);
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

            al_flip_display();
        }
    }

    al_destroy_bitmap(bmp_background);
    al_destroy_bitmap(bmp_battleship);
    al_destroy_event_queue(queue);
    
}