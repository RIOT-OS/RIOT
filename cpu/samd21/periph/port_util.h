


//TODO: License

#ifndef __PORT_UTIL_H
#define __PORT_UTIL_H

#include "cpu.h"
/*Defines for pinmux functions */ 
typedef enum
{
    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H
}MUXFUNCTION;

static inline PortGroup* system_pinmux_get_group_from_gpio_pin(
        const uint8_t gpio_pin)
{
    uint8_t port_index  = (gpio_pin / 128);
    uint8_t group_index = (gpio_pin / 32);

    /* Array of available ports. */
    Port *const ports[PORT_INST_NUM] = PORT_INSTS;
    if (port_index < PORT_INST_NUM) {
        return &(ports[port_index]->Group[group_index]);
    } else {
        return 0;
    }
}
static inline void pmux_set(uint32_t pin, MUXFUNCTION periph_function)
{
    PortGroup* group = system_pinmux_get_group_from_gpio_pin(pin);
    group->PINCFG[pin % 32].bit.PMUXEN = 1; //Enable mux
    if((pin % 32) & 1) //Test on first bit if odd or even
    {
        group->PMUX[(pin % 32) / 2].bit.PMUXO = periph_function; //PMUX odd        
    }
    else
    {
        group->PMUX[(pin % 32) / 2].bit.PMUXE =  periph_function;//PMUX even 
    }
}


#endif /** __PORT_UTIL_H **/
