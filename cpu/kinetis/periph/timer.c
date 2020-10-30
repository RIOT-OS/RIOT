/*
 * Copyright (C) 2016 Eistec AB
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2014-2015 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_kinetis
 * @ingroup     drivers_periph_timer
 *
 * @{
 *
 * @file
 * @brief       Low-level timer driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 *
 * @}
 */

#include <stdlib.h>

#include "cpu.h"
#include "bit.h"
#include "board.h"
#include "periph_conf.h"
#include "periph/timer.h"

#ifdef PIT_LTMR64H_LTH_MASK
/* The KW41Z PIT module provides only one IRQ for all PIT channels combined. */
/* TODO: find a better way to distinguish which Kinetis CPUs have separate PIT
 * channel interrupts */
#define KINETIS_PIT_COMBINED_IRQ 1
#else
/* K60, K64F etc have a separate IRQ number for each PIT channel */
#define KINETIS_PIT_COMBINED_IRQ 0
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

#define PIT_MAX_VALUE     (PIT_LDVAL_TSV_MASK >> PIT_LDVAL_TSV_SHIFT)
#define LPTMR_MAX_VALUE   (LPTMR_CNR_COUNTER_MASK >> LPTMR_CNR_COUNTER_SHIFT)

#if TIMER_NUMOF != (PIT_NUMOF + LPTMR_NUMOF)
#error TIMER_NUMOF should be the total of PIT and LPTMR timers in the system
#endif

/**
 * @brief  The number of ticks that will be lost when setting a new target in the LPTMR
 *
 * The counter will otherwise drop ticks when setting new timeouts.
 */
#define LPTMR_RELOAD_OVERHEAD 1

/* PIT channel state */
typedef struct {
    timer_isr_ctx_t isr_ctx;
    uint32_t count;
    uint32_t tctrl;
    uint32_t ldval;
} pit_t;

#ifdef KINETIS_HAVE_LPTMR
/* LPTMR state */
typedef struct {
    timer_isr_ctx_t isr_ctx;
    uint32_t cnr;
    uint32_t cmr;
    uint32_t running;
} lptmr_t;
#endif

static const pit_conf_t pit_config[PIT_NUMOF] = PIT_CONFIG;
#ifdef KINETIS_HAVE_LPTMR
static const lptmr_conf_t lptmr_config[LPTMR_NUMOF] = LPTMR_CONFIG;
#endif

static pit_t pit[PIT_NUMOF];
#ifdef KINETIS_HAVE_LPTMR
static lptmr_t lptmr[LPTMR_NUMOF];
#endif

/**
 * @brief  Find out whether a given timer is a LPTMR or a PIT timer
 */
static inline unsigned int _timer_variant(tim_t dev)
{
#ifdef KINETIS_HAVE_LPTMR
    if ((unsigned int) dev >= PIT_NUMOF) {
        return TIMER_LPTMR;
    }
    else
#endif
    {
        (void) dev;
        return TIMER_PIT;
    }
}

/**
 * @brief  Find device index in the pit_config array
 */
static inline unsigned int _pit_index(tim_t dev)
{
    return ((unsigned int)dev) - TIMER_DEV(0);
}

/**
 * @brief  Get TIMER_x enum value from PIT device index
 */
static inline tim_t _pit_tim_t(uint8_t dev)
{
    return (tim_t)(((unsigned int)TIMER_DEV(0)) + dev);
}

#ifdef KINETIS_HAVE_LPTMR
/**
 * @brief  Find device index in the lptmr_config array
 */
static inline unsigned int _lptmr_index(tim_t dev)
{
    return ((unsigned int)dev) - TIMER_DEV(0) - PIT_NUMOF;
}

#if defined(LPTMR_ISR_0) || defined(LPTMR_ISR_1)
/**
 * @brief  Get TIMER_x enum value from LPTMR device index
 */
static inline tim_t _lptmr_tim_t(uint8_t dev)
{
    return (tim_t)(((unsigned int)TIMER_DEV(0)) + PIT_NUMOF + dev);
}
#endif /* defined(LPTMR_ISR_0) || defined(LPTMR_ISR_1) */
#endif /* KINETIS_HAVE_LPTMR */

/* ****** PIT module functions ****** */

