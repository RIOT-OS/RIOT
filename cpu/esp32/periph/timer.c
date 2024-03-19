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
#include "periph/timer.h"

#include "driver/periph_ctrl.h"
#include "esp/common_macros.h"
#include "hal/interrupt_controller_types.h"
#include "hal/interrupt_controller_ll.h"
#include "hal/timer_hal.h"
#include "rom/ets_sys.h"
#include "soc/periph_defs.h"
#include "soc/rtc.h"
#include "soc/timer_group_struct.h"

#if __xtensa__
#include "xtensa/hal.h"
#include "xtensa/xtensa_api.h"
#endif

#include "esp_common.h"
#include "irq_arch.h"
#include "syscalls.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define RTC_PLL_480M    480 /* PLL with 480 MHz at maximum */
#define RTC_PLL_320M    320 /* PLL with 320 MHz at maximum */

#ifndef MODULE_ESP_HW_COUNTER

/* hardware timer modules used */

/**
 * ESP32 has four 64 bit hardware timers:
 * two timer groups TMG0 and TMG1 with 2 timers each
 *
 * TMG0, timer 0 is used for system time in us and is therefore not
 * available as low level timer. Timers have only one channel. Timer devices
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
 * ---
 * ESP32-C3 hast only two 54 bit hardware timers:
 * two timer groups TMG0 and TMG1 with 1 timer each
 *
 * TMG0, timer 0 is used for system time in us and is therefore not
 * available as low level timer. Timers have only one channel. Timer devices
 * are mapped to hardware timer as following:
 *
 *     0 -> TMG1 timer 0
 *
 * PLEASE NOTE: Don't use ETS timer functions ets_timer_* in and this hardware
 * timer implementation together!
 */

#if defined(CPU_FAM_ESP32)

#define HW_TIMER_CORRECTION   (RTC_PLL_320M / CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ)
#define HW_TIMER_DELTA_MIN    (MAX(HW_TIMER_CORRECTION << 1, 5))

#elif defined(CPU_FAM_ESP32C3)

#define HW_TIMER_CORRECTION   10
#define HW_TIMER_DELTA_MIN    (MAX(HW_TIMER_CORRECTION << 1, 5))

#elif defined(CPU_FAM_ESP32S2)

#define HW_TIMER_CORRECTION   (RTC_PLL_320M / CONFIG_ESP32S2_DEFAULT_CPU_FREQ_MHZ)
#define HW_TIMER_DELTA_MIN    (MAX(HW_TIMER_CORRECTION << 1, 5))

#elif defined(CPU_FAM_ESP32S3)

#define HW_TIMER_CORRECTION   (RTC_PLL_320M / CONFIG_ESP32S3_DEFAULT_CPU_FREQ_MHZ)
#define HW_TIMER_DELTA_MIN    (MAX(HW_TIMER_CORRECTION << 1, 5))

#else
#error "MCU implementation needed"
#endif

#define HW_TIMER_NUMOF      ARRAY_SIZE(_timers_desc)
#define HW_TIMER_CHANNELS   1

struct _hw_timer_t {
    bool  initialized; /* indicates whether timer is already initialized */
    bool  started;     /* indicates whether timer is already started */

    timer_isr_ctx_t isr_ctx;    /* registered ISR */
    timer_hal_context_t hw;     /* timer hardware reference */
};

struct _hw_timer_desc_t {
    uint8_t module;         /* hardware module identifier */
    timer_group_t group;    /* timer group identifier */
    timer_idx_t index;      /* timer index in timer group */
    uint8_t int_mask;       /* timer interrupt bit mask in interrupt regs */
    uint8_t int_src;        /* timer interrupt source */
};

