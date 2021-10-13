/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32
 * @ingroup     drivers_periph_timer
 * @{
 *
 * @file
 * @brief       Low-level timer driver implementation for ESP32 SDK
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#include <inttypes.h>

/*
 * WARNING! enable debugging will have timing side effects and can lead
 * to timer underflows, system crashes or system dead locks in worst case.
 */
#define ENABLE_DEBUG 0
#include "debug.h"

#include "periph/timer.h"

#include "driver/periph_ctrl.h"
#include "esp/common_macros.h"
#include "rom/ets_sys.h"
#include "soc/rtc.h"
#include "soc/timer_group_struct.h"
#include "xtensa/hal.h"
#include "xtensa/xtensa_api.h"

#include "esp_common.h"
#include "irq_arch.h"
#include "syscalls.h"
#include "xtimer.h"

#define RTC_PLL_480M    480 /* PLL with 480 MHz at maximum */
#define RTC_PLL_320M    320 /* PLL with 480 MHz at maximum */

#ifndef MODULE_ESP_HW_COUNTER

/* hardware timer modules used */

/**
  * ESP32 has four 64 bit hardware timers:
  * two timer groups TMG0 and TMG1 with 2 timers each
  *
  * TMG0, timer 0 is used for system time in us and is therefore not
  * available as low level timer. Timers have only one channel. Timer device
  * are mapped to hardware timer as following:
  *
  *     0 -> TMG0 timer 1
  *     1 -> TMG1 timer 0
  *     2 -> TMG1 timer 1
  *
  * The reason for this mapping is, that if only one timer is needed,
  * TMG1 is left disabled. TMG1 is only enabled when more than one
  * timer device is needed.
  *
  * PLEASE NOTE: Don't use ETS timer functions ets_timer_* in and this hardware
  * timer implementation together!
  */

#define HW_TIMER_NUMOF        3
#define HW_TIMER_CHANNELS     1
#define HW_TIMER_CLK_DIV      (rtc_clk_apb_freq_get() / 1000000)
#define HW_TIMER_CORRECTION   (RTC_PLL_320M / CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ)
#define HW_TIMER_DELTA_MIN    (MAX(HW_TIMER_CORRECTION << 1, 5))

struct hw_timer_regs_t {
    /* see Technical Reference, section 17.4 */
    struct {
        uint32_t unused      : 10;
        uint32_t ALARM_EN    : 1;  /* alarms are enabled */
        uint32_t LEVEL_INT_EN: 1;  /* alarms will generate level type interrupt */
        uint32_t EDGE_INT_EN : 1;  /* alarms will generate edge type interrupt */
        uint32_t DIVIDER     : 16; /* timer clock prescale value (basis is ABP) */
        uint32_t AUTORELOAD  : 1;  /* auto-reload on alarms */
        uint32_t INCREASE    : 1;  /* count up */
        uint32_t EN          : 1;  /* timer is enabled */
    } CONFIG_REG;
    uint32_t LO_REG;      /* time-base counter value low 32 bits */
    uint32_t HI_REG;      /* time-base counter value high 32 bits */
    uint32_t UPDATE_REG;  /* time-base counter value update trigger */
    uint32_t ALARMLO_REG; /* alarm trigger time-base counter value, low 32 bits */
    uint32_t ALARMHI_REG; /* alarm trigger time-base counter value, high 32 bits */
    uint32_t LOADLO_REG;  /* reload value, low 32 bits */
    uint32_t LOADHI_REG;  /* reload value, high 32 bits */
    uint32_t LOAD_REG;    /* reload trigger */
};

struct hw_timer_ints_t {
    /* see Technical Reference, section 17.4 */
    uint32_t INT_ENA_REG;    /* interrupt enable bits */
    uint32_t INT_RAW_REG;    /* raw interrupt status */
    uint32_t INT_STA_REG;    /* masked interrupt status */
    uint32_t INT_CLR_REG;    /* interrupt clear bits */
};

struct hw_timer_t {
    bool  initialized; /* indicates whether timer is already initialized */
    bool  started;     /* indicates whether timer is already started */

    timer_isr_ctx_t isr_ctx;
};

struct hw_timer_hw_t {
    volatile struct hw_timer_regs_t* regs;     /* timer configuration regs */
    volatile struct hw_timer_ints_t* int_regs; /* timer interrupt regs */
    uint8_t int_mask;  /* timer interrupt bit mask in interrupt regs */
    uint8_t int_src;   /* timer interrupt source */
};

static struct hw_timer_t timers[HW_TIMER_NUMOF] = { };
static const struct hw_timer_hw_t timers_hw[HW_TIMER_NUMOF] =
{
    {
        .regs = (struct hw_timer_regs_t*)&TIMERG0.hw_timer[1],
        .int_regs = (struct hw_timer_ints_t*)&TIMERG0.int_ena,
        .int_mask = BIT(1),
        .int_src  = ETS_TG0_T1_LEVEL_INTR_SOURCE
    },
    {
        .regs = (struct hw_timer_regs_t*)&TIMERG1.hw_timer[0],
        .int_regs = (struct hw_timer_ints_t*)&TIMERG1.int_ena,
        .int_mask = BIT(0),
        .int_src  = ETS_TG1_T0_LEVEL_INTR_SOURCE
    },
    {
        .regs = (struct hw_timer_regs_t*)&TIMERG1.hw_timer[1],
        .int_regs = (struct hw_timer_ints_t*)&TIMERG1.int_ena,
        .int_mask = BIT(1),
        .int_src  = ETS_TG1_T1_LEVEL_INTR_SOURCE
    }
};

/** Latches the current counter value and return only the low part */
static inline uint32_t timer_get_counter_lo(tim_t dev)
{
    /* latch the current timer value by writing any value to the update reg */
    timers_hw[dev].regs->UPDATE_REG = 0;
    /* read high and low part of counter */
    return timers_hw[dev].regs->LO_REG;
}

/** Latches the current counter value and return the high and the low part */
static inline void timer_get_counter(tim_t dev, uint32_t* hi, uint32_t* lo)
{
    /* parameter check */
    if (!hi || !lo) {
        return;
    }
    /* latch the current timer value by writing any value to the update reg */
    timers_hw[dev].regs->UPDATE_REG = 0;
    /* read high and low part of counter */
    *hi = timers_hw[dev].regs->HI_REG;
    *lo = timers_hw[dev].regs->LO_REG;
}

void IRAM hw_timer_handler(void* arg)
{
    (void)arg;

    /* since all timer interrupt sources are routed to the same cpu interrupt */
    /* signal, we can't use arg to identify the timer which caused the it */

    irq_isr_enter();

    for (unsigned dev = 0; dev < HW_TIMER_NUMOF; dev++) {
        /* iterate over all devices and check what interrupt flags are set */
        if (timers_hw[dev].int_regs->INT_STA_REG & timers_hw[dev].int_mask) {
            DEBUG("%s dev=%d\n", __func__, dev);
            /* disable alarms */
            timers_hw[dev].regs->CONFIG_REG.LEVEL_INT_EN = 0;
            timers_hw[dev].regs->CONFIG_REG.ALARM_EN = 0;
            /* clear the bit in interrupt enable and status register */
            timers_hw[dev].int_regs->INT_ENA_REG &= ~timers_hw[dev].int_mask;
            timers_hw[dev].int_regs->INT_CLR_REG |=  timers_hw[dev].int_mask;
            /* execute the callback function */
            timers[dev].isr_ctx.cb(timers[dev].isr_ctx.arg, 0);
        }
    }

    irq_isr_exit();
}

int timer_init (tim_t dev, uint32_t freq, timer_cb_t cb, void *arg)
{
    DEBUG("%s dev=%u freq=%" PRIu32 " cb=%p arg=%p\n",
          __func__, dev, freq, cb, arg);

    CHECK_PARAM_RET (dev  <  HW_TIMER_NUMOF, -1);
    CHECK_PARAM_RET (freq == XTIMER_HZ_BASE, -1);
    CHECK_PARAM_RET (cb   != NULL, -1);

    if (timers[dev].initialized) {
        DEBUG("%s timer dev=%u is already initialized (used)\n", __func__, dev);
        return -1;
    }

    /* initialize timer data structure */
    timers[dev].initialized = true;
    timers[dev].started     = false;
    timers[dev].isr_ctx.cb  = cb;
    timers[dev].isr_ctx.arg = arg;

    /* route all timer interrupt sources to the same level type interrupt */
    intr_matrix_set(PRO_CPU_NUM, timers_hw[dev].int_src, CPU_INUM_TIMER);

    /* we have to enable therefore the interrupt here */
    xt_set_interrupt_handler(CPU_INUM_TIMER, hw_timer_handler, NULL);
    xt_ints_on(BIT(CPU_INUM_TIMER));

    if (dev) {
        /* if dev > 0 we have to enable TMG1 module */
        periph_module_enable(PERIPH_TIMG1_MODULE);
    }

    /* hardware timer configuration */
    timers_hw[dev].regs->CONFIG_REG.EN = 0;
    timers_hw[dev].regs->CONFIG_REG.AUTORELOAD = 0;
    timers_hw[dev].regs->CONFIG_REG.INCREASE = 1;
    timers_hw[dev].regs->CONFIG_REG.DIVIDER = HW_TIMER_CLK_DIV;
    timers_hw[dev].regs->CONFIG_REG.EDGE_INT_EN = 0;
    timers_hw[dev].regs->CONFIG_REG.LEVEL_INT_EN = 0;
    timers_hw[dev].regs->CONFIG_REG.ALARM_EN = 0;

    /* start the timer */
    timer_start(dev);

    return 0;
}

int IRAM timer_set(tim_t dev, int chn, unsigned int delta)
{
    DEBUG("%s dev=%u channel=%d delta=%u\n", __func__, dev, chn, delta);

    CHECK_PARAM_RET (dev < HW_TIMER_NUMOF, -1);
    CHECK_PARAM_RET (chn < HW_TIMER_CHANNELS, -1);

    /* disable interrupts */
    int state = irq_disable ();

    /* disable alarms */
    timers_hw[dev].regs->CONFIG_REG.LEVEL_INT_EN = 0;
    timers_hw[dev].regs->CONFIG_REG.ALARM_EN = 0;

    delta = (delta > HW_TIMER_DELTA_MIN) ? delta : HW_TIMER_DELTA_MIN;
    delta = (delta > HW_TIMER_CORRECTION) ? delta - HW_TIMER_CORRECTION : HW_TIMER_CORRECTION;

    /* read the current value */
    uint32_t count_lo;
    uint32_t count_hi;
    timer_get_counter(dev, &count_hi, &count_lo);

    /* determine the alarm time */
    uint64_t alarm;
    alarm  = count_lo;
    alarm += ((uint64_t)count_hi) << 32;
    alarm += delta;

    timers_hw[dev].regs->ALARMHI_REG = (uint32_t)(alarm >> 32);
    timers_hw[dev].regs->ALARMLO_REG = (uint32_t)(alarm & 0xffffffff);

    /* enable alarms */
    timers_hw[dev].regs->CONFIG_REG.LEVEL_INT_EN = 1;
    timers_hw[dev].regs->CONFIG_REG.ALARM_EN = 1;

    /* wait until instructions have been finished */
    timers_hw[dev].regs->CONFIG_REG.EN = 1;

    /* clear the bit in status and set the bit in interrupt enable */
    timers_hw[dev].int_regs->INT_CLR_REG |= timers_hw[dev].int_mask;
    timers_hw[dev].int_regs->INT_ENA_REG |= timers_hw[dev].int_mask;

    /* restore interrupts enabled state */
    irq_restore (state);

    return 0;
}

int IRAM timer_set_absolute(tim_t dev, int chn, unsigned int value)
{
    DEBUG("%s dev=%u channel=%d value=%u\n", __func__, dev, chn, value);

    return timer_set (dev, chn, value - timer_read(dev));
}

int timer_clear(tim_t dev, int chn)
{
    DEBUG("%s dev=%u channel=%d\n", __func__, dev, chn);

    CHECK_PARAM_RET (dev < HW_TIMER_NUMOF, -1);
    CHECK_PARAM_RET (chn < HW_TIMER_CHANNELS, -1);

    /* disable alarms */
    timers_hw[dev].regs->CONFIG_REG.LEVEL_INT_EN = 0;
    timers_hw[dev].regs->CONFIG_REG.ALARM_EN = 0;
    /* clear the bit in interrupt enable and status register */
    timers_hw[dev].int_regs->INT_ENA_REG &= ~timers_hw[dev].int_mask;
    timers_hw[dev].int_regs->INT_CLR_REG |=  timers_hw[dev].int_mask;

    return 0;
}

unsigned int IRAM timer_read(tim_t dev)
{
    CHECK_PARAM_RET (dev < HW_TIMER_NUMOF, -1);

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        uint32_t count_lo = timer_get_counter_lo(dev);
        DEBUG("%s %u\n", __func__, count_lo);
        return count_lo;
    }
    else {
        return timer_get_counter_lo(dev);
    }
}

