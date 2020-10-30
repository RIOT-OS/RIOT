/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp8266
 * @ingroup     drivers_periph_timer
 * @{
 *
 * @file
 * @brief       Low-level timer driver implementation using ESP8266 SDK
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#include <inttypes.h>

/* WARNING! enable debugging will have timing side effects and can lead
 * to timer underflows, system crashes or system dead locks in worst case. */
#define ENABLE_DEBUG 0
#include "debug.h"
#include "log.h"

#include "xtimer.h"
#include "periph/timer.h"

#include "esp/common_macros.h"
#include "esp_common.h"
#include "irq_arch.h"
#include "rom/ets_sys.h"
#include "sdk/sdk.h"
#include "xtensa/hal.h"

#if !defined(MODULE_ESP_SW_TIMER)

/* hardware timer used */

#define HW_TIMER_NUMOF        1
#define HW_TIMER_CHANNELS     1

#define HW_TIMER_MASK         0xffffffff
#define HW_TIMER_DELTA_MAX    0x00ffffff    /* in us */
#define HW_TIMER_DELTA_MIN    0x00000001    /* in us */
#define HW_TIMER_DELTA_MASK   0x00ffffff
#define HW_TIMER_DELTA_RSHIFT 24
#define HW_TIMER_CORRECTION   2             /* overhead in us */

#define HW_TIMER_CLOCK             (APB_CLK_FREQ)

#define US_TO_HW_TIMER_TICKS(t)    (t * system_get_cpu_freq())
#define HW_TIMER_TICKS_TO_US(t)    (t / system_get_cpu_freq())

struct hw_channel_t
{
    bool        used;         /* indicates whether the channel is used */
    uint32_t    start_time;   /* physical time when the timer channel has been started */
    uint32_t    delta_time;   /* timer delta value (delta = cycles * timer_max + remainder) */
    uint32_t    cycles;       /* number of complete max timer cycles */
    uint32_t    remainder;    /* remainder timer value */
};

struct hw_timer_t
{
    bool                 initialized; /* indicates whether timer is already initialized */
    bool                 started;     /* indicates whether timer is already started */
    timer_isr_ctx_t      isr_ctx;
    struct hw_channel_t  channels[HW_TIMER_CHANNELS];
};

static struct hw_timer_t timers[HW_TIMER_NUMOF] = { };

static void __timer_channel_start (struct hw_timer_t* timer, struct hw_channel_t* channel);
static void __timer_channel_stop (struct hw_timer_t* timer, struct hw_channel_t* channel);

static uint32_t __hw_timer_ticks_max;
static uint32_t __hw_timer_ticks_min;

void IRAM hw_timer_handler(void* arg)
{
    uint32_t dev = (uint32_t)arg >> 4;
    uint32_t chn = (uint32_t)arg & 0xf;

    if (dev >= HW_TIMER_NUMOF && chn >= HW_TIMER_CHANNELS) {
        return;
    }

    irq_isr_enter();

    DEBUG("%s arg=%p\n", __func__, arg);

    struct hw_timer_t*   timer   = &timers[dev];
    struct hw_channel_t* channel = &timer->channels[chn];

    if (channel->cycles) {
        channel->cycles--;
        xthal_set_ccompare(0, __hw_timer_ticks_max + xthal_get_ccount());
    }
    else if (channel->remainder >= HW_TIMER_DELTA_MIN) {
        xthal_set_ccompare (0, US_TO_HW_TIMER_TICKS(channel->remainder) + xthal_get_ccount());
        channel->remainder = 0;
    }
    else {
        channel->remainder = 0;
        channel->used = false;
        ets_isr_mask (BIT(ETS_CCOM_INUM));
        xthal_set_ccompare (0, 0);
        timer->isr_ctx.cb(timer->isr_ctx.arg, chn);
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

    timers[dev].initialized = true;
    timers[dev].started     = false;
    timers[dev].isr_ctx.cb  = cb;
    timers[dev].isr_ctx.arg = arg;

    ets_isr_attach (ETS_CCOM_INUM, hw_timer_handler, NULL);

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
    channel->used       = true;

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

    return phy_get_mactime ();
}

void IRAM timer_start(tim_t dev)
{
    DEBUG("%s dev=%u @%u\n", __func__, dev, phy_get_mactime());

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
          __func__, channel->cycles, channel->remainder, phy_get_mactime());

    /* start timer either with full cycles, remaining or minimum time */
    if (channel->cycles) {
        channel->cycles--;
        xthal_set_ccompare(0, __hw_timer_ticks_max + xthal_get_ccount());
    }
    else if (channel->remainder >= HW_TIMER_DELTA_MIN) {
        xthal_set_ccompare (0, US_TO_HW_TIMER_TICKS(channel->remainder) + xthal_get_ccount());
        channel->remainder = 0;
    }
    else {
        channel->remainder = 0;
        xthal_set_ccompare(0, __hw_timer_ticks_min + xthal_get_ccount());
    }

    ets_isr_unmask (BIT(ETS_CCOM_INUM));
}

static void IRAM __timer_channel_stop (struct hw_timer_t* timer, struct hw_channel_t* channel)
{
    if (!channel->used) {
        return;
    }

    ets_isr_mask (BIT(ETS_CCOM_INUM));

    /* compute elapsed time */
    uint32_t elapsed_time = timer_read (0) - channel->start_time;

    if (channel->delta_time > elapsed_time) {
        /* compute new delta time if the timer has no been expired */
        channel->delta_time -= elapsed_time;
    }
    else {
        /* otherwise deactivate the channel */
        channel->used = false;
    }
}

void timer_print_config(void)
{
    for (unsigned i = 0; i < HW_TIMER_NUMOF; i++) {
        printf("\tTIMER_DEV(%u)\t%d channel(s)\n", i,
               ARRAY_SIZE(timers[i].channels));
    }
}

#else /* MODULE_ESP_SW_TIMER */

/* software timer based on os_timer_arm functions */

#define OS_TIMER_NUMOF        1
#define OS_TIMER_CHANNELS     10

#define OS_TIMER_MASK         0xffffffff
#define OS_TIMER_DELTA_MAX    0x0000ffff
#define OS_TIMER_DELTA_MIN    0x00000064
#define OS_TIMER_DELTA_MASK   0x0000ffff
#define OS_TIMER_DELTA_RSHIFT 16
#define OS_TIMER_CORRECTION   4

extern void os_timer_arm_us(os_timer_t *ptimer, uint32_t time, bool repeat_flag);

/* Since hardware timer FRC1 is needed to implement PWM, we have to map our */
/* timer using the exsting ETS timer with 1 us clock rate */

struct phy_channel_t
{
    bool        used;         /* indicates whether the channel is used */
    uint32_t    start_time;   /* physical time when the timer channel has been started */
    uint32_t    delta_time;   /* timer delta value (delta = cycles * timer_max + remainder) */
    uint32_t    cycles;       /* number of complete max timer cycles */
    uint32_t    remainder;    /* remainder timer value */
    os_timer_t  os_timer;     /* used system software timer */
};

struct phy_timer_t
{
    bool                 initialized; /* indicates whether timer is already initialized */
    bool                 started;     /* indicates whether timer is already started */
    timer_isr_ctx_t      isr_ctx;
    struct phy_channel_t channels[OS_TIMER_CHANNELS];
};

static struct phy_timer_t timers[OS_TIMER_NUMOF] = { };

static void __timer_channel_start (struct phy_timer_t* timer, struct phy_channel_t* channel);
static void __timer_channel_stop (struct phy_timer_t* timer, struct phy_channel_t* channel);

/* Since we use ETS software timers, it is not really an ISR. Therefore */
/* we don't need to run in interrupt context. */

void IRAM os_timer_handler (void* arg)
{
    uint32_t dev = (uint32_t)arg >> 4;
    uint32_t chn = (uint32_t)arg & 0xf;

    if (dev >= OS_TIMER_NUMOF && chn >= OS_TIMER_CHANNELS) {
        return;
    }

    irq_isr_enter ();

    struct phy_timer_t*   timer   = &timers[dev];
    struct phy_channel_t* channel = &timer->channels[chn];

    if (channel->cycles) {
        channel->cycles--;
        os_timer_arm_us (&channel->os_timer, OS_TIMER_DELTA_MAX, false);
    }
    else if (channel->remainder >= OS_TIMER_DELTA_MIN) {
        os_timer_arm_us (&channel->os_timer, channel->remainder, false);
        channel->remainder = 0;
    }
    else {
        channel->remainder = 0;
        channel->used = false;
        timer->isr_ctx.cb(timer->isr_ctx.arg, chn);
    }

    irq_isr_exit ();
}

int timer_init (tim_t dev, uint32_t freq, timer_cb_t cb, void *arg)
{
    DEBUG("%s dev=%u freq=%lu cb=%p arg=%p\n", __func__, dev, freq, cb, arg);

    CHECK_PARAM_RET (dev  <  OS_TIMER_NUMOF, -1);
    CHECK_PARAM_RET (freq == XTIMER_HZ_BASE, -1);
    CHECK_PARAM_RET (cb   != NULL, -1);

    if (timers[dev].initialized) {
        DEBUG("%s timer dev=%u is already initialized (used)\n", __func__, dev);
        return -1;
    }

    timers[dev].initialized = true;
    timers[dev].started     = false;
    timers[dev].isr_ctx.cb  = cb;
    timers[dev].isr_ctx.arg = arg;

    for (int i = 0; i < OS_TIMER_CHANNELS; i++) {
        os_timer_setfn(&timers[dev].channels[i].os_timer,
                       os_timer_handler, (void*)((dev << 4) | i));
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

    CHECK_PARAM_RET (dev < OS_TIMER_NUMOF, -1);
    CHECK_PARAM_RET (chn < OS_TIMER_CHANNELS, -1);

    int state = irq_disable ();

    struct phy_timer_t*   timer   = &timers[dev];
    struct phy_channel_t* channel = &timer->channels[chn];

    /* set delta time and channel used flag */
    channel->delta_time = delta > OS_TIMER_CORRECTION ? delta - OS_TIMER_CORRECTION : 0;
    channel->used       = true;

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

    CHECK_PARAM_RET (dev < OS_TIMER_NUMOF, -1);
    CHECK_PARAM_RET (chn < OS_TIMER_CHANNELS, -1);

    int state = irq_disable ();

    /* stop running timer channel */
    __timer_channel_stop (&timers[dev], &timers[dev].channels[chn]);

    irq_restore (state);

    return 0;
}

unsigned int IRAM timer_read(tim_t dev)
{
    (void)dev;

    return phy_get_mactime ();
}

void IRAM timer_start(tim_t dev)
{
    DEBUG("%s dev=%u\n", __func__, dev);

    CHECK_PARAM (dev < OS_TIMER_NUMOF);
    CHECK_PARAM (!timers[dev].started);

    int state = irq_disable ();

    struct phy_timer_t* timer = &timers[dev];

    timer->started = true;

    for (int i = 0; i < OS_TIMER_CHANNELS; i++) {
         __timer_channel_start (timer, &timer->channels[i]);
    }

    irq_restore (state);
}

void IRAM timer_stop(tim_t dev)
{
    DEBUG("%s dev=%u\n", __func__, dev);

    CHECK_PARAM (dev < OS_TIMER_NUMOF);

    int state = irq_disable ();

    struct phy_timer_t* timer = &timers[dev];

    timer->started = false;

    for (int i = 0; i < OS_TIMER_CHANNELS; i++) {
        __timer_channel_stop (timer, &timer->channels[i]);
    }

    irq_restore (state);
}


static void IRAM __timer_channel_start (struct phy_timer_t* timer, struct phy_channel_t* channel)
{
    if (!timer->started || !channel->used) {
        return;
    }

    /* disarm old timer if already started */
    os_timer_disarm (&channel->os_timer);

    /* save channel starting time */
    channel->start_time = timer_read (0);
    channel->cycles     = channel->delta_time >> OS_TIMER_DELTA_RSHIFT;
    channel->remainder  = channel->delta_time &  OS_TIMER_DELTA_MASK;

    DEBUG("%s cycles=%u remainder=%u @%u\n",
          __func__, channel->cycles, channel->remainder, phy_get_mactime());

    /* start timer either with full cycles, remainder or minimum time */
    if (channel->cycles) {
        channel->cycles--;
        os_timer_arm_us (&channel->os_timer, OS_TIMER_DELTA_MAX, false);
    }
    else if (channel->remainder > OS_TIMER_DELTA_MIN) {
        os_timer_arm_us (&channel->os_timer, channel->remainder, false);
        channel->remainder = 0;
    }
    else {
        channel->remainder = 0;
        os_timer_arm_us (&channel->os_timer, OS_TIMER_DELTA_MIN, false);
    }
}

static void IRAM __timer_channel_stop (struct phy_timer_t* timer, struct phy_channel_t* channel)
{
    if (!channel->used) {
        return;
    }

    os_timer_disarm (&channel->os_timer);

    /* compute elapsed time */
    uint32_t elapsed_time = timer_read (0) - channel->start_time;

    if (channel->delta_time > elapsed_time) {
        /* compute new delta time if the timer has no been expired */
        channel->delta_time -= elapsed_time;
    }
    else {
        /* otherwise deactivate the channel */
        channel->used = false;
    }
}

void timer_print_config(void)
{
    for (unsigned i = 0; i < OS_TIMER_NUMOF; i++) {
        printf("\tTIMER_DEV(%u)\t%d channel(s)\n", i,
               ARRAY_SIZE(timers[i].channels));
    }
}

#endif /* MODULE_ESP_SW_TIMER */