static const struct _hw_timer_desc_t _timers_desc[] =
{
#if defined(CPU_FAM_ESP32) || defined(CPU_FAM_ESP32S2) || defined(CPU_FAM_ESP32S3)
    {
        .module = PERIPH_TIMG0_MODULE,
        .group = TIMER_GROUP_0,
        .index = TIMER_1,
        .int_mask = BIT(TIMER_1),
        .int_src  = ETS_TG0_T1_LEVEL_INTR_SOURCE,
    },
    {
        .module = PERIPH_TIMG1_MODULE,
        .group = TIMER_GROUP_1,
        .index = TIMER_0,
        .int_mask = BIT(TIMER_0),
        .int_src  = ETS_TG1_T0_LEVEL_INTR_SOURCE,
    },
    {
        .module = PERIPH_TIMG1_MODULE,
        .group = TIMER_GROUP_1,
        .index = TIMER_1,
        .int_mask = BIT(TIMER_1),
        .int_src  = ETS_TG1_T1_LEVEL_INTR_SOURCE,
    }
#elif defined(CPU_FAM_ESP32C3)
    {
        .module = PERIPH_TIMG1_MODULE,
        .group = TIMER_GROUP_1,
        .index = TIMER_0,
        .int_mask = BIT(TIMER_0),
        .int_src  = ETS_TG1_T0_LEVEL_INTR_SOURCE
    },
#else
#error "MCU implementation needed"
#endif
};

static struct _hw_timer_t _timers[HW_TIMER_NUMOF] = { };

/** Latches the current counter value and return only the low part */
static inline uint32_t _timer_get_counter_lo(tim_t dev)
{
    /* latch the current timer value and get current timer value */
    uint64_t value;
    timer_hal_get_counter_value(&_timers[dev].hw, &value);

    /* return high and low part of timer */
    return value;
}

void IRAM_ATTR _timer_int_handler(void* arg)
{
    (void)arg;

    /* since all timer interrupt sources are routed to the same cpu interrupt */
    /* signal, we can't use arg to identify the timer which caused the it */

    irq_isr_enter();

    for (unsigned dev = 0; dev < HW_TIMER_NUMOF; dev++) {
        /* iterate over all devices and check what interrupt flags are set */

        if (!_timers[dev].initialized) {
            continue;
        }
        uint32_t int_status;
        timer_hal_get_intr_status(&_timers[dev].hw, &int_status);

        if (int_status & _timers_desc[dev].int_mask) {
            DEBUG("%s dev=%d\n", __func__, dev);

            /* disable alarms */
            timer_hal_set_alarm_enable(&_timers[dev].hw, false);

            /* disable interrupt source and clear the bit in interrupt status */
            timer_hal_set_level_int_enable(&_timers[dev].hw, false);
            timer_hal_intr_disable(&_timers[dev].hw);
            timer_hal_clear_intr_status(&_timers[dev].hw);

            /* execute the callback function */
            _timers[dev].isr_ctx.cb(_timers[dev].isr_ctx.arg, 0);
        }
    }

    irq_isr_exit();
}

int timer_init(tim_t dev, uint32_t freq, timer_cb_t cb, void *arg)
{
    _Static_assert(HW_TIMER_NUMOF == TIMER_NUMOF,
                   "Number of timer descriptors does not match with TIMER_NUMOF");

    DEBUG("%s dev=%u freq=%" PRIu32 " cb=%p arg=%p\n",
          __func__, dev, freq, cb, arg);

    uint32_t clk_div = rtc_clk_apb_freq_get() / freq;

    assert(dev <  HW_TIMER_NUMOF);
    assert(clk_div >= 2 && clk_div <= 65536);
    assert(cb != NULL);

    /* initialize timer data structure */
    _timers[dev].initialized = true;
    _timers[dev].started     = false;
    _timers[dev].isr_ctx.cb  = cb;
    _timers[dev].isr_ctx.arg = arg;

    /* route all timer interrupt sources to the same level type interrupt */
    intr_matrix_set(PRO_CPU_NUM, _timers_desc[dev].int_src, CPU_INUM_TIMER);

    /* we have to enable therefore the interrupt here */
    intr_cntrl_ll_set_int_handler(CPU_INUM_TIMER, _timer_int_handler, NULL);
    intr_cntrl_ll_enable_interrupts(BIT(CPU_INUM_TIMER));

    /* enable TMG module */
    periph_module_enable(_timers_desc[dev].module);

    /* hardware timer configuration */
    timer_hal_init(&_timers[dev].hw, _timers_desc[dev].group, _timers_desc[dev].index);

    timer_hal_set_counter_enable(&_timers[dev].hw, false);
    timer_hal_set_counter_increase(&_timers[dev].hw, true);
    timer_hal_set_divider(&_timers[dev].hw, clk_div);
    timer_hal_set_auto_reload(&_timers[dev].hw, false);

    /* disable alarm and interrupt source */
    timer_hal_set_alarm_enable(&_timers[dev].hw, false);
    timer_hal_intr_disable(&_timers[dev].hw);
    timer_hal_set_level_int_enable(&_timers[dev].hw, false);
    timer_hal_set_edge_int_enable(&_timers[dev].hw, false);

    /* start the timer */
    timer_start(dev);

    return 0;
}

