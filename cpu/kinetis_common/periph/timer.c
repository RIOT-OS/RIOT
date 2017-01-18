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
 * @ingroup     cpu_kinetis_common_timer
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
#include "board.h"
#include "periph_conf.h"
#include "periph/timer.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define PIT_MAX_VALUE     (PIT_LDVAL_TSV_MASK >> PIT_LDVAL_TSV_SHIFT)
#define LPTMR_MAX_VALUE   (LPTMR_CNR_COUNTER_MASK >> LPTMR_CNR_COUNTER_SHIFT)

#if TIMER_NUMOF != (PIT_NUMOF + LPTMR_NUMOF)
#error TIMER_NUMOF should be the total of PIT and LPTMR timers in the system
#endif

/*
 * The RTC prescaler will normally count to 32767 every second unless configured
 * otherwise through the time compensation register.
 */
#define TIMER_RTC_SUBTICK_MAX (0x7fff)
/*
 * Number of bits in the ideal RTC prescaler counter
 */
#define TIMER_RTC_SUBTICK_BITS (15)

/**
 * @brief  The number of ticks that will be lost when setting a new target in the LPTMR
 *
 * The counter will otherwise drop ticks when setting new timeouts.
 */
#define LPTMR_RELOAD_OVERHEAD 2

/**
 * @brief  Base clock frequency of the LPTMR module
 */
/* The LPTMR implementation is hard-coded to use ER32KCLK */
#define LPTMR_BASE_FREQ (32768ul)

/* PIT channel state */
typedef struct {
    timer_isr_ctx_t isr_ctx;
    uint32_t count;
    uint32_t tctrl;
    uint32_t ldval;
} pit_t;

/* LPTMR state */
typedef struct {
    timer_isr_ctx_t isr_ctx;
    uint32_t csr;
    uint32_t cmr;
    uint32_t running;
    uint16_t reference;
    uint16_t rtt_offset;
} lptmr_t;

static const pit_conf_t pit_config[PIT_NUMOF] = PIT_CONFIG;
static const lptmr_conf_t lptmr_config[LPTMR_NUMOF] = LPTMR_CONFIG;

static pit_t pit[PIT_NUMOF];
static lptmr_t lptmr[LPTMR_NUMOF];

/**
 * @brief  lvalue accessor for PIT channel enable bit in the bitband region
 */
#define PIT_BITBAND_TEN(ch) (BITBAND_REG32(PIT->CHANNEL[ch].TCTRL, PIT_TCTRL_TEN_SHIFT))

/**
 * @brief  Find out whether a given timer is a LPTMR or a PIT timer
 */
static inline unsigned int _timer_variant(tim_t dev) {
    if ((unsigned int) dev >= PIT_NUMOF) {
        return TIMER_LPTMR;
    }
    else {
        return TIMER_PIT;
    }
}

/**
 * @brief  Find device index in the pit_config array
 */
static inline unsigned int _pit_index(tim_t dev) {
    return ((unsigned int)dev) - TIMER_DEV(0);
}

/**
 * @brief  Get TIMER_x enum value from PIT device index
 */
static inline tim_t _pit_tim_t(uint8_t dev) {
    return (tim_t)(((unsigned int)TIMER_DEV(0)) + dev);
}

/**
 * @brief  Find device index in the lptmr_config array
 */
static inline unsigned int _lptmr_index(tim_t dev) {
    return ((unsigned int)dev) - TIMER_DEV(0) - PIT_NUMOF;
}

/**
 * @brief  Get TIMER_x enum value from LPTMR device index
 */
static inline tim_t _lptmr_tim_t(uint8_t dev) {
    return (tim_t)(((unsigned int)TIMER_DEV(0)) + PIT_NUMOF + dev);
}

/* ****** PIT module functions ****** */

/* Forward declarations */
inline static int pit_init(uint8_t dev, uint32_t freq, timer_cb_t cb, void *arg);
inline static int pit_set(uint8_t dev, uint32_t timeout);
inline static int pit_set_absolute(uint8_t dev, uint32_t target);
inline static int pit_clear(uint8_t dev);
inline static uint32_t pit_read(uint8_t dev);
inline static void pit_start(uint8_t dev);
inline static void pit_stop(uint8_t dev);
inline static void pit_irq_handler(tim_t dev);

inline static void _pit_set_cb_config(uint8_t dev, timer_cb_t cb, void *arg)
{
    /* set callback function */
    pit[dev].isr_ctx.cb = cb;
    pit[dev].isr_ctx.arg = arg;
}