void IRAM timer_start(tim_t dev)
{
    DEBUG("%s dev=%u @%u\n", __func__, dev, system_get_time());

    CHECK_PARAM (dev < HW_TIMER_NUMOF);

    timers_hw[dev].regs->CONFIG_REG.EN = 1;
}

void IRAM timer_stop(tim_t dev)
{
    DEBUG("%s dev=%u\n", __func__, dev);

    CHECK_PARAM (dev < HW_TIMER_NUMOF);

    timers_hw[dev].regs->CONFIG_REG.EN = 0;
}

#else /* MODULE_ESP_HW_COUNTER */

/* hardware counter used as timer */

/**
  * ESP32 has 3 ccompare registers. Each of them can generate an interrupt
  * at different levels:
  *
  * CCOMPARE    INT                           Level   Priority
  *     0        6 XCHAL_TIMER0_INTERRUPT       1     low
  *     1       15 XCHAL_TIMER1_INTERRUPT       3     medium
  *     2       16 XCHAL_TIMER2_INTERRUPT       5     high
  *
  * PLEASE NOTE: High level interrupts are not disabled in any case. So be
  * careful to to use CCOMPARE register 2 and timer num 2, respectively.
  * By default, TIMER_NUMOF is therefore set to only 2 in periph_conf.h.
  */
