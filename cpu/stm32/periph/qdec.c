/*
 * SPDX-FileCopyrightText: 2017 Gilles DOFFE <gdoffe@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_cortexm_common
 * @ingroup     drivers_periph_qdec
 * @{
 *
 * @file
 * @brief       Low-level QDEC driver implementation
 *
 * @author      Gilles DOFFE <gilles.doffe@gmail.com>
 *
 * @}
 */

#include <errno.h>

#include "cpu.h"
#include "assert.h"
#include "periph/qdec.h"
#include "periph/gpio.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifdef QDEC_NUMOF

/**
 * @brief   Interrupt context for each configured qdec
 */
static qdec_isr_ctx_t isr_ctx[QDEC_NUMOF];

/**
 * @brief Read the current value of the given qdec device. Internal use.
 *
 * @param[in] qdec          the qdec to read the current value from
 * @param[in] reset         perform a reset of qdec counter if not 0
 *
 * @return                  the qdecs current value
 */
static int32_t _qdec_read(qdec_t qdec, uint8_t reset);

static inline TIM_TypeDef *dev(qdec_t qdec)
{
    return qdec_config[qdec].dev;
}

int32_t qdec_init(qdec_t qdec, qdec_mode_t mode, qdec_cb_t cb, void *arg)
{
    /* Control variables */
    uint8_t i = 0;

    /* Verify parameters */
    assert((qdec < QDEC_NUMOF));

    /* Power on the used timer */
    periph_clk_en(qdec_config[qdec].bus, qdec_config[qdec].rcc_mask);

    /* Route peripheral to correct pins (STM32F1 only, other MCU families route
     * pins to peripheral rather than peripheral to pins */
#ifdef CPU_FAM_STM32F1
    DEBUG("[qdec] AFIO->MAPR = 0x%" PRIx32 ", |= 0x%" PRIx32 "\n",
          AFIO->MAPR, qdec_config[qdec].remap);
    AFIO->MAPR |= qdec_config[qdec].remap;
    DEBUG("[qdec] AFIO->MAPR = 0x%" PRIx32 "\n", AFIO->MAPR);
#endif

    /* Reset configuration and CC channels */
    dev(qdec)->CR1 = 0;
    dev(qdec)->CR2 = 0;
    dev(qdec)->SMCR = 0;
    dev(qdec)->CCER = 0;
    for (i = 0; i < QDEC_CHAN; i++) {
        TIM_CHAN(qdec, i) = 0;
    }

    /* Count on A (TI1) signal edges, B (TI2) signal edges or both,
     * default to EINVAL (Invalid argument).
     */
    switch (mode) {
        /* X2 mode */
        case QDEC_X2:
            dev(qdec)->SMCR |= (0x02 << TIM_SMCR_SMS_Pos);
            break;
        /* X4 mode */
        case QDEC_X4:
            dev(qdec)->SMCR |= (0x03 << TIM_SMCR_SMS_Pos);
            break;
        /* X1 mode does not exist on STM32 as STM32 always counts
         * on both rising and falling edges from encoder
         */
        case QDEC_X1:
        default:
            errno = EINVAL;
            goto err_invalid_mode;
    }

    /* Reset configuration and CC channels */
    for (i = 0; i < QDEC_CHAN; i++) {
        TIM_CHAN(qdec, i) = 0;
    }

    /* Configure the used pins */
    i = 0;
    while ((i < QDEC_CHAN) && (qdec_config[qdec].chan[i].pin != GPIO_UNDEF)) {
        gpio_init(qdec_config[qdec].chan[i].pin, GPIO_IN);
#ifndef CPU_FAM_STM32F1
        gpio_init_af(qdec_config[qdec].chan[i].pin, qdec_config[qdec].af);
#endif
        i++;
    }

    /* Set counting max value */
    dev(qdec)->ARR = qdec_config[qdec].max;

    /* Set TIMx_CNT value to half of of TIMx_ARR to permit countdown */
    dev(qdec)->CNT = dev(qdec)->ARR / 2;

    /* Remember the interrupt context */
    isr_ctx[qdec].cb = cb;
    isr_ctx[qdec].arg = arg;

    /* Enable the qdec's interrupt only if there is a callback provided */
    if (cb) {
        NVIC_EnableIRQ(qdec_config[qdec].irqn);
        dev(qdec)->DIER |= TIM_DIER_UIE;
    } else {
        dev(qdec)->DIER &= ~TIM_DIER_UIE;
    }

    /* Reset counter and start qdec */
    qdec_start(qdec);

    return 0;

/* Error management */
err_invalid_mode:
    return errno;
}

inline int32_t qdec_read(qdec_t qdec)
{
    return _qdec_read(qdec, false);
}

inline int32_t qdec_read_and_reset(qdec_t qdec)
{
    return _qdec_read(qdec, true);
}

static int32_t _qdec_read(qdec_t qdec, uint8_t reset)
{
    int32_t count = 0;
    uint32_t irq_save = 0;

    /* Protect critical section */
    irq_save = irq_disable();

    /* Get counter value */
    count = dev(qdec)->CNT;

    /* Reset counter if asked */
    if (reset)
    {
        dev(qdec)->CNT = dev(qdec)->ARR / 2;
    }

    /* Restore IRQ */
    irq_restore(irq_save);

    /* Subtract offset before return */
    count -= dev(qdec)->ARR / 2;

    /* Return count minus offset */
    return count;
}

void qdec_start(qdec_t qdec)
{
    dev(qdec)->CR1 |= TIM_CR1_CEN;
}

void qdec_stop(qdec_t qdec)
{
    dev(qdec)->CR1 &= ~TIM_CR1_CEN;
}

static inline void irq_handler(qdec_t qdec)
{
    uint32_t status = (dev(qdec)->SR & dev(qdec)->DIER);

    if (status & (TIM_SR_UIF)) {
        dev(qdec)->SR &= ~(TIM_SR_UIF);
        isr_ctx[qdec].cb(isr_ctx[qdec].arg);
    }
    cortexm_isr_end();
}

#ifdef QDEC_0_ISR
void QDEC_0_ISR(void)
{
    irq_handler(0);
}
#endif

#ifdef QDEC_1_ISR
void QDEC_1_ISR(void)
{
    irq_handler(1);
}
#endif

#ifdef QDEC_2_ISR
void QDEC_2_ISR(void)
{
    irq_handler(2);
}
#endif

#ifdef QDEC_3_ISR
void QDEC_3_ISR(void)
{
    irq_handler(3);
}
#endif

#ifdef QDEC_4_ISR
void QDEC_4_ISR(void)
{
    irq_handler(4);
}
#endif

#endif /* QDEC_NUMOF */
