//
// Created by ubuntu on 10/19/16.
//

#include "battleship.h"

#include <stdio.h>
#include <math.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_memfile.h>
#include <allegro5/allegro_font.h>
#include "main.h"
#include "resources/img/battleship.png.h"
#include "game.h"


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
        case BATTLESHIP_CLASS_M:
            return 84;
        default:
            return 0;
    }
}

int get_battleship_width(BATTLESHIP_CLASS class){
    switch (class){
        case BATTLESHIP_CLASS_5:
            return 90;
        case BATTLESHIP_CLASS_M:
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
            vx = 1.5; vy = 0.5;
            break;
        case BATTLESHIP_CLASS_M:
            battleship->bmp = bmp_bs_c5;
            vx = 0; vy = 0;
            break;
        default:
            battleship->bmp = bmp_bs_c5;
            vx = 3; vy = 1;
            break;
    }

    float x = DISPLAY_W / 2, y = DISPLAY_H - 90;

    battleship->dx = dx;
    battleship->dy = dy;

    battleship->ll = battleship->dx - 100;
    battleship->lr = battleship->dx + 100;

    battleship->ml = (battleship->dy - y) / (battleship->ll - x);
    battleship->mr = (battleship->dy - y) / (battleship->lr - x);

    battleship->vx = vx;
    battleship->vy = vy;
    battleship->vxi = vx;
    battleship->vyi = vy;


    battleship->turning_direction = TURNING_DIRECTION_NONE;
    battleship->turning_frame = 0;
    change_battleship_state(battleship, BATTLESHIP_MOVE_STATE_INITAL_STATE);

    battleship->word = NULL;

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

    float x = DISPLAY_W / 2, y = DISPLAY_H - bsh;

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
        case BATTLESHIP_MOVE_STATE_IN_GAME: {
            //int margin = 200;
            if (battleship->owner == BATTLESHIP_OWNER_PLAYER) {
                dist_r = (DISPLAY_W - (battleship->dx + bsw / 2) <= 0) ? 1 : DISPLAY_W - (battleship->dx + bsw / 2);
                dist_l = (battleship->dx + bsw / 2 <= 0) ? 1 : battleship->dx + bsw / 2;
                //static int turning_frame = 0;
                double prob, mod = (rand() % 100) / 100.0;

                // Calcula chance de inverter velocidade horizontal
                prob=(battleship->vx>0)?(1.0/pow(dist_r,7.0/8.0))+mod:(1.0/pow(dist_l,7.0/8.0))+mod;

                // Inverte a velocidade vertical ao se aproximar das bordas de cima ou de baixo
                //battleship->vy=((battleship->vy>0 && (bsh+battleship->dy+(bsh/2))==DISPLAY_H-270)||
                //                (battleship->vy<0 && battleship->dy-(bsh/2)==20))?battleship->vy*(-1):battleship->vy;


                if (prob >= 1 && battleship->turning_direction == TURNING_DIRECTION_NONE) {
                            battleship->turning_direction = (battleship->vx > 0) ? TURNING_DIRECTION_LEFT
                                                                                                         : TURNING_DIRECTION_RIGHT;
                        }

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
                battleship->dy = ((battleship->dy-45) >= game_bs_host_limit) ? battleship->dy - battleship->vy : battleship->dy;

            }else if(battleship->owner == BATTLESHIP_OWNER_OPPONENT) {
                        //battleship->dy += 0.5;
                        battleship->ll = ((battleship->dy - y) / battleship->ml) + x;
                        battleship->lr = ((battleship->dy - y) / battleship->mr) + x;

                        dist_r = (battleship->lr-(battleship->dx+bsw/2)<=0)?1:battleship->lr-(battleship->dx+bsw/2);
                        dist_l = ((battleship->dx-bsw/2)-battleship->ll<=0)?1:(battleship->dx-bsw/2)-battleship->ll;

                        printf("me:%f\nmd:%f\nll:%f\nlr:%f\ndx:%f\n\n",battleship->ml,battleship->mr,battleship->ll,battleship->lr,battleship->dx);
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
                        battleship->dy = (battleship->dy+45 <= game_bs_client_limit) ? battleship->dy + battleship->vy : battleship->dy;
                    }
        }
            break;
        default:
            break;
    }
}

