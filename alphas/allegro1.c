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

ALLEGRO_BITMAP *bmp_battleship, *bmp_background;
ALLEGRO_FONT *main_font;
bool exiting = false, start_sp = false;


int DISPLAY_H = 800;
int DISPLAY_W = 500;

void init_menu_buttons();
void load_font(ALLEGRO_FONT* *font, ALLEGRO_FILE* *file,int size, int flags);
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
    float bx, by, bw, bh, br, margin;  
    int fh, i;
    ALLEGRO_COLOR fill_color = al_map_rgba(63,81,181,230);
    ALLEGRO_COLOR fill_hover_color = al_map_rgb(244,67,54);
    ALLEGRO_COLOR text_color = al_map_rgb(158,158,158);
    ALLEGRO_COLOR text_hover_color = al_map_rgb(255,255,255);
    
    bh = 60;
    bw = 300;
    bx = (DISPLAY_W/2.0)-(bw/2.0);
    by = 0;
    br = 2;    
    margin = 20;
    
    for (i = 0; i < count_menu_itens; i++){
        
        if (i < 3){ //main menu
            by = DISPLAY_H - ((bh+margin) * (4-i));            
        } 
        
        menu_buttons[i].visible = true;
        menu_buttons[i].x = bx;
        menu_buttons[i].y = by;
        menu_buttons[i].w = bw;
        menu_buttons[i].h = bh;
        menu_buttons[i].r = br;
        menu_buttons[i].fill_color = fill_color;
        menu_buttons[i].fill_hover_color = fill_hover_color;        
        menu_buttons[i].text_color = text_color;        
        menu_buttons[i].text_hover_color = text_hover_color;     
        menu_buttons[i].hover = false;
        
        switch (i){
            case 0:
                menu_buttons[i].text = "Single Player";  
                break;
            case 1:
                menu_buttons[i].text = "Multi Player";  
                break;
            case 2:
                menu_buttons[i].text = "Exit";  
                break;                
        }     
    }
    
    
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
        dy-=pow(1.1,++vy);
    }
    
    //Bagunçado mas funcional :)
    
    al_draw_bitmap(bmp_battleship,dx,dy, 0);
}
void draw_menu(ALLEGRO_DISPLAY *display){
    int fh, i;
    
    fh = al_get_font_line_height(main_font);
    
    for (i = 0; i < count_menu_itens; i++){
        if (!menu_buttons[i].visible) continue;
        al_draw_filled_rounded_rectangle(
                menu_buttons[i].x,menu_buttons[i].y,
                menu_buttons[i].x+menu_buttons[i].w,
                menu_buttons[i].y+menu_buttons[i].h,
                menu_buttons[i].r,menu_buttons[i].r,
                (menu_buttons[i].hover)?menu_buttons[i].fill_hover_color:menu_buttons[i].fill_color);        
        al_draw_text(main_font,
                (menu_buttons[i].hover)?menu_buttons[i].text_hover_color:menu_buttons[i].text_color,
                (DISPLAY_W/2),menu_buttons[i].y+(menu_buttons[i].h/2)-(fh/2)-3,
                ALLEGRO_ALIGN_CENTER,menu_buttons[i].text);
    } 
}

bool is_mouse_over_button(int x, int y){
    int i;
    bool is_over_button = false;
    
    for (i = 0; i < count_menu_itens; i++){
        if (menu_buttons[i].visible &&
                x > menu_buttons[i].x && x < menu_buttons[i].x+menu_buttons[i].w &&
                y > menu_buttons[i].y && y < menu_buttons[i].y+menu_buttons[i].h){
            menu_buttons[i].hover = true;
            is_over_button = true;
        } else menu_buttons[i].hover = false;
    }
        
    return is_over_button;
}

void on_mouse_up(){    
    int i;
    for (i = 0; i < count_menu_itens; i++){
        if (menu_buttons[i].hover) {
            switch (i){
                case (0):
                    menu_buttons[0].visible = false;
                    //menu_buttons[1].visible = false;
                    //menu_buttons[2].visible = false;
			start_sp = true;//Start single player
                    break;
                case 1:
                    menu_buttons[0].visible = true;
                    break;                    
                case (2): //Exit
                    exiting = true;
                break;
            }
            break;
        }
    }
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
                if (is_mouse_over_button(event.mouse.x,event.mouse.y))
                    al_set_system_mouse_cursor(event.mouse.display,ALLEGRO_SYSTEM_MOUSE_CURSOR_ALT_SELECT);
                else 
                    al_set_system_mouse_cursor(event.mouse.display,ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
                break;
            case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
                break;
            case ALLEGRO_EVENT_MOUSE_BUTTON_UP:
                on_mouse_up();
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