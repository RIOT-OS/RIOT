/**
 * @brief EFM32 syscalls
 * @details System calls for EFM32 use without startup files.
 */


#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "board.h"
#include "periph/uart.h"

//#include "arch/irq_arch.h"

extern uint32_t __bss_start__;
extern uint32_t __bss_end__;

extern uint32_t _end;                       /* address of last used memory cell */
void *heap_top = (void *) &_end + 4;

// Initialisation function
void _init(void)
{
    int *addr;

    //Clear bss
    for (uint32_t i = &__bss_start__; i < &__bss_end__; i++) {
        addr = i;
        *addr = NULL;
    }

    // Set up UART
    uart_init_blocking(STDIO, 115200);

    // nothing to do here
    asm("nop");
}

// Heap allocation function
void *_sbrk_r(struct _reent *r, size_t incr)
{
    //unsigned int state = irq_arch_disable();

    void *res = heap_top;
    heap_top += incr;

    //irq_arch_restore(state);

    return res;
}

//Serial read function
int _read_r(struct _reent *r, int fd, void *buffer, unsigned int count)
{
    char c;
    char *buff = (char *)buffer;
    uart_read_blocking(STDIO, &c);
    buff[0] = c;
    return 1;
}

//Serial write function
int _write_r(struct _reent *r, int fd, const void *data, unsigned int count)
{
    char *c = (char *)data;
    for (int i = 0; i < count; i++) {
        uart_write_blocking(STDIO, c[i]);
    }
    return count;
}