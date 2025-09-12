/*
 * SPDX-FileCopyrightText: 2018 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_esp32
 * @{
 *
 * @file
 * @brief       ETS timer to ztimer mapper
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#define ENABLE_DEBUG 0
#include "debug.h"

#include <assert.h>
#include <string.h>

#include "esp_common.h"
#include "log.h"

#include "esp_attr.h"
#include "irq_arch.h"
#include "ztimer.h"
#include "timex.h"

#include "rom/ets_sys.h"

struct _ets_to_ztimer {
    ETSTimer   *ets_timer;
    ztimer_t   ztimer;
};

/* maximum number of ETS timer to ztimer mapper objects */
/* TODO tune the value */
#define ETS_TO_TIMER_NUM 40

/* table of ETS timer to ztimer mapper objects */
static struct _ets_to_ztimer _ets_to_ztimer_map[ETS_TO_TIMER_NUM] = {};

/**
 * @brief   Get the ETS timer to ztimer mapper object for the given timer.
 *
 * If there is no object, the function registers a new one and returns it.
 * If there is no more object available, it returns NULL.
 *
 * @param   pointer to the ETS timer
 * @return  pointer to the mapper object or NULL in case of error
 */
struct _ets_to_ztimer* _ets_to_ztimer_get(ETSTimer *timer)
{
    /* search for an existing mapper object */
    for (int i = 0; i < ETS_TO_TIMER_NUM; i++) {
        if (_ets_to_ztimer_map[i].ets_timer == timer) {
            return &_ets_to_ztimer_map[i];
        }
    }
    /* search for a free mapper object */
    for (int i = 0; i < ETS_TO_TIMER_NUM; i++) {
        if (_ets_to_ztimer_map[i].ets_timer == NULL) {
            _ets_to_ztimer_map[i].ets_timer = timer;
            return &_ets_to_ztimer_map[i];
        }
    }
    LOG_TAG_ERROR("esp_ztimer", "There is no free ETS timer to ztimer mapper "
                  "object available\n");
    return NULL;
}

/**
 * @brief   Free the ETS timer to ztimer mapper object for the given timer.
 * @param   pointer to the ETS timer
 */
void _ets_to_ztimer_free(ETSTimer *timer)
{
    /* search for an existing mapper object */
    for (int i = 0; i < ETS_TO_TIMER_NUM; i++) {
        if (_ets_to_ztimer_map[i].ets_timer == timer) {
            _ets_to_ztimer_map[i].ets_timer = NULL;
            return;
        }
    }
    DEBUG("%s There is no ETS timer to ztimer for ETS timer %p\n",
          __func__, timer);
}

/* ztimer call back function, distributes ets_timer callbacks */
void IRAM_ATTR _ets_to_ztimer_callback (void *arg)
{
    struct _ets_to_ztimer* e2xt = (struct _ets_to_ztimer*)arg;

    assert(arg != NULL);
    assert(e2xt->ets_timer != NULL);

    irq_isr_enter();

    /* if timer is periodic, start it again with period */
    if (e2xt->ets_timer->timer_period) {
        ets_timer_arm_us(e2xt->ets_timer, e2xt->ets_timer->timer_period, true);
    }
    /* execute the ets_timer callback function */
    e2xt->ets_timer->timer_func(e2xt->ets_timer->timer_arg);

    irq_isr_exit();
}

void ets_timer_setfn(ETSTimer *ptimer, ETSTimerFunc *pfunc, void *parg)
{
    DEBUG("%s timer=%p pfunc=%p parg=%p\n", __func__, ptimer, pfunc, parg);

    struct _ets_to_ztimer* e2xt = _ets_to_ztimer_get(ptimer);

    assert(e2xt != NULL);

    e2xt->ets_timer->timer_func = pfunc;
    e2xt->ets_timer->timer_arg  = parg;

    e2xt->ztimer.callback = &_ets_to_ztimer_callback;
    e2xt->ztimer.arg = (void*)e2xt;
}

void ets_timer_done(ETSTimer *ptimer)
{
    DEBUG("%s timer=%p\n", __func__, ptimer);

    struct _ets_to_ztimer* e2xt = _ets_to_ztimer_get(ptimer);

    assert(e2xt != NULL);

    e2xt->ets_timer->timer_func = NULL;
    e2xt->ets_timer->timer_arg  = NULL;
}

void ets_timer_arm_us(ETSTimer *timer, uint32_t tmout, bool repeat)
{
    DEBUG("%s timer=%p tmout=%"PRIu32" repeat=%d\n", __func__, timer, tmout, repeat);

    struct _ets_to_ztimer* e2xt = _ets_to_ztimer_get(timer);

    assert(e2xt != NULL);
    assert(e2xt->ztimer.callback != NULL);

    tmout = (tmout + 500) / 1000;
    uint32_t now = ztimer_set(ZTIMER_MSEC, &e2xt->ztimer, tmout);

    /* Note: this is approximating the expiry time since for very short timeout
             this might be set to a minimum value that guarantees the ISR
             being executed */
    e2xt->ets_timer->timer_expire = (now + tmout) * 1000;
    e2xt->ets_timer->timer_period = repeat ? (tmout * 1000) : 0;
}

void ets_timer_arm(ETSTimer *timer, uint32_t tmout, bool repeat)
{
    ets_timer_arm_us(timer, tmout * US_PER_MS, repeat);
}

void ets_timer_disarm(ETSTimer *timer)
{
    DEBUG("%s timer=%p\n", __func__, timer);

    struct _ets_to_ztimer* e2xt = _ets_to_ztimer_get(timer);

    assert(e2xt != NULL);

    ztimer_remove(ZTIMER_MSEC, &e2xt->ztimer);
}

void ets_timer_init(void)
{
    /* initialization is not necessary */
}

void ets_timer_deinit(void)
{
    /* deinitialization is not necessary */
}

void os_timer_setfn(ETSTimer *ptimer, ETSTimerFunc *pfunction, void *parg)
                    __attribute__((alias("ets_timer_setfn")));
void os_timer_disarm(ETSTimer *ptimer)
                    __attribute__((alias("ets_timer_disarm")));
void os_timer_arm_us(ETSTimer *ptimer, uint32_t u_seconds, bool repeat_flag)
                    __attribute__((alias("ets_timer_arm_us")));
void os_timer_arm(ETSTimer *ptimer, uint32_t milliseconds, bool repeat_flag)
                  __attribute__((alias("ets_timer_arm")));
void os_timer_done(ETSTimer *ptimer)
                   __attribute__((alias("ets_timer_done")));
