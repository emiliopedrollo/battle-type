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
#include <ctype.h>
#include "main.h"
#include "resources/img/spaceship_r.png.h"
#include "resources/img/spaceship_b.png.h"
#include "resources/img/missile_r.png.h"
#include "resources/img/missile_b.png.h"
#include "game.h"
#include "utils.h"


ALLEGRO_BITMAP *bmp_spaceship_blue;
ALLEGRO_BITMAP *bmp_spaceship_red;
ALLEGRO_BITMAP *bmp_missile_blue;
ALLEGRO_BITMAP *bmp_missile_red;


void load_resources_battleship(){
    ALLEGRO_FILE* spaceship_blue_png = al_open_memfile(img_spaceship_b_png,img_spaceship_b_png_len,"r");
    load_bitmap(&bmp_spaceship_blue,&spaceship_blue_png,".png");

    ALLEGRO_FILE* spaceship_red_png = al_open_memfile(img_spaceship_r_png,img_spaceship_r_png_len,"r");
    load_bitmap(&bmp_spaceship_red,&spaceship_red_png,".png");

    ALLEGRO_FILE* missile_blue_png = al_open_memfile(img_missile_b_png,img_missile_b_png_len,"r");
    load_bitmap(&bmp_missile_blue,&missile_blue_png,".png");

    ALLEGRO_FILE* missile_red_png = al_open_memfile(img_missile_r_png,img_missile_r_png_len,"r");
    load_bitmap(&bmp_missile_red,&missile_red_png,".png");
}

void unload_resources_battleship(){
    al_destroy_bitmap(bmp_spaceship_blue);
    al_destroy_bitmap(bmp_spaceship_red);
    al_destroy_bitmap(bmp_missile_blue);
    al_destroy_bitmap(bmp_missile_red);
}

int get_battleship_height(BATTLESHIP_CLASS class){
    switch (class){
        case BATTLESHIP_CLASS_MISSILE:
            return 15;
        case BATTLESHIP_CLASS_SPACESHIP:
            return 90;
        default:
            return 0;
    }
}

int get_battleship_width(BATTLESHIP_CLASS class){
    switch (class){
        case BATTLESHIP_CLASS_MISSILE:
            return 8;
        case BATTLESHIP_CLASS_SPACESHIP:
            return 90;
        default:
            return 0;
    }
}

