#include <string.h>
#include "hash_string.h"

unsigned long hash_string(unsigned char *str)
{
    unsigned long hash = 5381;
    int c;

    while((c = *str++)) {
        hash = ((hash << 5) + hash) + c;    /* hash * 33 + c */
    }

    return hash;
}

int cmp_string(char *a, char *b)
{
    return (strcmp(a, b) == 0);
}
