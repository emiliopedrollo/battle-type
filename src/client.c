//
// Created by ubuntu on 10/20/16.
//

#include <stdbool.h>
#include <stdio.h>
#include <enet/enet.h>
#include <pthread.h>
#include "enet_common.h"
#include "client.h"
#include "utils.h"

struct Client_Thread_Args{
    ENetHost *client;
};

bool client_connected;
bool connection_set_to_close;
pthread_t client_thread;
ENetHost *client;

ENetHost* create_client(void);
pthread_t init_client_thread(ENetHost *client);
ENetPeer* create_peer(ENetHost *client, ENetAddress address, unsigned short port);

bool connect_client(char* host_ip){
    unsigned short port = DEFAULT_PORT;
    ENetAddress host_address;
    ENetPeer *peer;

    if (client_connected) return true;
    client_connected = true;

    if (enet_initialize() != 0) {
        fprintf(stderr, "An error occurred while initializing ENet.\n");
    }

    enet_address_set_host(&host_address, host_ip);
    client = create_client();
    peer = create_peer(client, host_address, port);

    client_thread = init_client_thread(client);

    return true;
}

void disconnect_client(){
    connection_set_to_close = true;
    pthread_join(client_thread,NULL);

    enet_host_destroy(client);
    enet_deinitialize();
    client_connected = false;

}

void *client_loop(void *arguments){


    struct Client_Thread_Args *args = (struct Client_Thread_Args*)arguments;

    ENetHost *client = (*args).client;

    connection_set_to_close = false;

    while(!connection_set_to_close){
        msleep(16);
    }

    pthread_exit(NULL);
}

ENetPeer* create_peer(ENetHost *client, ENetAddress address, unsigned short port){
    //ENetAddress address;
    ENetEvent event;
    ENetPeer *server;
    //enet_address_set_host(&address, "localhost");
    address.port = port;
    /* Initiate the connection, allocating the two channels 0 and 1. */
    server = enet_host_connect(client, &address, 2, 0);
    if (server == NULL)
        fprintf(stderr,"Client: No available peers for initiating an ENet connection.\n");

    /* Wait up to 5 seconds for the connection attempt to succeed. */
    if (enet_host_service(client, &event, 5000) > 0 &&
        event.type == ENET_EVENT_TYPE_CONNECT)
    {
        printf("Client: Connected to %x:%u.\n",
               event.peer->address.host,
               event.peer->address.port);
    }
    else
    {
        /* Either the 5 seconds are up or a disconnect event was */
        /* received. Reset the peer in the event the 5 seconds   */
        /* had run out without any significant event.            */
        enet_peer_reset(server);
        printf("Client: Connection to server failed.\n");
    }

    return server;
}

ENetHost* create_client(void){
    ENetHost * client;
    client = enet_host_create(NULL /* create a client host */,
                              1 /* only allow 1 outgoing connection */,
                              2 /* allow up 2 channels to be used, 0 and 1 */,
                              0 /* 56K modem with 56 Kbps downstream bandwidth */,
                              0 /* 56K modem with 14 Kbps upstream bandwidth */);

    if (client == NULL)
        fprintf(stderr,"Client: Failed to create the client.\n");

    return client;
}

pthread_t init_client_thread(ENetHost *client) {

    //pthread_t threads;
    int rc;
    struct Client_Thread_Args *args;

    pthread_t *cmp_thread = malloc(sizeof(pthread_t));
    args = malloc(sizeof(struct Client_Thread_Args));
    (*args).client = client;

    rc = pthread_create(cmp_thread, NULL, client_loop, (void *) args);
    if (rc) {
        fprintf(stderr,"return code from pthread_create() is %d\n", rc);
    }

    return *cmp_thread;

}