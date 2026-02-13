/*
 * SPDX-FileCopyrightText: 2025 Tom Hert <git@annsann.eu>
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include <stdio.h>

#include "bit.h"
#include "xh3irq.h"

uint32_t xh3irq_has_pending(void)
{
    /*
     * Get MEIP which is the external interrupt pending bit
     * from the Machine Interrupt Pending Register address
     */
    uint32_t mip_reg = read_csr(0x344);
    uint32_t meip = bit_check32(&mip_reg, MEIP_OFFSET);

    return (meip != 0);
}

void xh3irq_handler(void)
{
    /*
     * Get MEINEXT at 0xbe4 which is the next highest interrupt to handle (Bit 2-10).
     * This will also automagically clear the interrupt (See 3.8.6.1.2.)
     *
     * Contains the index of the highest-priority external interrupt
     * which is both asserted in meipa and enabled in meiea, left-
     * shifted by 2 so that it can be used to index an array of 32-bit
     * function pointers. If there is no such interrupt, the MSB is set.
     */
    uint32_t meinext = (read_csr(0xBE4) >> MEINEXT_IRQ_OFFSET) & MEINEXT_MASK;

    void (*isr)(void) = (void (*)(void)) vector_cpu[meinext];
#ifdef DEVELHELP
    printf("Calling isr %p for irq %ld\n", isr, meinext);
#endif
    isr();
}

void _meiea_set_req_bit(uint32_t irq_no, uint32_t bit_val)
{
    uint32_t index = irq_no / INTERRUPT_ARRAY_MASK_OFFSET;
    uint32_t mask = bit_val << (irq_no % INTERRUPT_ARRAY_MASK_OFFSET);

    /*
     * 0xbe0 is the external interrupt enable array.
     * The array contains a read-write bit for each external interrupt request:
     * a 1 bit indicates that interrupt is currently enabled.
     * There are up to 512 external interrupts.
     * The upper half of this register contains a 16-bit window into the full
     * 512-bit vector. The window is indexed by the 5 LSBs of the write data.
     */
    __asm__ volatile(
        "csrs 0xbe0, %0\n"
        : : "r"(index | (mask << INTERRUPT_ARRAY_MASK_OFFSET))
    );
}

void xh3irq_enable_irq(uint32_t irq_no)
{
    _meiea_set_req_bit(irq_no, 1);
}

void xh3irq_disable_irq(uint32_t irq_no)
{
    _meiea_set_req_bit(irq_no, 0);
}

void xh3irq_force_irq(uint32_t irq_no)
{
    uint32_t index = irq_no / INTERRUPT_ARRAY_MASK_OFFSET;
    uint32_t mask = 1u << (irq_no % INTERRUPT_ARRAY_MASK_OFFSET);

    /**
     * 0xbe2 is the external interrupt force array.
     * See _meiea_set_req_bit / 0xbe0 for more details.
     */
    __asm__ volatile(
        "csrs 0xbe2, %0\n"
        : : "r"(index | (mask << INTERRUPT_ARRAY_MASK_OFFSET)));
}
