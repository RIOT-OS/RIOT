/*
 * Copyright (C) 2015 HAW Hamburg
 *               2016 INRIA
 *               2023 Gerson Fernando Budke
 *               2023 Hugues Larrive
 *               2023 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_sam0_common
 * @ingroup     drivers_periph_gpio_ll_irq
 * @{
 *
 * @file
 * @brief       IRQ implementation of the GPIO Low-Level API for SAM0
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 *
 * @}
 */

#include <errno.h>

#include "bitarithm.h"
#include "cpu.h"
#include "irq.h"
#include "periph/gpio_ll_irq.h"
#include "periph_conf.h"
#include "periph_cpu.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief   Number of external interrupt lines
 */
#ifdef CPU_COMMON_SAML1X
#define IRQS_NUMOF                  (8U)
#else
#define IRQS_NUMOF                  (16U)
#endif

/**
 * @brief   The GCLK used for clocking EXTI
 */
#ifndef CONFIG_SAM0_GCLK_GPIO
#define CONFIG_SAM0_GCLK_GPIO       (SAM0_GCLK_MAIN)
#endif

/* Consistify naming */
#ifndef EIC_SEC
#define EIC_SEC EIC
#endif

struct isr_ctx {
    gpio_ll_cb_t cb;
    void *arg;
};

static struct isr_ctx isr_ctx[IRQS_NUMOF];

extern void gpio_ll_mux(gpio_port_t port, uint8_t pin, gpio_mux_t mux);

static int get_exti_num(unsigned port_num, uint8_t pin)
{
    if (port_num >= ARRAY_SIZE(exti_config)) {
        return -1;
    }
    return exti_config[port_num][pin];
}

static IRQn_Type exti2irqn(unsigned exti_num)
{
    (void)exti_num;
    assume(exti_num < IRQS_NUMOF);
#if defined(CPU_COMMON_SAMD5X)
    return EIC_0_IRQn + exti_num;
#elif defined(CPU_COMMON_SAML1X)
    if (exti_num <= 3) {
        return EIC_0_IRQn + exti_num;
    }
    return EIC_OTHER_IRQn;
#else
    return EIC_IRQn;
#endif
}

static void enable_trigger(unsigned exti_num, gpio_irq_trig_t trig)
{
    unsigned config_reg = exti_num >> 3;
    unsigned config_pos = (exti_num & 0x7) << 2;

    /* configure trigger with IRQs disabled */
    unsigned irq_state = irq_disable();
    uint32_t conf = EIC_SEC->CONFIG[config_reg].reg;
    conf &= ~(EIC_CONFIG_SENSE0_Msk << config_pos);
    conf |= ((uint32_t)trig) << config_pos;
    EIC_SEC->CONFIG[config_reg].reg = conf;
    irq_restore(irq_state);

    NVIC_EnableIRQ(exti2irqn(exti_num));
}

static void disable_trigger(unsigned exti_num)
{
    unsigned config_reg = exti_num >> 3;
    unsigned config_pos = (exti_num & 0x7) << 2;

    /* configure trigger with IRQs disabled */
    unsigned irq_state = irq_disable();
    uint32_t conf = EIC_SEC->CONFIG[config_reg].reg;
    conf &= ~(EIC_CONFIG_SENSE0_Msk << config_pos);
    EIC_SEC->CONFIG[config_reg].reg = conf;
    irq_restore(irq_state);
}

static void eic_sync(void)
{
#ifdef EIC_STATUS_SYNCBUSY
    while (EIC_SEC->STATUS.reg & EIC_STATUS_SYNCBUSY) {}
#endif
#ifdef EIC_SYNCBUSY_ENABLE
    while (EIC_SEC->SYNCBUSY.reg & EIC_SYNCBUSY_ENABLE) {}
#endif
}

static void eic_enable_clock(void)
{
    /* Enable EIC clock */
#ifdef PM_APBAMASK_EIC
    PM->APBAMASK.reg |= PM_APBAMASK_EIC;
    GCLK->CLKCTRL.reg = EIC_GCLK_ID
                      | GCLK_CLKCTRL_CLKEN
                      | GCLK_CLKCTRL_GEN(CONFIG_SAM0_GCLK_GPIO);
    while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY) {}
#endif
#ifdef MCLK_APBAMASK_EIC
    MCLK->APBAMASK.reg |= MCLK_APBAMASK_EIC;
    GCLK->PCHCTRL[EIC_GCLK_ID].reg = GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN(CONFIG_SAM0_GCLK_GPIO);
    /* disable the EIC module*/
    EIC_SEC->CTRLA.reg = 0;
    eic_sync();
#endif
}

static void eic_enable(void)
{
#ifdef EIC_CTRL_ENABLE
    EIC_SEC->CTRL.reg = EIC_CTRL_ENABLE;
#endif
#ifdef EIC_CTRLA_ENABLE
    EIC_SEC->CTRLA.reg = EIC_CTRLA_ENABLE;
#endif
}

void gpio_ll_irq_mask(gpio_port_t port, uint8_t pin)
{
    unsigned port_num = GPIO_PORT_NUM(port);
    int exti_num = get_exti_num(port_num, pin);
    assume((unsigned)exti_num < IRQS_NUMOF);

    EIC_SEC->INTENCLR.reg = 1U << exti_num;
}

