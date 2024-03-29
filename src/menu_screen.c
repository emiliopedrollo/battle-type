#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include "main.h"
#include "buttons.h"
#include "menu_screen.h"
#include "battleship.h"
#include "utils.h"
#include "server.h"
#include "client.h"

int shortcut_key_pressed_menu_principal = -1;
bool is_mouse_down_menu_screen = false;
bool is_mouse_down_on_button_menu_screen = false;
bool draw_menu_buttons = true;
Button buttons[10];

BATTLESHIP *demo_ship;

MENU_SCREEN current_menu_screen;

void init_menu_buttons();

void init_demo_ship();

void draw_demo_ship();

void draw_menu();

void on_button_click_menu_screen(int index);

void on_menu_change();

void draw_address_box();

void change_menu_state(MENU_SCREEN state);

void on_menu_change() {

    int total_buttons = sizeof(buttons) / sizeof(buttons[0]);
    for (int i = 0; i < total_buttons; i++) buttons[i].visible = false;

    switch (current_menu_screen) {
        case MENU_SCREEN_MAIN:
            buttons[BTN_SINGLE_PLAYER].visible = true;
            buttons[BTN_MULTI_PLAYER].visible = true;
            buttons[BTN_RANK].visible = true;
            buttons[BTN_EXIT].visible = true;
            break;
        case MENU_SCREEN_MULTIPLAYER_SELECT:
            buttons[BTN_MULTIPLAYER_JOIN].visible = true;
            buttons[BTN_MULTIPLAYER_HOST].visible = true;
            buttons[BTN_MULTIPLAYER_BACK].visible = true;
            break;
        case MENU_SCREEN_MULTIPLAYER_JOIN:
            buttons[BTN_MULTIPLAYER_JOIN_ENTER].visible = true;
            buttons[BTN_MULTIPLAYER_JOIN_CANCEL].visible = true;
            break;
        case MENU_SCREEN_MULTIPLAYER_HOST:
            printf("ip address found: %s\n", (char *) get_ip_address());
            buttons[BTN_MULTIPLAYER_HOST_CANCEL].visible = true;
            break;
        default:
            break;
    }

}

void init_menu_screen() {
    current_menu_screen = MENU_SCREEN_MAIN;
    init_menu_buttons();
    init_demo_ship();
}

void load_resources_menu_screen() {
    return;
}

void unload_resources_menu_screen() {
}

int on_game_state_changing_count_steps_menu_screen(GAME_STATE new_state) {
    switch (new_state) {
        case GAME_STATE_IN_GAME_SINGLE_PLAYER:
        case GAME_STATE_IN_GAME_MULTIPLAYER_HOST:
        case GAME_STATE_IN_GAME_MULTIPLAYER_CLIENT:
            return 1;
        case GAME_STATE_VISUALIZING_RANK:
            return 0;
        default:
            return -1;
    }
}

void demo_ship_push_back_callback() {
    check_game_state_complete();
}

void start_game_state_change_menu_screen(GAME_STATE new_state) {
    switch (new_state) {
        case GAME_STATE_IN_GAME_SINGLE_PLAYER:
        case GAME_STATE_IN_GAME_MULTIPLAYER_HOST:
        case GAME_STATE_IN_GAME_MULTIPLAYER_CLIENT:
            draw_menu_buttons = false;
            demo_ship->push_back_callback = demo_ship_push_back_callback;
            change_battleship_state(demo_ship, BATTLESHIP_MOVE_STATE_DEMO_PUSHBACK);
        default:
            break;
    }
}

//bool on_game_state_change_menu_screen(GAME_STATE old_state, GAME_STATE new_state){
//
//    if (old_state == GAME_STATE_MAIN_MENU){
//        if (new_state == GAME_STATE_IN_GAME){
//            return true;
//        }
//    }
//
//    return true;
//}

