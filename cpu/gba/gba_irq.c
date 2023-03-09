#include "periph_cpu.h"
#include "gba_irq.h"
#include "irq_arch.h"

typedef void IsrFn(void);
extern IsrFn isrColor;

void init_isr(void)
{
    irq_disable();
    /* Enable the irq via register, otherwise the CPSR has no effect */
    GBA_IME = 1;

    // clear interrupt enable register
    GBA_IE = 0;

    // ack all possible pending irq
    GBA_IRF = 0xFFFF;

    // set an isr
    (*(IsrFn * (*))(0x03FFFFFC)) = isrColor;

    // Enable global interrupt
    // GBA_IME = 1;
    // irq_enable();

    // enable irq on vblank
    // GBA_IE = 1;

    // make hardware throw irq on vblank
    // GBA_DISPSTAT = 0x8;

    // Clear the force-blank bit
    // GBA_DISPCNT = 0;
}

void vblank_irq_enable(void)
{
    GBA_IE = 1;
    GBA_DISPSTAT = 0x8;
    // for now:
    GBA_DISPCNT = 0;
    irq_enable();
}