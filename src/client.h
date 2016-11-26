//
// Created by ubuntu on 10/20/16.
//

#ifndef BATTLE_TYPE_CLIENT_H
#define BATTLE_TYPE_CLIENT_H

extern void (*on_success_client_connect)(void);
extern void (*on_failure_client_connect)(void);

bool connect_client(char* host_ip,
                    void (*on_success_connect)(void), void (*on_failure_connect)(void));
void disconnect_client();
void send_key_press(unsigned char key_press);

#endif //BATTLE_TYPE_CLIENT_H
