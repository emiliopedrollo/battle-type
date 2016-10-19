#ifndef ALLEGRO1_H
#define ALLEGRO1_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

int show_screen();

typedef enum {
    BTN_SINGLE_PLAYER = 0,
    BTN_MULTI_PLAYER = 1,
    BTN_EXIT = 2,
    BTN_MULTIPLAYER_JOIN = 3,
    BTN_MULTIPLAYER_HOST = 4,
    BTN_MULTIPLAYER_BACK = 5,
    BTN_MULTIPLAYER_JOIN_ENTER = 6,
    BTN_MULTIPLAYER_JOIN_CANCEL = 7,
    BTN_MULTIPLAYER_HOST_CANCEL = 8,
} BUTTON;

typedef enum {
    GAME_STATE_MAIN_MENU,
    GAME_STATE_IN_GAME,
} GAME_STATE;

typedef enum {
    MENU_SCREEN_MAIN,
    MENU_SCREEN_MULTIPLAYER_SELECT,
    MENU_SCREEN_MULTIPLAYER_JOIN,
    MENU_SCREEN_MULTIPLAYER_HOST,
} MENU_SCREEN;

typedef enum {
    GAME_FLOW_STATE_RUNNING,
    GAME_FLOW_STATE_PAUSE
} GAME_FLOW_STATE;

GAME_STATE current_game_state;
MENU_SCREEN current_menu_screen;
GAME_FLOW_STATE current_game_flow_state;

#endif /* ALLEGRO1_H */