/* Forward declarations */
static inline int pit_init(uint8_t dev, uint32_t freq, timer_cb_t cb, void *arg);
static inline int pit_set(uint8_t dev, uint32_t timeout);
static inline int pit_set_absolute(uint8_t dev, uint32_t target);
static inline int pit_clear(uint8_t dev);
static inline uint32_t pit_read(uint8_t dev);
static inline void pit_start(uint8_t dev);
static inline void pit_stop(uint8_t dev);
static inline void pit_irq_handler(tim_t dev);

static inline int pit_init(uint8_t dev, uint32_t freq, timer_cb_t cb, void *arg)
{
    /* Turn on module clock gate */
    PIT_CLKEN();
    /* Completely disable the module before messing with the settings */
    PIT->MCR = PIT_MCR_MDIS_MASK;

    /* Disable IRQs to avoid race with ISR */
    unsigned int mask = irq_disable();
    uint8_t count_ch = pit_config[dev].count_ch;
    /* Clear configuration */
    PIT->CHANNEL[count_ch].TCTRL = 0;

    /* Freeze timers during debug break, resume normal operations (clear MDIS) */
    PIT->MCR = PIT_MCR_FRZ_MASK;

    /* set callback function */
    pit[dev].isr_ctx.cb = cb;
    pit[dev].isr_ctx.arg = arg;

    /* Clear IRQ flag */
    PIT->CHANNEL[count_ch].TFLG = PIT_TFLG_TIF_MASK;
#if KINETIS_PIT_COMBINED_IRQ
    /* One IRQ for all channels */
    /* NVIC_ClearPendingIRQ(PIT_IRQn); */ /* does it make sense to clear this IRQ flag? */
    NVIC_EnableIRQ(PIT_IRQn);
#else
    /* Refactor the below lines if there are any CPUs where the PIT IRQs are not sequential */
    NVIC_ClearPendingIRQ(PIT0_IRQn + count_ch);
    NVIC_EnableIRQ(PIT0_IRQn + count_ch);
#endif
    /* Reset up-counter */
    pit[dev].count = PIT_MAX_VALUE;
    PIT->CHANNEL[count_ch].LDVAL = PIT_MAX_VALUE;
    /* Disable prescaler channel */
    PIT->CHANNEL[pit_config[dev].prescaler_ch].TCTRL = 0x0;
    /* Load prescaler value */
    PIT->CHANNEL[pit_config[dev].prescaler_ch].LDVAL = (PIT_BASECLOCK / freq) - 1;
    /* Start the prescaler counter */
    PIT->CHANNEL[pit_config[dev].prescaler_ch].TCTRL = (PIT_TCTRL_TEN_MASK);
    PIT->CHANNEL[count_ch].TCTRL = PIT_TCTRL_CHN_MASK | PIT_TCTRL_TEN_MASK;

    irq_restore(mask);
    return 0;
}

static inline int pit_set(uint8_t dev, uint32_t timeout)
{
    const uint8_t ch = pit_config[dev].count_ch;
    /* Disable IRQs to minimize the number of lost ticks */
    unsigned int mask = irq_disable();
    /* Subtract if there was anything left on the counter */
    pit[dev].count -= PIT->CHANNEL[ch].CVAL;
    /* Set new timeout */
    PIT->CHANNEL[ch].TCTRL = 0;
    PIT->CHANNEL[ch].LDVAL = timeout;
    PIT->CHANNEL[ch].TFLG = PIT_TFLG_TIF_MASK;
    PIT->CHANNEL[ch].TCTRL = PIT_TCTRL_TIE_MASK | PIT_TCTRL_CHN_MASK | PIT_TCTRL_TEN_MASK;
    /* Add the new timeout offset to the up-counter */
    pit[dev].count += timeout;
    /* Set the timer to reload the maximum value to be able to count the number
     * of overflow ticks inside the ISR */
    PIT->CHANNEL[ch].LDVAL = PIT_MAX_VALUE;
    irq_restore(mask);
    return 0;
}

