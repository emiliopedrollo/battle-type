/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.c
 * Author: emilio
 *
 * Created on 7 de Outubro de 2016, 20:45
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdbool.h>
#include "alphas/allegro1.h"
#include "alphas/allegro_samples/ex_enet_server.h"
#include "alphas/allegro_samples/ex_enet_client.h"

/*
 * 
 */
int main(int argc, char** argv) {
  
    bool is_server = false;
    char* host = "localhost";
    int port = 0;
    
    int c;
    static struct option long_options[] = {
        {"server",  optional_argument, 0,  0 },
        {"port",    required_argument, 0,  0 },
        {"host",    required_argument, 0,  0 },
        {0,         0,                 0,  0 }
    };
    
    while (1){        
        int option_index = 0;        
        c = getopt_long(argc, argv, "",
                 long_options, &option_index);
        
        if (c == -1)
            break;
        
        switch (c) {
            case 0:
                if (long_options[option_index].name == "server") {
                    is_server = true;
                } else if (long_options[option_index].name == "port") {
                    port = atoi(optarg);
                } else if (long_options[option_index].name == "host") {
                    host = optarg;
                }
                break;
            default:
                printf("?? getopt returned character code 0%o ??\n", c);
        }
                
    }
    
    if (is_server){
        start_server("0.0.0.0", port);
    } else {
        init_client(host, port);
    }
        
    //show_screen();
    return (EXIT_SUCCESS);
}

