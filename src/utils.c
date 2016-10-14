#define _GNU_SOURCE     /* To get defns of NI_MAXSERV and NI_MAXHOST */
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/if_link.h>
#include <memory.h>
#include <stdbool.h>


char *concat(char* part1, char* part2){
    char* result = malloc(strlen(part1)+1+strlen(part2));
    strcpy(result,part1);
    strcat(result,part2);
    return result;
}

void get_list_of_interfaces(){
    struct ifaddrs *addrs,*tmp;

    getifaddrs(&addrs);
    tmp = addrs;

    while (tmp)
    {
        if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_PACKET)
            printf("%s\n", tmp->ifa_name);

        tmp = tmp->ifa_next;
    }

    freeifaddrs(addrs);
}

char *get_ip_address(){
    struct ifaddrs *ifaddr, *ifa;
    int family, s, n;
    static char host[NI_MAXHOST];

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return NULL;
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;

        s=getnameinfo(ifa->ifa_addr,sizeof(struct sockaddr_in),host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);

        get_list_of_interfaces();

        if((strcmp(ifa->ifa_name,"enp8s0")==0)&&(ifa->ifa_addr->sa_family==AF_INET))
        {
            if (s != 0)
            {
                printf("getnameinfo() failed: %s\n", gai_strerror(s));
                return false;
            }
            printf("\tInterface : <%s>\n",ifa->ifa_name );
            printf("\t  Address : <%s>\n", host);
        }
    }

    freeifaddrs(ifaddr);

    return host;

}