#define HW_TIMER_NUMOF        XCHAL_NUM_TIMERS
#define HW_TIMER_CHANNELS     1

#define HW_TIMER_MASK         0xffffffff
#define HW_TIMER_DELTA_MAX    0x00ffffff  /* in us */
#define HW_TIMER_DELTA_MASK   0x00ffffff
#define HW_TIMER_DELTA_RSHIFT 24

#define HW_TIMER_CORRECTION   (RTC_PLL_480M / CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ)
#define HW_TIMER_DELTA_MIN    (MAX(HW_TIMER_CORRECTION, 5))

#define US_TO_HW_TIMER_TICKS(t)    (t * system_get_cpu_freq())
#define HW_TIMER_TICKS_TO_US(t)    (t / system_get_cpu_freq())

struct hw_channel_t {
    bool        used;         /* indicates whether the channel is used */
    uint32_t    start_time;   /* physical time when the timer channel has been started */
    uint32_t    delta_time;   /* timer delta value (delta = cycles * timer_max + remainder) */
    uint32_t    cycles;       /* number of complete max timer cycles */
    uint32_t    remainder;    /* remainder timer value */
};

struct hw_timer_t {
    tim_t                dev;         /* the timer device num */
    bool                 initialized; /* indicates whether timer is already initialized */
    bool                 started;     /* indicates whether timer is already started */
    timer_isr_ctx_t      isr_ctx;
    struct hw_channel_t  channels[HW_TIMER_CHANNELS];
};

