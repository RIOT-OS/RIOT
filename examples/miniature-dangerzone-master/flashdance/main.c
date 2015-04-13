#include <stdio.h>
#include "cpu.h"

int main(void)
{
    puts("FLASH!");
    int *bad;
    int *foo;

    hwtimer_init();

    printf("%04X\n", WDMOD); 
    hwtimer_wait(1000 * 1000);

    memcpy(bad, foo, 1000);
    return 0;
}
