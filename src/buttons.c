#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include "buttons.h"
#include "utils.h"

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

RectangleCoordinate get_button_coordinate(Button button){
    RectangleCoordinate coord;

    coord.x1 = button.x;
    coord.y1 = button.y;
    coord.x2 = button.x+button.w;
    coord.y2 = button.y+button.h;

    return coord;
}

Button init_button(ALLEGRO_FONT* font, char* text, int cx, int cy, int w){
    Button btn;

    //Button States
    btn.visible = true;
    btn.state = BUTTON_STATE_NORMAL;

    //Button Text
    btn.text = text;
    btn.font = font;

    btn.w = w;
    btn.h = 70;

    btn.x = cx - btn.w/2;
    btn.y = cy - btn.h/2;

    return btn;
}

void draw_button(Button button){
    int fh = al_get_font_line_height(button.font);
    int pixel_thickness = 4;
    RectangleCoordinate coord = get_button_coordinate(button);

    if (!button.visible) return;

    // Preenche fundo
    al_draw_filled_rectangle(
            coord.x1+pixel_thickness*2,
            coord.y1+pixel_thickness*2,
            coord.x2-pixel_thickness*2,
            coord.y2-pixel_thickness*2,
            btn_fill_color);

    // Desenha borda de cima
    al_draw_line(
            coord.x1+pixel_thickness,
            coord.y1+pixel_thickness/2,
            coord.x2-pixel_thickness,
            coord.y1+pixel_thickness/2,
            btn_border_color,pixel_thickness);

    // Desenha borda de baixo
    al_draw_line(
            coord.x1+pixel_thickness,
            coord.y2-pixel_thickness/2,
            coord.x2-pixel_thickness,
            coord.y2-pixel_thickness/2,
            btn_border_color,pixel_thickness);

    // Desenha borda lateral esquerda
    al_draw_line(
            coord.x1+pixel_thickness/2,
            coord.y1+pixel_thickness,
            coord.x1+pixel_thickness/2,
            coord.y2-pixel_thickness,
            btn_border_color,pixel_thickness);

    // Desenha borda lateral direita
    al_draw_line(
            coord.x2-pixel_thickness/2,
            coord.y1+pixel_thickness,
            coord.x2-pixel_thickness/2,
            coord.y2-pixel_thickness,
            btn_border_color,pixel_thickness);

    if (button.state == BUTTON_STATE_NORMAL ||
        button.state == BUTTON_STATE_HOVER){
         // Desenha cantos iluminados
        al_draw_line(
                coord.x1+pixel_thickness*3/2,
                coord.y1+pixel_thickness,
                coord.x1+pixel_thickness*3/2,
                coord.y2-pixel_thickness,
                btn_light_color,pixel_thickness);

        al_draw_line(
                coord.x1+pixel_thickness*2,
                coord.y1+pixel_thickness*3/2,
                coord.x2-pixel_thickness,
                coord.y1+pixel_thickness*3/2,
                btn_light_color,pixel_thickness);

        // Desenha cantos sombrios
        al_draw_line(
                coord.x1+pixel_thickness*2,
                coord.y2-pixel_thickness*3/2,
                coord.x2-pixel_thickness,
                coord.y2-pixel_thickness*3/2,
                btn_dark_color,pixel_thickness);
        al_draw_line(
                coord.x2-pixel_thickness*3/2,
                coord.y1+pixel_thickness*2,
                coord.x2-pixel_thickness*3/2,
                coord.y2-pixel_thickness,
                btn_dark_color,pixel_thickness);

        al_draw_filled_rectangle(
                coord.x2-pixel_thickness*3,
                coord.y2-pixel_thickness*3,
                coord.x2-pixel_thickness*2,
                coord.y2-pixel_thickness*2,
                btn_dark_color);

    } else if (button.state == BUTTON_STATE_ACTIVE){

         // Desenha cantos iluminados
        al_draw_line(
                coord.x1+pixel_thickness*3/2,
                coord.y1+pixel_thickness,
                coord.x1+pixel_thickness*3/2,
                coord.y2-pixel_thickness,
                btn_dark_color,pixel_thickness);

        al_draw_line(
                coord.x1+pixel_thickness*2,
                coord.y1+pixel_thickness*3/2,
                coord.x2-pixel_thickness,
                coord.y1+pixel_thickness*3/2,
                btn_dark_color,pixel_thickness);

        // Desenha cantos sombrios
        al_draw_line(
                coord.x1+pixel_thickness*2,
                coord.y2-pixel_thickness*3/2,
                coord.x2-pixel_thickness,
                coord.y2-pixel_thickness*3/2,
                btn_light_color,pixel_thickness);
        al_draw_line(
                coord.x2-pixel_thickness*3/2,
                coord.y1+pixel_thickness*2,
                coord.x2-pixel_thickness*3/2,
                coord.y2-pixel_thickness,
                btn_light_color,pixel_thickness);


        al_draw_filled_rectangle(
                coord.x1+pixel_thickness*2,
                coord.y1+pixel_thickness*2,
                coord.x1+pixel_thickness*3,
                coord.y1+pixel_thickness*3,
                btn_dark_color);
    }



    // Desenha texto

    int i,a, under_index;
    char* text;
    char* under_text;
    char* button_text;

    if (strstr(button.text,"&") != NULL){
        under_index = (int)(strstr(button.text,"&") - button.text);
        button_text = remove_char(button.text,'&');
        under_text = malloc(strlen(button_text)+1);
        for(i=0;i<strlen(button_text);i++){
            under_text[i] = (char)((i == under_index)?'_':' ');
        }
        under_text[i] = 0;
    } else {
        button_text = button.text;
        under_text = 0;
    }

    for(i=0;i<2;i++){
        text = (i==0)?button_text:under_text;
        if (text == NULL) continue;
        a = (i==0)?0:7;
        if (button.state == BUTTON_STATE_NORMAL){

            al_draw_text(button.font,
                         btn_text_color,
                         coord.x1 + (button.w/2),
                         coord.y1 + (button.h/2) - (fh/2)-3 + a,
                         ALLEGRO_ALIGN_CENTER,text);

        } else if (button.state == BUTTON_STATE_HOVER){

            al_draw_text(button.font,
                         btn_text_hover_color,
                         coord.x1 + (button.w/2),
                         coord.y1 + (button.h/2) - (fh/2)-3 + a,
                         ALLEGRO_ALIGN_CENTER,text);

        } else if (button.state == BUTTON_STATE_ACTIVE){

            al_draw_text(button.font,
                         btn_text_hover_color,
                         coord.x1 + (button.w/2),
                         coord.y1 + (button.h/2) - (fh/2)-1 + a,
                         ALLEGRO_ALIGN_CENTER,text);

        }
    }
}

bool is_coordinate_inside_button(Button button, int x, int y){
    return x > button.x && x < button.x+button.w &&
           y > button.y && y < button.y+button.h;
}