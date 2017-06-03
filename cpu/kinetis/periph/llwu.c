/*
 * Copyright (C) 2017 SKF AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_kinetis
 * @{
 *
 * @file
 * @brief       Low-leakage wakeup unit (LLWU) driver
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include "cpu.h"
#include "bit.h"
#include "llwu.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

struct {
    gpio_cb_t cb;
    void *arg;
} config[LLWU_WAKEUP_PIN_NUMOF];

void llwu_init(void)
{
    /* Setup Low Leakage Wake-up Unit (LLWU) */
#ifdef SIM_SCGC4_LLWU_SHIFT
    /* Only the first generation Kinetis K CPUs have a clock gate for the LLWU,
     * for all others the LLWU is always on */
    bit_set32(&SIM->SCGC4, SIM_SCGC4_LLWU_SHIFT);   /* Enable LLWU clock gate */
#endif

    /* Enable LLWU interrupt, or else we can never resume from LLS */
    NVIC_EnableIRQ(LLWU_IRQn);
    /* LLWU needs to have the lowest possible priority, or it will block the
     * other modules from performing their IRQ handling */
    NVIC_SetPriority(LLWU_IRQn, 0xff);
}

void llwu_wakeup_pin_set(llwu_wakeup_pin_t pin, llwu_wakeup_edge_t edge, gpio_cb_t cb, void *arg)
{
    assert(pin < LLWU_WAKEUP_PIN_NUMOF);
    config[pin].cb = cb;
    config[pin].arg = arg;

    /* The fields are two bits per pin, and the setting registers are 8 bits wide */
    if(pin < 4) {
        unsigned field_offset = pin * 2;
        LLWU->PE1 = (LLWU->PE1 & (~(0b11 << field_offset))) | (edge << field_offset);
    }
    else if(pin < 8) {
        unsigned field_offset = (pin - 4) * 2;
        LLWU->PE2 = (LLWU->PE2 & (~(0b11 << field_offset))) | (edge << field_offset);
    }
    else if(pin < 12) {
        unsigned field_offset = (pin - 8) * 2;
        LLWU->PE3 = (LLWU->PE3 & (~(0b11 << field_offset))) | (edge << field_offset);
    }
    else {
        unsigned field_offset = (pin - 12) * 2;
        LLWU->PE4 = (LLWU->PE4 & (~(0b11 << field_offset))) | (edge << field_offset);
    }
}

void isr_llwu(void)
{
    DEBUG("LLWU IRQ\n");

    for (unsigned reg = 0; reg < ((LLWU_WAKEUP_PIN_NUMOF + 7) / 8); ++reg) {
        uint8_t flags = *(&LLWU->F1 + reg);
        if (flags == 0) {
            continue;
        }
        /* Clear pin interrupt flags */
        *(&LLWU->F1 + reg) = flags;
        DEBUG("llwu: F%u = %02x\n", reg + 1, (unsigned) flags);
        for (unsigned i = 0; i < 8; ++i) {
            if (flags & 1) {
                DEBUG("llwu: pin %u\n", reg * 8 + i);
                if (config[i].cb != NULL) {
                    config[i].cb(config[i].arg);
                }
            }
            flags >>= 1;
        }
    }
    DEBUG("llwu: F3 = %02x\n", LLWU->F3);
    /* Read only register F3, the flag will need to be cleared in the peripheral
     * instead of writing a 1 to the MWUFx bit. */

    cortexm_isr_end();
}
