#include <stdio.h>

void (_putchar(int)) = uart1_putchar;

void putchar(int c) {
    _putchar(c);
}
