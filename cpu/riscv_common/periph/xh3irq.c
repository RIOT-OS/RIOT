/*
 * SPDX-FileCopyrightText: 2025 Tom Hert <git@annsann.eu>
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include "xh3irq.h"
#include <stdio.h>

uint32_t xh3irq_has_pending(void) {
    /* Get MEIP at 0x344 which is the external interrupt pending bit */
    uint32_t meip = (read_csr(0x344) >> 11) & 0x1;

    return (meip != 0);
}

void xh3irq_handler(void) {
    /* Get MEINEXT at 0xbe4 which is the next highest interrupt to handle (Bit 2-10).
     * This will also automagically clear the interrupt (See 3.8.6.1.2.) */
    uint32_t meinext = (read_csr(0xbe4) >> 2) & 0x1ff;

    void (*isr)(void) = (void (*)(void))vector_cpu[meinext];
    printf("Calling isr %p for irq %ld\n", isr, meinext);
    isr();
}

void _meiea_set_req_bit(uint32_t irq_no, uint32_t bit_val) {
    uint32_t index = irq_no / 16;
    uint32_t mask = bit_val << (irq_no % 16);

    __asm__ volatile(
        "csrs 0xbe0, %0\n"
        : : "r"(index | (mask << 16))

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
    uint32_t index = irq_no / 16;
    uint32_t mask = 1u << (irq_no % 16);

    __asm__ volatile(
        "csrs 0xbe2, %0\n"
        : : "r"(index | (mask << 16))
    );
}
