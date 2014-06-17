#ifndef __DTLS_PLAYGROUND_UTILS_H
#define __DTLS_PLAYGROUND_UTILS_H

#include <unistd.h>

size_t hexstrlen(char* str);
int hex2bin(uint8_t* buffer, char* str);

#endif