int IRAM_ATTR timer_set(tim_t dev, int chn, unsigned int delta)
{
    DEBUG("%s dev=%u channel=%d delta=%u\n", __func__, dev, chn, delta);

    assert(dev < HW_TIMER_NUMOF);

    if (chn >= HW_TIMER_CHANNELS) {
        return -1;
    }

    /* disable interrupts */
    int state = irq_disable ();

    /* disable alarms and interrupt source */
    timer_hal_set_alarm_enable(&_timers[dev].hw, false);
    timer_hal_set_level_int_enable(&_timers[dev].hw, false);
    timer_hal_intr_disable(&_timers[dev].hw);

    delta = (delta > HW_TIMER_DELTA_MIN) ? delta : HW_TIMER_DELTA_MIN;
    delta = (delta > HW_TIMER_CORRECTION) ? delta - HW_TIMER_CORRECTION : HW_TIMER_CORRECTION;

     /* latch and read current timer value */
    uint64_t alarm;
    timer_hal_get_counter_value(&_timers[dev].hw, &alarm);

    /* determine the alarm time and set the alarm */
    alarm += delta;
    timer_hal_set_alarm_value(&_timers[dev].hw, alarm);

    /* enable alarms and interrupt sources */
    timer_hal_set_alarm_enable(&_timers[dev].hw, true);

    /* clear possible pending interrupts and enable interrupt source */
    timer_hal_set_level_int_enable(&_timers[dev].hw, true);
    timer_hal_intr_enable(&_timers[dev].hw);
    timer_hal_clear_intr_status(&_timers[dev].hw);

    /* enable the counter */
    timer_hal_set_counter_enable(&_timers[dev].hw, true);

    /* restore interrupts enabled state */
    irq_restore (state);

    return 0;
}

int IRAM_ATTR timer_set_absolute(tim_t dev, int chn, unsigned int value)
{
    DEBUG("%s dev=%u channel=%d value=%u\n", __func__, dev, chn, value);

    return timer_set(dev, chn, value - timer_read(dev));
}

int timer_clear(tim_t dev, int chn)
{
    DEBUG("%s dev=%u channel=%d\n", __func__, dev, chn);

    assert(dev < HW_TIMER_NUMOF);

    if (chn >= HW_TIMER_CHANNELS) {
        return -1;
    }

    /* disable alarms */
    timer_hal_set_alarm_enable(&_timers[dev].hw, false);

    /* disable interrupt source and clear possible pending interrupts */
    timer_hal_set_level_int_enable(&_timers[dev].hw, false);
    timer_hal_intr_disable(&_timers[dev].hw);
    timer_hal_clear_intr_status(&_timers[dev].hw);

    return 0;
}

unsigned int IRAM_ATTR timer_read(tim_t dev)
{
    assert(dev < HW_TIMER_NUMOF);

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        uint32_t count_lo = _timer_get_counter_lo(dev);
        DEBUG("%s %" PRIu32 "\n", __func__, count_lo);
        return count_lo;
    }
    else {
        return _timer_get_counter_lo(dev);
    }
}

