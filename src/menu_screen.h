#ifndef ALLEGRO1_H
#define ALLEGRO1_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

int show_screen();
void init_menu_screen();
void load_resources_menu_screen();
void unload_resources_menu_screen();
bool on_game_state_change_menu_screen(GAME_STATE old_state, GAME_STATE new_state);

void on_key_press_menu_screen(ALLEGRO_KEYBOARD_EVENT event);
void on_mouse_move_menu_screen(int x, int y);
void on_mouse_down_menu_screen(int x, int y);
void on_mouse_up_menu_screen(int x, int y);
void on_redraw_menu_screen();

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
    MENU_SCREEN_MAIN,
    MENU_SCREEN_MULTIPLAYER_SELECT,
    MENU_SCREEN_MULTIPLAYER_JOIN,
    MENU_SCREEN_MULTIPLAYER_HOST,
} MENU_SCREEN;

MENU_SCREEN current_menu_screen;


#endif /* ALLEGRO1_H */

