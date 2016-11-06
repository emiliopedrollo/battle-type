//
// Created by ubuntu on 10/19/16.
//

#include "battleship.h"

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
            vx = 1.5f; vy = .2f;
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

    battleship->dx = dx;
    battleship->dy = dy;



    battleship->vx = vx;
    battleship->vy = vy;
    battleship->vxi = vx;
    battleship->vyi = vy;


    battleship->turning_direction = TURNING_DIRECTION_NONE;
    battleship->turning_frame = 0;
    change_battleship_state(battleship, BATTLESHIP_MOVE_STATE_INITAL_STATE);

    battleship->locked = false;
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
                dist_l = (battleship->dx - bsw / 2 <= 0) ? 1 : battleship->dx - bsw / 2;
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
                        battleship->dy = (battleship->dy+45 <= game_bs_client_limit) ? battleship->dy + battleship->vy : battleship->dy;
                    }
        }
            break;
        default:
            break;
    }
}

float get_normalized_dx(BATTLESHIP *battleship){
    float dx = battleship->dx;
    if (current_game_state == GAME_STATE_IN_GAME_MULTIPLAYER_CLIENT) {
        dx = DISPLAY_W - dx;
    }
    return dx;
}

float get_normalized_dy(BATTLESHIP *battleship){
    float dy = battleship->dy;
    if (current_game_state == GAME_STATE_IN_GAME_MULTIPLAYER_CLIENT) {
        dy = DISPLAY_H - dy;
    }
    return dy;
}

float get_left_dx(BATTLESHIP *battleship){
    float dx = get_normalized_dx(battleship);
    int bsw = get_battleship_width(battleship->class);
    return dx - (bsw/2.0f);
}

float get_top_dy(BATTLESHIP *battleship){
    float dy = get_normalized_dy(battleship);
    int bsh = get_battleship_height(battleship->class);
    return dy - (bsh/2.0f);
}

float get_righ_dx(BATTLESHIP *battleship){
    float dx = get_normalized_dx(battleship);
    int bsw = get_battleship_width(battleship->class);
    return dx + (bsw/2.0f);
}

float get_bottom_dy(BATTLESHIP *battleship){
    float dy = get_normalized_dy(battleship);
    int bsh = get_battleship_height(battleship->class);
    return dy + (bsh/2.0f);
}

void draw_debug(BATTLESHIP *battleship){
    int bsh = get_battleship_height(battleship->class);
    int bsw = get_battleship_width(battleship->class);

    float dx, dy;

    dx = get_normalized_dx(battleship);
    dy = get_normalized_dy(battleship);

    ALLEGRO_COLOR color;

    switch (battleship->owner){
        case BATTLESHIP_OWNER_NONE:
        case BATTLESHIP_OWNER_PLAYER:
            color = (battleship->turning_direction == TURNING_DIRECTION_NONE)?
                    al_map_rgb(250,0,0): al_map_rgb(0,250,0);
            break;
        case BATTLESHIP_OWNER_OPPONENT:
            color = (battleship->turning_direction == TURNING_DIRECTION_NONE)?
                    al_map_rgb(0,0,255): al_map_rgb(0,250,0);
            break;
        case BATTLESHIP_OWNER_SPECIAL:
        default:
            color = (battleship->turning_direction == TURNING_DIRECTION_NONE)?
                    al_map_rgb(255,255,153): al_map_rgb(0,250,0);
            break;
    }

    al_draw_rectangle(dx-bsw/2,dy-bsh/2,
                      dx+bsw/2,dy+bsh/2,
                      color,2);
    al_draw_filled_rectangle(dx-2,dy-2,
                             dx+2,dy+2
            ,color);

}

