#include "battleship.h"
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
    // Carrega o imagem das spaceships e mísseis para ambos os jogadores, ou máquina.
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
    // Limpa e desassocia da memória todos recursos e espaços utilizados pelas imagens.
    al_destroy_bitmap(bmp_spaceship_blue);
    al_destroy_bitmap(bmp_spaceship_red);
    al_destroy_bitmap(bmp_missile_blue);
    al_destroy_bitmap(bmp_missile_red);
}

int get_battleship_height(BATTLESHIP_CLASS class){
    // Define a altura de cada nave dependendo de sua classe.
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
    // De forma análoga a altura, define a largura de cada nave.
    switch (class){
        case BATTLESHIP_CLASS_MISSILE:
            return 8;
        case BATTLESHIP_CLASS_SPACESHIP:
            return 90;
        default:
            return 0;
    }
}

BATTLESHIP* init_battleship(BATTLESHIP_CLASS class, BATTLESHIP_OWNER owner, float dx, float dy, float x, int game_level) {
    // Inicializa naves com parametros iniciais:
    BATTLESHIP *battleship = malloc(sizeof(BATTLESHIP));
    float vx = 0, vy = 0;

    // validação para print;
    battleship->active = true;

    // jogador a que pertence;
    battleship->owner = owner;

    // classe, mísseis e spaceships;
    battleship->class = class;

    switch (class) {
        case BATTLESHIP_CLASS_MISSILE:
            battleship->bmp = (owner == BATTLESHIP_OWNER_OPPONENT) ? bmp_missile_red : bmp_missile_blue;
            vy = ((float)(rand()%game_level)/10)+1;
            vx = vy+1;
            break;
        case BATTLESHIP_CLASS_SPACESHIP:
            battleship->bmp = (owner == BATTLESHIP_OWNER_OPPONENT) ? bmp_spaceship_red : bmp_spaceship_blue;
            vx = 1;
            vy = 0;
            break;
        default:
            break;
    }

    // posição atual;
    battleship->dx = dx;
    battleship->dy = dy;
    // posição inicial;
    battleship->dxi = dx;
    battleship->dyi = dy;

    if (BATTLESHIP_CLASS_MISSILE == battleship->class) {
        float y;

        y = (battleship->owner == BATTLESHIP_OWNER_OPPONENT) ? DISPLAY_H - 90 : 90;

        // limites esquerdos e direitos iniciais;
        battleship->ll = battleship->dx - 100;
        battleship->lr = battleship->dx + 100;

        // coeficiente angular de cada limite;
        battleship->ml = (battleship->dy - y) / (battleship->ll - x);
        battleship->mr = (battleship->dy - y) / (battleship->lr - x);
    }

    // velocidade atual;
    battleship->vx = vx;
    battleship->vy = vy;
    // velocidade inicial;
    battleship->vxi = vx;
    battleship->vyi = vy;

    // mudança de direção;
    battleship->turning_direction = TURNING_DIRECTION_NONE;
    battleship->turning_frame = 0;
    // estado inicial da nave em questão;
    change_battleship_state(battleship, BATTLESHIP_MOVE_STATE_INITAL_STATE);

    // define como NULL o valor inicial `word`, este que recebera uma palavra posteriormente;
    battleship->word = NULL;

    // seta para falso `exploding`, variável responsável por decidir o início da animação de explosão das spaceships ;
    battleship->exploding = false;
    // seta para falso `exploding_with_lasers`, mesma função de `exploding`, porém para mísseis;
    battleship->exploding_with_lasers = false;
    // define o frame de explosão com um valor inícial inválido (necessita ser maior que zero).
    battleship->explosion_frame = -1;

    return battleship;
}

void change_battleship_state(BATTLESHIP *battleship,BATTLESHIP_MOVE_STATE state){

    // Em caso do estado de movimento inícial da nave ser BATTLESHIP_MOVE_STATE_DEMO_PUSHBACK,
    // define alguns parámetros para o push back.
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

    // Atribuí um estado de movimento a nave.
    battleship->state = state;

}

void calculate_ship_turn_frame(BATTLESHIP *battleship){
    // Faz com que nos 10 frames de virada da nave, a velocidade seja reduzida gradualmente.
    float dvx;
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
}

void update_ship_frame_count(BATTLESHIP *battleship){
    // Em caso de `exploding` ser verdadeiro incremeta `explosion_frame`,
    // responsável por passa um a um os frames da explosão.
    if (battleship->exploding) battleship->explosion_frame++;
}

