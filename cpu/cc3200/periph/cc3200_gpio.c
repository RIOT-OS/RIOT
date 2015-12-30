#include "cpu.h"

void gpio_set(uint32_t gpio, uint32_t value)
{
    uint32_t ulReg[]= {
        GPIOA0_BASE,
        GPIOA1_BASE,
        GPIOA2_BASE,
        GPIOA3_BASE,
        GPIOA4_BASE
    };
    uint32_t pin = (1 << (gpio % PINS_IN_PORT));
    uint32_t port = (ulReg[gpio / PINS_IN_PORT]);
    
    MAP_GPIOPinWrite(port, pin, (value << (gpio % PINS_IN_PORT)));
}