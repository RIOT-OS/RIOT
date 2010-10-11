#include <stdio.h>
#include <board.h>
#include <kernel_intern.h>

extern void board_init();

__attribute__ ((constructor)) static void startup() {
    /* use putchar so the linker links it in: */
    putchar('\n');

    board_init();

    puts("ukleos MSP430 hardware initialization complete.\n");

    kernel_init();
}
