//#include <stdio.h>
//#include <stdlib.h>
//#include <getopt.h>
//#include <stdbool.h>
#include "menu_screen.h"
//#include "alphas/allegro_samples/ex_enet_server.h"
//#include "alphas/allegro_samples/ex_enet_client.h"

/*
 * 
 */
int main(int argc, char** argv) {
  
//    bool is_server = false;
//    char* host = "localhost";
//    int port = 0;
    
//    int c;
//    static struct option long_options[] = {
//        {"server",  no_argument,       0,  0 },
//        {"port",    required_argument, 0,  0 },
//        {"host",    required_argument, 0,  0 },
//        {0,         0,                 0,  0 }
//    };
//
//    while (1){
//        int option_index = 0;
//        c = getopt_long(argc, argv, "",
//                 long_options, &option_index);
//
//        if (c == -1)
//            break;
//
//        switch (c) {
//            case 0:
//                if (long_options[option_index].name == "server") {
//                    is_server = true;
//                } else if (long_options[option_index].name == "port") {
//                    port = atoi(optarg);
//                } else if (long_options[option_index].name == "host") {
//                    host = optarg;
//                }
//                break;
//        }
//
//    }
    
//    if (is_server){
//        start_server("0.0.0.0", port);
//    } else {
//        init_client(host, port);
//    }
        
    return show_screen();
}