bool move_ship(BATTLESHIP *battleship, float target_dx) {

    double dist_r, dist_l;

    // Define a altura e largura das naves.
    int bsw = al_get_bitmap_width(battleship->bmp);
    int bsh = al_get_bitmap_height(battleship->bmp);
    int n_frames_pushback_placement = 120;

    double prob, mod;

    // Define um modificador randôminco de 0.01 a 1.00
    mod = (rand() % 100) / 100.0;



    float y;

    // Seta `y` com a posição da spaceship inimiga.
    y = (battleship->owner == BATTLESHIP_OWNER_OPPONENT) ? DISPLAY_H - bsh : bsh;

    // Seleciona o movimento apropriado de acordo com o `state` da nave.
    switch (battleship->state) {
        case BATTLESHIP_MOVE_STATE_DEMO:
            // Define as distâncias laterais, usadas posteriormente, para
            // calcular a chance de inversão de direção no movimento da nave.
            dist_r = (DISPLAY_W-(battleship->dx+bsw/2)<=0)?1:DISPLAY_W-(battleship->dx+bsw/2);
            dist_l = (battleship->dx-bsw/2<=0)?1:battleship->dx-bsw/2;

            // Calcula chance de inverter velocidade horizontal.
            prob=(battleship->vx>0)?(1.0/pow(dist_r,7.0/8.0))+mod:(1.0/pow(dist_l,7.0/8.0))+mod;

            // Inverte a velocidade vertical ao se aproximar das bordas de cima ou de baixo.
            battleship->vy = ((battleship->vy > 0 && (bsh + battleship->dy + (bsh / 2)) == DISPLAY_H - 270) ||
                              (battleship->vy < 0 && battleship->dy - (bsh / 2) == 20)) ? battleship->vy * (-1)
                                                                                        : battleship->vy;

            // Altera o estado de `turning_direction` para a direção contrária ao movimento atual da nave,
            // se `prob` for maior que 1.00 e a nave já não estiver mudando de direção.
            if (prob >= 1 && battleship->turning_direction == TURNING_DIRECTION_NONE)
                battleship->turning_direction = (battleship->vx > 0) ? TURNING_DIRECTION_LEFT : TURNING_DIRECTION_RIGHT;

            calculate_ship_turn_frame(battleship);

            // Altera a posição da nave, incrementando o valor responsável por esta,
            // através das velocidades de `vx` e `vy`.
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
            if (battleship->class == BATTLESHIP_CLASS_MISSILE){
                // Define os pontos limites, cordenadas horizontais, a esquerda e direita do míssil,
                // levendo em consideração sua distância atual da spaceship inimiga.
                battleship->ll = (((battleship->dy - y) / battleship->ml) + target_dx < 0 + bsw/2)?
                                 0 + bsw/2 : ((battleship->dy - y) / battleship->ml) + target_dx ;
                battleship->lr = (((battleship->dy - y) / battleship->mr) + target_dx > DISPLAY_W - bsw/2)?
                                 DISPLAY_W - bsw/2 :((battleship->dy - y) / battleship->mr) + target_dx;
                // Através dos limites determina a distância até cada um deles, e se for zero retorna 1;
                dist_r = (battleship->lr-(battleship->dx+bsw/2)<=0)?1:battleship->lr-(battleship->dx+bsw/2);
                dist_l = ((battleship->dx-bsw/2)-battleship->ll<=0)?1:(battleship->dx-bsw/2)-battleship->ll;
            } else {
                // Determina as distâncias entre as bordas da tela e a spaceship.
                dist_r = (DISPLAY_W-(battleship->dx+bsw/2)<=0)?1:DISPLAY_W-(battleship->dx+bsw/2);
                dist_l = (battleship->dx-bsw/2<=0)?1:battleship->dx-bsw/2;
            }

            //Calcula o valor que será utilizado em seguida para condição de volta dos mísseis e das spaceships.
            prob=(battleship->vx>0)?(1.0/pow(dist_r,7.0/8.0))+mod:(1.0/pow(dist_l,7.0/8.0))+mod;

            // Altera o estado de `turning_direction` para a direção contrária ao movimento atual da nave,
            // se `prob` for maior que 1.00 e a nave já não estiver mudando de direção.
            if (prob >= 1 && battleship->turning_direction == TURNING_DIRECTION_NONE) {
                battleship->turning_direction = (battleship->vx > 0) ? TURNING_DIRECTION_LEFT:TURNING_DIRECTION_RIGHT;
            }

            calculate_ship_turn_frame(battleship);

            // Faz o cálculo do coeficiente angular da reta entre míssil e a spaceship inimiga.
            //battleship->m = (battleship->dy - y) / (battleship->dx - target_dx);

            // Move as battleships verticalmente de acordo com seus `vy` e previne que passem de seus objetivos.
            if (battleship->owner == BATTLESHIP_OWNER_PLAYER) {
                battleship->dy = (get_top_dy(battleship) >= game_bs_host_limit) ? battleship->dy - battleship->vy : battleship->dy;
            }else if(battleship->owner == BATTLESHIP_OWNER_OPPONENT) {
                battleship->dy = (get_bottom_dy(battleship) <= game_bs_client_limit) ? battleship->dy + battleship->vy : battleship->dy;
            }

            /*if(battleship->class == BATTLESHIP_CLASS_MISSILE && (y - get_bottom_dy(battleship)) < 200 ) {
                battleship->dx = ((battleship->dy + (battleship->vy*5) - y) / battleship->m) + target_dx;

            }else{*/
                battleship->dx += battleship->vx;
            //}

            // Checa se um míssel acerta uma spaceship do oponente e seta `exploding`
            // para verdadeiro, após retorna falso ou verdadeiro que auxiliarão na retirada
            if (!battleship->exploding) {
                if(get_bottom_dy(battleship) >= game_bs_client_limit && battleship->owner == BATTLESHIP_OWNER_OPPONENT){
                    battleship->exploding = true;
                    return true;
                } else if((is_multiplayer() || !PITTHAN_MODE) &&
                          get_top_dy(battleship) <= game_bs_host_limit && battleship->owner == BATTLESHIP_OWNER_PLAYER){
                    battleship->exploding = true;
                    return true;
                }
            }
        default:
            break;
    }
    return false;
}