/** use channel n-1 as prescaler */
inline static void _pit_set_prescaler(uint8_t ch, uint32_t freq)
{
    /* Disable channel completely */
    PIT->CHANNEL[ch].TCTRL = 0x0;
    PIT->CHANNEL[ch].LDVAL = (PIT_BASECLOCK / freq) - 1;
    /* Start the prescaler counter immediately */
    PIT->CHANNEL[ch].TCTRL = (PIT_TCTRL_TEN_MASK);
}

inline static void _pit_set_counter(uint8_t dev)
{
    const uint8_t ch = pit_config[dev].count_ch;
    /* Disable channel completely */
    PIT->CHANNEL[ch].TCTRL = 0x0;
    PIT->CHANNEL[ch].LDVAL = pit[dev].ldval;
    PIT->CHANNEL[ch].TFLG = PIT_TFLG_TIF_MASK;
    /* Restore previous timer state */
    PIT->CHANNEL[ch].TCTRL = pit[dev].tctrl;
}

inline static int pit_init(uint8_t dev, uint32_t freq, timer_cb_t cb, void *arg)
{
    PIT_CLOCKGATE = 1;
    /* Completely disable the module before messing with the settings */
    PIT->MCR = PIT_MCR_MDIS_MASK;

    /* Disable IRQs to avoid race with ISR */
    unsigned int mask = irq_disable();

    /* Clear configuration */
    PIT->CHANNEL[pit_config[dev].count_ch].TCTRL = 0;

    /* Freeze timers during debug break, resume normal operations (clear MDIS) */
    PIT->MCR = PIT_MCR_FRZ_MASK;

    _pit_set_cb_config(dev, cb, arg);

    /* Clear IRQ flag */
    PIT->CHANNEL[pit_config[dev].count_ch].TFLG = PIT_TFLG_TIF_MASK;
    /* Refactor the below lines if there are any CPUs where the PIT IRQs are not sequential */
    NVIC_ClearPendingIRQ(PIT0_IRQn + pit_config[dev].count_ch);
    NVIC_EnableIRQ(PIT0_IRQn + pit_config[dev].count_ch);

    /* Reset up-counter */
    pit[dev].count = PIT_MAX_VALUE;
    pit[dev].ldval = PIT_MAX_VALUE;
    pit[dev].tctrl = PIT_TCTRL_CHN_MASK | PIT_TCTRL_TEN_MASK;
    _pit_set_prescaler(pit_config[dev].prescaler_ch, freq);
    _pit_set_counter(dev);

    irq_restore(mask);
    return 0;
}

inline static int pit_set(uint8_t dev, uint32_t timeout)
{
    const uint8_t ch = pit_config[dev].count_ch;
    /* Disable IRQs to minimize the number of lost ticks */
    unsigned int mask = irq_disable();
    pit[dev].ldval = timeout;
    pit[dev].tctrl = PIT_TCTRL_TIE_MASK | PIT_TCTRL_CHN_MASK | PIT_TCTRL_TEN_MASK;
    /* Add the new timeout offset to the up-counter */
    pit[dev].count += timeout;
    if (PIT_BITBAND_TEN(ch) != 0) {
        /* Timer is currently running */
        uint32_t cval = PIT->CHANNEL[ch].CVAL;
        /* Subtract if there was anything left on the counter */
        pit[dev].count -= cval;
        _pit_set_counter(dev);
    }
    irq_restore(mask);
    return 0;
}

inline static int pit_set_absolute(uint8_t dev, uint32_t target)
{
    uint8_t ch = pit_config[dev].count_ch;
    /* Disable IRQs to minimize the number of lost ticks */
    unsigned int mask = irq_disable();
    uint32_t now = pit_read(dev);
    uint32_t offset = target - now;
    pit[dev].ldval = offset;
    pit[dev].tctrl = PIT_TCTRL_TIE_MASK | PIT_TCTRL_CHN_MASK | PIT_TCTRL_TEN_MASK;
    /* Set the new target time in the up-counter */
    pit[dev].count = target;
    if (PIT_BITBAND_TEN(ch) != 0) {
        _pit_set_counter(dev);
    }

    irq_restore(mask);
    return 0;
}

