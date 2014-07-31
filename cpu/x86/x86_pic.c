/*
 * Copyright (C) 2014  René Kijewski  <rene.kijewski@fu-berlin.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * @ingroup     x86-irq
 * @{
 *
 * @file
 * @brief       Configuration and interrupt handling for the Programmable Interrupt Controller (PIC).
 *
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include "x86_interrupts.h"
#include "x86_pic.h"
#include "x86_ports.h"
#include "cpu.h"
#include "irq.h"

#include <stdio.h>

static x86_irq_handler_t x86_registered_irqs[X86_IRQ_COUNT];

static const char *const IRQ_NAMES[X86_IRQ_COUNT] __attribute__((unused)) = {
    "PIT",
    "KBC 1",
    NULL,
    "COM 2/4",
    "COM 1/3",
    "LPT 2",
    "FLOPPY",
    "LPT 1",
    "RTC",
    "#9",
    "ATA 4",
    "ATA 3",
    "KBC 2",
    NULL,
    "ATA 1",
    "ATA 2",
};

static bool spurious_irq(uint8_t irq_num)
{
    if (irq_num < 8) {
        outb(PIC_MASTER + PIC_COMMAND, PIC_READ_ISR);
        return (inb(PIC_MASTER + PIC_COMMAND) & (1 << irq_num)) == 0;
    }

    return false;
#if 0 /* TODO: does not work */
    irq_num -= 8;
    outb(PIC_SLAVE + PIC_COMMAND, PIC_READ_ISR);
    if (inb(PIC_SLAVE + PIC_COMMAND) & (1 << irq_num)) {
        outb(PIC_MASTER + PIC_COMMAND, PIC_EOI);
        return true;
    }
    return false;
#endif
}

static void pic_interrupt_entry(uint8_t intr_num, struct x86_pushad *orig_ctx, unsigned long error_code)
{
    (void) error_code;
    (void) orig_ctx;

    uint8_t irq_num = intr_num - PIC_MASTER_INTERRUPT_BASE;
    if (spurious_irq(irq_num)) {
        return;
    }

    x86_irq_handler_t handler = x86_registered_irqs[irq_num];
    if (handler) {
        handler(irq_num);
    }

    outb(PIC_MASTER + PIC_COMMAND, PIC_EOI);
    if (irq_num > 7) {
        outb(PIC_SLAVE + PIC_COMMAND, PIC_EOI);
    }
}

static void pic_remap(void)
{
    /* tell both PICs we want to initialize them */
    outb(PIC_MASTER + PIC_COMMAND, PIC_ICW1_INIT + PIC_ICW1_ICW4);
    io_wait();
    outb(PIC_SLAVE + PIC_COMMAND, PIC_ICW1_INIT + PIC_ICW1_ICW4);
    io_wait();

    /* which  ISR should be called if an IRQ occurs */
    outb(PIC_MASTER + PIC_DATA, PIC_MASTER_INTERRUPT_BASE);
    io_wait();
    outb(PIC_SLAVE + PIC_DATA, PIC_SLAVE_INTERRUPT_BASE);
    io_wait();

    /* IRQ of the master the slave talks to */
    outb(PIC_MASTER + PIC_DATA, 1 << PIC_NUM_SLAVE);
    io_wait();
    outb(PIC_SLAVE + PIC_DATA, PIC_NUM_SLAVE);
    io_wait();

    /* use PC mode */
    outb(PIC_MASTER + PIC_DATA, PIC_ICW4_8086);
    io_wait();
    outb(PIC_SLAVE + PIC_DATA, PIC_ICW4_8086);
}

static void pic_register_handler(void)
{
    for (unsigned i = 0; i < X86_IRQ_COUNT; ++i) {
        x86_interrupt_handler_set(PIC_MASTER_INTERRUPT_BASE + i, &pic_interrupt_entry);
    }
}

void x86_pic_set_enabled_irqs(uint16_t mask)
{
    unsigned old_status = disableIRQ();

    mask |= PIC_MASK_SLAVE;
    mask &= ~PIC_MASK_FPU;
    outb(PIC_MASTER + PIC_IMR, ~(uint8_t) mask);
    io_wait();
    outb(PIC_SLAVE + PIC_IMR, ~(uint8_t) (mask >> 8));

    restoreIRQ(old_status);
}

void x86_pic_enable_irq(unsigned num)
{
    unsigned old_status = disableIRQ();

    uint16_t port;
    if (num < 8) {
        port = PIC_MASTER;
    }
    else {
        num -= 8;
        port = PIC_SLAVE;
    }
    uint8_t cur = inb(port + PIC_IMR);
    outb(port + PIC_IMR, cur & ~(1 << num));

    restoreIRQ(old_status);
}

void x86_pic_disable_irq(unsigned num)
{
    unsigned old_status = disableIRQ();

    uint16_t port;
    if (num < 8) {
        port = PIC_MASTER;
    }
    else {
        num -= 8;
        port = PIC_SLAVE;
    }
    uint8_t cur = inb(port + PIC_IMR);
    outb(port + PIC_IMR, cur | (1 << num));

    restoreIRQ(old_status);
}

void x86_init_pic(void)
{
    pic_register_handler();
    pic_remap();
    x86_pic_set_enabled_irqs(0);

    puts("PIC initialized");
}

void x86_pic_set_handler(unsigned irq, x86_irq_handler_t handler)
{
    x86_registered_irqs[irq] = handler;
}