static struct hw_timer_t timers[HW_TIMER_NUMOF] = { };
static const uint8_t timers_int[HW_TIMER_NUMOF] = { XCHAL_TIMER0_INTERRUPT,
                                                    XCHAL_TIMER1_INTERRUPT,
                                                    XCHAL_TIMER2_INTERRUPT };

static void __timer_channel_start (struct hw_timer_t* timer, struct hw_channel_t* channel);
static void __timer_channel_stop (struct hw_timer_t* timer, struct hw_channel_t* channel);

static uint32_t __hw_timer_ticks_max;
static uint32_t __hw_timer_ticks_min;

void IRAM hw_timer_handler(void* arg)
{
    uint32_t dev = (uint32_t)arg;
    uint32_t chn = 0;

    if (dev >= HW_TIMER_NUMOF && chn >= HW_TIMER_CHANNELS) {
        return;
    }

    irq_isr_enter();

    DEBUG("%s arg=%p\n", __func__, arg);

    struct hw_timer_t*   timer   = &timers[dev];
    struct hw_channel_t* channel = &timer->channels[chn];

    if (channel->cycles) {
        channel->cycles--;
        xthal_set_ccompare(dev, xthal_get_ccount() + __hw_timer_ticks_max);
    }
    else if (channel->remainder >= HW_TIMER_DELTA_MIN) {
        xthal_set_ccompare (dev, xthal_get_ccount() +
                                 US_TO_HW_TIMER_TICKS(channel->remainder));
        channel->remainder = 0;
    }
    else {
        channel->remainder = 0;
        channel->used = false;
        xt_ints_off(BIT(timers_int[dev]));
        xthal_set_ccompare (dev, 0);
        timer->isr_ctx.cb(timer->isr_ctx.arg, chn);
    }

    irq_isr_exit();
}

