//
// Created by ubuntu on 10/19/16.
//

#include "battleship.h"

#include <math.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_memfile.h>
#include "main.h"
#include "resources/img/battleship.png.h"

ALLEGRO_BITMAP *bmp_bs_c5;

void load_resources_battleship(){
    ALLEGRO_FILE* battleship_c5_png = al_open_memfile(img_battleship_png,img_battleship_png_len,"r");
    load_bitmap(&bmp_bs_c5,&battleship_c5_png,".png");
}

void unload_resources_battleship(){
    al_destroy_bitmap(bmp_bs_c5);
}

int get_battleship_height(BATTLESHIP_CLASS class){
    switch (class){
        case BATTLESHIP_CLASS_5:
            return 84;
        default:
            return 0;
    }
}

int get_battleship_width(BATTLESHIP_CLASS class){
    switch (class){
        case BATTLESHIP_CLASS_5:
            return 90;
        default:
            return 0;
    }
}

BATTLESHIP* init_battleship(BATTLESHIP_CLASS class, float dx, float dy){

    BATTLESHIP* battleship = malloc(sizeof(BATTLESHIP));
    float vx,vy;

    battleship->active = true;

    battleship->class = class;

    switch (class){
        case BATTLESHIP_CLASS_5:
            battleship->bmp = bmp_bs_c5;
            vx = 3; vy = 1;
            break;
        default:
            battleship->bmp = bmp_bs_c5;
            vx = 3; vy = 1;
            break;
    }

    battleship->dx = dx;
    battleship->dy = dy;



    battleship->vx = vx;
    battleship->vy = vy;
    battleship->vxi = vx;
    battleship->vyi = vy;


    battleship->turning_direction = TURNING_DIRECTION_NONE;
    battleship->turning_frame = 0;
    change_battleship_state(battleship, BATTLESHIP_MOVE_STATE_INITAL_STATE);

    return battleship;
}

void change_battleship_state(BATTLESHIP *battleship,BATTLESHIP_MOVE_STATE state){

    switch (state){
        case BATTLESHIP_MOVE_STATE_DEMO_PUSHBACK:
            battleship->push_back_done = false;
            battleship->push_back_ended = false;
            battleship->push_back_set_speed = false;
            battleship->push_back_frame = 0;
            battleship->push_back_k = 0;
            battleship->turning_direction = TURNING_DIRECTION_NONE;
            break;
        default:
            break;
    }

    battleship->state = state;

}

