//
// Created by ubuntu on 10/19/16.
//

#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

#ifndef BATTLE_TYPE_MAIN_H
#define BATTLE_TYPE_MAIN_H

extern ALLEGRO_DISPLAY *display;

extern const int DISPLAY_H;
extern const int DISPLAY_W;

extern bool exiting;
extern bool DEBUG;

typedef enum {
    GAME_STATE_MAIN_MENU,
    GAME_STATE_IN_GAME,
} GAME_STATE;

typedef enum {
    GAME_FLOW_STATE_RUNNING,
    GAME_FLOW_STATE_PAUSE
} GAME_FLOW_STATE;

extern GAME_STATE current_game_state;
extern GAME_FLOW_STATE current_game_flow_state;
extern ALLEGRO_BITMAP *bmp_background;
extern ALLEGRO_FONT *main_font;

void load_font(ALLEGRO_FONT* *font, ALLEGRO_FILE* *file,int size, int flags);
void load_bitmap(ALLEGRO_BITMAP* *bitmap, ALLEGRO_FILE* *file, char* ident);

void change_game_state(GAME_STATE state);

#endif //BATTLE_TYPE_MAIN_H