BATTLESHIP* init_battleship(BATTLESHIP_CLASS class, BATTLESHIP_OWNER owner, float dx, float dy){

    BATTLESHIP* battleship = malloc(sizeof(BATTLESHIP));
    float vx,vy;

    battleship->active = true;

    battleship->owner = owner;

    battleship->class = class;

    switch (class){
        case BATTLESHIP_CLASS_MISSILE:
            battleship->bmp = (owner == BATTLESHIP_OWNER_OPPONENT)?bmp_missile_red:bmp_missile_blue;
            vx = 1.5; vy = 0.5;
            break;
        case BATTLESHIP_CLASS_SPACESHIP:
            battleship->bmp = (owner == BATTLESHIP_OWNER_OPPONENT)?bmp_spaceship_red:bmp_spaceship_blue;
            vx = 0; vy = 0;
            break;
        default:
            break;
    }

    float x = DISPLAY_W / 2, y ;

    y = (battleship->owner == BATTLESHIP_OWNER_OPPONENT)? DISPLAY_H - 90 : 90 ;

    battleship->dx = dx;
    battleship->dy = dy;
    battleship->dxi = dx;
    battleship->dyi = dy;

    //battleship->ll = (battleship->owner == BATTLESHIP_OWNER_OPPONENT)? battleship->dx - 100 : battleship->dx + 100 ;
    //battleship->lr = (battleship->owner == BATTLESHIP_OWNER_OPPONENT)? battleship->dx + 100 : battleship->dx - 100 ;
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

    float x = DISPLAY_W / 2, y;

    y = (battleship->owner == BATTLESHIP_OWNER_OPPONENT)? DISPLAY_H - bsh: bsh ;

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
                battleship->ll = (((battleship->dy - y) / battleship->ml) + x < 0 + bsw/2)?
                                 0 + bsw/2 : ((battleship->dy - y) / battleship->ml) + x ;
                battleship->lr = (((battleship->dy - y) / battleship->mr) + x > DISPLAY_W - bsw/2)?
                                 DISPLAY_W - bsw/2 :((battleship->dy - y) / battleship->mr) + x;

                dist_r = (battleship->lr-(battleship->dx+bsw/2)<=0)?1:battleship->lr-(battleship->dx+bsw/2);
                dist_l = ((battleship->dx-bsw/2)-battleship->ll<=0)?1:(battleship->dx-bsw/2)-battleship->ll;

                printf("me:%f\nmd:%f\nll:%f\nlr:%f\ndx:%f\n\n",battleship->ml,battleship->mr,battleship->ll,battleship->lr,battleship->dx);

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
                                battleship->turning_frame = 0;
                            }
                        }

                battleship->dx += battleship->vx;
                battleship->dy = ((battleship->dy-45) >= game_bs_host_limit) ? battleship->dy - battleship->vy : battleship->dy;

            }else if(battleship->owner == BATTLESHIP_OWNER_OPPONENT) {
                        //battleship->dy += 0.5;
                        battleship->ll = (((battleship->dy - y) / battleship->ml) + x < 0 + bsw/2 )?
                                 0 + bsw/2 : ((battleship->dy - y) / battleship->ml) + x ;
                        battleship->lr = (((battleship->dy - y) / battleship->mr) + x > DISPLAY_W - bsw/2)?
                                 DISPLAY_W - bsw/2 :((battleship->dy - y) / battleship->mr) + x;



                        dist_r = (battleship->lr-(battleship->dx+bsw/2)<=0)?1:battleship->lr-(battleship->dx+bsw/2);
                        dist_l = ((battleship->dx-bsw/2)-battleship->ll<=0)?1:(battleship->dx-bsw/2)-battleship->ll;


                        //static int turning_frame = 0;
                        double prob,mod=(rand()%100)/100.0;

                        // Calcula chance de inverter velocidade horizontal
                        prob=(battleship->vx>0)?(1.0/pow(dist_r,7.0/8.0))+mod:(1.0/pow(dist_l,7.0/8.0))+mod;

                        // Inverte a velocidade vertical ao se aproximar das bordas de cima ou de baixo
                        //battleship->vy=((battleship->vy>0 && (bsh+battleship->dy+(bsh/2))==DISPLAY_H-270)||
                        //                (battleship->vy<0 && battleship->dy-(bsh/2)==20))?battleship->vy*(-1):battleship->vy;

                        if (prob >= 1.0 && battleship->turning_direction == TURNING_DIRECTION_NONE)
                            battleship->turning_direction = (battleship->vx>0)?TURNING_DIRECTION_LEFT:TURNING_DIRECTION_RIGHT;

                        if (battleship->turning_direction != TURNING_DIRECTION_NONE){
                            battleship->turning_frame++;
                            dvx = (float)fabs(battleship->vxi)/10;

                            battleship->vx = (battleship->turning_direction == TURNING_DIRECTION_LEFT)?
                                             battleship->vx-dvx : battleship->vx+dvx ;
                            if (battleship->turning_frame > 10){
                                battleship->turning_direction = TURNING_DIRECTION_NONE;
                                battleship->vx = ((battleship->vx > 0) ? 1 : -1) * (float)fabs(battleship->vxi);
                                battleship->turning_frame = 0;
                            }
                        }

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

void draw_debug(BATTLESHIP *battleship) {
    int bsh = get_battleship_height(battleship->class);
    int bsw = get_battleship_width(battleship->class);

    float dx, dy;

    dx = get_normalized_dx(battleship);
    dy = get_normalized_dy(battleship);

    static bool started = false;

    if (DEBUG && battleship->owner == BATTLESHIP_OWNER_OPPONENT && battleship->class != BATTLESHIP_CLASS_SPACESHIP) {
        float x = DISPLAY_W / 2, y = DISPLAY_H - bsh;
        static float dyp, dxp, dypl, dxpl;
        static float dxe, dxd, dxel, dxdl;

        if (!started) {
            dyp = battleship->dyi;
            dxp = battleship->dxi;
            dxe = dxp - 200;
            dxd = dxp + 200;
            started = true;
        }

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
            for (i = -45; i < DISPLAY_H - bsh; i++) {
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
                } else {
                    al_draw_filled_rectangle(dxdl - 2, i - 2,
                                             dxdl + 2, i + 2,
                                             al_map_rgb(255, 0, 0));
                }
            }
            for (i = (int) dypl; i < DISPLAY_H - bsh; i++) {
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
        if (current_game_state == GAME_STATE_IN_GAME_MULTIPLAYER_CLIENT) {
            battleship->dx = (float) fabs(DISPLAY_W - battleship->dx);
            battleship->dy = (float) fabs(DISPLAY_H - battleship->dy);
        }

        ALLEGRO_COLOR color;

        switch (battleship->owner) {
            case BATTLESHIP_OWNER_NONE:
            case BATTLESHIP_OWNER_PLAYER:
                color = (battleship->turning_direction == TURNING_DIRECTION_NONE) ?
                        al_map_rgb(250, 0, 0) : al_map_rgb(0, 250, 0);
                break;
            case BATTLESHIP_OWNER_OPPONENT:
                color = (battleship->turning_direction == TURNING_DIRECTION_NONE) ?
                        al_map_rgb(0, 0, 255) : al_map_rgb(0, 250, 0);
                break;
            case BATTLESHIP_OWNER_SPECIAL:
            default:
                color = (battleship->turning_direction == TURNING_DIRECTION_NONE) ?
                        al_map_rgb(255, 255, 153) : al_map_rgb(0, 250, 0);
                break;
        }

        al_draw_rectangle(battleship->dx - bsw / 2, battleship->dy - bsh / 2,
                          battleship->dx + bsw / 2, battleship->dy + bsh / 2,
                          color, 2);
        al_draw_filled_rectangle(battleship->dx - 2, battleship->dy - 2,
                                 battleship->dx + 2, battleship->dy + 2,
                                 color);
    }
}

void draw_target_lock(BATTLESHIP *battleship){

    int outer_margin = 4;

//    float ldx = get_left_dx(battleship) - outer_margin;
//    float tdy = get_top_dy(battleship) - outer_margin;
//    float rdx = get_righ_dx(battleship) + outer_margin;
//    float bdy = get_bottom_dy(battleship) + outer_margin;
//
//    int bsw = get_battleship_width(battleship->class);
//    int bsh = get_battleship_height(battleship->class);
//
//    float stop_x = (bsw+2*outer_margin) / 6.0f;
//    float stop_y = (bsh+2*outer_margin) / 6.0f;

    int bsw = get_battleship_width(battleship->class);
    int bsh = get_battleship_height(battleship->class);

    int stop = (bsw > bsh) ? bsw : bsh;

    float ldx = battleship->dx - stop/2 - outer_margin;
    float tdy = battleship->dy - stop/2 - outer_margin;
    float rdx = battleship->dx + stop/2 + outer_margin;
    float bdy = battleship->dy + stop/2 + outer_margin;


    float stop_x = (stop+2*outer_margin) / 6.0f;
    float stop_y = (stop+2*outer_margin) / 6.0f;

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
        flags = (battleship->owner == BATTLESHIP_OWNER_PLAYER)?ALLEGRO_FLIP_VERTICAL|ALLEGRO_FLIP_HORIZONTAL:0;
        if (PITTHAN_MODE) draw_ship = (battleship->owner != BATTLESHIP_OWNER_OPPONENT);

    } else {
        flags = (battleship->owner == BATTLESHIP_OWNER_OPPONENT)?ALLEGRO_FLIP_VERTICAL|ALLEGRO_FLIP_HORIZONTAL:0;
        if (PITTHAN_MODE) draw_ship = (battleship->owner != BATTLESHIP_OWNER_PLAYER);
    }

    if (PITTHAN_MODE && battleship->class == BATTLESHIP_CLASS_SPACESHIP) draw_ship = !draw_ship;

    if (!draw_ship) return;

    al_draw_bitmap(battleship->bmp, get_left_dx(battleship), get_top_dy(battleship),flags);

    //if (DEBUG) draw_debug(battleship);
}

void draw_ship_word(BATTLESHIP *battleship,bool is_target){
    int bsh = get_battleship_height(battleship->class);
    float dx = get_normalized_dx(battleship);
    float dy = get_normalized_dy(battleship);

    ALLEGRO_COLOR color = (is_target)?al_map_rgb(200,115,0):al_map_rgb(255,255,255);

    al_draw_text(main_font_size_25,color,dx,dy+bsh/2,
                 ALLEGRO_ALIGN_CENTER,battleship->word);
}

unsigned short remove_next_letter_from_battleship(BATTLESHIP *battleship){
    unsigned short i;
    for (i=0;i<strlen(battleship->word);i++){
       if (battleship->word[i] != ' '){
           if (!__isascii(battleship->word[i])){
               battleship->word[i] = '|';
               battleship->word = remove_char(battleship->word,'|');
           }
           battleship->word[i] = ' ';
           break;
       }
    }
    return (unsigned short)strlen(battleship->word)-i-(unsigned short)1;
}

char get_next_letter_from_battleship(BATTLESHIP *battleship) {

    for (int i = 0; i < strlen(battleship->word); i++) {
        if (battleship->word[i] != ' ') {
            return get_next_ascii_char(battleship->word + i);
        }
    }
    return 0;
}