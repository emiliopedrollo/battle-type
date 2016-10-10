/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   allegro1.h
 * Author: emilio
 *
 * Created on 7 de Outubro de 2016, 22:50
 */

#ifndef ALLEGRO1_H
#define ALLEGRO1_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

int show_screen();

static int count_menu_itens = 3;

struct {
    bool visible;
    int x, y, h, w, r;
    char* text;
    ALLEGRO_COLOR fill_color, fill_hover_color, text_color, text_hover_color;
    bool hover;
} menu_buttons[3];

#endif /* ALLEGRO1_H */

