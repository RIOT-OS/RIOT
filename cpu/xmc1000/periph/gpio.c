/*
 * Copyright (C) 2015 Sebastian Sontberg <sebastian@sontberg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     driver_periph
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation
 *
 * @author      Sebastian Sontberg <sebastian@sontberg.de>
 *
 * @}
 */

#include <stddef.h>

#include "periph/gpio.h"
#include "periph_conf.h"
#include "sched.h"
#include "thread.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#define PIN(gpio)  (gpio & 0x000f)
#define PORT(gpio) (PORT0_BASE | ((gpio & 0x30) << 4))

#define OMR(gpio)  ((uint32_t *)(PORT(gpio) + offsetof(PORT0_Type, OMR)))
#define IN(gpio)   ((uint32_t *)(PORT(gpio) + offsetof(PORT0_Type, IN)))
#define IOCR(gpio) ((uint8_t *)(PORT(gpio) + offsetof(PORT0_Type, IOCR0) + PIN(gpio)))

typedef struct {
    gpio_cb_t cb;
    void *arg;
} gpio_int_t;

/* storage for the callbacks of the 4 available external interrupts */
static gpio_int_t gpio_ints[4];

int gpio_init(gpio_t gpio, gpio_dir_t dir, gpio_pp_t pushpull)
{
    *(IOCR(gpio)) = (dir | pushpull) << 3;

    return 0;
}

int gpio_init_int(gpio_t gpio, gpio_pp_t pullup, gpio_flank_t flank,
                  gpio_cb_t cb, void *arg)
{
    /* only pins on PORT2 can be programmed for external interrupts */
    if (!(gpio & P2)) {
        return -1;
    }

    /* init gpio */
    gpio_init(gpio, GPIO_DIR_IN, pullup);

    /* index 0..3 of ERS */
    uint8_t exs = eru_mapping[PIN(gpio)].exs;
    /* position of this ERS in EXISEL (+2 for B channel) */
    uint8_t pos = exs << 2;
    /* value for input selection */
    uint8_t val = eru_mapping[PIN(gpio)].connection;
    /* source select: 0 = A channel, 1 = B channel */
    uint8_t chan = 0;

    switch (val) {
        case B0:                    /* 0 */
        case B1:                    /* 1 */
            chan = 1;
            pos += 2;
            break;
        case A1:                    /* 2 */
            val = 1;
    }
    ;

    /* setup the 'Event Request Select Unit' (ERS) */
    ERU0->EXISEL &= ~(0x03 << pos);
    ERU0->EXISEL |= (val << pos);

    /* setup the 'Event Input Control' (ETL) */
    ERU0->EXICON[exs] = ((1 << ERU_EXICON_PE_Pos) |
                            /* Rising/falling edge/both  */
                         (flank << ERU_EXICON_RE_Pos) |
                            /* Input A/B without combination: */
                         (chan << ERU_EXICON_SS_Pos) |
                            /* Trigger pulses to OGU[exs] */
                         (exs << ERU_EXICON_OCS_Pos));

    /* setup 'Event Output Trigger Control' */
    ERU0->EXOCON[exs] = ((1 << ERU_EXOCON_GP_Pos) |
                         (1 << (ERU_EXOCON_IPEN0_Pos + exs)));

    /* Save the callback */
    gpio_ints[exs].cb = cb;
    gpio_ints[exs].arg = arg;

    /* enable NVIC interrupts */
    NVIC_SetPriority(ERU0_0_IRQn + exs, TIMER_IRQ_PRIO);
    NVIC_EnableIRQ(ERU0_0_IRQn + exs);

    DEBUG("gpio_init_int: GPIO_PIN(P%u, %u), exs: %u, pos: %u, val: %u, chan: %u\n",
          (gpio & 0xf0) >> 4, gpio & 0xf, exs, pos, val, chan);

    DEBUG("gpio_init_int: EXISEL: 0x%0x EXICON[%u]: 0x%0x EXOCON[%u]: 0x%0x\n",
          (unsigned)ERU0->EXISEL,
          exs, (unsigned)ERU0->EXICON[exs],
          exs, (unsigned)ERU0->EXOCON[exs]);

    return 0;
}


void gpio_write(gpio_t gpio, int value)
{
    if (value) {
        gpio_set(gpio);
    }
    else {
        gpio_clear(gpio);
    }
}

void gpio_set(gpio_t gpio)
{
    *(OMR(gpio)) = 1 << PIN(gpio);
}

void gpio_clear(gpio_t gpio)
{
    *(OMR(gpio)) = 1 << (PIN(gpio) + 16);
}

void gpio_toggle(gpio_t gpio)
{
    uint32_t msk = 1 << PIN(gpio);

    /* set both port set and reset bit */
    *(OMR(gpio)) = (msk << 16) | msk;
}

int gpio_read(gpio_t gpio)
{
    return *(IN(gpio)) & (1 << PIN(gpio));
}

void gpio_irq_enable(gpio_t gpio)
{
    uint8_t exs = eru_mapping[PIN(gpio)].exs;

    NVIC_EnableIRQ(ERU0_0_IRQn + exs);
}

void gpio_irq_disable(gpio_t gpio)
{
    uint8_t exs = eru_mapping[PIN(gpio)].exs;

    NVIC_DisableIRQ(ERU0_0_IRQn + exs);
}


void isr_eru(int ogu)
{
    gpio_ints[ogu].cb(gpio_ints[ogu].arg);

    if (sched_context_switch_request) {
        thread_yield();
    }
}

void isr_eru0(void)
{
    isr_eru(0);
}

void isr_eru1(void)
{
    isr_eru(1);
}

void isr_eru2(void)
{
    isr_eru(2);
}

void isr_eru3(void)
{
    isr_eru(3);
}