void init_menu_buttons() {
    int margin = 20;
    RectangleCoordinate coord;
    int top_position = (DISPLAY_H - 280);
    int middle_position, bottom_position;

    init_button_colors();

    //Main Menu
    buttons[BTN_SINGLE_PLAYER] = init_button(main_font_size_45, "&1 Jogador", DISPLAY_W / 2, top_position, 380);

    coord = get_button_coordinate(buttons[0]);
    middle_position = coord.y2 + buttons[0].h / 2 + margin;
    buttons[BTN_MULTI_PLAYER] = init_button(main_font_size_45, "&2 Jogadores", DISPLAY_W / 2, middle_position, 380);

    coord = get_button_coordinate(buttons[1]);
    bottom_position = coord.y2 + buttons[1].h / 2 + margin;
    buttons[BTN_RANK] = init_button(main_font_size_45, "&Rank", (DISPLAY_W / 4) + 25, bottom_position, 180);
    buttons[BTN_EXIT] = init_button(main_font_size_45, "&Sair", (DISPLAY_W / 4) * 3 - 25, bottom_position, 180);


    // Multiplayer Menu
    buttons[BTN_MULTIPLAYER_JOIN] = init_button(main_font_size_45, "&Conectar", DISPLAY_W / 2, top_position, 380);
    buttons[BTN_MULTIPLAYER_HOST] = init_button(main_font_size_45, "&Hospedar", DISPLAY_W / 2, middle_position, 380);
    buttons[BTN_MULTIPLAYER_BACK] = init_button(main_font_size_45, "&Voltar", DISPLAY_W / 2, bottom_position, 380);

    // Multiplayer Join Menu
    buttons[BTN_MULTIPLAYER_JOIN_ENTER] = init_button(main_font_size_45, "&Entrar", DISPLAY_W / 2, middle_position,
                                                      380);
    buttons[BTN_MULTIPLAYER_JOIN_CANCEL] = init_button(main_font_size_45, "&Cancelar", DISPLAY_W / 2, bottom_position,
                                                       380);

    // Multiplayer Host Menu
    buttons[BTN_MULTIPLAYER_HOST_CANCEL] = init_button(main_font_size_45, "&Cancelar", DISPLAY_W / 2, bottom_position,
                                                       380);

    draw_menu_buttons = true;

    on_menu_change();

}

void init_demo_ship() {
    demo_ship = init_battleship(BATTLESHIP_CLASS_SPACESHIP, BATTLESHIP_OWNER_NONE,
                                DISPLAY_W / 2, DISPLAY_H / 2, 0, 0);

    demo_ship->vx = 3;
    demo_ship->vxi = 3;
    demo_ship->vy = -1;

    change_battleship_state(demo_ship, BATTLESHIP_MOVE_STATE_DEMO);
}

void draw_demo_ship() {
    move_ship(demo_ship, 0);
    draw_ship(demo_ship);
}

char remote_ip[16] = "";

void draw_address_box() {
    static int frame = 0;
    static char *pipe = "";

    if (frame++ == 30) {
        pipe = (pipe == "") ? "|" : "";
        frame = 0;
    }


    int width = 380;
    int height = 70;

    int top = (DISPLAY_H - 280) - (height / 2);
    int left = (DISPLAY_W) / 2 - (width / 2);

    int fh = al_get_font_line_height(main_font_size_45);

    ALLEGRO_COLOR bg = al_map_rgba(255, 255, 255, 200);
    ALLEGRO_COLOR border = al_map_rgba(160, 160, 160, 200);
    ALLEGRO_COLOR black = al_map_rgba(0, 0, 0, 255);

    al_draw_rectangle(left, top, left + width, top + height, border, 4);
    al_draw_filled_rectangle(left, top, left + width, top + height, bg);

    al_draw_text(main_font_size_45, black, (DISPLAY_W) / 2 + 20 - (width / 2), (DISPLAY_H - 280) - (fh / 2),
                 ALLEGRO_ALIGN_LEFT, concat(remote_ip, pipe));
}

void draw_menu() {

    int total_buttons = sizeof(buttons) / sizeof(buttons[0]);
    for (int i = 0; i < total_buttons; i++) {
        draw_button(buttons[i]);
    }

    if (current_menu_screen == MENU_SCREEN_MULTIPLAYER_JOIN) {
        draw_address_box();
    } else if (current_menu_screen == MENU_SCREEN_MULTIPLAYER_HOST) {
        // TODO: Show ip address
    }

}

void change_menu_state(MENU_SCREEN state) {
    current_menu_screen = state;
    on_menu_change();
}

void on_success_connect() {

    change_game_state(GAME_STATE_IN_GAME_MULTIPLAYER_CLIENT);

}

void on_failure_connect() {


}

void on_client_connect() {

    change_game_state(GAME_STATE_IN_GAME_MULTIPLAYER_HOST);

}

