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
    ENetAddress host_address;
    unsigned short port;
};

bool connected;
bool client_connected;
bool connection_set_to_close;
pthread_t client_thread;
ENetHost *client;
ENetPeer *peer;
CLIENT_KEY_PRESS *key_press_list;

ENetHost* create_client(void);
pthread_t init_client_thread(ENetAddress host_address, unsigned short port);
ENetPeer* create_peer(ENetHost *client, ENetAddress address, unsigned short port);
void disconnect_peer(ENetHost *client, ENetPeer *peer);
void client_send_receive(ENetHost *client);

void (*on_success_client_connect)(void);
void (*on_failure_client_connect)(void);

bool connect_client(char* host_ip,
                    void (*on_success_connect_callback)(void), void (*on_failure_connect_callback)(void)){

    unsigned short port = DEFAULT_PORT;
    ENetAddress host_address;

    if (client_connected) return true;
    client_connected = true;

    on_success_client_connect = on_success_connect_callback;
    on_failure_client_connect = on_failure_connect_callback;

    if (enet_initialize() != 0) {
        fprintf(stderr, "An error occurred while initializing ENet.\n");
    }

    enet_address_set_host(&host_address, host_ip);
    client = create_client();

    client_thread = init_client_thread(host_address, port);

    return true;
}

void disconnect_client(){
    connection_set_to_close = true;
    pthread_join(client_thread,NULL);

    enet_host_destroy(client);
    client = NULL;
    enet_deinitialize();
    client_connected = false;

}

void *client_loop(void *arguments){
    struct Client_Thread_Args *args = (struct Client_Thread_Args*)arguments;

    unsigned short port = (*args).port;
    ENetAddress host_address = (*args).host_address;

    connected = false;
    connection_set_to_close = false;

    peer = create_peer(client, host_address, port);

    if (connected) on_success_client_connect(); else on_failure_client_connect();

    key_press_list = malloc(sizeof(CLIENT_KEY_PRESS)*20);
    for (int i=0;i<20;i++) key_press_list[i].KEY_PRESSED = 0;

    while(!connection_set_to_close && connected){
        client_send_receive(client);
        msleep(16);
    }

    if (connected) {
        disconnect_peer(client, peer);
    } else {
        disconnect_client();
    }

    pthread_exit(NULL);

}

void send_key_press(unsigned char key_press){

    CLIENT_KEY_PRESS msg;
    msg.KEY_PRESSED = key_press;


    ENetPacket *packet = enet_packet_create(&msg,sizeof(CLIENT_KEY_PRESS),ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 0, packet);

    for (int i=0;i<20;i++){
        if (key_press_list[i].KEY_PRESSED == 0){
            key_press_list[i] = msg;
            break;
        }

    }
}

void client_send_receive(ENetHost *client){
    ENetEvent event;
    SERVER_MESSAGE *msg;

    for (int i=0;i<20;i++){
        if (key_press_list[i].KEY_PRESSED != 0){
//            ENetPacket *packet = enet_packet_create(&key_press_list[i],sizeof(CLIENT_KEY_PRESS),ENET_PACKET_FLAG_RELIABLE);
//            enet_peer_send(peer, 0, packet);
//            key_press_list[i].KEY_PRESSED = 0;
        }

    }

    // Check if we have any queued incoming messages, but do not wait otherwise.
    // This also sends outgoing messages queued with enet_peer_send.
    while (enet_host_service(client, &event, 0) > 0) {
        // clients only care about incoming packets, they will not receive
        // connect/disconnect events.
        if (event.type == ENET_EVENT_TYPE_RECEIVE) {
            msg = (SERVER_MESSAGE*)event.packet->data;
            switch (msg->type){
                case MESSAGE_TYPE_GAME_SNAPSHOP:
                    game = msg->game;
                    received_first_snapshot = true;
                    break;
            }

//            msg = (ServerMessage*)event.packet->data;

//            switch (msg->type) {
//                case POSITION_UPDATE:
//                    players[msg->player_id].x = msg->x;
//                    players[msg->player_id].y = msg->y;
//                    break;
//                case PLAYER_JOIN:
//                    printf("Client: player #%d joined\n", msg->player_id);
//                    players[msg->player_id].active = true;
//                    players[msg->player_id].x = msg->x;
//                    players[msg->player_id].y = msg->y;
//                    players[msg->player_id].color = msg->color;
//                    break;
//                case PLAYER_LEAVE:
//                    printf("Client: player #%d left\n", msg->player_id);
//                    players[msg->player_id].active = false;
//                    break;
//            }

            /* Clean up the packet now that we're done using it. */
            enet_packet_destroy(event.packet);
        } else if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
            printf("Server went Offline\n");
        }
    }
}

void disconnect_peer(ENetHost *client, ENetPeer *peer){
    enet_peer_disconnect(peer, 0);

    /* Allow up to 3 seconds for the disconnect to succeed
     * and drop any packets received packets.
     */
    ENetEvent event;
    while (enet_host_service (client, &event, 1000) > 0) {
        switch (event.type) {
            case ENET_EVENT_TYPE_RECEIVE:
                enet_packet_destroy(event.packet);
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                puts("Client: Disconnect succeeded.");
                return;
            case ENET_EVENT_TYPE_NONE:
            case ENET_EVENT_TYPE_CONNECT:
                break;
        }
    }

    // failed to disconnect gracefully, force the connection closed
    enet_peer_reset(peer);
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
    if (enet_host_service(client, &event, 2000) > 0 &&
        event.type == ENET_EVENT_TYPE_CONNECT)
    {
        printf("Client: Connected to %x:%u.\n",
               event.peer->address.host,
               event.peer->address.port);
        connected = true;
    }
    else
    {
        /* Either the 5 seconds are up or a disconnect event was */
        /* received. Reset the peer in the event the 5 seconds   */
        /* had run out without any significant event.            */
        enet_peer_reset(server);
        printf("Client: Connection to server failed.\n");
        connected = false;
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

pthread_t init_client_thread(ENetAddress host_address, unsigned short port) {

    //pthread_t threads;
    int rc;
    struct Client_Thread_Args *args;

    pthread_t *cmp_thread = malloc(sizeof(pthread_t));
    args = malloc(sizeof(struct Client_Thread_Args));
    (*args).host_address = host_address;
    (*args).port = port;

    rc = pthread_create(cmp_thread, NULL, client_loop, (void *) args);
    if (rc) {
        fprintf(stderr,"return code from pthread_create() is %d\n", rc);
    }

    return *cmp_thread;

}