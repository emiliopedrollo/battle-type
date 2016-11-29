#define _GNU_SOURCE

#include <allegro5/allegro_font.h>
#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro_primitives.h>
#include "rank.h"
#include "main.h"
#include "buttons.h"
#include "utils.h"
#include "sha256.h"
#include "game.h"

int shortcut_key_pressed_rank = -1;
Button buttons[1];
RANK_ENTRY rank[10];
bool is_mouse_down_rank = false;
bool is_mouse_down_on_button_rank = false;
bool is_entering_new_rank = false;
unsigned short new_rank_pos;

void init_buttons_rank();
void load_rank_entries();
void create_hash(char **hash, char *text);
void create_rank_file();
void draw_buttons();
void process_new_rank_entry();
void on_button_click_rank(int i);
void persist_rank_entries();
bool is_valid_rank_file(FILE *rank);

void init_buttons_rank(){
    buttons[0] = init_button(main_font_size_45,"Voltar ao &menu",DISPLAY_W/2,700,380);
}

void load_rank_entries(){

    if( access( "rank", F_OK ) == -1 ) {
        create_rank_file();
    }

    FILE *rank_file = fopen("rank","r");

    if (!is_valid_rank_file(rank_file)){
        create_rank_file();
    };

    size_t len = 0;
    unsigned char *text = malloc(1);
    char *entry;
    char *pos;
    char *tok;
    char *endptr;

    text[0] = 0;

    rewind(rank_file);
    for (int i = 0; i < 10; i++){
        entry = NULL;
        getline(&entry,&len,rank_file);
        if ((pos=strchr(entry, '\n')) != NULL) *pos = '\0';

        char *str = malloc(strlen(entry) + 1);
        memcpy(str, entry, sizeof entry);
        tok = strtok(str, "|");
        strcpy(rank[i].name,tok);
        tok = strtok(NULL, "|");
        rank[i].score = (unsigned int)strtoimax(tok,&endptr,10);
        free(str);
    }

    fclose(rank_file);

}

void create_hash(char **hash, char *text){

    char salt[] = "rUEpf3wKpty9ii9jecVUx617cbd64GmNs0P9URpSiDLn2dRGvq6UaqUx9y6BmNvGcwLKDFVGrYFgRUQivZTpGKZBTS";

    char *cypher = concat(salt,text);

    BYTE buf[SHA256_BLOCK_SIZE];

    SHA256_CTX context;
    sha256_init(&context);
    sha256_update(&context, (BYTE*)cypher, strlen(cypher));
    sha256_final(&context, buf);

    free(*hash);
    *hash = calloc(64,sizeof(char));


    char *part = malloc(sizeof(char)*3);

    for (int i = 0; i < SHA256_BLOCK_SIZE; i++){
        sprintf(part,"%02x",(unsigned char)buf[i]);
        strcat(*hash,part);
    }
}

bool is_valid_rank_file(FILE *rank){

    rewind(rank);

    size_t len = 0;
    unsigned char *text = malloc(1);
    char *score_entry;
    char *pos;
    char *file_hash;
    char *computed_hash;

    text[0] = 0;

    for (int i = 0; i < 10; i++){
        score_entry = NULL;
        getline(&score_entry,&len,rank);
        text = (unsigned char*)concat((char*)text,score_entry);
    }

    file_hash = NULL;
    getline(&file_hash,&len,rank);
    if ((pos=strchr(file_hash, '\n')) != NULL) *pos = '\0';

    computed_hash = NULL;
    create_hash(&computed_hash,(char*)text);

    rewind(rank);
    return (bool) strcmp(file_hash,computed_hash) == 0;
}

void create_rank_file(){
    FILE *rank_file = fopen("rank","wb+");

    unsigned char *text;

    text = malloc(1);
    text[0] = 0;

    char *entry;
    char *player = "SYS|";
    char *score = malloc(1);
    score[0] = 0;

    for (int i=0;i<10;i++){
        sprintf(score,"%03u\n",100-(10*i));
        entry = concat(player,score);
        text = (unsigned char*)concat((char*)text,entry);
        fwrite(entry,1,sizeof(char)*strlen(entry),rank_file);
    }

    char *hash = NULL;
    create_hash(&hash,(char*)text);

    fwrite(hash,1,sizeof(char)*strlen(hash),rank_file);

    fclose(rank_file);
}