void draw_ship(BATTLESHIP *battleship){

    int bsh = get_battleship_height(battleship->class);
    int bsw = get_battleship_width(battleship->class);

    float dx, dy;
    int flags;

    dx = battleship->dx;
    dy = battleship->dy;

    if (current_game_state == GAME_STATE_IN_GAME_MULTIPLAYER_CLIENT) {
        flags = (battleship->owner == BATTLESHIP_OWNER_PLAYER)?ALLEGRO_FLIP_VERTICAL:0;
        dx = (float)fabs(DISPLAY_W - dx);
        dy = (float)fabs(DISPLAY_H - dy);

        if (battleship->owner == BATTLESHIP_OWNER_PLAYER && battleship->class != BATTLESHIP_CLASS_M){
            al_draw_text(main_font_size_30,al_map_rgb(255,255,255),dx,dy+bsh/2,
                         ALLEGRO_ALIGN_CENTER,battleship->word);
        }

    } else {
        flags = (battleship->owner == BATTLESHIP_OWNER_OPPONENT)?ALLEGRO_FLIP_VERTICAL:0;

        if (battleship->owner == BATTLESHIP_OWNER_OPPONENT && battleship->class != BATTLESHIP_CLASS_M){
            al_draw_text(main_font_size_30,al_map_rgb(255,255,255),dx,dy+bsh/2,
                         ALLEGRO_ALIGN_CENTER,battleship->word);
        }
    }

    al_draw_bitmap(battleship->bmp,dx-(bsw/2),dy-(bsh/2),flags);


    if (DEBUG){
        float x = DISPLAY_W / 2, y = DISPLAY_H - bsh;
        static float dyp = -45, dxp = 50, dypl, dxpl;
        static float dxe = 50 - 200, dxd = 50 + 200, dxel, dxdl;
        const float m = (dyp - y) / (dxp - x);
        const float me = (dyp - y) / (dxe - x), md = (dyp - y) / (dxd - x);
        int i;

        if (current_game_state == GAME_STATE_IN_GAME_SINGLE_PLAYER ||
            current_game_state == GAME_STATE_IN_GAME_MULTIPLAYER_HOST ||
            current_game_state == GAME_STATE_IN_GAME_MULTIPLAYER_CLIENT) {
            //printf("x:%f\n\ny:%f\n\ndxt:%f\n\ndyt:%f\n\nm:%f\n\n", x, y, battleship->dx, battleship->dy, m);
            dyp += 0.5;
            dxp = ((dyp - y) / m) + x;
            dxe = ((dyp - y) / me) + x;
            dxd = ((dyp - y) / md) + x;
            dypl = dyp;
            dxpl = dxp;
            dxel = dxe;
            dxdl = dxd;
            for(i = -45; i < DISPLAY_H - bsh; i++){
                dxdl = ((i - y) / md) + x;
                dxel = ((i - y) / me) + x;

                if (dxel < 0) {
                    al_draw_filled_rectangle(1 - 2, i - 2,
                                             1 + 2, i + 2,
                                             al_map_rgb(255, 0, 0));
                } else {
                    al_draw_filled_rectangle(dxel - 2, i - 2,
                                             dxel + 2, i + 2,
                                             al_map_rgb(255, 0, 0));
                }

                if (dxdl > 500) {
                    al_draw_filled_rectangle(499 - 2, i - 2,
                                             499 + 2, i + 2,
                                             al_map_rgb(255, 0, 0));
                }else{
                    al_draw_filled_rectangle(dxdl - 2, i - 2,
                                             dxdl + 2, i + 2,
                                             al_map_rgb(255, 0, 0));
                }
            }
            for (i = dypl; i < DISPLAY_H - bsh; i++) {
                dypl += 1;
                dxpl = ((dypl - y) / m) + x;

                al_draw_filled_rectangle(dxpl - 2, dypl - 2,
                                         dxpl + 2, dypl + 2,
                                         al_map_rgb(255, 0, 0));
            }
            al_draw_filled_rectangle(dxp - 2, dyp - 2,
                                     dxp + 2, dyp + 2,
                                     al_map_rgb(0, 255, 0));
        }
        if (current_game_state == GAME_STATE_IN_GAME_MULTIPLAYER_CLIENT){
            battleship->dx=(float)fabs(DISPLAY_W-battleship->dx);
            battleship->dy=(float)fabs(DISPLAY_H-battleship->dy);
        }


        ALLEGRO_COLOR color = (battleship->turning_direction == TURNING_DIRECTION_NONE)?
                              al_map_rgb(250,0,0): al_map_rgb(0,250,0);

        al_draw_rectangle(battleship->dx-bsw/2,battleship->dy-bsh/2,
                          battleship->dx+bsw/2,battleship->dy+bsh/2,
                          color,2);
        al_draw_filled_rectangle(battleship->dx-2,battleship->dy-2,
                                 battleship->dx+2,battleship->dy+2,
                                 color);
    }
}