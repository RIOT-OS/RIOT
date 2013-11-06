#include "lpc214x.h"
#include "bits.h"

#include "rs232.h"

void debug_putchar(int character)
{
    UART1WriteChar(character);
}

void bl_uart_init(void)
{
    UART1Initialize(115200U);
}