void IRAM_ATTR timer_start(tim_t dev)
{
    DEBUG("%s dev=%u @%" PRIu32 "\n", __func__, dev, system_get_time());
    assert(dev < HW_TIMER_NUMOF);
    timer_hal_set_counter_enable(&_timers[dev].hw, true);
}

void IRAM_ATTR timer_stop(tim_t dev)
{
    DEBUG("%s dev=%u\n", __func__, dev);
    assert(dev < HW_TIMER_NUMOF);
    timer_hal_set_counter_enable(&_timers[dev].hw, false);
}

#else /* MODULE_ESP_HW_COUNTER */

#include "xtensa/config/core-isa.h"

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
#define HW_TIMER_FREQUENCY    (1000000UL) /* only 1MHz is supported */

#if defined(CPU_FAM_ESP32)

#define HW_TIMER_CORRECTION   (RTC_PLL_480M / CONFIG_ESP32_DEFAULT_CPU_FREQ_MHZ)
#define HW_TIMER_DELTA_MIN    (MAX(HW_TIMER_CORRECTION, 5))

#elif defined(CPU_FAM_ESP32S2)

#define HW_TIMER_CORRECTION   (RTC_PLL_480M / CONFIG_ESP32S2_DEFAULT_CPU_FREQ_MHZ)
#define HW_TIMER_DELTA_MIN    (MAX(HW_TIMER_CORRECTION, 5))

#elif defined(CPU_FAM_ESP32S3)

#define HW_TIMER_CORRECTION   (RTC_PLL_480M / CONFIG_ESP32S3_DEFAULT_CPU_FREQ_MHZ)
#define HW_TIMER_DELTA_MIN    (MAX(HW_TIMER_CORRECTION, 5))

#else
#error "MCU implementation needed"
#endif

#define US_TO_HW_TIMER_TICKS(t)     (t * system_get_cpu_freq())
#define HW_TIMER_TICKS_TO_US(t)     (t / system_get_cpu_freq())

extern int esp_clk_cpu_freq(void);

struct hw_channel_t {
    bool        used;         /* indicates whether the channel is used */
    uint32_t    start_time;   /* physical time when the timer channel has been started */
    uint32_t    delta_time;   /* timer delta value (delta = cycles * timer_max + remainder) */
    uint32_t    cycles;       /* number of complete max timer cycles */
    uint32_t    remainder;    /* remainder timer value */
};

struct _hw_timer_t {
    tim_t                dev;         /* the timer device num */
    bool                 initialized; /* indicates whether timer is already initialized */
    bool                 started;     /* indicates whether timer is already started */
    timer_isr_ctx_t      isr_ctx;
    struct hw_channel_t  channels[HW_TIMER_CHANNELS];
};

static struct _hw_timer_t _timers[HW_TIMER_NUMOF] = { };
static const uint8_t timers_int[HW_TIMER_NUMOF] = { XCHAL_TIMER0_INTERRUPT,
                                                    XCHAL_TIMER1_INTERRUPT,
                                                    XCHAL_TIMER2_INTERRUPT };

static void __timer_channel_start (struct _hw_timer_t* timer, struct hw_channel_t* channel);
static void __timer_channel_stop (struct _hw_timer_t* timer, struct hw_channel_t* channel);

static uint32_t ___hw_timer_ticks_max;
static uint32_t ___hw_timer_ticks_min;