static inline int pit_set_absolute(uint8_t dev, uint32_t target)
{
    uint8_t ch = pit_config[dev].count_ch;
    /* Disable IRQs to minimize the number of lost ticks */
    unsigned int mask = irq_disable();
    uint32_t now = pit[dev].count - PIT->CHANNEL[ch].CVAL;
    uint32_t offset = target - now;

    /* Set new timeout */
    PIT->CHANNEL[ch].TCTRL = 0;
    PIT->CHANNEL[ch].LDVAL = offset;
    PIT->CHANNEL[ch].TFLG = PIT_TFLG_TIF_MASK;
    PIT->CHANNEL[ch].TCTRL = PIT_TCTRL_TIE_MASK | PIT_TCTRL_CHN_MASK | PIT_TCTRL_TEN_MASK;
    /* Set the new target time in the up-counter */
    pit[dev].count = target;
    /* Set the timer to reload the maximum value to be able to count the number
     * of overflow ticks inside the ISR */
    PIT->CHANNEL[ch].LDVAL = PIT_MAX_VALUE;
    irq_restore(mask);
    return 0;
}

static inline int pit_clear(uint8_t dev)
{
    uint8_t ch = pit_config[dev].count_ch;
    /* Disable IRQs to minimize the number of lost ticks */
    unsigned int mask = irq_disable();

    /* Subtract if there was anything left on the counter */
    pit[dev].count -= PIT->CHANNEL[ch].CVAL;
    /* No need to add PIT_MAX_VALUE + 1 to the counter because of modulo 2**32 */
    /* Set a long timeout */
    PIT->CHANNEL[ch].TCTRL = 0;
    PIT->CHANNEL[ch].LDVAL = PIT_MAX_VALUE;
    PIT->CHANNEL[ch].TFLG = PIT_TFLG_TIF_MASK;
    PIT->CHANNEL[ch].TCTRL = PIT_TCTRL_CHN_MASK | PIT_TCTRL_TEN_MASK;
    irq_restore(mask);
    return 0;
}

/* CVAL is unreliable if the timer is not enabled (TCTRL_TEN bit clear),
 * by stopping the prescaler instead of the counter channel we avoid this issue,
 * and additionally do not need to worry about saving the control registers or
 * recomputing the target time when starting the timer */
static inline uint32_t pit_read(uint8_t dev)
{
    uint8_t ch = pit_config[dev].count_ch;

    return pit[dev].count - PIT->CHANNEL[ch].CVAL;
}

static inline void pit_start(uint8_t dev)
{
    uint8_t ch = pit_config[dev].prescaler_ch;

    PIT->CHANNEL[ch].TCTRL = PIT_TCTRL_TEN_MASK;
}

static inline void pit_stop(uint8_t dev)
{
    uint8_t ch = pit_config[dev].prescaler_ch;

    PIT->CHANNEL[ch].TCTRL = 0;
}

static inline void pit_irq_handler(tim_t dev)
{
    uint8_t ch = pit_config[_pit_index(dev)].count_ch;
    pit_t *pit_ctx = &pit[_pit_index(dev)];

    if (!PIT->CHANNEL[ch].TFLG) {
        DEBUG("PIT%u!TFLG\n", (unsigned)dev);
        return;
    }
    /* Add the overflow amount to the counter before resetting */
    /* (this may be > 0 if the IRQ handler was delayed e.g. by irq_disable etc.) */
    pit_ctx->count += PIT->CHANNEL[ch].LDVAL - PIT->CHANNEL[ch].CVAL;
    /* inline pit_clear */
    PIT->CHANNEL[ch].TCTRL = 0;
    PIT->CHANNEL[ch].LDVAL = PIT_MAX_VALUE;
    PIT->CHANNEL[ch].TFLG = PIT_TFLG_TIF_MASK;
    PIT->CHANNEL[ch].TCTRL = PIT_TCTRL_CHN_MASK | PIT_TCTRL_TEN_MASK;

    if (pit_ctx->isr_ctx.cb != NULL) {
        pit_ctx->isr_ctx.cb(pit_ctx->isr_ctx.arg, 0);
    }

    cortexm_isr_end();
}

#ifdef KINETIS_HAVE_LPTMR
/* ****** LPTMR module functions ****** */

