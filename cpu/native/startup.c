#include <stdio.h>
#include <kernel_intern.h>

extern void board_init(void);

__attribute__ ((constructor)) static void startup(void)
{
    native_cpu_init();
    native_interrupt_init();

    board_init();

    puts("RIOT native hardware initialization complete.\n");
    kernel_init();
}