void gpio_ll_irq_unmask(gpio_port_t port, uint8_t pin)
{
    unsigned port_num = GPIO_PORT_NUM(port);
    int exti_num = get_exti_num(port_num, pin);
    assume((unsigned)exti_num < IRQS_NUMOF);

    EIC_SEC->INTENSET.reg = 1U << exti_num;
}

void gpio_ll_irq_unmask_and_clear(gpio_port_t port, uint8_t pin)
{
    unsigned port_num = GPIO_PORT_NUM(port);
    int exti_num = get_exti_num(port_num, pin);
    assume(exti_num >= 0);

    uint32_t mask = 1U << exti_num;
    EIC_SEC->INTFLAG.reg = mask;
    EIC_SEC->INTENSET.reg = mask;
}

int gpio_ll_irq(gpio_port_t port, uint8_t pin, gpio_irq_trig_t trig,
                gpio_ll_cb_t cb, void *arg)
{
    unsigned port_num = GPIO_PORT_NUM(port);
    int exti_num = get_exti_num(port_num, pin);

    assume(cb);

    if (exti_num < 0) {
        return -ENOTSUP;
    }

    if (isr_ctx[exti_num].cb) {
        DEBUG("[gpio_ll_irq] IRQ already configured for EXTI %d (P%c%u)\n",
              exti_num, 'A' + (char)port_num, (unsigned)pin);
    }

    isr_ctx[exti_num].cb = cb;
    isr_ctx[exti_num].arg = arg;

    gpio_ll_mux(port, pin, GPIO_MUX_A);

    eic_enable_clock();

    enable_trigger(exti_num, trig);

    /* clear any spurious IRQ */
    EIC_SEC->INTFLAG.reg = 1U << exti_num;

    /* enable IRQ */
    EIC_SEC->INTENSET.reg = 1U << exti_num;

#ifdef EIC_WAKEUP_WAKEUPEN0
    unsigned irq_state = irq_disable();
    EIC_SEC->WAKEUP.reg |= 1U << exti_num;
    irq_restore(irq_state);
#endif

    eic_enable();

    eic_sync();

    return 0;
}

void gpio_ll_irq_off(gpio_port_t port, uint8_t pin)
{
    unsigned port_num = GPIO_PORT_NUM(port);
    int exti_num = get_exti_num(port_num, pin);

    assume((unsigned)exti_num < IRQS_NUMOF);

    /* First, disable IRQs */
    EIC_SEC->INTENCLR.reg = 1U << exti_num;

    gpio_ll_mux(port, pin, GPIO_MUX_DISABLED);

    /* Disabling the trigger may conserve power */
    disable_trigger(exti_num);

#ifdef EIC_WAKEUP_WAKEUPEN0
    unsigned irq_state = irq_disable();
    EIC_SEC->WAKEUP.reg &= ~(1U << exti_num);
    irq_restore(irq_state);
#endif

    /* Finally, clear the callback */
    isr_ctx[exti_num].cb = NULL;
}

MAYBE_UNUSED
static void isr_eic_unknown_num(void)
{
    /* read & clear interrupt flags */
    uint32_t state = EIC_SEC->INTFLAG.reg & EIC_SEC->INTENSET.reg;
    state &= EIC_INTFLAG_EXTINT_Msk;
    EIC_SEC->INTFLAG.reg = state;

    /* execute interrupt callbacks */
    uint8_t num = 0;
    while (state) {
        state = bitarithm_test_and_clear(state, &num);
        isr_ctx[num].cb(isr_ctx[num].arg);
    }

    cortexm_isr_end();
}

MAYBE_UNUSED
static void isr_eic_known_num(unsigned num)
{
    EIC_SEC->INTFLAG.reg = 1U << num;
    isr_ctx[num].cb(isr_ctx[num].arg);
    cortexm_isr_end();
}

#if !defined(CPU_COMMON_SAML1X) && !defined(CPU_COMMON_SAMD5X)
void isr_eic(void) { isr_eic_unknown_num(); }
#endif

#if defined(CPU_COMMON_SAML1X)
void isr_eic_other(void) { isr_eic_unknown_num(); }
#endif

#if defined(CPU_COMMON_SAML1X) || defined(CPU_COMMON_SAMD5X)
void isr_eic0(void) { isr_eic_known_num(0); }
void isr_eic1(void) { isr_eic_known_num(1); }
void isr_eic2(void) { isr_eic_known_num(2); }
void isr_eic3(void) { isr_eic_known_num(3); }
#endif
#if defined(CPU_COMMON_SAMD5X)
void isr_eic4(void) { isr_eic_known_num(4); }
void isr_eic5(void) { isr_eic_known_num(5); }
void isr_eic6(void) { isr_eic_known_num(6); }
void isr_eic7(void) { isr_eic_known_num(7); }
void isr_eic8(void) { isr_eic_known_num(8); }
void isr_eic9(void) { isr_eic_known_num(9); }
void isr_eic10(void) { isr_eic_known_num(10); }
void isr_eic11(void) { isr_eic_known_num(11); }
void isr_eic12(void) { isr_eic_known_num(12); }
void isr_eic13(void) { isr_eic_known_num(13); }
void isr_eic14(void) { isr_eic_known_num(14); }
void isr_eic15(void) { isr_eic_known_num(15); }
#endif