void draw_target_lock(BATTLESHIP *battleship){

    int outer_margin = 4;

    float ldx = get_left_dx(battleship) - outer_margin;
    float tdy = get_top_dy(battleship) - outer_margin;
    float rdx = get_righ_dx(battleship) + outer_margin;
    float bdy = get_bottom_dy(battleship) + outer_margin;

    int bsw = get_battleship_width(battleship->class);
    int bsh = get_battleship_height(battleship->class);

    float stop_x = (bsw+2*outer_margin) / 6.0f;
    float stop_y = (bsh+2*outer_margin) / 6.0f;

    ALLEGRO_COLOR color = al_map_rgb(165,0,0);

    // outer lines
    al_draw_line(ldx + stop_x * 1, tdy + stop_y * 0,
                 rdx - stop_x * 1, tdy + stop_y * 0,
                 color,2);

    al_draw_line(ldx + stop_x * 1, bdy - stop_y * 0,
                 rdx - stop_x * 1, bdy - stop_y * 0,
                 color,2);

    al_draw_line(ldx + stop_x * 0, tdy + stop_y * 1,
                 ldx + stop_x * 0, bdy - stop_y * 1,
                 color,2);

    al_draw_line(rdx - stop_x * 0, tdy + stop_y * 1,
                 rdx - stop_x * 0, bdy - stop_y * 1,
                 color,2);

    // middle lines
    al_draw_line(ldx + stop_x * 2, tdy + stop_y * 1,
                 rdx - stop_x * 2, tdy + stop_y * 1,
                 color,2);

    al_draw_line(ldx + stop_x * 2, bdy - stop_y * 1,
                 rdx - stop_x * 2, bdy - stop_y * 1,
                 color,2);

    al_draw_line(ldx + stop_x * 1, tdy + stop_y * 2,
                 ldx + stop_x * 1, bdy - stop_y * 2,
                 color,2);

    al_draw_line(rdx - stop_x * 1, tdy + stop_y * 2,
                 rdx - stop_x * 1, bdy - stop_y * 2,
                 color,2);

//    // inner lines
//    al_draw_line(ldx + stop_x * 3, tdy + stop_y * 2,
//                 rdx - stop_x * 3, tdy + stop_y * 2,
//                 color,2);
//
//    al_draw_line(ldx + stop_x * 3, bdy - stop_y * 2,
//                 rdx - stop_x * 3, bdy - stop_y * 2,
//                 color,2);
//
//    al_draw_line(ldx + stop_x * 2, tdy + stop_y * 3,
//                 ldx + stop_x * 2, bdy - stop_y * 3,
//                 color,2);
//
//    al_draw_line(rdx - stop_x * 2, tdy + stop_y * 3,
//                 rdx - stop_x * 2, bdy - stop_y * 3,
//                 color,2);


    //cross lines
    al_draw_line(ldx + stop_x * 1, battleship->dy,
                 ldx + stop_x * 2, battleship->dy,
                 color,2);

    al_draw_line(rdx - stop_x * 2, battleship->dy,
                 rdx - stop_x * 1, battleship->dy,
                 color,2);

    al_draw_line(battleship->dx, tdy + stop_y * 1,
                 battleship->dx, tdy + stop_y * 2,
                 color,2);

    al_draw_line(battleship->dx, bdy - stop_y * 2,
                 battleship->dx, bdy - stop_y * 1,
                 color,2);

    // center dot
    al_draw_filled_rectangle(battleship->dx-2,battleship->dy-2,
                             battleship->dx+2,battleship->dy+2,
                             color);

}

void draw_ship(BATTLESHIP *battleship){

    int bsh = get_battleship_height(battleship->class);
    int bsw = get_battleship_width(battleship->class);

    int flags;

    float dx = get_normalized_dx(battleship);
    float dy = get_normalized_dy(battleship);

    bool draw_ship = true;

    if (current_game_state == GAME_STATE_IN_GAME_MULTIPLAYER_CLIENT) {
        flags = (battleship->owner == BATTLESHIP_OWNER_PLAYER)?ALLEGRO_FLIP_VERTICAL:0;

        if (battleship->owner == BATTLESHIP_OWNER_PLAYER && battleship->class != BATTLESHIP_CLASS_M){
            al_draw_text(main_font_size_25,al_map_rgb(255,255,255),dx,dy+bsh/2,
                         ALLEGRO_ALIGN_CENTER,battleship->word);
        }

        if (PITTHAN_MODE) draw_ship = (battleship->owner != BATTLESHIP_OWNER_OPPONENT);

    } else {
        flags = (battleship->owner == BATTLESHIP_OWNER_OPPONENT)?ALLEGRO_FLIP_VERTICAL:0;

        if (battleship->owner == BATTLESHIP_OWNER_OPPONENT && battleship->class != BATTLESHIP_CLASS_M){
            al_draw_text(main_font_size_25,al_map_rgb(255,255,255),dx,dy+bsh/2,
                         ALLEGRO_ALIGN_CENTER,battleship->word);
        }

        if (PITTHAN_MODE) draw_ship = (battleship->owner != BATTLESHIP_OWNER_PLAYER);
    }

    if (PITTHAN_MODE && battleship->class == BATTLESHIP_CLASS_M) draw_ship = !draw_ship;

    if (!draw_ship) return;

    al_draw_bitmap(battleship->bmp, get_left_dx(battleship), get_top_dy(battleship),flags);

    if (DEBUG) draw_debug(battleship);
}