void move_ship(BATTLESHIP *battleship) {

    //static TURNING_DIRECTION turning_direction = TURNING_DIRECTION_NONE;
    //const float dvx = 0.8;
    float dvx;
    double dist_r, dist_l;
    //float dx = ,dy,vx=4,vy=1;


    int bsw = al_get_bitmap_width(battleship->bmp);
    int bsh = al_get_bitmap_height(battleship->bmp);
    int n_frames_pushback_placement = 120;

    switch (battleship->state) {
        case BATTLESHIP_MOVE_STATE_DEMO:
            dist_r = (DISPLAY_W-(battleship->dx+bsw/2)<=0)?1:DISPLAY_W-(battleship->dx+bsw/2);
            dist_l = (battleship->dx-bsw/2<=0)?1:battleship->dx-bsw/2;
            //static int turning_frame = 0;
            double prob, mod = (rand() % 100) / 100.0;

            // Calcula chance de inverter velocidade horizontal
            prob=(battleship->vx>0)?(1.0/pow(dist_r,7.0/8.0))+mod:(1.0/pow(dist_l,7.0/8.0))+mod;

            // Inverte a velocidade vertical ao se aproximar das bordas de cima ou de baixo
            battleship->vy = ((battleship->vy > 0 && (bsh + battleship->dy + (bsh / 2)) == DISPLAY_H - 270) ||
                              (battleship->vy < 0 && battleship->dy - (bsh / 2) == 20)) ? battleship->vy * (-1)
                                                                                        : battleship->vy;


            if (prob >= 1 && battleship->turning_direction == TURNING_DIRECTION_NONE)
                battleship->turning_direction = (battleship->vx > 0) ? TURNING_DIRECTION_LEFT : TURNING_DIRECTION_RIGHT;

            if (battleship->turning_direction != TURNING_DIRECTION_NONE) {
                battleship->turning_frame++;
                dvx = (float) fabs(battleship->vxi) / 10;

                battleship->vx = (battleship->turning_direction == TURNING_DIRECTION_LEFT) ?
                                 battleship->vx - dvx : battleship->vx + dvx;
                if (battleship->turning_frame > 10) {
                    battleship->turning_direction = TURNING_DIRECTION_NONE;
                    battleship->vx = ((battleship->vx > 0) ? 1 : -1) * (float) fabs(battleship->vxi);
                }
            } else battleship->turning_frame = 0;

            battleship->dx += battleship->vx;
            battleship->dy += battleship->vy;
            break;
        case BATTLESHIP_MOVE_STATE_DEMO_PUSHBACK:

            if (!battleship->push_back_set_speed) {
                battleship->push_back_set_speed = true;
                battleship->vx = ((DISPLAY_W / 2) - battleship->dx) / n_frames_pushback_placement;
                battleship->vy = ((DISPLAY_H / 2) - battleship->dy) / n_frames_pushback_placement;
            }

            if (battleship->push_back_k++ < n_frames_pushback_placement) {
                battleship->dx += battleship->vx;
                battleship->dy += battleship->vy;
            } else if (!battleship->push_back_done) {
                if (battleship->push_back_frame++ < 60) {
                    battleship->dy++;
                } else battleship->push_back_done = true;
            } else if (battleship->dy > -bsh) {
                battleship->dy -= pow(1.2, ++battleship->vy);
            } else if (!battleship->push_back_ended) {
                battleship->push_back_ended = true;
                battleship->push_back_callback();
            }
            break;
        case BATTLESHIP_MOVE_STATE_IN_GAME:
            if (battleship->owner == BATTLESHIP_OWNER_PLAYER) {
                dist_r = (DISPLAY_W - (battleship->dx + bsw / 2) <= 0) ? 1 : DISPLAY_W - (battleship->dx + bsw / 2);
                dist_l = (battleship->dx - bsw / 2 <= 0) ? 1 : battleship->dx - bsw / 2;
                //static int turning_frame = 0;
                double prob, mod = (rand() % 100) / 100.0;

                // Calcula chance de inverter velocidade horizontal
                prob=(battleship->vx>0)?(1.0/pow(dist_r,7.0/8.0))+mod:(1.0/pow(dist_l,7.0/8.0))+mod;

                // Inverte a velocidade vertical ao se aproximar das bordas de cima ou de baixo
                //battleship->vy=((battleship->vy>0 && (bsh+battleship->dy+(bsh/2))==DISPLAY_H-270)||
                //                (battleship->vy<0 && battleship->dy-(bsh/2)==20))?battleship->vy*(-1):battleship->vy;


                if (prob >= 1 && battleship->turning_direction == TURNING_DIRECTION_NONE)
                    battleship->turning_direction = (battleship->vx > 0) ? TURNING_DIRECTION_LEFT
                                                                         : TURNING_DIRECTION_RIGHT;

                if (battleship->turning_direction != TURNING_DIRECTION_NONE) {
                    battleship->turning_frame++;
                    dvx = (float) fabs(battleship->vxi) / 10;

                    battleship->vx = (battleship->turning_direction == TURNING_DIRECTION_LEFT) ?
                                     battleship->vx - dvx : battleship->vx + dvx;
                    if (battleship->turning_frame > 10) {
                        battleship->turning_direction = TURNING_DIRECTION_NONE;
                        battleship->vx = ((battleship->vx > 0) ? 1 : -1) * (float) fabs(battleship->vxi);
                    }
                } else battleship->turning_frame = 0;

                battleship->dx += battleship->vx;
                if(battleship->limit >= 50)
                    battleship->dy -= battleship->vy;
            }else if(battleship->owner == BATTLESHIP_OWNER_OPPONENT){
                dist_r = (DISPLAY_W-(battleship->dx+bsw/2)<=0)?1:DISPLAY_W-(battleship->dx+bsw/2);
                dist_l = (battleship->dx-bsw/2<=0)?1:battleship->dx-bsw/2;
                //static int turning_frame = 0;
                double prob,mod=(rand()%100)/100.0;

                // Calcula chance de inverter velocidade horizontal
                prob=(battleship->vx>0)?(1.0/pow(dist_r,7.0/8.0))+mod:(1.0/pow(dist_l,7.0/8.0))+mod;

                // Inverte a velocidade vertical ao se aproximar das bordas de cima ou de baixo
                //battleship->vy=((battleship->vy>0 && (bsh+battleship->dy+(bsh/2))==DISPLAY_H-270)||
                //                (battleship->vy<0 && battleship->dy-(bsh/2)==20))?battleship->vy*(-1):battleship->vy;


                if (prob >= 1 && battleship->turning_direction == TURNING_DIRECTION_NONE)
                    battleship->turning_direction = (battleship->vx>0)?TURNING_DIRECTION_LEFT:TURNING_DIRECTION_RIGHT;

                if (battleship->turning_direction != TURNING_DIRECTION_NONE){
                    battleship->turning_frame++;
                    dvx = (float)fabs(battleship->vxi)/10;

                    battleship->vx = (battleship->turning_direction == TURNING_DIRECTION_LEFT)?
                                     battleship->vx-dvx : battleship->vx+dvx ;
                    if (battleship->turning_frame > 10){
                        battleship->turning_direction = TURNING_DIRECTION_NONE;
                        battleship->vx = ((battleship->vx > 0) ? 1 : -1) * (float)fabs(battleship->vxi);
                    }
                } else battleship->turning_frame = 0;

                battleship->dx += battleship->vx;

                if(battleship->limit >= 50)
                    battleship->dy += battleship->vy;
            }
            break;
        default:
            break;
    }
}

void draw_ship(BATTLESHIP *battleship){

    int bsh = get_battleship_height(battleship->class);
    int bsw = get_battleship_width(battleship->class);

    int flag = (battleship->owner == BATTLESHIP_OWNER_OPPONENT)?ALLEGRO_FLIP_VERTICAL:0;
    al_draw_bitmap(battleship->bmp,battleship->dx-(bsw/2),battleship->dy-(bsh/2),flag);

    if (DEBUG){
        ALLEGRO_COLOR color = (battleship->turning_direction == TURNING_DIRECTION_NONE)?
                              al_map_rgb(250,0,0): al_map_rgb(0,250,0);
        al_draw_rectangle(battleship->dx-bsw/2,battleship->dy-bsh/2,
                          battleship->dx+bsw/2,battleship->dy+bsh/2,
                          color,2);
        al_draw_filled_rectangle(battleship->dx-2,battleship->dy-2,
                                 battleship->dx+2,battleship->dy+2
                ,color);
    }
}