int timer_init (tim_t dev, uint32_t freq, timer_cb_t cb, void *arg)
{
    DEBUG("%s dev=%u freq=%lu cb=%p arg=%p\n", __func__, dev, freq, cb, arg);

    CHECK_PARAM_RET (dev  <  HW_TIMER_NUMOF, -1);
    CHECK_PARAM_RET (freq == XTIMER_HZ_BASE, -1);
    CHECK_PARAM_RET (cb   != NULL, -1);

    if (timers[dev].initialized) {
        DEBUG("%s timer dev=%u is already initialized (used)\n", __func__, dev);
        return -1;
    }

    timers[dev].dev = dev;
    timers[dev].initialized = true;
    timers[dev].started     = false;
    timers[dev].isr_ctx.cb  = cb;
    timers[dev].isr_ctx.arg = arg;

    xt_set_interrupt_handler(timers_int[dev], hw_timer_handler, (void *)dev);

    for (int i = 0; i < HW_TIMER_CHANNELS; i++) {
        timers[dev].channels[i].used = false;
        timers[dev].channels[i].cycles = 0;
        timers[dev].channels[i].remainder = 0;
    }

    timer_start(dev);

    return 0;
}

int IRAM timer_set(tim_t dev, int chn, unsigned int delta)
{
    DEBUG("%s dev=%u channel=%d delta=%u\n", __func__, dev, chn, delta);

    CHECK_PARAM_RET (dev < HW_TIMER_NUMOF, -1);
    CHECK_PARAM_RET (chn < HW_TIMER_CHANNELS, -1);

    int state = irq_disable ();

    struct hw_timer_t*   timer   = &timers[dev];
    struct hw_channel_t* channel = &timer->channels[chn];

    /* set delta time and channel used flag */
    channel->delta_time = delta > HW_TIMER_CORRECTION ? delta - HW_TIMER_CORRECTION : 0;
    channel->used = true;

    /* start channel with new delta time */
    __timer_channel_start (timer, channel);

    irq_restore (state);

    return 0;
}