inline static int pit_clear(uint8_t dev)
{
    uint8_t ch = pit_config[dev].count_ch;
    /* Disable IRQs to minimize the number of lost ticks */
    unsigned int mask = irq_disable();

    pit[dev].ldval = PIT_MAX_VALUE;
    pit[dev].tctrl = PIT_TCTRL_CHN_MASK | PIT_TCTRL_TEN_MASK;
    /* pit[dev].count += PIT_MAX_VALUE + 1; */ /* == 0 (mod 2**32) */

    if (PIT_BITBAND_TEN(ch) != 0) {
        /* Timer is currently running */
        uint32_t cval = PIT->CHANNEL[ch].CVAL;
        /* Subtract if there was anything left on the counter */
        pit[dev].count -= cval;
        /* Set a long timeout */
        _pit_set_counter(ch);
    }

    irq_restore(mask);
    return 0;
}

inline static uint32_t pit_read(uint8_t dev)
{
    uint8_t ch = pit_config[dev].count_ch;
    if (PIT_BITBAND_TEN(ch) != 0) {
        /* Timer running */
        return pit[dev].count - PIT->CHANNEL[ch].CVAL;
    }
    else {
        /* Timer stopped */
        return pit[dev].count;
    }
}

inline static void pit_start(uint8_t dev)
{
    uint8_t ch = pit_config[dev].count_ch;
    if (PIT_BITBAND_TEN(ch) != 0) {
        /* Already running */
        return;
    }
    PIT->CHANNEL[ch].LDVAL = pit[dev].ldval;
    pit[dev].count += pit[dev].ldval;
    PIT->CHANNEL[ch].TCTRL = pit[dev].tctrl;
}

inline static void pit_stop(uint8_t dev)
{
    uint8_t ch = pit_config[dev].count_ch;
    if (PIT_BITBAND_TEN(ch) == 0) {
        /* Already stopped */
        return;
    }
    uint32_t cval = PIT->CHANNEL[ch].CVAL;
    pit[dev].tctrl = PIT->CHANNEL[ch].TCTRL;
    PIT->CHANNEL[ch].TCTRL = 0;
    pit[dev].count -= cval;
    pit[dev].ldval = cval;
}

inline static void pit_irq_handler(tim_t dev)
{
    uint8_t ch = pit_config[_pit_index(dev)].count_ch;
    pit_t *pit_ctx = &pit[_pit_index(dev)];
    pit_ctx->ldval = PIT_MAX_VALUE;
    pit_ctx->count += PIT_MAX_VALUE;
    pit_ctx->tctrl = PIT_TCTRL_CHN_MASK | PIT_TCTRL_TEN_MASK;
    _pit_set_counter(_pit_index(dev));

    if (pit_ctx->isr_ctx.cb != NULL) {
        pit_ctx->isr_ctx.cb(pit_ctx->isr_ctx.arg, 0);
    }

    PIT->CHANNEL[ch].TFLG = PIT_TFLG_TIF_MASK;

    cortexm_isr_end();
}

/* ****** LPTMR module functions ****** */

/* Forward declarations */
inline static int lptmr_init(uint8_t dev, uint32_t freq, timer_cb_t cb, void *arg);
inline static int lptmr_set(uint8_t dev, uint16_t timeout);
inline static int lptmr_set_absolute(uint8_t dev, uint16_t target);
inline static int lptmr_clear(uint8_t dev);
inline static uint16_t lptmr_read(uint8_t dev);
inline static void lptmr_start(uint8_t dev);
inline static void lptmr_stop(uint8_t dev);
inline static void lptmr_irq_handler(tim_t tim);

/**
 * @brief Read the prescaler register from the RTC as a reliable 47 bit time counter
 */
inline static uint32_t _rtt_get_subtick(void)
{
    uint32_t tpr;
    uint32_t tsr;

    for (int i = 0; i < 5; i++) {
        /* Read twice to make sure we get a stable reading */
        tpr = RTC->TPR & RTC_TPR_TPR_MASK;
        tsr = RTC->TSR & RTC_TSR_TSR_MASK;

        if ((tsr == (RTC->TSR & RTC_TSR_TSR_MASK)) &&
            (tpr == (RTC->TPR & RTC_TPR_TPR_MASK))) {
            break;
        }
    }
    if (tpr > TIMER_RTC_SUBTICK_MAX) {
        /* This only happens if the RTC time compensation value has been
         * modified to compensate for RTC drift. See Kinetis ref.manual,
         *  RTC Time Compensation Register (RTC_TCR).
         */
        tpr = TIMER_RTC_SUBTICK_MAX;
    }

    return (tsr << TIMER_RTC_SUBTICK_BITS) | tpr;
}