void IRAM hw_timer_handler(void* arg)
{
    uint32_t dev = (uint32_t)arg;
    uint32_t chn = 0;

    if (dev >= HW_TIMER_NUMOF && chn >= HW_TIMER_CHANNELS) {
        return;
    }

    irq_isr_enter();

    DEBUG("%s arg=%p\n", __func__, arg);

    struct _hw_timer_t*   timer   = &_timers[dev];
    struct hw_channel_t* channel = &timer->channels[chn];

    if (channel->cycles) {
        channel->cycles--;
        xthal_set_ccompare(dev, xthal_get_ccount() + ___hw_timer_ticks_max);
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
    DEBUG("%s dev=%u freq=%"PRIu32" cb=%p arg=%p\n", __func__, dev, freq, cb, arg);

    assert(dev  <  HW_TIMER_NUMOF);
    assert(freq == HW_TIMER_FREQUENCY);
    assert(cb   != NULL);

    if (_timers[dev].initialized) {
        DEBUG("%s timer dev=%u is already initialized (used)\n", __func__, dev);
        return -1;
    }

    _timers[dev].dev = dev;
    _timers[dev].initialized = true;
    _timers[dev].started     = false;
    _timers[dev].isr_ctx.cb  = cb;
    _timers[dev].isr_ctx.arg = arg;

    xt_set_interrupt_handler(timers_int[dev], hw_timer_handler, (void *)dev);

    for (int i = 0; i < HW_TIMER_CHANNELS; i++) {
        _timers[dev].channels[i].used = false;
        _timers[dev].channels[i].cycles = 0;
        _timers[dev].channels[i].remainder = 0;
    }

    timer_start(dev);

    return 0;
}

int IRAM timer_set(tim_t dev, int chn, unsigned int delta)
{
    DEBUG("%s dev=%u channel=%d delta=%u\n", __func__, dev, chn, delta);

    assert(dev < HW_TIMER_NUMOF);
    assert(chn < HW_TIMER_CHANNELS);

    int state = irq_disable ();

    struct _hw_timer_t*   timer   = &_timers[dev];
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

    assert(dev < HW_TIMER_NUMOF);
    assert(chn < HW_TIMER_CHANNELS);

    int state = irq_disable ();

    /* stop running timer channel */
    __timer_channel_stop (&_timers[dev], &_timers[dev].channels[chn]);

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
    DEBUG("%s dev=%u @%"PRIu32"\n", __func__, dev, system_get_time());

    assert(dev < HW_TIMER_NUMOF);
    assert(!_timers[dev].started);

    int state = irq_disable ();

    ___hw_timer_ticks_max = US_TO_HW_TIMER_TICKS(HW_TIMER_DELTA_MAX);
    ___hw_timer_ticks_min = US_TO_HW_TIMER_TICKS(HW_TIMER_DELTA_MIN);

    struct _hw_timer_t* timer = &_timers[dev];

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

    struct _hw_timer_t* timer = &_timers[dev];

    timer->started = false;

    for (int i = 0; i < HW_TIMER_CHANNELS; i++) {
        __timer_channel_stop (timer, &timer->channels[i]);
    }

    irq_restore (state);
}

static void IRAM __timer_channel_start (struct _hw_timer_t* timer, struct hw_channel_t* channel)
{
    if (!timer->started || !channel->used) {
        return;
    }

    /* save channel starting time */
    channel->start_time = timer_read (0);
    channel->cycles     = channel->delta_time >> HW_TIMER_DELTA_RSHIFT;
    channel->remainder  = channel->delta_time &  HW_TIMER_DELTA_MASK;

    DEBUG("%s cycles=%"PRIu32" remainder=%"PRIu32" @%"PRIu32"\n",
          __func__, channel->cycles, channel->remainder, system_get_time());

    /* start timer either with full cycles, remaining or minimum time */
    if (channel->cycles) {
        channel->cycles--;
        xthal_set_ccompare(timer->dev, xthal_get_ccount() + ___hw_timer_ticks_max);
    }
    else if (channel->remainder > HW_TIMER_DELTA_MIN) {
        xthal_set_ccompare(timer->dev, xthal_get_ccount() +
                                       US_TO_HW_TIMER_TICKS(channel->remainder));
        channel->remainder = 0;
    }
    else {
        channel->remainder = 0;
        xthal_set_ccompare(timer->dev, xthal_get_ccount() + ___hw_timer_ticks_min);
    }

    xt_ints_on(BIT(timers_int[timer->dev]));
}

static void IRAM __timer_channel_stop (struct _hw_timer_t* timer, struct hw_channel_t* channel)
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
