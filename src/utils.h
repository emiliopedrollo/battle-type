#ifndef BATTLE_TYPE_NETWORK_UTILS_H
#define BATTLE_TYPE_NETWORK_UTILS_H

#include <stdio.h>

extern void substr(char *buffer, size_t buflen, char const *source, int len);
char *concat(char* part1, char* part2);
#ifndef WIN32
void *get_ip_address();
#else
size_t getline(char **lineptr, size_t *n, FILE *stream);
char *strdup(const char *s);
#endif
void msleep(int milliseconds);
char get_next_ascii_char(char *s);
bool is_next_char(char *haystack, char *needle);
char* remove_char(char *s, char c);

#endif //BATTLE_TYPE_NETWORK_UTILS_H
