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
    ENetAddress listener;
};

ENetHost *create_server(ENetAddress listener);
void *server_loop(void *arguments);
pthread_t init_server_thread(ENetAddress host);
void server_send_receive();

bool ready_to_send = false;

bool server_running = false;
bool server_set_to_stop = false;
pthread_t server_thread;
ENetHost *host;
ENetPeer *client;

void (*on_server_client_connect)(void);

void start_server(void (*on_client_connect_callback)(void)){

    unsigned short port = DEFAULT_PORT;
    char *binder = "0.0.0.0";

    ready_to_send = false;

    ENetAddress listener;

    if (server_running) return;
    server_running = true;

    on_server_client_connect = on_client_connect_callback;

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
    if (host != NULL) enet_host_flush(host);

    pthread_join(server_thread,NULL);

    if (client != NULL) enet_peer_disconnect_now(client,0);
    if (host != NULL) enet_host_destroy(host);
    host = NULL;
    enet_deinitialize();
    enet_free(NULL);
    server_running = false;
}

void server_send_receive(){
    ENetEvent event;
    CLIENT_KEY_PRESS *msg;
    if (host == NULL) return;
    while (enet_host_service(host, &event, 0) > 0) {
        switch (event.type){
            case ENET_EVENT_TYPE_CONNECT:
                client = event.peer;
                printf("Server: A new client connected from %x:%hu.\n",
                       event.peer->address.host,
                       event.peer->address.port);
                on_server_client_connect();
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                printf("Server: client disconnected.\n");
                break;

            case ENET_EVENT_TYPE_RECEIVE:
                msg = (CLIENT_KEY_PRESS *) event.packet->data;

                process_key_press(msg->KEY_PRESSED,PLAYER_CLIENT);

                enet_packet_destroy(event.packet);
                break;
            case ENET_EVENT_TYPE_NONE:
            default:
                break;
        }
    }
}

void update_game(){

    if (client == NULL) return;

    // notify all clients of this player's new position
    SERVER_MESSAGE msg;
    msg.type = MESSAGE_TYPE_GAME_SNAPSHOP;
    msg.game = game;

    if (!ready_to_send) return;

    ENetPacket *packet = enet_packet_create(&msg,
                                            sizeof(SERVER_MESSAGE),
                                            ENET_PACKET_FLAG_RELIABLE);

//    enet_peer_send(client, 0, packet);
    enet_host_broadcast(host, 0, packet);

}

void *server_loop(void *arguments){

    struct Host_Thread_Args *args = (struct Host_Thread_Args*)arguments;

    ENetAddress listener = (*args).listener;

    host = create_server(listener);

    server_set_to_stop = false;

    while (!server_set_to_stop){
        server_send_receive();
        update_game();
        msleep(16);
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
                              2      /* allow up to 32 clients and/or outgoing connections */,
                              2      /* allow up to 2 channels to be used, 0 and 1 */,
                              0      /* assume any amount of incoming bandwidth */,
                              0      /* assume any amount of outgoing bandwidth */);
    if (host == NULL) {
        fprintf(stderr, "Failed to create the server.\n");
    }

    return host;
}

