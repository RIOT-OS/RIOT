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

typedef struct {
    gpio_cb_t cb;
    void *arg;
} llwu_pin_config_t;

static llwu_pin_config_t llwu_pin_config[LLWU_WAKEUP_PIN_NUMOF];

void llwu_init(void)
{
    /* Setup Low Leakage Wake-up Unit (LLWU) */
#ifdef SIM_SCGC4_LLWU_SHIFT
    /* Only the first generation Kinetis K CPUs have a clock gate for the LLWU,
     * for all others the LLWU clock is always on */
    bit_set32(&SIM->SCGC4, SIM_SCGC4_LLWU_SHIFT);   /* Enable LLWU clock gate */
#endif

    /* LLWU needs to have a priority which is equal to or more urgent than the
     * PORT module to avoid races between the LLWU pin detect interrupt and the
     * PORT pin detect interrupt */
    NVIC_SetPriority(LLWU_IRQn, 0);
}

void llwu_wakeup_pin_set(llwu_wakeup_pin_t pin, llwu_wakeup_edge_t edge, gpio_cb_t cb, void *arg)
{
    assert(pin < LLWU_WAKEUP_PIN_NUMOF);
    llwu_pin_config[pin].cb = cb;
    llwu_pin_config[pin].arg = arg;

    /* The fields are two bits per pin, and the setting registers are 8 bits
     * wide, hence 4 pins per register. */
    unsigned field_offset = (pin & 0x03) << 1;
    unsigned reg_offset = pin >> 2;
    (&LLWU->PE1)[reg_offset] = ((&LLWU->PE1)[reg_offset] &
        (~(0b11 << field_offset))) |
        (edge << field_offset);
}

void isr_llwu(void)
{
    DEBUG("LLWU IRQ\n");

    for (unsigned reg = 0; reg < ((LLWU_WAKEUP_PIN_NUMOF + 7) / 8); ++reg) {
        uint8_t status = *(&LLWU->F1 + reg);
        /* Clear pin interrupt flags */
        *(&LLWU->F1 + reg) = status;
        DEBUG("llwu: F%u = %02x\n", reg + 1, (unsigned) status);

        while (status) {
            unsigned pin = bitarithm_lsb(status);
            status &= ~(1 << pin);
            pin += reg * 8;
            DEBUG("llwu: wakeup pin %u\n", pin);
            gpio_cb_t cb = llwu_pin_config[pin].cb;
            if (cb) {
                cb(llwu_pin_config[pin].arg);
            }
            /* Clear PORT interrupt flag to avoid spurious duplicates. */
            /* In essence, this behavior is similar to a software debounce. Even
             * very quick contact bounces after the LLWU has begun to wake the
             * CPU may cause the PORT module to pick up the same trigger event,
             * which may lead to duplicate software events when using the same
             * callback for gpio_init_int and llwu_wakeup_pin_set. The bounces
             * would normally be ignored because of the processing delay in the
             * interrupt handling before the interrupt flag is cleared, but
             * since there are two interrupt flags, one in the LLWU module and
             * one in the PORT module, we can get two events. */
            DEBUG("PORT ISFR: %08" PRIx32 "\n", llwu_wakeup_pin_to_port[pin].port->ISFR);
            llwu_wakeup_pin_to_port[pin].port->ISFR = llwu_wakeup_pin_to_port[pin].isfr_mask;
        }
    }
    /* Read only register F3, the flag will need to be cleared in the peripheral
     * instead of writing a 1 to the MWUFx bit. */
    DEBUG("llwu: F3 = %02x\n", LLWU->F3);
    /* Mask the LLWU IRQ until the module interrupt handlers have had a chance
     * to run and clear the F3 flags */
    NVIC_DisableIRQ(LLWU_IRQn);

    cortexm_isr_end();
}