float get_normalized_dx(BATTLESHIP *battleship){
    // Inverte o ´dx´ das neves client.
    float dx = battleship->dx;
    if (is_multiplayer_client()) {
        dx = DISPLAY_W - dx;
    }
    return dx;
}

float get_normalized_dy(BATTLESHIP *battleship){
    // Inverte o ´dy´ das neves client.
    float dy = battleship->dy;
    if (is_multiplayer_client()) {
        dy = DISPLAY_H - dy;
    }
    return dy;
}


float get_left_dx(BATTLESHIP *battleship){
    // Encontra o ponto da lateral esquerda da nave.
    float dx = get_normalized_dx(battleship);
    int bsw = get_battleship_width(battleship->class);
    return dx - (bsw/2.0f);
}


float get_top_dy(BATTLESHIP *battleship){
    // Encontra o ponto da lateral superior da nave
    float dy = get_normalized_dy(battleship);
    int bsh = get_battleship_height(battleship->class);
    return dy - (bsh/2.0f);
}


float get_righ_dx(BATTLESHIP *battleship){
    // Encontra o ponto da lateral direita da nave.
    float dx = get_normalized_dx(battleship);
    int bsw = get_battleship_width(battleship->class);
    return dx + (bsw/2.0f);
}


float get_bottom_dy(BATTLESHIP *battleship){
    // Encontra o ponto da lateral inferior da nave.
    float dy = get_normalized_dy(battleship);
    int bsh = get_battleship_height(battleship->class);
    return dy + (bsh/2.0f);
}



void draw_debug(BATTLESHIP *battleship) {
    // Desenha de debugs váriados.
    int bsh = get_battleship_height(battleship->class);
    int bsw = get_battleship_width(battleship->class);

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
    // Desenha a mira na nave.

    // Cria uma margem de 4 pixels.
    int outer_margin = 4;

    // Seta variáveis com tamanhao de altura e largura da nava.
    int bsw = get_battleship_width(battleship->class);
    int bsh = get_battleship_height(battleship->class);

    // Define uma variável `stop` que pega a maior dimensão da nave,
    // pra desenhar uma mira, quadrada, que englobe a nave por inteiro.
    int stop = (bsw > bsh) ? bsw : bsh;

    float dx = get_normalized_dx(battleship);
    float dy = get_normalized_dy(battleship);

    // Popula quatro variaveis encarregadas dos pontos
    // direiro, esquerdo, inferior e superior da mira.
    float ldx = dx - stop/2 - outer_margin;
    float tdy = dy - stop/2 - outer_margin;
    float rdx = dx + stop/2 + outer_margin;
    float bdy = dy + stop/2 + outer_margin;


    float stop_x = (stop+2*outer_margin) / 6.0f;
    float stop_y = (stop+2*outer_margin) / 6.0f;

    // Define a cor a ser desenhada.
    ALLEGRO_COLOR color = al_map_rgb(165,0,0);

    // Linhas exteriores.
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

    // Linhas intermediarias.
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

    // Linhas cruzadas.
    al_draw_line(ldx + stop_x * 1, dy,
                 ldx + stop_x * 2, dy,
                 color,2);

    al_draw_line(rdx - stop_x * 2, dy,
                 rdx - stop_x * 1, dy,
                 color,2);

    al_draw_line(dx, tdy + stop_y * 1,
                 dx, tdy + stop_y * 2,
                 color,2);

    al_draw_line(dx, bdy - stop_y * 2,
                 dx, bdy - stop_y * 1,
                 color,2);

    // Ponto central.
    al_draw_filled_rectangle(dx-2,dy-2,
                             dx+2,dy+2,
                             color);

}

