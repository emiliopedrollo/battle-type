#ifndef BATTLE_TYPE_RANK_H
#define BATTLE_TYPE_RANK_H

#include <allegro5/events.h>

typedef struct {
    char name[3];
    long score;
} RANK_ENTRY;

void init_rank();
void on_redraw_rank();
void on_key_press_rank(ALLEGRO_KEYBOARD_EVENT event);

void on_mouse_move_rank(int x,int y);
void on_mouse_down_rank(int x,int y);
void on_mouse_up_rank(int x,int y);
void on_timer_rank();

#endif //BATTLE_TYPE_RANK_H
