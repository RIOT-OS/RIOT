#include <irq.h>
#ifdef CC430
#include <cc430f6137.h>
#else
#include <msp430x16x.h>
#endif
#include <cpu.h>

unsigned int disableIRQ()
{
    unsigned int state;
    __asm__("mov.w r2,%0" : "=r"(state));
    state &= GIE;

    if(state) {
        dINT();
    }

    return state;
}

unsigned int enableIRQ()
{
    unsigned int state;
    __asm__("mov.w r2,%0" : "=r"(state));
    state &= GIE;

    if(!state) {
        eINT();
    }

    return state;
}

void restoreIRQ(unsigned int state)
{
    if(state) {
        eINT();
    }
}