void on_button_click_menu_screen(int index) {

    switch (index) {
        case BTN_SINGLE_PLAYER:
            change_game_state(GAME_STATE_IN_GAME_SINGLE_PLAYER);
            break;
        case BTN_MULTI_PLAYER:
            change_menu_state(MENU_SCREEN_MULTIPLAYER_SELECT);
            break;
        case BTN_RANK:
            change_game_state(GAME_STATE_VISUALIZING_RANK);
            break;
        case BTN_EXIT:
            exiting = true;
            break;
        case BTN_MULTIPLAYER_JOIN:
            change_menu_state(MENU_SCREEN_MULTIPLAYER_JOIN);
            break;
        case BTN_MULTIPLAYER_JOIN_ENTER:
            connect_client(remote_ip, on_success_connect, on_failure_connect);
            break;
        case BTN_MULTIPLAYER_HOST:
            change_menu_state(MENU_SCREEN_MULTIPLAYER_HOST);
            start_server(on_client_connect);
            break;
        case BTN_MULTIPLAYER_JOIN_CANCEL:
            change_menu_state(MENU_SCREEN_MULTIPLAYER_SELECT);
            disconnect_client();
            break;
        case BTN_MULTIPLAYER_HOST_CANCEL:
            change_menu_state(MENU_SCREEN_MULTIPLAYER_SELECT);
            stop_server();
            break;
        case BTN_MULTIPLAYER_BACK:
            change_menu_state(MENU_SCREEN_MAIN);
            break;
        default:
            break;
    }

    al_set_system_mouse_cursor(display, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
}

void on_key_press_menu_screen(ALLEGRO_KEYBOARD_EVENT event) {

    if (event.keycode == ALLEGRO_KEY_ESCAPE) {
        switch (current_menu_screen) {
            case MENU_SCREEN_MAIN:
                exiting = true;
                break;
            case MENU_SCREEN_MULTIPLAYER_SELECT:
                change_menu_state(MENU_SCREEN_MAIN);
                break;
            case MENU_SCREEN_MULTIPLAYER_JOIN:
                change_menu_state(MENU_SCREEN_MULTIPLAYER_SELECT);
                disconnect_client();
                break;
            case MENU_SCREEN_MULTIPLAYER_HOST:
                change_menu_state(MENU_SCREEN_MULTIPLAYER_SELECT);
                stop_server();
                break;
        }
    }

    static int itmp = -1;
    char *add = NULL;
    // 192.168.000.001

    switch (current_menu_screen) {
        case MENU_SCREEN_MULTIPLAYER_JOIN:
            switch (event.keycode) {
                case ALLEGRO_KEY_BACKSPACE:
                    if (itmp < 0) break;
                    substr(remote_ip, sizeof(remote_ip), remote_ip, itmp--);
                    break;
                case ALLEGRO_KEY_PAD_DELETE:
                case ALLEGRO_KEY_FULLSTOP:
                    add = ".";
                    break;
                case ALLEGRO_KEY_PAD_0:
                case ALLEGRO_KEY_0:
                    add = "0";
                    break;
                case ALLEGRO_KEY_PAD_1:
                case ALLEGRO_KEY_1:
                    add = "1";
                    break;
                case ALLEGRO_KEY_PAD_2:
                case ALLEGRO_KEY_2:
                    add = "2";
                    break;
                case ALLEGRO_KEY_PAD_3:
                case ALLEGRO_KEY_3:
                    add = "3";
                    break;
                case ALLEGRO_KEY_PAD_4:
                case ALLEGRO_KEY_4:
                    add = "4";
                    break;

                case ALLEGRO_KEY_PAD_5:
                case ALLEGRO_KEY_5:
                    add = "5";
                    break;
                case ALLEGRO_KEY_PAD_6:
                case ALLEGRO_KEY_6:
                    add = "6";
                    break;
                case ALLEGRO_KEY_PAD_7:
                case ALLEGRO_KEY_7:
                    add = "7";
                    break;
                case ALLEGRO_KEY_PAD_8:
                case ALLEGRO_KEY_8:
                    add = "8";
                    break;
                case ALLEGRO_KEY_PAD_9:
                case ALLEGRO_KEY_9:
                    add = "9";
                    break;
                case ALLEGRO_KEY_PAD_ENTER:
                case ALLEGRO_KEY_ENTER:
                    on_button_click_menu_screen(BTN_MULTIPLAYER_JOIN_ENTER);
                    break;
                case ALLEGRO_KEY_E:
                    shortcut_key_pressed_menu_principal = BTN_MULTIPLAYER_JOIN_ENTER;
                    break;
                case ALLEGRO_KEY_C:
                    shortcut_key_pressed_menu_principal = BTN_MULTIPLAYER_JOIN_CANCEL;
                    break;
                default:
                    break;
            }
            if (add != NULL && itmp < 14) {
                itmp++;
                memmove(remote_ip, concat(remote_ip, add), strlen(concat(remote_ip, add)));
            }
            break;
        case MENU_SCREEN_MAIN:
            switch (event.keycode) {
                case ALLEGRO_KEY_1:
                    shortcut_key_pressed_menu_principal = BTN_SINGLE_PLAYER;
                    break;
                case ALLEGRO_KEY_2:
                    shortcut_key_pressed_menu_principal = BTN_MULTI_PLAYER;
                    break;
                case ALLEGRO_KEY_R:
                    shortcut_key_pressed_menu_principal = BTN_RANK;
                    break;
                case ALLEGRO_KEY_S:
                    shortcut_key_pressed_menu_principal = BTN_EXIT;
                    break;
                default:
                    break;
            }
            break;
        case MENU_SCREEN_MULTIPLAYER_SELECT:
            switch (event.keycode) {
                case ALLEGRO_KEY_C:
                    shortcut_key_pressed_menu_principal = BTN_MULTIPLAYER_JOIN;
                    break;
                case ALLEGRO_KEY_H:
                    shortcut_key_pressed_menu_principal = BTN_MULTIPLAYER_HOST;
                    break;
                case ALLEGRO_KEY_V:
                    shortcut_key_pressed_menu_principal = BTN_MULTIPLAYER_BACK;
                    break;
                default:
                    break;
            }
            break;
        case MENU_SCREEN_MULTIPLAYER_HOST:
            switch (event.keycode) {
                case ALLEGRO_KEY_C:
                    shortcut_key_pressed_menu_principal = BTN_MULTIPLAYER_JOIN_CANCEL;
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }

    printf("Keycode: %d | Modifier %o\n",
           event.keycode, event.modifiers);
}

void on_mouse_move_menu_screen(int x, int y) {

    if (draw_menu_buttons) {

        bool is_over_button = false;

        if (!is_mouse_down_menu_screen) {
            int total_buttons = sizeof(buttons) / sizeof(buttons[0]);
            for (int i = 0; i < total_buttons; i++) {
                if (!buttons[i].visible) continue;
                if (is_coordinate_inside_button(buttons[i], x, y)) {
                    is_over_button = true;
                    buttons[i].state = (buttons[i].state != BUTTON_STATE_ACTIVE) ?
                                       BUTTON_STATE_HOVER : buttons[i].state;
                } else
                    buttons[i].state = (buttons[i].state == BUTTON_STATE_HOVER) ?
                                       BUTTON_STATE_NORMAL : buttons[i].state;
            }
        }

        if (is_over_button || is_mouse_down_on_button_menu_screen)
            al_set_system_mouse_cursor(display,
                                       ALLEGRO_SYSTEM_MOUSE_CURSOR_ALT_SELECT);
        else
            al_set_system_mouse_cursor(display,
                                       ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
    }
}

void on_mouse_down_menu_screen(int x, int y) {

    int total_buttons = sizeof(buttons) / sizeof(buttons[0]);
    for (int i = 0; i < total_buttons; i++) {
        if (buttons[i].visible && is_coordinate_inside_button(buttons[i], x, y)) {
            is_mouse_down_on_button_menu_screen = true;
            buttons[i].state = BUTTON_STATE_ACTIVE;
            break;
        }
    }
    is_mouse_down_menu_screen = true;

}

void on_mouse_up_menu_screen(int x, int y) {

    int total_buttons = sizeof(buttons) / sizeof(buttons[0]);
    for (int i = 0; i < total_buttons; i++) {
        if (buttons[i].visible && buttons[i].state == BUTTON_STATE_ACTIVE) {
            if (is_coordinate_inside_button(buttons[i], x, y)) {
                buttons[i].state = BUTTON_STATE_HOVER;
                on_button_click_menu_screen(i);
            } else {
                buttons[i].state = BUTTON_STATE_NORMAL;
            }
            break;
        }
    }
    is_mouse_down_menu_screen = false;
    is_mouse_down_on_button_menu_screen = false;

    on_mouse_move_menu_screen(x, y);
}

void on_timer_menu_screen() {
    int static frame_count = 0;

    if (shortcut_key_pressed_menu_principal == -1) return;

    switch (frame_count++) {
        case 0:
            on_mouse_down_menu_screen(
                    buttons[shortcut_key_pressed_menu_principal].x + 1,
                    buttons[shortcut_key_pressed_menu_principal].y + 1);
            break;
        case 10:
            on_mouse_up_menu_screen(
                    buttons[shortcut_key_pressed_menu_principal].x + 1,
                    buttons[shortcut_key_pressed_menu_principal].y + 1);
            frame_count = 0;
            shortcut_key_pressed_menu_principal = -1;
            break;
        default:
            break;
    }
}

void on_redraw_menu_screen() {

    draw_demo_ship();
    if (draw_menu_buttons) draw_menu();

}