//
// Created by fabio on 19/10/16.
//

#include <enet/enet.h>
#include <stdio.h>
#include <pthread.h>
#include <allegro5/altime.h>
#include "server.h"
#include "enet_common.h"
#include "utils.h"

struct Host_Thread_Args{
    ENetHost *host;
};

ENetHost *create_server(ENetAddress listener);
void *server_loop(void *arguments);
pthread_t init_server_thread(ENetHost *host);

bool server_running = false;
bool server_set_to_stop = false;
pthread_t server_thread;
ENetHost *host;
double last_time;
double cur_time;

void start_server(){

    unsigned short port = DEFAULT_PORT;
    char *binder = "0.0.0.0";

    ENetAddress listener;

    if (server_running) return;
    server_running = true;

    if (enet_initialize() != 0) {
        fprintf(stderr, "An error occurred while initializing ENet.\n");
    }

    enet_address_set_host(&listener, binder);
    listener.port = port;

    host = create_server(listener);

    last_time = al_get_time();

    server_thread = init_server_thread(host);

}

void stop_server(){
    server_set_to_stop = true;
    pthread_join(server_thread,NULL);

    enet_host_destroy(host);
    enet_deinitialize();
    server_running = false;
}

void *server_loop(void *arguments){

    ENetEvent event;
    struct Host_Thread_Args *args = (struct Host_Thread_Args*)arguments;

    ENetHost *host = (*args).host;

    server_set_to_stop = false;

    while (!server_set_to_stop){
        while (enet_host_service(host, &event, 0) > 0) {
            switch (event.type){
                case ENET_EVENT_TYPE_CONNECT:
                    printf("Server: A new client connected from %x:%hu.\n",
                           event.peer->address.host,
                           event.peer->address.port);
                    break;
                case ENET_EVENT_TYPE_DISCONNECT:
                    printf("Server: client disconnected.\n");
                    break;
                case ENET_EVENT_TYPE_NONE:
                default:
                    break;
            }
        }
        msleep(16);
    }
    pthread_exit(NULL);
}

pthread_t init_server_thread(ENetHost *host) {

    //pthread_t threads;
    int rc;
    struct Host_Thread_Args *args;

    pthread_t *cmp_thread = malloc(sizeof(pthread_t));
    args = malloc(sizeof(struct Host_Thread_Args));
    (*args).host = host;

    printf("Creating server comms thread.\n");
    rc = pthread_create(cmp_thread, NULL, server_loop, (void *) args);
    if (rc) {
        fprintf(stderr,"return code from pthread_create() is %d\n", rc);
    }

    return *cmp_thread;

}

ENetHost *create_server(ENetAddress listener) {

    ENetHost *host;

    host = enet_host_create(&listener /* the address to bind the server host to */,
                              1      /* allow up to 32 clients and/or outgoing connections */,
                              2      /* allow up to 2 channels to be used, 0 and 1 */,
                              0      /* assume any amount of incoming bandwidth */,
                              0      /* assume any amount of outgoing bandwidth */);
    if (host == NULL) {
        fprintf(stderr, "Failed to create the server.\n");
    }

    return host;
}
