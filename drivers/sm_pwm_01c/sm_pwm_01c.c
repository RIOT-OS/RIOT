/*
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sm_pwm_01c
 * @{
 * @file
 * @brief       Implementation of SM_PWM_01C dust sensor
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#include <assert.h>
#include <string.h>

#include "log.h"
#include "ztimer.h"

#include "periph/gpio.h"

#include "sm_pwm_01c.h"
#include "sm_pwm_01c_params.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* Scaling value to get 1/100 of a % resolution for lpo values */
#define LPO_SCALING         (100)

/* Circular average for moving average calculation, this is always
   called in irq context */
#ifdef MODULE_SM_PWM_01C_MA
static void _circ_buf_push(circ_buf_t *buf, uint16_t data)
{
    buf->buf[buf->head] = data;
    buf->head = (buf->head + 1) % (SM_PWM_01C_BUFFER_LEN);
}

static uint16_t _circ_buf_avg(circ_buf_t *buf)
{
    uint32_t sum = 0;

    for (size_t i = 0; i < SM_PWM_01C_BUFFER_LEN; i++) {
        sum += buf->buf[i];
    }
    return (uint16_t)(sum / SM_PWM_01C_BUFFER_LEN);
}
#endif

/* Interval approximation of theoretical Dust Concentration / LPO % curve
   https://www.sgbotic.com/products/datasheets/sensors/app-SM-PWM-01C.pdf */
static uint16_t _lpo_to_dust_cons(uint16_t lpo)
{
    if (lpo <= (2 * LPO_SCALING)) {
        return (143 * lpo) / (2 * LPO_SCALING);
    }
    else if (lpo <= (4 * LPO_SCALING)) {
        return (208 * lpo + 130) / (3 * LPO_SCALING);
    }
    else if (lpo <= (15 * LPO_SCALING)) {
        return (1155 * lpo - 1572) / (10 * LPO_SCALING);
    }
    else {
        return (2354 * lpo - 19560) / (10 * LPO_SCALING);
    }
}

static void _sample_timer_cb(void *arg)
{
    sm_pwm_01c_t *dev = (sm_pwm_01c_t *)arg;

    /* schedule next sample */
    ztimer_set(ZTIMER_USEC, &dev->_sampler, CONFIG_SM_PWM_01C_SAMPLE_TIME);
    DEBUG("[sm_pwm_01c] tsp_lpo %" PRIu32 "\n", dev->_values.tsp_lpo);
    DEBUG("[sm_pwm_01c] tlp_lpo %" PRIu32 "\n", dev->_values.tlp_lpo);

    /* calculate low Pulse Output Occupancy in (% * LPO_SCALING),
       e.g. 1% -> 100 */
    uint16_t tsp_ratio =
        (uint16_t)((uint64_t)(100 * LPO_SCALING * dev->_values.tsp_lpo) /
                   CONFIG_SM_PWM_01C_SAMPLE_TIME);
    uint16_t tlp_ratio =
        (uint16_t)((uint64_t)(100 * LPO_SCALING * dev->_values.tlp_lpo) /
                   CONFIG_SM_PWM_01C_SAMPLE_TIME);
    DEBUG("[sm_pwm_01c] tsp_ratio %" PRIu16 "/%d %%\n", tsp_ratio, LPO_SCALING);
    DEBUG("[sm_pwm_01c] tlp_ratio %" PRIu16 "/%d %%\n", tlp_ratio, LPO_SCALING);

    /* convert lpo to particle concentration */
    uint16_t tsp = _lpo_to_dust_cons(tsp_ratio);
    uint16_t tlp = _lpo_to_dust_cons(tlp_ratio);
    DEBUG("[sm_pwm_01c] new sample tsp conc: %" PRIu16 " ug/m3\n", tsp);
    DEBUG("[sm_pwm_01c] new sample tlp conc: %" PRIu16 " ug/m3\n", tlp);

    /* update concentration values*/
#ifdef MODULE_SM_PWM_01C_MA
    _circ_buf_push(&dev->_values.tsp_circ_buf, tsp);
    _circ_buf_push(&dev->_values.tlp_circ_buf, tlp);
#else
    dev->_values.data.mc_pm_10 =
        (uint16_t)((tlp + (uint32_t)(CONFIG_SM_PWM_01C_EXP_WEIGHT - 1) *
                    dev->_values.data.mc_pm_10) / CONFIG_SM_PWM_01C_EXP_WEIGHT);
    dev->_values.data.mc_pm_2p5 =
        (uint16_t)((tsp + (uint32_t)(CONFIG_SM_PWM_01C_EXP_WEIGHT - 1) *
                    dev->_values.data.mc_pm_2p5) / CONFIG_SM_PWM_01C_EXP_WEIGHT);
#endif

    /* reset lpo */
    dev->_values.tlp_lpo = 0;
    dev->_values.tsp_lpo = 0;
}

