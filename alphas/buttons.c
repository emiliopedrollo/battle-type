#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/color.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "buttons.h"

ALLEGRO_COLOR btn_border_color;
ALLEGRO_COLOR btn_light_color;
ALLEGRO_COLOR btn_dark_color;
ALLEGRO_COLOR btn_fill_color;
ALLEGRO_COLOR btn_text_color;
ALLEGRO_COLOR btn_text_hover_color;

void init_button_colors(){
    btn_border_color = al_map_rgb(61, 61, 61);
    btn_light_color = al_map_rgb(126, 200, 239);
    btn_dark_color = al_map_rgb(19, 119, 169);
    btn_fill_color = al_map_rgb(67, 168, 222);
    btn_text_color = al_map_rgb(230, 230, 230);
    btn_text_hover_color = al_map_rgb(255, 255, 255);
}

Button init_button(ALLEGRO_FONT* font, char* text, int cx, int cy){
    Button btn;    
    
    //Button States
    btn.visible = true;    
    btn.state = BUTTON_STATE_NORMAL;
    
    //Button Text
    btn.text = text; 
    btn.font = font;
    
    btn.w = 380;
    btn.h = 70;    
    
    btn.x = cx - btn.w/2;
    btn.y = cy - btn.h/2;
    
    return btn;
}

void draw_button(Button button){
    int fh = al_get_font_line_height(button.font);
    int pixel_thiknes = 4;
    
    if (!button.visible) return;
    
    // Preenche fundo
    al_draw_filled_rectangle(
            button.x+pixel_thiknes*2,
            button.y+pixel_thiknes*2,
            button.x+button.w-pixel_thiknes*2,
            button.y+button.h-pixel_thiknes*2,
            btn_fill_color);
    
    // Desenha borda de cima
    al_draw_line(
            button.x+pixel_thiknes,
            button.y+pixel_thiknes/2,
            button.x+button.w-pixel_thiknes,
            button.y+pixel_thiknes/2,
            btn_border_color,pixel_thiknes);
    
    // Desenha borda de baixo
    al_draw_line(
            button.x+pixel_thiknes,
            button.y+button.h-pixel_thiknes/2,
            button.x+button.w-pixel_thiknes,
            button.y+button.h-pixel_thiknes/2,
            btn_border_color,pixel_thiknes);
    
    // Desenha borda lateral esquerda
    al_draw_line(
            button.x+pixel_thiknes/2,
            button.y+pixel_thiknes,
            button.x+pixel_thiknes/2,
            button.y+button.h-pixel_thiknes,
            btn_border_color,pixel_thiknes);
    
    // Desenha borda lateral direita
    al_draw_line(
            button.x+button.w-pixel_thiknes/2,
            button.y+pixel_thiknes,
            button.x+button.w-pixel_thiknes/2,
            button.y+button.h-pixel_thiknes,
            btn_border_color,pixel_thiknes);
    
    if (button.state == BUTTON_STATE_NORMAL || button.state == BUTTON_STATE_HOVER){
         // Desenha cantos iluminados
        al_draw_line(
                button.x+pixel_thiknes*3/2,
                button.y+pixel_thiknes,
                button.x+pixel_thiknes*3/2,
                button.y+button.h-pixel_thiknes,
                btn_light_color,pixel_thiknes);

        al_draw_line(
                button.x+pixel_thiknes*2,
                button.y+pixel_thiknes*3/2,
                button.x+button.w-pixel_thiknes,
                button.y+pixel_thiknes*3/2,
                btn_light_color,pixel_thiknes);

        // Desenha cantos sombrios
        al_draw_line(
                button.x+pixel_thiknes*2,
                button.y+button.h-pixel_thiknes*3/2,
                button.x+button.w-pixel_thiknes,
                button.y+button.h-pixel_thiknes*3/2,
                btn_dark_color,pixel_thiknes);
        al_draw_line(
                button.x+button.w-pixel_thiknes*3/2,
                button.y+pixel_thiknes*2,
                button.x+button.w-pixel_thiknes*3/2,
                button.y+button.h-pixel_thiknes,
                btn_dark_color,pixel_thiknes);
                
        al_draw_filled_rectangle(
                button.x+button.w-pixel_thiknes*3,           
                button.y+button.h-pixel_thiknes*3,
                button.x+button.w-pixel_thiknes*2,          
                button.y+button.h-pixel_thiknes*2,      
                btn_dark_color);
        
    } else if (button.state == BUTTON_STATE_ACTIVE){
        
         // Desenha cantos iluminados
        al_draw_line(
                button.x+pixel_thiknes*3/2,
                button.y+pixel_thiknes,
                button.x+pixel_thiknes*3/2,
                button.y+button.h-pixel_thiknes,
                btn_dark_color,pixel_thiknes);

        al_draw_line(
                button.x+pixel_thiknes*2,
                button.y+pixel_thiknes*3/2,
                button.x+button.w-pixel_thiknes,
                button.y+pixel_thiknes*3/2,
                btn_dark_color,pixel_thiknes);

        // Desenha cantos sombrios
        al_draw_line(
                button.x+pixel_thiknes*2,
                button.y+button.h-pixel_thiknes*3/2,
                button.x+button.w-pixel_thiknes,
                button.y+button.h-pixel_thiknes*3/2,
                btn_light_color,pixel_thiknes);
        al_draw_line(
                button.x+button.w-pixel_thiknes*3/2,
                button.y+pixel_thiknes*2,
                button.x+button.w-pixel_thiknes*3/2,
                button.y+button.h-pixel_thiknes,
                btn_light_color,pixel_thiknes);
        
                
        al_draw_filled_rectangle(
                button.x+pixel_thiknes*2,           
                button.y+pixel_thiknes*2,
                button.x+pixel_thiknes*3,          
                button.y+pixel_thiknes*3,      
                btn_dark_color);
    }
    
   

    // Desenha texto
    if (button.state == BUTTON_STATE_NORMAL){
        
        al_draw_text(button.font,
            btn_text_color,
            button.x + (button.w/2),
            button.y + (button.h/2) - (fh/2)-3,            
            ALLEGRO_ALIGN_CENTER,button.text);
        
    } else if (button.state == BUTTON_STATE_HOVER){
        
        al_draw_text(button.font,
            btn_text_hover_color,
            button.x + (button.w/2),
            button.y + (button.h/2) - (fh/2)-3,            
            ALLEGRO_ALIGN_CENTER,button.text);
    
    } else if (button.state == BUTTON_STATE_ACTIVE){
        
        al_draw_text(button.font,
            btn_text_hover_color,
            button.x + (button.w/2),
            button.y + (button.h/2) - (fh/2)-1,            
            ALLEGRO_ALIGN_CENTER,button.text);
        
    }
    
    
    
    
}

bool is_coordenate_inside_button(Button button, int x, int y){
    return x > button.x && x < button.x+button.w &&
           y > button.y && y < button.y+button.h;
}