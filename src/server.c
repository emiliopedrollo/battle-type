//
// Created by fabio on 19/10/16.
//

#include <enet/enet.h>
#include <stdio.h>
#include <pthread.h>
#include <allegro5/altime.h>
#include <allegro5/events.h>
#include <allegro5/timer.h>
#include "server.h"
#include "enet_common.h"

struct Host_Thread_Args{
    ENetAddress listener;
};

ENetHost *create_server(ENetAddress listener);
void *server_loop(void *arguments);
pthread_t init_server_thread(ENetAddress host);
void server_send_receive();

bool server_running = false;
bool server_set_to_stop = false;
pthread_t server_thread;
ENetHost *host;

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

//    host = create_server(listener);
    server_thread = init_server_thread(listener);



}

void stop_server(){
    server_set_to_stop = true;
    pthread_join(server_thread,NULL);

    enet_host_destroy(host);
    enet_deinitialize();
    server_running = false;
}

void server_send_receive(){
    ENetEvent event;
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
}

void *server_loop(void *arguments){

    ALLEGRO_EVENT event_timer;

    ALLEGRO_TIMER *timer;
    ALLEGRO_EVENT_QUEUE *queue;

    struct Host_Thread_Args *args = (struct Host_Thread_Args*)arguments;

    ENetAddress listener = (*args).listener;

    host = create_server(listener);

    server_set_to_stop = false;


    timer = al_create_timer(1.0 / 30); // Run at 30FPS
    queue = al_create_event_queue();
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_start_timer(timer);


    while (!server_set_to_stop){
        al_wait_for_event(queue, &event_timer);
        if (event_timer.type != ALLEGRO_EVENT_TIMER) continue;
        server_send_receive();
    }
    pthread_exit(NULL);
}

pthread_t init_server_thread(ENetAddress listener) {

    //pthread_t threads;
    int rc;
    struct Host_Thread_Args *args;

    pthread_t *cmp_thread = malloc(sizeof(pthread_t));
    args = malloc(sizeof(struct Host_Thread_Args));
    (*args).listener = listener;

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
