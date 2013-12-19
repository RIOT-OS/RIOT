#include "irq.h"
#include "cpu.h"

unsigned int disableIRQ()
{
    unsigned int state;
    __asm__("mov.w r2,%0" : "=r"(state));
    state &= GIE;

    if (state) {
        dINT();
    }

    return state;
}

unsigned int enableIRQ()
{
    unsigned int state;
    __asm__("mov.w r2,%0" : "=r"(state));
    state &= GIE;

    if (!state) {
        eINT();
    }

    return state;
}

void restoreIRQ(unsigned int state)
{
    if (state) {
        eINT();
    }
}

