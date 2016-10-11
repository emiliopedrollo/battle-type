/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   buttons.h
 * Author: emilio
 *
 * Created on 10 de Outubro de 2016, 20:20
 */

#ifndef BUTTONS_H
#define BUTTONS_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

typedef enum {
    BUTTON_STATE_NORMAL,
    BUTTON_STATE_HOVER,
    BUTTON_STATE_ACTIVE    
} BUTTON_STATE;

typedef struct {
    bool visible;
    BUTTON_STATE state;
    ALLEGRO_FONT* font;
    char* text;
    int x, y, h, w;
} Button;

typedef struct {
    int x1,x2,y1,y2;
} RectangleCoordinate;

void init_button_colors();
RectangleCoordinate get_button_coordinate(Button button);
Button init_button(ALLEGRO_FONT* font, char* text, int cx, int cy);
void draw_button(Button button);
bool is_coordenate_inside_button(Button button, int x, int y);

#endif /* BUTTONS_H */

