#ifndef BATTLE_TYPE_SERVER_H
#define BATTLE_TYPE_SERVER_H

extern bool ready_to_send;

extern void (*on_server_client_connect)(void);

void start_server(void (*on_client_connect_callback)(void));
void stop_server();

#endif //BATTLE_TYPE_SERVER_H
