#include <stdio.h>
#include <kernel_intern.h>

extern void board_init(void);

__attribute__((constructor)) static void startup(void)
{
    /* use putchar so the linker links it in: */
    putchar('\n');

    board_init();

    puts("RIOT MSP430 hardware initialization complete.\n");

    kernel_init();
}
