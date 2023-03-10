#include "periph_cpu.h"
#include "gba_irq.h"
#include "irq_arch.h"

#define NUM_CB 3

typedef struct
{
    uint16_t mask;
    IsrFn *cb;
} irq_handler_t;

irq_handler_t irq_handler[NUM_CB];

int _insert_cb(IsrFn *cb, uint16_t mask)
{
    for (uint16_t i = 0; i < NUM_CB; i++)
    {
        if (irq_handler[i].mask == 0)
        {
            GBA_IE |= mask;
            irq_handler[i].mask = mask;
            irq_handler[i].cb = cb;
            return i;
        }
    }
    return -1;
}

void _gba_irq_lcd(uint16_t mask)
{
    /* activate irq generation */
    if (mask & GBA_IRQ_VBLANK)
        GBA_DISPSTAT |= 0x0008;
    if (mask & GBA_IRQ_HBLANK)
        GBA_DISPSTAT |= 0x0010;
    if (mask & GBA_IRQ_VCOUNTER)
        GBA_DISPSTAT |= 0x0020;
}

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
    (*(IsrFn * (*))(GBA_IRQ_ISR)) = gba_isr; // isrColor;

    irq_enable();
}

void gba_isr(void)
{
    irq_disable();
    for (uint16_t i = 0; i < NUM_CB; i++)
    {
        if (GBA_IRF & irq_handler[i].mask)
        {
            irq_handler[i].cb();
        }
    }
    GBA_IRQ_BIOS_ACK = GBA_IRF;
    GBA_IRF = GBA_IRF;
    irq_enable();
}

int gba_irq_register_cb(IsrFn *cb, uint16_t mask)
{
    if (cb == 0 || mask == 0)
        return -1;

    int handler_id = _insert_cb(cb, mask);
    if (handler_id < 0)
        return -2;

    if (mask & (GBA_IRQ_VBLANK | GBA_IRQ_HBLANK | GBA_IRQ_VCOUNTER))
    {
        _gba_irq_lcd(mask);
    }
    if (mask & GBA_IRQ_KEYPAD)
    {
        GBA_KEYPAD_IRQCNTL |= GBA_KEY_ALL | GBA_KEY_IE;
    }

    return handler_id;
}

void gba_irq_unregister_cb(int id)
{
    if (id >= 0 && id < NUM_CB)
    {
        irq_handler[id].mask = 0;
        irq_handler[id].cb = 0;
    }
}