inline static void _lptmr_set_cb_config(uint8_t dev, timer_cb_t cb, void *arg)
{
    /* set callback function */
    lptmr[dev].isr_ctx.cb = cb;
    lptmr[dev].isr_ctx.arg = arg;
}

/**
 * @brief  Compute the LPTMR prescaler setting, see reference manual for details
 */
inline static int32_t _lptmr_compute_prescaler(uint32_t freq) {
    uint32_t prescale = 0;
    if ((freq > LPTMR_BASE_FREQ) || (freq == 0)) {
        /* Frequency out of range */
        return -1;
    }
    while (freq < LPTMR_BASE_FREQ){
        ++prescale;
        freq <<= 1;
    }
    if (freq != LPTMR_BASE_FREQ) {
        /* freq was not a power of two division of LPTMR_BASE_FREQ */
        return -2;
    }
    if (prescale > 0) {
        /* LPTMR_PSR_PRESCALE == 0 yields LPTMR_BASE_FREQ/2,
         * LPTMR_PSR_PRESCALE == 1 yields LPTMR_BASE_FREQ/4 etc.. */
        return LPTMR_PSR_PRESCALE(prescale - 1);
    }
    else {
        /* Prescaler bypass enabled */
        return LPTMR_PSR_PBYP_MASK;
    }
}

/**
 * @brief  Update the offset between RTT and LPTMR
 */
inline static void _lptmr_update_rtt_offset(uint8_t dev)
{
    lptmr[dev].rtt_offset = _rtt_get_subtick();
}

/**
 * @brief  Update the reference time point (CNR=0)
 */
inline static void _lptmr_update_reference(uint8_t dev)
{
    lptmr[dev].reference = _rtt_get_subtick() + LPTMR_RELOAD_OVERHEAD - lptmr[dev].rtt_offset;
}

inline static void _lptmr_set_counter(uint8_t dev)
{
    _lptmr_update_reference(dev);
    LPTMR_Type *hw = lptmr_config[dev].dev;
    hw->CSR = 0;
    hw->CMR = lptmr[dev].cmr;
    /* restore saved state */
    hw->CSR = lptmr[dev].csr;
}

inline static int lptmr_init(uint8_t dev, uint32_t freq, timer_cb_t cb, void *arg)
{
    int32_t prescale = _lptmr_compute_prescaler(freq);
    if (prescale < 0) {
        return -1;
    }
    LPTMR_Type *hw = lptmr_config[dev].dev;
    /* Disable IRQs to avoid race with ISR */
    unsigned int mask = irq_disable();

    /* Turn on module clock */
    *lptmr_config[dev].clk_gate = 1;
    /* Completely disable the module before messing with the settings */
    hw->CSR = 0;
    /* select ERCLK32K as clock source for LPTMR */
    hw->PSR = LPTMR_PSR_PCS(2) | ((uint32_t)prescale);

    /* Clear IRQ flag in case it was already set */
    hw->CSR = LPTMR_CSR_TCF_MASK;
    /* Enable IRQs on the counting channel */
    /* Refactor the below lines if there are any CPUs where the LPTMR IRQs are not sequential */
    NVIC_ClearPendingIRQ(LPTMR0_IRQn + lptmr_config[dev].index);
    NVIC_EnableIRQ(LPTMR0_IRQn + lptmr_config[dev].index);

    _lptmr_set_cb_config(dev, cb, arg);

    /* Reset state */
    _lptmr_update_rtt_offset(dev);
    lptmr[dev].running = 1;
    lptmr_clear(dev);

    irq_restore(mask);

    return 0;
}

inline static uint16_t lptmr_read(uint8_t dev)
{
    LPTMR_Type *hw = lptmr_config[dev].dev;
    /* latch the current timer value into CNR */
    hw->CNR = 0;
    return lptmr[dev].reference + hw->CNR;
}

inline static int lptmr_set(uint8_t dev, uint16_t timeout)
{
    /* Disable IRQs to minimize jitter */
    unsigned int mask = irq_disable();
    lptmr[dev].cmr = timeout;
    /* Enable interrupt, enable timer */
    lptmr[dev].csr = LPTMR_CSR_TEN_MASK | LPTMR_CSR_TIE_MASK;
    if (lptmr[dev].running != 0) {
        /* Timer is currently running */
        /* Set new target */
        _lptmr_set_counter(dev);
    }
    irq_restore(mask);
    return 0;
}

