#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "utils.h"

size_t hexstrlen(char* str)
{
    size_t size = 1;
    do {
        if (*str == ' ')
            size++;
    } while (*(str++));

    return size;
}


int hex2bin(uint8_t* buffer, char* str)
{
    int size = hexstrlen(str);
    for (int i=0; i < size; ++i)
        buffer[i] = strtol(str, &str, 16);
    return size;
}