/* Forward declarations */
static inline int lptmr_init(uint8_t dev, uint32_t freq, timer_cb_t cb, void *arg);
static inline int lptmr_set(uint8_t dev, uint16_t timeout);
static inline int lptmr_set_absolute(uint8_t dev, uint16_t target);
static inline int lptmr_clear(uint8_t dev);
static inline uint16_t lptmr_read(uint8_t dev);
static inline void lptmr_start(uint8_t dev);
static inline void lptmr_stop(uint8_t dev);
#if defined(LPTMR_ISR_0) || defined(LPTMR_ISR_1)
static inline void lptmr_irq_handler(tim_t tim);
#endif

static inline void _lptmr_set_cb_config(uint8_t dev, timer_cb_t cb, void *arg)
{
    /* set callback function */
    lptmr[dev].isr_ctx.cb = cb;
    lptmr[dev].isr_ctx.arg = arg;
}

/**
 * @brief  Compute the LPTMR prescaler setting, see reference manual for details
 */
static inline int32_t _lptmr_compute_prescaler(uint8_t dev, uint32_t freq)
{
    uint32_t prescale = 0;

    if ((freq > lptmr_config[dev].base_freq) || (freq == 0)) {
        /* Frequency out of range */
        return -1;
    }
    while (freq < lptmr_config[dev].base_freq) {
        ++prescale;
        freq <<= 1;
    }
    if (freq != lptmr_config[dev].base_freq) {
        /* freq was not a power of two division of base_freq */
        return -2;
    }
    if (prescale == 0) {
        /* Prescaler bypass enabled */
        return LPTMR_PSR_PBYP_MASK;
    }
    /* LPTMR_PSR_PRESCALE == 0 yields base_freq / 2,
     * LPTMR_PSR_PRESCALE == 1 yields base_freq / 4 etc.. */
    return LPTMR_PSR_PRESCALE(prescale - 1);
}

static inline int lptmr_init(uint8_t dev, uint32_t freq, timer_cb_t cb, void *arg)
{
    int32_t prescale = _lptmr_compute_prescaler(dev, freq);

    if (prescale < 0) {
        return -1;
    }
    LPTMR_Type *hw = lptmr_config[dev].dev;
    /* Disable IRQs to avoid race with ISR */
    unsigned int mask = irq_disable();

    /* Turn on module clock */
    LPTMR_CLKEN();

    /* Completely disable the module before messing with the settings */
    hw->CSR = 0;

    /* select clock source and configure prescaler */
    hw->PSR = LPTMR_PSR_PCS(lptmr_config[dev].src) | ((uint32_t)prescale);

    /* Enable IRQs on the counting channel */
    NVIC_ClearPendingIRQ(lptmr_config[dev].irqn);
    NVIC_EnableIRQ(lptmr_config[dev].irqn);

    _lptmr_set_cb_config(dev, cb, arg);

    /* Reset state */
    lptmr[dev].running = 1;
    lptmr[dev].cnr = 0;
    lptmr[dev].cmr = 0;
    hw->CMR = 0;
    hw->CSR = LPTMR_CSR_TEN_MASK | LPTMR_CSR_TFC_MASK;

    irq_restore(mask);

    return 0;
}

static inline uint16_t lptmr_read(uint8_t dev)
{
    LPTMR_Type *hw = lptmr_config[dev].dev;

    /* latch the current timer value into CNR */
    hw->CNR = 0;
    return lptmr[dev].cnr + hw->CNR;
}

/**
 * @brief Reload the timer with the given timeout
 *
 * @pre IRQs masked, timer running
 */
static inline void lptmr_reload(uint8_t dev, uint16_t timeout)
{
    LPTMR_Type *hw = lptmr_config[dev].dev;

    /* Disable timer and set target, 1 to 2 ticks will be dropped by the
     * hardware during the disable-enable cycle */
    /* Disable the timer interrupt first */
    hw->CSR = LPTMR_CSR_TEN_MASK | LPTMR_CSR_TFC_MASK;
    if (timeout >= LPTMR_RELOAD_OVERHEAD) {
        timeout -= LPTMR_RELOAD_OVERHEAD;
    }
    /* Update reference */
    hw->CNR = 0;
    lptmr[dev].cnr += hw->CNR + LPTMR_RELOAD_OVERHEAD;
    /* Disable timer */
    hw->CSR = 0;
    hw->CMR = timeout;
    /* Enable timer and IRQ */
    hw->CSR = LPTMR_CSR_TEN_MASK | LPTMR_CSR_TFC_MASK | LPTMR_CSR_TIE_MASK;
}