inline static int lptmr_set_absolute(uint8_t dev, uint16_t target)
{
    /* Disable IRQs to minimize jitter */
    unsigned int mask = irq_disable();
    uint16_t offset = target - lptmr[dev].reference;
    lptmr[dev].cmr = offset;
    /* Enable interrupt, enable timer */
    lptmr[dev].csr = LPTMR_CSR_TEN_MASK | LPTMR_CSR_TIE_MASK;
    if (lptmr[dev].running != 0) {
        /* Timer is currently running */
        /* Set new target */
        _lptmr_set_counter(dev);
    }
    irq_restore(mask);
    return 0;
}

inline static int lptmr_clear(uint8_t dev)
{
    /* Disable IRQs to minimize jitter */
    unsigned int mask = irq_disable();
    lptmr[dev].cmr = LPTMR_MAX_VALUE;
    /* Disable interrupt, enable timer */
    lptmr[dev].csr = LPTMR_CSR_TEN_MASK;
    if (lptmr[dev].running != 0) {
        /* Timer is currently running */
        /* Set new target */
        _lptmr_set_counter(dev);
    }
    irq_restore(mask);
    return 0;
}

inline static void lptmr_start(uint8_t dev)
{
    if (lptmr[dev].running != 0) {
        /* Timer already running */
        return;
    }
    lptmr[dev].running = 1;
    _lptmr_set_counter(dev);
}

inline static void lptmr_stop(uint8_t dev)
{
    if (lptmr[dev].running == 0) {
        /* Timer already stopped */
        return;
    }
    /* Disable IRQs to avoid race with ISR */
    unsigned int mask = irq_disable();
    lptmr[dev].running = 0;
    LPTMR_Type *hw = lptmr_config[dev].dev;
    /* latch the current timer value into CNR */
    hw->CNR = 12345;
    uint16_t cnr = hw->CNR;
    lptmr[dev].cmr = hw->CMR - cnr;
    lptmr[dev].csr = hw->CSR;
    _lptmr_update_reference(dev);
    /* Disable counter and clear interrupt flag */
    hw->CSR = LPTMR_CSR_TCF_MASK;
    /* Clear any pending IRQ */
    NVIC_ClearPendingIRQ(LPTMR0_IRQn + lptmr_config[dev].index);
    irq_restore(mask);
}

inline static void lptmr_irq_handler(tim_t tim)
{
    uint8_t dev = _lptmr_index(tim);
    LPTMR_Type *hw = lptmr_config[dev].dev;
    lptmr_t *lptmr_ctx = &lptmr[dev];
    lptmr_ctx->cmr = LPTMR_MAX_VALUE;
    _lptmr_set_counter(dev);

    if (lptmr_ctx->isr_ctx.cb != NULL) {
        lptmr_ctx->isr_ctx.cb(lptmr_ctx->isr_ctx.arg, 0);
    }

    /* Clear interrupt flag */
    BITBAND_REG32(hw->CSR, LPTMR_CSR_TCF_SHIFT) = 1;

    cortexm_isr_end();
}

/* ****** Common timer API functions ****** */

int timer_init(tim_t dev, unsigned long freq, timer_cb_t cb, void *arg)
{
    if ((unsigned int)dev >= TIMER_NUMOF) {
        /* invalid timer */
        return -1;
    }
    /* demultiplex to handle two types of hardware timers */
    switch (_timer_variant(dev)) {
        case TIMER_PIT:
            return pit_init(_pit_index(dev), freq, cb, arg);
        case TIMER_LPTMR:
            return lptmr_init(_lptmr_index(dev), freq, cb, arg);
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
        case TIMER_LPTMR:
            return lptmr_set(_lptmr_index(dev), timeout);
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
        case TIMER_LPTMR:
            return lptmr_set_absolute(_lptmr_index(dev), target);;
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
        case TIMER_LPTMR:
            return lptmr_clear(_lptmr_index(dev));
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
        case TIMER_LPTMR:
            return lptmr_read(_lptmr_index(dev));
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
        case TIMER_LPTMR:
            lptmr_start(_lptmr_index(dev));
            return;
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
        case TIMER_LPTMR:
            lptmr_stop(_lptmr_index(dev));
            return;
        default:
            return;
    }
}

/* ****** ISR instances ****** */

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
