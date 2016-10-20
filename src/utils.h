#ifndef BATTLE_TYPE_NETWORK_UTILS_H
#define BATTLE_TYPE_NETWORK_UTILS_H

extern void substr(char *buffer, size_t buflen, char const *source, int len);
char *concat(char* part1, char* part2);
void *get_ip_address();
void msleep(int milliseconds);

#endif //BATTLE_TYPE_NETWORK_UTILS_H