static inline int lptmr_set(uint8_t dev, uint16_t timeout)
{
    LPTMR_Type *hw = lptmr_config[dev].dev;
    /* Disable IRQs to minimize jitter */
    unsigned int mask = irq_disable();

    lptmr[dev].running = 1;
    if (!(hw->CSR & LPTMR_CSR_TEN_MASK)) {
        /* Timer is stopped, only update target */
        if (timeout > LPTMR_RELOAD_OVERHEAD) {
            /* Compensate for the reload delay */
            lptmr[dev].cmr = timeout - LPTMR_RELOAD_OVERHEAD;
        }
        else {
            lptmr[dev].cmr = 0;
        }
    }
    else if ((timeout > 0) && (hw->CSR & LPTMR_CSR_TCF_MASK)) {
        /* TCF is set, safe to update CMR live */
        hw->CNR = 0;
        hw->CMR = timeout + hw->CNR;
        /* cppcheck-suppress selfAssignment
         * (reason: intentional self assignment to clear all pending IRQs) */
        hw->CSR = hw->CSR;
        /* Enable timer and IRQ */
        hw->CSR = LPTMR_CSR_TEN_MASK | LPTMR_CSR_TFC_MASK | LPTMR_CSR_TIE_MASK;
    }
    else {
        lptmr_reload(dev, timeout);
    }
    irq_restore(mask);
    return 0;
}

static inline int lptmr_set_absolute(uint8_t dev, uint16_t target)
{
    LPTMR_Type *hw = lptmr_config[dev].dev;
    /* Disable IRQs to minimize jitter */
    unsigned int mask = irq_disable();

    lptmr[dev].running = 1;
    if (!(hw->CSR & LPTMR_CSR_TEN_MASK)) {
        /* Timer is stopped, only update target */
        uint16_t timeout = target - lptmr[dev].cnr;
        if (timeout > LPTMR_RELOAD_OVERHEAD) {
            /* Compensate for the reload delay */
            lptmr[dev].cmr = timeout - LPTMR_RELOAD_OVERHEAD;
        }
        else {
            lptmr[dev].cmr = 0;
        }
    }
    else if (hw->CSR & LPTMR_CSR_TCF_MASK) {
        /* TCF is set, safe to update CMR live */
        hw->CMR = target - lptmr[dev].cnr;
        /* cppcheck-suppress selfAssignment
         * (reason: intentional self assignment to clear all pending IRQs) */
        hw->CSR = hw->CSR;
        /* Enable timer and IRQ */
        hw->CSR = LPTMR_CSR_TEN_MASK | LPTMR_CSR_TFC_MASK | LPTMR_CSR_TIE_MASK;
    }
    else {
        uint16_t timeout = target - lptmr_read(dev);
        lptmr_reload(dev, timeout);
    }
    irq_restore(mask);
    return 0;
}

static inline int lptmr_clear(uint8_t dev)
{
    /* Disable IRQs to minimize jitter */
    LPTMR_Type *hw = lptmr_config[dev].dev;
    unsigned int mask = irq_disable();

    if (!lptmr[dev].running) {
        /* Already clear */
        irq_restore(mask);
        return 0;
    }
    lptmr[dev].running = 0;
    if (!(hw->CSR & LPTMR_CSR_TEN_MASK)) {
        /* Timer is stopped */
        irq_restore(mask);
        return 0;
    }
    /* Disable interrupt, enable timer */
    hw->CSR = LPTMR_CSR_TEN_MASK | LPTMR_CSR_TFC_MASK;
    /* Clear IRQ if it occurred during this function */
    NVIC_ClearPendingIRQ(lptmr_config[dev].irqn);
    irq_restore(mask);
    return 0;
}