void persist_rank_entries(){
    FILE *rank_file = fopen("rank","wb+");

    unsigned char *text;

    text = malloc(1);
    text[0] = 0;

    char *entry;
    char *score = malloc(1);
    score[0] = 0;

    for (int i=0;i<10;i++){
        sprintf(score,"|%03u\n",rank[i].score);
        entry = concat(rank[i].name,score);
        text = (unsigned char*)concat((char*)text,entry);
        fwrite(entry,1,sizeof(char)*strlen(entry),rank_file);
    }

    char *hash = NULL;
    create_hash(&hash,(char*)text);

    fwrite(hash,1,sizeof(char)*strlen(hash),rank_file);

    fclose(rank_file);
}

void process_new_rank_entry(){
    unsigned int score = get_last_game_score();
    short pos = -1;
    for (short i=0;i<10;i++) {
        if (score > rank[i].score) {
            pos = i;
            break;
        }
    }

    if (pos == -1) return;

    is_entering_new_rank = true;
    new_rank_pos = (unsigned short)pos;

    for (short i=9;i>=0;i--) {
        if (i>pos){
            rank[i] = rank[i-1];
        } else if (pos == i) {
            rank[i].name[0] = 0;
            rank[i].score = score;
        }
    }
}

void init_rank() {
    init_buttons_rank();
    load_rank_entries();
    process_new_rank_entry();
}

void on_button_click_rank(int i){
    switch (i){
        case 0:
            if (!is_entering_new_rank){
                change_game_state(GAME_STATE_MAIN_MENU);
            }
            break;
        default:
            break;
    }
}

void on_key_press_rank(ALLEGRO_KEYBOARD_EVENT event){

    if (!is_entering_new_rank){
        switch (event.keycode) {
            case ALLEGRO_KEY_M:
                shortcut_key_pressed_rank = 0;
                break;
            default:
                break;
        }
    } else {
        char key = NULL;
        short len;

        switch (event.keycode) {
            case ALLEGRO_KEY_A:
                key = 'A';
                break;
            case ALLEGRO_KEY_B:
                key = 'B';
                break;
            case ALLEGRO_KEY_C:
            case ALLEGRO_KEY_BACKSLASH2: // Ç
                key = 'C';
                break;
            case ALLEGRO_KEY_D:
                key = 'D';
                break;
            case ALLEGRO_KEY_E:
                key = 'E';
                break;
            case ALLEGRO_KEY_F:
                key = 'F';
                break;
            case ALLEGRO_KEY_G:
                key = 'G';
                break;
            case ALLEGRO_KEY_H:
                key = 'H';
                break;
            case ALLEGRO_KEY_I:
                key = 'I';
                break;
            case ALLEGRO_KEY_J:
                key = 'J';
                break;
            case ALLEGRO_KEY_K:
                key = 'K';
                break;
            case ALLEGRO_KEY_L:
                key = 'L';
                break;
            case ALLEGRO_KEY_M:
                key = 'M';
                break;
            case ALLEGRO_KEY_N:
                key = 'N';
                break;
            case ALLEGRO_KEY_O:
                key = 'O';
                break;
            case ALLEGRO_KEY_P:
                key = 'P';
                break;
            case ALLEGRO_KEY_Q:
                key = 'Q';
                break;
            case ALLEGRO_KEY_R:
                key = 'R';
                break;
            case ALLEGRO_KEY_S:
                key = 'S';
                break;
            case ALLEGRO_KEY_T:
                key = 'T';
                break;
            case ALLEGRO_KEY_U:
                key = 'U';
                break;
            case ALLEGRO_KEY_V:
                key = 'V';
                break;
            case ALLEGRO_KEY_W:
                key = 'W';
                break;
            case ALLEGRO_KEY_X:
                key = 'X';
                break;
            case ALLEGRO_KEY_Y:
                key = 'Y';
                break;
            case ALLEGRO_KEY_Z:
                key = 'Z';
                break;
            case ALLEGRO_KEY_MINUS:
                key = '-';
                break;
            case ALLEGRO_KEY_BACKSPACE:
                len = (short)strlen(rank[new_rank_pos].name);
                rank[new_rank_pos].name[len-1] = 0;
                break;
            default:
                break;
        }

        if (key != NULL){
            char *add = calloc(2,sizeof(char));
            add[0] = key;
            strcat(rank[new_rank_pos].name,add);
        }

        if (strlen(rank[new_rank_pos].name) >= 3){
            is_entering_new_rank = false;
            persist_rank_entries();
        }

    }

}

void draw_buttons(){
    int total_buttons = sizeof(buttons)/sizeof(buttons[0]);
    for (int i = 0; i < total_buttons; i++){
        draw_button(buttons[i]);
    }
}

