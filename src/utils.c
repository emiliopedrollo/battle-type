#define _GNU_SOURCE     /* To get defns of NI_MAXSERV and NI_MAXHOST */
#include <arpa/inet.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <stdbool.h>

#ifdef WIN32
#include <windows.h>
#elif _POSIX_C_SOURCE >= 199309L
#include <time.h>   // for nanosleep
#else
#include <unistd.h> // for usleep
#endif

bool is_next_char(char *haystack, char *needle){
    if (strstr(haystack,needle)){
        return strstr(haystack,needle)-haystack == 0;
    } else return false;
}

char get_next_ascii_char(char *s){
    if (is_next_char(s,"Á") ||
        is_next_char(s,"Â") ||
        is_next_char(s,"Ã") ||
        is_next_char(s,"Ä") ||
        is_next_char(s,"À")) {
        return 'A';
    } else if (is_next_char(s,"É") ||
               is_next_char(s,"Ê") ||
               is_next_char(s,"Ẽ") ||
               is_next_char(s,"Ë") ||
               is_next_char(s,"È")) {
        return 'E';
    } else if (is_next_char(s,"Í") ||
               is_next_char(s,"Î") ||
               is_next_char(s,"Ĩ") ||
               is_next_char(s,"Ï") ||
               is_next_char(s,"Ì")) {
        return 'I';
    } else if (is_next_char(s,"Ó") ||
               is_next_char(s,"Ô") ||
               is_next_char(s,"Õ") ||
               is_next_char(s,"Ö") ||
               is_next_char(s,"Ò")) {
        return 'O';
    } else if (is_next_char(s,"Ú") ||
               is_next_char(s,"Û") ||
               is_next_char(s,"Ũ") ||
               is_next_char(s,"Ü") ||
               is_next_char(s,"Ù")) {
        return 'U';
    } else if (is_next_char(s,"Ç")) {
        return 'C';
    } else return s[0];
}

char* remove_char(char *s, char c){
    int writer = 0, reader = 0;

    char* r = malloc(strlen(s)+1);

    while (s[reader])
    {
        if (s[reader]!=c)
        {
            r[writer++] = s[reader];
        }

        reader++;
    }

    r[writer]=0;

    return r;
}

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
    int s;
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

void substr(char *buffer, size_t buflen, char const *source, int len)
{
    size_t srclen = strlen(source);
    size_t nbytes = 0;
    size_t offset = 0;
    size_t sublen;

    if (buflen == 0)    /* Can't write anything anywhere */
        return;
    if (len > 0)
    {
        sublen = (size_t)len;
        nbytes = (sublen > srclen) ? srclen : sublen;
        offset = 0;
    }
    else if (len < 0)
    {
        sublen = (size_t)-len;
        nbytes = (sublen > srclen) ? srclen : sublen;
        offset = srclen - nbytes;
    }
    if (nbytes >= buflen)
        nbytes = 0;
    if (nbytes > 0)
        memmove(buffer, source + offset, nbytes);
    buffer[nbytes] = '\0';
}

void msleep(int milliseconds) // cross-platform sleep function
{
#ifdef WIN32
    Sleep(milliseconds);
#elif _POSIX_C_SOURCE >= 199309L
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
#else
    usleep(milliseconds * 1000);
#endif
}