int IRAM timer_set_absolute(tim_t dev, int chn, unsigned int value)
{
    DEBUG("%s dev=%u channel=%d value=%u\n", __func__, dev, chn, value);
    return timer_set (dev, chn, value - timer_read(dev));
}

int timer_clear(tim_t dev, int chn)
{
    DEBUG("%s dev=%u channel=%d\n", __func__, dev, chn);

    CHECK_PARAM_RET (dev < HW_TIMER_NUMOF, -1);
    CHECK_PARAM_RET (chn < HW_TIMER_CHANNELS, -1);

    int state = irq_disable ();

    /* stop running timer channel */
    __timer_channel_stop (&timers[dev], &timers[dev].channels[chn]);

    irq_restore (state);

    return 0;
}

unsigned int IRAM timer_read(tim_t dev)
{
    (void)dev;

    return system_get_time ();
}

void IRAM timer_start(tim_t dev)
{
    DEBUG("%s dev=%u @%u\n", __func__, dev, system_get_time());

    CHECK_PARAM (dev < HW_TIMER_NUMOF);
    CHECK_PARAM (!timers[dev].started);

    int state = irq_disable ();

    __hw_timer_ticks_max = US_TO_HW_TIMER_TICKS(HW_TIMER_DELTA_MAX);
    __hw_timer_ticks_min = US_TO_HW_TIMER_TICKS(HW_TIMER_DELTA_MIN);

    struct hw_timer_t* timer = &timers[dev];

    timer->started = true;

    for (int i = 0; i < HW_TIMER_CHANNELS; i++) {
         __timer_channel_start (timer, &timer->channels[i]);
    }

    irq_restore (state);
}

void IRAM timer_stop(tim_t dev)
{
    DEBUG("%s dev=%u\n", __func__, dev);

    CHECK_PARAM (dev < HW_TIMER_NUMOF);

    int state = irq_disable ();

    struct hw_timer_t* timer = &timers[dev];

    timer->started = false;

    for (int i = 0; i < HW_TIMER_CHANNELS; i++) {
        __timer_channel_stop (timer, &timer->channels[i]);
    }

    irq_restore (state);
}

static void IRAM __timer_channel_start (struct hw_timer_t* timer, struct hw_channel_t* channel)
{
    if (!timer->started || !channel->used) {
        return;
    }

    /* save channel starting time */
    channel->start_time = timer_read (0);
    channel->cycles     = channel->delta_time >> HW_TIMER_DELTA_RSHIFT;
    channel->remainder  = channel->delta_time &  HW_TIMER_DELTA_MASK;

    DEBUG("%s cycles=%u remainder=%u @%u\n",
          __func__, channel->cycles, channel->remainder, system_get_time());

    /* start timer either with full cycles, remaining or minimum time */
    if (channel->cycles) {
        channel->cycles--;
        xthal_set_ccompare(timer->dev, xthal_get_ccount() + __hw_timer_ticks_max);
    }
    else if (channel->remainder > HW_TIMER_DELTA_MIN) {
        xthal_set_ccompare(timer->dev, xthal_get_ccount() +
                                       US_TO_HW_TIMER_TICKS(channel->remainder));
        channel->remainder = 0;
    }
    else {
        channel->remainder = 0;
        xthal_set_ccompare(timer->dev, xthal_get_ccount() + __hw_timer_ticks_min);
    }

    xt_ints_on(BIT(timers_int[timer->dev]));
}

static void IRAM __timer_channel_stop (struct hw_timer_t* timer, struct hw_channel_t* channel)
{
    if (!channel->used) {
        return;
    }

    xt_ints_off(BIT(timers_int[timer->dev]));

    /* compute elapsed time */
    uint32_t elapsed_time = timer_read (0) - channel->start_time;

    if (channel->delta_time > elapsed_time)  {
        /* compute new delta time if the timer has no been expired */
        channel->delta_time -= elapsed_time;
    }
    else {
        /* otherwise deactivate the channel */
        channel->used = false;
    }
}
#endif /* MODULE_ESP_HW_COUNTER */