void on_mouse_move_rank(int x,int y){
    bool is_over_button = false;

    if (!is_mouse_down_rank){
        int total_buttons = sizeof(buttons)/sizeof(buttons[0]);
        for (int i = 0; i < total_buttons; i++){
            if (!buttons[i].visible) continue;
            if (is_coordinate_inside_button(buttons[i], x, y)){
                is_over_button = true;
                buttons[i].state = (buttons[i].state != BUTTON_STATE_ACTIVE)?
                                   BUTTON_STATE_HOVER:buttons[i].state;
            } else
                buttons[i].state = (buttons[i].state == BUTTON_STATE_HOVER)?
                                   BUTTON_STATE_NORMAL:buttons[i].state;
        }
    }

    if (is_over_button || is_mouse_down_on_button_rank)
        al_set_system_mouse_cursor(display,
                                   ALLEGRO_SYSTEM_MOUSE_CURSOR_ALT_SELECT);
    else
        al_set_system_mouse_cursor(display,
                                   ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
}

void on_mouse_down_rank(int x,int y){
    int total_buttons = sizeof(buttons)/sizeof(buttons[0]);
    for (int i = 0; i < total_buttons; i++){
        if (buttons[i].visible && is_coordinate_inside_button(buttons[i], x, y)){
            is_mouse_down_on_button_rank = true;
            buttons[i].state = BUTTON_STATE_ACTIVE;
            break;
        }
    }
    is_mouse_down_rank = true;
}

void on_mouse_up_rank(int x,int y){
    int total_buttons = sizeof(buttons)/sizeof(buttons[0]);
    for (int i = 0; i < total_buttons; i++){
        if (buttons[i].visible && buttons[i].state == BUTTON_STATE_ACTIVE){
            if (is_coordinate_inside_button(buttons[i], x, y)){
                buttons[i].state = BUTTON_STATE_HOVER;
                on_button_click_rank(i);
            } else {
                buttons[i].state = BUTTON_STATE_NORMAL;
            }
            break;
        }
    }
    is_mouse_down_rank = false;
    is_mouse_down_on_button_rank = false;

    on_mouse_move_rank(x,y);
}

void on_timer_rank(){

    int static frame_count = 0;

    if (shortcut_key_pressed_rank == -1) return;

    switch (frame_count++){
        case 0:
            on_mouse_down_rank(
                    buttons[shortcut_key_pressed_rank].x+1,buttons[shortcut_key_pressed_rank].y+1);
            break;
        case 10:
            on_mouse_up_rank(
                    buttons[shortcut_key_pressed_rank].x+1,buttons[shortcut_key_pressed_rank].y+1);
            frame_count = 0;
            shortcut_key_pressed_rank = -1;
            break;
        default:
            break;
    }
}

void on_redraw_rank(){

    char underscore1[2] = "_";
    char underscore2[3] = " _";
    char underscore3[4] = "  _";
    char *underscore = malloc(sizeof underscore3);

    static short frame_count = 0;

    if (!is_entering_new_rank) draw_buttons();

    al_draw_text(main_font_size_60,al_map_rgb(255,255,255),DISPLAY_W/2,60,ALLEGRO_ALIGN_CENTER,"RANK");

    int top;
    char *score = malloc(1);
    score[0] = NULL;


    for (int i = 0; i < 10; i++){
        sprintf(score,"%03u",rank[i].score);

        top = 160+(45*i);

        al_draw_text(main_font_size_60,al_map_rgb(255,255,255),60,top,ALLEGRO_ALIGN_LEFT,rank[i].name);
//        al_draw_text(main_font_size_60,al_map_rgb(255,255,255),60,top,ALLEGRO_ALIGN_LEFT,rank[i].name);
        al_draw_text(main_font_size_60,al_map_rgb(255,255,255),DISPLAY_W-60,top,ALLEGRO_ALIGN_RIGHT,score);
    }

    if (is_entering_new_rank){

        top = 165+(45*new_rank_pos);

        if ((frame_count++%40) < 20){
            switch (strlen(rank[new_rank_pos].name)){
                case 0:
                    strcpy(underscore,underscore1);
                    break;
                case 1:
                    strcpy(underscore,underscore2);
                    break;
                case 2:
                    strcpy(underscore,underscore3);
                    break;
                default:
                    break;
            }

            al_draw_text(main_font_size_60,al_map_rgb(255,255,255),60,top,ALLEGRO_ALIGN_LEFT,underscore);
        }

    }

}