static inline void lptmr_start(uint8_t dev)
{
    LPTMR_Type *hw = lptmr_config[dev].dev;

    if (hw->CSR & LPTMR_CSR_TEN_MASK) {
        /* Timer is running */
        return;
    }
    /* Disable IRQs to avoid race with ISR */
    unsigned int mask = irq_disable();
    /* ensure hardware is reset */
    hw->CSR = 0;
    if (lptmr[dev].running) {
        /* set target */
        hw->CMR = lptmr[dev].cmr;
        /* enable interrupt and start timer */
        hw->CSR = LPTMR_CSR_TEN_MASK | LPTMR_CSR_TFC_MASK | LPTMR_CSR_TIE_MASK;
    }
    else {
        /* no target */
        hw->CMR = 0;
        /* Disable interrupt, enable timer */
        hw->CSR = LPTMR_CSR_TEN_MASK | LPTMR_CSR_TFC_MASK;
    }
    /* compensate for the reload delay when starting the timer */
    lptmr[dev].cnr += LPTMR_RELOAD_OVERHEAD;
    irq_restore(mask);
}

static inline void lptmr_stop(uint8_t dev)
{
    /* Disable IRQs to avoid race with ISR */
    unsigned int mask = irq_disable();
    LPTMR_Type *hw = lptmr_config[dev].dev;

    if (!(hw->CSR & LPTMR_CSR_TEN_MASK)) {
        /* Timer is already stopped */
        return;
    }
    /* Update state */
    /* Latch counter value */
    hw->CNR = 0;
    lptmr[dev].cnr += hw->CNR;
    uint16_t timeout = hw->CMR - hw->CNR;
    /* Disable timer */
    hw->CSR = 0;
    if (timeout > LPTMR_RELOAD_OVERHEAD) {
        /* Compensate for the delay in reloading */
        lptmr[dev].cmr = timeout - LPTMR_RELOAD_OVERHEAD;
    }
    else {
        lptmr[dev].cmr = timeout;
    }
    /* Clear any pending IRQ */
    NVIC_ClearPendingIRQ(lptmr_config[dev].irqn);
    irq_restore(mask);
}

#if defined(LPTMR_ISR_0) || defined(LPTMR_ISR_1)
static inline void lptmr_irq_handler(tim_t tim)
{
    uint8_t dev = _lptmr_index(tim);
    LPTMR_Type *hw = lptmr_config[dev].dev;

    lptmr[dev].running = 0;
    /* Disable interrupt generation, keep timer running */
    /* Do not clear TCF flag here, it is required for writing CMR without
     * disabling timer first */
    hw->CSR = LPTMR_CSR_TEN_MASK | LPTMR_CSR_TFC_MASK;

    if (lptmr[dev].isr_ctx.cb != NULL) {
        lptmr[dev].isr_ctx.cb(lptmr[dev].isr_ctx.arg, 0);
    }

    cortexm_isr_end();
}
#endif /* defined(LPTMR_ISR_0) || defined(LPTMR_ISR_1) */

#endif
/* ****** Common timer API functions ****** */

int timer_init(tim_t dev, uint32_t freq, timer_cb_t cb, void *arg)
{
    if ((unsigned int)dev >= TIMER_NUMOF) {
        /* invalid timer */
        return -1;
    }
    /* demultiplex to handle two types of hardware timers */
    switch (_timer_variant(dev)) {
        case TIMER_PIT:
            return pit_init(_pit_index(dev), freq, cb, arg);
#ifdef KINETIS_HAVE_LPTMR
        case TIMER_LPTMR:
            return lptmr_init(_lptmr_index(dev), freq, cb, arg);
#endif
        default:
            return -1;
    }
}

int timer_set(tim_t dev, int channel, unsigned int timeout)
{
    if (channel != 0) {
        /* only one channel is supported */
        return -1;
    }
    if ((unsigned int)dev >= TIMER_NUMOF) {
        /* invalid timer */
        return -1;
    }
    /* demultiplex to handle two types of hardware timers */
    switch (_timer_variant(dev)) {
        case TIMER_PIT:
            return pit_set(_pit_index(dev), timeout);
#ifdef KINETIS_HAVE_LPTMR
        case TIMER_LPTMR:
            return lptmr_set(_lptmr_index(dev), timeout);
#endif
        default:
            return -1;
    }
}

