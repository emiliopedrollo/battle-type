//
// Created by ubuntu on 10/19/16.
//

#include "battleship.h"

#include <math.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "main.h"


BATTLESHIP* init_battleship(ALLEGRO_BITMAP *bmp, int dx, int dy, int vx, int vy){

    BATTLESHIP* battleship = malloc(sizeof(BATTLESHIP));

    battleship->bmp = bmp;
    battleship->dx = dx;
    battleship->dy = dy;
    battleship->vx = vx;
    battleship->vy = vy;


    battleship->turning_direction = TURNING_DIRECTION_NONE;
    battleship->turning_frame = 0;
    change_battleship_state(battleship, BATTLESHIP_MOVE_STATE_INITAL_STATE);

    return battleship;
}

void change_battleship_state(BATTLESHIP *battleship,BATTLESHIP_MOVE_STATE state){

    switch (state){
        case BATTLESHIP_MOVE_STATE_DEMO_PUSHBACK:
            battleship->push_back_done = false;
            battleship->push_back_set_speed = false;
            battleship->push_back_frame = 0;
            battleship->push_back_k = 0;
            break;
        default:
            break;
    }

    battleship->state = state;

}

void draw_ship(BATTLESHIP *battleship){

    //static TURNING_DIRECTION turning_direction = TURNING_DIRECTION_NONE;
    const float dvx = 0.8;
    double dist_r, dist_l;
    //float dx = ,dy,vx=4,vy=1;


    int bsw = al_get_bitmap_width(battleship->bmp);
    int bsh = al_get_bitmap_height(battleship->bmp);

    switch (battleship->state){
        case BATTLESHIP_MOVE_STATE_DEMO:
            dist_r = (DISPLAY_W-(battleship->dx+bsw)<=0)?1:DISPLAY_W-(battleship->dx+bsw);
            dist_l = (battleship->dx<=0)?1:battleship->dx;
            //static int turning_frame = 0;
            double prob,mod=(rand()%100)/100.0;

            // Calcula chance de inverter velocidade horizontal
            prob=(battleship->vx>0)?(1.0/pow(dist_r,7.0/8.0))+mod:(1.0/pow(dist_l,7.0/8.0))+mod;

            // Inverte a velocidade vertical ao se aproximar das bordas de cima ou de baixo
            battleship->vy=((battleship->vy>0 && (bsh+battleship->dy)==DISPLAY_H-20)||
                    (battleship->vy<0 && battleship->dy==20))?battleship->vy*(-1):battleship->vy;


            if (prob >= 1 && battleship->turning_direction == TURNING_DIRECTION_NONE)
                battleship->turning_direction = (battleship->vx>0)?TURNING_DIRECTION_LEFT:TURNING_DIRECTION_RIGHT;

            if (battleship->turning_direction != TURNING_DIRECTION_NONE){
                battleship->turning_frame++;
                battleship->vx = (battleship->turning_direction == TURNING_DIRECTION_LEFT)?
                                 battleship->vx-dvx : battleship->vx+dvx ;
                if (battleship->turning_frame > 10){
                    battleship->turning_direction = TURNING_DIRECTION_NONE;
                    battleship->vx = (battleship->vx > 0) ? 4: -4;
                }
            } else battleship->turning_frame = 0;

            battleship->dx += battleship->vx;
            battleship->dy += battleship->vy;
            break;
        case BATTLESHIP_MOVE_STATE_DEMO_PUSHBACK:

            if (!battleship->push_back_set_speed){
                battleship->push_back_set_speed = true;
                battleship->vx = (((DISPLAY_W-bsw)/2)-battleship->dx)/battleship->push_back_k;
                battleship->vy = (((DISPLAY_H-bsh)/2)-battleship->dy)/battleship->push_back_k;
            }

            if(battleship->push_back_k++ > 180){
                battleship->dx+=battleship->vx;
                battleship->dy+=battleship->vy;
            }else if (!battleship->push_back_done) {
                if (battleship->push_back_frame++ < 60){
                    battleship->dy++;
                } else battleship->push_back_done = true;
            }else if (battleship->dy!=-bsh){
                battleship->dy-=pow(1.2,++battleship->vy);
            }
            break;
        default:
            break;
    }

    al_draw_bitmap(battleship->bmp,battleship->dx,battleship->dy, 0);

    if (DEBUG){
        al_draw_rectangle(battleship->dx,battleship->dy,battleship->dx+bsw,battleship->dy+bsh,al_map_rgb(250,0,0),4);
    }
}