static void _tsp_pin_cb(void *arg)
{
    sm_pwm_01c_t *dev = (sm_pwm_01c_t *)arg;
    uint32_t now = ztimer_now(ZTIMER_USEC);

    if (gpio_read(dev->params.tsp_pin) == 0) {
        dev->_values.tsp_start_time = now;
    }
    else {
        dev->_values.tsp_lpo += (now - dev->_values.tsp_start_time);
    }
}

static void _tlp_pin_cb(void *arg)
{
    sm_pwm_01c_t *dev = (sm_pwm_01c_t *)arg;
    uint32_t now = ztimer_now(ZTIMER_USEC);

    if (gpio_read(dev->params.tlp_pin) == 0) {
        dev->_values.tlp_start_time = now;
    }
    else {
        dev->_values.tlp_lpo += (now - dev->_values.tlp_start_time);
    }
}

int sm_pwm_01c_init(sm_pwm_01c_t *dev, const sm_pwm_01c_params_t *params)
{
    dev->params = *params;

    /* set up irq */
    if (gpio_init_int(dev->params.tsp_pin, GPIO_IN_PU, GPIO_BOTH, _tsp_pin_cb,
                      dev) < 0) {
        DEBUG("[sm_pwm_01c] init_int of tsp_pin failed [ERROR]\n");
        return -EIO;
    }
    if (gpio_init_int(dev->params.tlp_pin, GPIO_IN_PU, GPIO_BOTH, _tlp_pin_cb,
                      dev) < 0) {
        DEBUG("[sm_pwm_01c] init_int of tlp_pin failed [ERROR]\n");
        return -EIO;
    }

    /* setup timer */
    dev->_sampler.callback = _sample_timer_cb;
    dev->_sampler.arg = dev;

#ifdef MODULE_SM_PWM_01C_MA
    memset(&dev->_values.tsp_circ_buf, 0, sizeof(circ_buf_t));
    memset(&dev->_values.tlp_circ_buf, 0, sizeof(circ_buf_t));
#endif

    return 0;
}

void sm_pwm_01c_start(sm_pwm_01c_t *dev)
{
    assert(dev);
    /* reset old values */
    memset((void *)&dev->_values, 0, sizeof(sm_pwm_01c_values_t));
    /* enable irq and set timer */
    ztimer_set(ZTIMER_USEC, &dev->_sampler, CONFIG_SM_PWM_01C_SAMPLE_TIME);
    gpio_irq_enable(dev->params.tsp_pin);
    gpio_irq_enable(dev->params.tlp_pin);
    DEBUG("[sm_pwm_01c] started average measurements\n");
}

void sm_pwm_01c_stop(sm_pwm_01c_t *dev)
{
    assert(dev);
    /* disable irq and remove timer */
    ztimer_remove(ZTIMER_USEC, &dev->_sampler);
    gpio_irq_disable(dev->params.tsp_pin);
    gpio_irq_disable(dev->params.tlp_pin);
    DEBUG("[sm_pwm_01c] stopped average measurements\n");
}

void sm_pwm_01c_read_data(sm_pwm_01c_t *dev, sm_pwm_01c_data_t *data)
{
    assert(dev);
    unsigned int state = irq_disable();
#ifdef MODULE_SM_PWM_01C_MA
    data->mc_pm_10 = _circ_buf_avg(&dev->_values.tlp_circ_buf);
    data->mc_pm_2p5 = _circ_buf_avg(&dev->_values.tsp_circ_buf);
#else
    data->mc_pm_10 = dev->_values.data.mc_pm_10;
    data->mc_pm_2p5 = dev->_values.data.mc_pm_2p5;
#endif
    irq_restore(state);
}