void draw_ship(BATTLESHIP *battleship) {

    int flags;

    bool draw_ship = true;

    if (current_game_state == GAME_STATE_IN_GAME_MULTIPLAYER_CLIENT) {
        // Define que no estado um Dois Jogadores, no lado do Cliente, as imagens das naves do Servidor serão ivertidas.
        flags = (battleship->owner == BATTLESHIP_OWNER_PLAYER)?ALLEGRO_FLIP_VERTICAL|ALLEGRO_FLIP_HORIZONTAL:0;
    } else {
        // Define que no estado um Dois Jogadores ou Um Jogador, no lado do Servidor,
        // as imagens das naves do Cliente serão ivertidas.
        flags = (battleship->owner == BATTLESHIP_OWNER_OPPONENT)?ALLEGRO_FLIP_VERTICAL|ALLEGRO_FLIP_HORIZONTAL:0;
        if (PITTHAN_MODE && current_game_state == GAME_STATE_IN_GAME_SINGLE_PLAYER)
            // Seta como falso todas as naves do Servidor.
            draw_ship = (battleship->owner != BATTLESHIP_OWNER_PLAYER);
    }

    // Define como verdadeiro as spaceships, porém os mísseis do Servidor continuam em falso.
    if (PITTHAN_MODE && battleship->class == BATTLESHIP_CLASS_SPACESHIP) draw_ship = true;

    // Enterrompe o print de naves se `draw_ship` for falso.
    if (!draw_ship) return;

    // Desenha imagem das naves.
    al_draw_bitmap(battleship->bmp, get_left_dx(battleship), get_top_dy(battleship),flags);

    //if (DEBUG) draw_debug(battleship);

    if (battleship->exploding && battleship->explosion_frame >= 0) {
        // Testa as condições apropriadas para ver se o desenho de explosão é necessário.

        float target_y;

        // Se estiver no modo Dois Jogadores, no lado do Cliente, e for uma nave
        // do Servidor `target_y` será o topo da spaceship do Cliente. Porém se for
        // uma nave do Cliente `target_y` será o fundo da spaceship do Servidor.
        if (is_multiplayer_client())
            target_y = (battleship->owner == BATTLESHIP_OWNER_PLAYER)?
                       get_top_dy(client_mothership):get_bottom_dy(host_mothership);
        else
            target_y = (battleship->owner == BATTLESHIP_OWNER_PLAYER)?
                       get_bottom_dy(client_mothership):get_top_dy(host_mothership);

        float target_x = get_normalized_dx((battleship->owner == BATTLESHIP_OWNER_PLAYER)?client_mothership:host_mothership);

        float x2 = get_normalized_dx(battleship), y2 = get_normalized_dy(battleship);
        if (battleship->exploding_with_lasers && battleship->explosion_frame < 9) {


            int thickness = ((battleship->explosion_frame < 4)||(battleship->explosion_frame > 6))?1:2;

            if(battleship->owner == BATTLESHIP_OWNER_OPPONENT)
                al_draw_line(target_x,target_y,x2,y2,al_map_rgb(0, 0, 255),thickness);
            else
                al_draw_line(target_x,target_y,x2,y2,al_map_rgb(255, 0, 0),thickness);

        }

        if(battleship->explosion_frame < 16)
            al_draw_bitmap(rsc_explosion[battleship->explosion_frame],
                           get_normalized_dx(battleship)-32,get_normalized_dy(battleship)-32, 0);

        if (battleship->explosion_frame >= 15){
            battleship->active = false;
            if (battleship->on_explosion_end != NULL && !is_multiplayer_client())
                battleship->on_explosion_end(&battleship->owner);
        }
    }
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