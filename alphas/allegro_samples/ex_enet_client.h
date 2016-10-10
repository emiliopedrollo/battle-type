/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ex_enet_client.h
 * Author: root
 *
 * Created on 8 de Outubro de 2016, 13:19
 */

#include <enet/enet.h>
#include <stdbool.h>

#ifndef EX_ENET_CLIENT_H
#define EX_ENET_CLIENT_H

int init_client(char* host, int port);

struct Client_Thread_Args{
    ENetHost *client;
};

bool thread_done;

#endif /* EX_ENET_CLIENT_H */