int timer_set_absolute(tim_t dev, int channel, unsigned int target)
{
    if (channel != 0) {
        /* only one channel is supported */
        return -1;
    }
    if ((unsigned int)dev >= TIMER_NUMOF) {
        /* invalid timer */
        return -1;
    }
    /* demultiplex to handle two types of hardware timers */
    switch (_timer_variant(dev)) {
        case TIMER_PIT:
            return pit_set_absolute(_pit_index(dev), target);
#ifdef KINETIS_HAVE_LPTMR
        case TIMER_LPTMR:
            return lptmr_set_absolute(_lptmr_index(dev), target);
#endif
        default:
            return -1;
    }

    return 0;
}

int timer_clear(tim_t dev, int channel)
{
    if (channel != 0) {
        /* only one channel is supported */
        return -1;
    }
    if ((unsigned int)dev >= TIMER_NUMOF) {
        /* invalid timer */
        return -1;
    }
    /* demultiplex to handle two types of hardware timers */
    switch (_timer_variant(dev)) {
        case TIMER_PIT:
            return pit_clear(_pit_index(dev));
#ifdef KINETIS_HAVE_LPTMR
        case TIMER_LPTMR:
            return lptmr_clear(_lptmr_index(dev));
#endif
        default:
            return -1;
    }

    return 0;
}

unsigned int timer_read(tim_t dev)
{
    if ((unsigned int)dev >= TIMER_NUMOF) {
        /* invalid timer */
        return 0;
    }
    /* demultiplex to handle two types of hardware timers */
    switch (_timer_variant(dev)) {
        case TIMER_PIT:
            return pit_read(_pit_index(dev));
#ifdef KINETIS_HAVE_LPTMR
        case TIMER_LPTMR:
            return lptmr_read(_lptmr_index(dev));
#endif
        default:
            return 0;
    }
}

void timer_start(tim_t dev)
{
    if ((unsigned int)dev >= TIMER_NUMOF) {
        /* invalid timer */
        return;
    }
    /* demultiplex to handle two types of hardware timers */
    switch (_timer_variant(dev)) {
        case TIMER_PIT:
            pit_start(_pit_index(dev));
            return;
#ifdef KINETIS_HAVE_LPTMR
        case TIMER_LPTMR:
            lptmr_start(_lptmr_index(dev));
            return;
#endif
        default:
            return;
    }
}

void timer_stop(tim_t dev)
{
    if ((unsigned int)dev >= TIMER_NUMOF) {
        /* invalid timer */
        return;
    }
    /* demultiplex to handle two types of hardware timers */
    switch (_timer_variant(dev)) {
        case TIMER_PIT:
            pit_stop(_pit_index(dev));
            return;
#ifdef KINETIS_HAVE_LPTMR
        case TIMER_LPTMR:
            lptmr_stop(_lptmr_index(dev));
            return;
#endif
        default:
            return;
    }
}

/* ****** ISR instances ****** */

void isr_pit(void)
{
    /* Some of the lower end Kinetis CPUs combine the individual PIT interrupt
     * flags into a single NVIC IRQ signal. This means that software needs to
     * test which timer(s) went off when an IRQ occurs. */
    for (size_t i = 0; i < PIT_NUMOF; ++i) {
        if (PIT->CHANNEL[pit_config[i].count_ch].TCTRL & PIT_TCTRL_TIE_MASK) {
            /* Interrupt is enabled */
            if (PIT->CHANNEL[pit_config[i].count_ch].TFLG) {
                /* Timer interrupt flag is set */
                pit_irq_handler(_pit_tim_t(i));
            }
        }
    }
}

#ifdef PIT_ISR_0
void PIT_ISR_0(void)
{
    pit_irq_handler(_pit_tim_t(0));
}
#endif

#ifdef PIT_ISR_1
void PIT_ISR_1(void)
{
    pit_irq_handler(_pit_tim_t(1));
}
#endif

#ifdef PIT_ISR_2
void PIT_ISR_2(void)
{
    pit_irq_handler(_pit_tim_t(2));
}
#endif

#ifdef PIT_ISR_3
void PIT_ISR_3(void)
{
    pit_irq_handler(_pit_tim_t(3));
}
#endif

#ifdef LPTMR_ISR_0
void LPTMR_ISR_0(void)
{
    lptmr_irq_handler(_lptmr_tim_t(0));
}
#endif

#ifdef LPTMR_ISR_1
void LPTMR_ISR_1(void)
{
    lptmr_irq_handler(_lptmr_tim_t(1));
}
#endif
