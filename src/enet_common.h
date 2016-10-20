//
// Created by fabio on 19/10/16.
//

#ifndef BATTLE_TYPE_ENET_COMMON_H
#define BATTLE_TYPE_ENET_COMMON_H

#include "game.h"

#define DEFAULT_PORT 9233

typedef enum {
    MESSAGE_TYPE_GAME_SNAPSHOP
} MESSAGE_TYPE;


typedef struct {
    MESSAGE_TYPE type;
    GAME_SNAPSHOT game;
} SERVER_MESSAGE;

#endif //BATTLE_TYPE_ENET_COMMON_H
