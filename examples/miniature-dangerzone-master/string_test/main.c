#include <stdio.h>

#include "bitarithm.h"
#include "strings.h"

int main(void)
{
    puts("foo");

    int i;
    
    for (i = 1; i < 0xFF; i++) {
        printf("ffs(%i):                  %u\n", i, ffs(i));
        printf("number_of_lowest_bit(%i): %u\n\n", i, number_of_lowest_bit(i)+1);
    }

    return 0;
}
