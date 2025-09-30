/*
 * Copyright (C) 2016 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     sys_can_dll
 * @{
 * @file
 * @brief       CAN device interface
 *
 * @author      Toon Stegen <toon.stegen@altran.com>
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Aurelien Gonce <aurelien.gonce@altran.com>
 * @}
 */

#include <assert.h>
#include <errno.h>

#include "thread.h"
#include "ztimer.h"
#include "can/device.h"
#include "can/common.h"
#include "can/pkt.h"
#include "can/dll.h"

#ifdef MODULE_CAN_TRX
#include "can/can_trx.h"
#endif

#define ENABLE_DEBUG 0
#include "debug.h"

#ifdef MODULE_FDCAN
/**
 * The loop delay in CAN, especially in CAN FD with bitrate switching, affects synchronization due to increased data rates.
 * The unit is nanoseconds.
 */
#ifndef CONFIG_FDCAN_DEVICE_TRANSCEIVER_LOOP_DELAY
#error "CONFIG_FDCAN_DEVICE_TRANSCEIVER_LOOP_DELAY must be defined. This property can be found in the datasheet of the CAN transceiver in use. The unit is nanoseconds."
#endif /* CONFIG_FDCAN_DEVICE_TRANSCEIVER_LOOP_DELAY */
#endif /* MODULE_FDCAN */

#ifndef CAN_DEVICE_MSG_QUEUE_SIZE
#define CAN_DEVICE_MSG_QUEUE_SIZE 64
#endif

#ifdef MODULE_CAN_PM
#define CAN_DEVICE_PM_DEFAULT_RX_TIMEOUT (10 * US_PER_SEC)
#define CAN_DEVICE_PM_DEFAULT_TX_TIMEOUT (2 * US_PER_SEC)
static void pm_cb(void *arg);
static void pm_reset(candev_dev_t *candev_dev, uint32_t value);
#endif

static int power_up(candev_dev_t *candev_dev);
static int power_down(candev_dev_t *candev_dev);

static void _can_event(candev_t *dev, candev_event_t event, void *arg)
{
    msg_t msg;
    can_frame_t *frame;
    can_pkt_t *pkt;
    candev_dev_t *candev_dev = dev->isr_arg;

    DEBUG("_can_event: dev=%p, params=%p\n", (void*)dev, (void*)candev_dev);
    DEBUG("_can_event: params->ifnum=%d, params->pid=%" PRIkernel_pid ", params->dev=%p\n",
          candev_dev->ifnum, candev_dev->pid, (void*)candev_dev->dev);

    switch (event) {
    case CANDEV_EVENT_ISR:
        DEBUG("_can_event: CANDEV_EVENT_ISR\n");
        msg.type = CAN_MSG_EVENT;
        if (msg_send(&msg, candev_dev->pid) <= 0) {
            DEBUG("can device: isr lost\n");
        }
        break;
    case CANDEV_EVENT_WAKE_UP:
        DEBUG("_can_event: CANDEV_EVENT_WAKE_UP\n");
        power_up(candev_dev);
#ifdef MODULE_CAN_PM
        pm_reset(candev_dev, candev_dev->rx_inactivity_timeout);
#endif
        break;
    case CANDEV_EVENT_TX_CONFIRMATION:
        DEBUG("_can_event: CANDEV_EVENT_TX_CONFIRMATION\n");
        /* frame pointer in arg */
        pkt = container_of((can_frame_t *)arg, can_pkt_t, frame);
        can_dll_dispatch_tx_conf(pkt);
        break;
    case CANDEV_EVENT_TX_ERROR:
        DEBUG("_can_event: CANDEV_EVENT_TX_ERROR\n");
        /* frame pointer in arg */
        pkt = container_of((can_frame_t *)arg, can_pkt_t, frame);
        can_dll_dispatch_tx_error(pkt);
        break;
    case CANDEV_EVENT_RX_INDICATION:
        DEBUG("_can_event: CANDEV_EVENT_RX_INDICATION\n");
#ifdef MODULE_CAN_PM
        pm_reset(candev_dev, candev_dev->rx_inactivity_timeout);
#endif
        /* received frame in arg */
        frame = (can_frame_t *) arg;
        can_dll_dispatch_rx_frame(frame, candev_dev->pid);
        break;
    case CANDEV_EVENT_RX_ERROR:
        DEBUG("_can_event: CANDEV_EVENT_RX_ERROR\n");
        break;
    case CANDEV_EVENT_BUS_OFF:
        dev->state = CAN_STATE_BUS_OFF;
        break;
    case CANDEV_EVENT_ERROR_PASSIVE:
        dev->state = CAN_STATE_ERROR_PASSIVE;
        break;
    case CANDEV_EVENT_ERROR_WARNING:
        dev->state = CAN_STATE_ERROR_WARNING;
        break;
    default:
        DEBUG("_can_event: unknown event\n");
        break;
    }
}

static int power_up(candev_dev_t *candev_dev)
{
    candev_t *dev = candev_dev->dev;

    DEBUG("candev: power up\n");

#ifdef MODULE_CAN_TRX
    can_trx_set_mode(candev_dev->trx, TRX_NORMAL_MODE);
#endif
    canopt_state_t state = CANOPT_STATE_ON;
    int res = dev->driver->set(dev, CANOPT_STATE, &state, sizeof(state));
    dev->state = CAN_STATE_ERROR_ACTIVE;

    return res;
}

static int power_down(candev_dev_t *candev_dev)
{
    candev_t *dev = candev_dev->dev;

    DEBUG("candev: power down\n");

#ifdef MODULE_CAN_TRX
    can_trx_set_mode(candev_dev->trx, TRX_SLEEP_MODE);
#endif
    canopt_state_t state = CANOPT_STATE_SLEEP;
    int res = dev->driver->set(dev, CANOPT_STATE, &state, sizeof(state));

    if (dev->state != CAN_STATE_BUS_OFF) {
        dev->state = CAN_STATE_SLEEPING;
    }

#ifdef MODULE_CAN_PM
    ztimer_remove(ZTIMER_USEC, &candev_dev->pm_timer);
    candev_dev->last_pm_update = 0;
#endif

    return res;
}

#ifdef MODULE_CAN_PM
static void pm_cb(void *arg)
{
    candev_dev_t *candev_dev = (candev_dev_t *)arg;
    msg_t msg;
    msg.type = CAN_MSG_PM;

    msg_send(&msg, candev_dev->pid);
}

static void pm_reset(candev_dev_t *candev_dev, uint32_t value)
{
    DEBUG("pm_reset: dev=%p, value=%" PRIu32 ", last_pm_value=%" PRIu32
          ", last_pm_update=%" PRIu32 "\n", (void *)candev_dev, value,
          candev_dev->last_pm_value, candev_dev->last_pm_update);

    if (value == 0) {
        candev_dev->last_pm_value = 0;
        ztimer_remove(ZTIMER_USEC, &candev_dev->pm_timer);
        return;
    }

    if (candev_dev->last_pm_update == 0 ||
            value > (candev_dev->last_pm_value - (ztimer_now(ZTIMER_USEC) - candev_dev->last_pm_update))) {
        candev_dev->last_pm_value = value;
        candev_dev->last_pm_update = ztimer_now(ZTIMER_USEC);
        ztimer_set(ZTIMER_USEC, &candev_dev->pm_timer, value);
    }
}
#endif

static void wake_up(candev_dev_t *candev_dev)
{
    candev_t *dev = candev_dev->dev;
    if (dev->state == CAN_STATE_BUS_OFF || dev->state == CAN_STATE_SLEEPING) {
        DEBUG("can device: waking up driver\n");
        power_up(candev_dev);
    }
#ifdef MODULE_CAN_PM
    pm_reset(candev_dev, candev_dev->tx_wakeup_timeout);
#endif
}

static void *_can_device_thread(void *args)
{
    candev_dev_t *candev_dev = (candev_dev_t *) args;
    candev_t *dev = candev_dev->dev;

    DEBUG("_can_device_thread: starting thread for ifnum=%d, pid=%" PRIkernel_pid "\n",
          candev_dev->ifnum, thread_getpid());
    DEBUG("_cand_device_thread: dev=%p, params=%p\n", (void*)dev, (void*)candev_dev);

    candev_dev->pid = thread_getpid();

#ifdef MODULE_CAN_TRX
    can_trx_init(candev_dev->trx);
#endif
#ifdef MODULE_CAN_PM
    if (candev_dev->rx_inactivity_timeout == 0) {
        candev_dev->rx_inactivity_timeout = CAN_DEVICE_PM_DEFAULT_RX_TIMEOUT;
    }
    if (candev_dev->tx_wakeup_timeout == 0) {
        candev_dev->tx_wakeup_timeout = CAN_DEVICE_PM_DEFAULT_TX_TIMEOUT;
    }
    candev_dev->pm_timer.callback = pm_cb;
    candev_dev->pm_timer.arg = candev_dev;
    pm_reset(candev_dev, candev_dev->rx_inactivity_timeout);
#endif

    int res;
    can_pkt_t *pkt;
    can_opt_t *opt;
    msg_t msg, reply, msg_queue[CAN_DEVICE_MSG_QUEUE_SIZE];

    /* setup the device layers message queue */
    msg_init_queue(msg_queue, CAN_DEVICE_MSG_QUEUE_SIZE);

    dev->event_callback = _can_event;
    dev->isr_arg = candev_dev;

    candev_dev->ifnum = can_dll_register_candev(candev_dev);

#if defined(MODULE_FDCAN)
    if (candev_dev->loop_delay == 0) {
        candev_dev->loop_delay = CONFIG_FDCAN_DEVICE_TRANSCEIVER_LOOP_DELAY;
    }
    dev->loop_delay = candev_dev->loop_delay;
#endif

    dev->driver->init(dev);
    power_up(candev_dev);

    while (1) {
        msg_receive(&msg);
        switch (msg.type) {
        case CAN_MSG_EVENT:
            DEBUG("can device: CAN_MSG_EVENT received\n");
            dev->driver->isr(dev);
            break;
        case CAN_MSG_ABORT_FRAME:
            DEBUG("can device: CAN_MSG_ABORT_FRAME received\n");
            pkt = (can_pkt_t *) msg.content.ptr;
            dev->driver->abort(dev, &pkt->frame);
            reply.type = CAN_MSG_ACK;
            reply.content.value = 0;
            msg_reply(&msg, &reply);
            break;
        case CAN_MSG_SEND_FRAME:
            DEBUG("can device: CAN_MSG_SEND_FRAME received\n");
            wake_up(candev_dev);
            /* read incoming pkt */
            pkt = (can_pkt_t *) msg.content.ptr;
            dev->driver->send(dev, &pkt->frame);
            break;
        case CAN_MSG_SET:
            DEBUG("can device: CAN_MSG_SET received\n");
            /* read incoming options */
            opt = (can_opt_t *)msg.content.ptr;
            /* set option for device driver */
            res = dev->driver->set(dev, opt->opt, opt->data, opt->data_len);
            /* send reply to calling thread */
            reply.type = CAN_MSG_ACK;
            reply.content.value = (uint32_t)res;
            msg_reply(&msg, &reply);
            break;
        case CAN_MSG_GET:
            DEBUG("can device: CAN_MSG_GET received\n");
            /* read incoming options */
            opt = (can_opt_t *)msg.content.ptr;
            /* get option for device driver */
            res = dev->driver->get(dev, opt->opt, opt->data, opt->data_len);
            /* send reply to calling thread */
            reply.type = CAN_MSG_ACK;
            reply.content.value = (uint32_t)res;
            msg_reply(&msg, &reply);
            break;
        case CAN_MSG_SET_FILTER:
            DEBUG("can device: CAN_MSG_SET_FILTER received\n");
            wake_up(candev_dev);
            /* set filter for device driver */
            res = dev->driver->set_filter(dev, msg.content.ptr);
            /* send reply to calling thread */
            reply.type = CAN_MSG_ACK;
            reply.content.value = (uint32_t)res;
            msg_reply(&msg, &reply);
            break;
        case CAN_MSG_REMOVE_FILTER:
            DEBUG("can device: CAN_MSG_REMOVE_FILTER received\n");
            wake_up(candev_dev);
            /* set filter for device driver */
            res = dev->driver->remove_filter(dev, msg.content.ptr);
            /* send reply to calling thread */
            reply.type = CAN_MSG_ACK;
            reply.content.value = (uint32_t)res;
            msg_reply(&msg, &reply);
            break;
        case CAN_MSG_POWER_UP:
            DEBUG("can device: CAN_MSG_POWER_UP received\n");
            res = power_up(candev_dev);
#ifdef MODULE_CAN_PM
            pm_reset(candev_dev, candev_dev->tx_wakeup_timeout);
#endif
            /* send reply to calling thread */
            reply.type = CAN_MSG_ACK;
            reply.content.value = (uint32_t)res;
            msg_reply(&msg, &reply);
            break;
        case CAN_MSG_POWER_DOWN:
            DEBUG("can device: CAN_MSG_POWER_DOWN received\n");
            res = power_down(candev_dev);
#ifdef MODULE_CAN_PM
            pm_reset(candev_dev, 0);
#endif
            /* send reply to calling thread */
            reply.type = CAN_MSG_ACK;
            reply.content.value = (uint32_t)res;
            msg_reply(&msg, &reply);
            break;
#ifdef MODULE_CAN_TRX
        case CAN_MSG_SET_TRX:
            DEBUG("can device: CAN_MSG_SET_TRX received\n");
            reply.type = CAN_MSG_ACK;
            if (dev->state != CAN_STATE_SLEEPING) {
                reply.content.value = -EBUSY;
            }
            else {
                candev_dev->trx = msg.content.ptr;
                reply.content.value = 0;
            }
            msg_reply(&msg, &reply);
            break;
#endif
#ifdef MODULE_CAN_PM
        case CAN_MSG_PM:
            DEBUG("can device: pm power down\n");
            power_down(candev_dev);
            break;
#endif
        default:
            break;
        }
    }

    return NULL;
}

kernel_pid_t can_device_init(char *stack, int stacksize, char priority,
                             const char *name, candev_dev_t *params)
{
    kernel_pid_t res;

    /* check if given device is defined and the driver is set */
    if (params == NULL || params->dev == NULL || params->dev->driver == NULL) {
        return -ENODEV;
    }

    /* create new can device thread */
    res = thread_create(stack, stacksize, priority, 0,
                         _can_device_thread, (void *)params, name);
    if (res <= 0) {
        return -EINVAL;
    }

    return res;
}

#define SJW 2
#define CAN_SYNC_SEG 1

static inline uint32_t min(uint32_t x, uint32_t y)
{
    return x < y ? x : y;
}

static inline uint32_t max(uint32_t x, uint32_t y)
{
    return x > y ? x : y;
}

static inline uint32_t clamp(uint32_t val, uint32_t lo, uint32_t hi)
{
    return min(max(val, lo), hi);
}

/**
 * @brief Compute tseg1 and tseg2 and returns the sample point
 *
 * tseg1 and tseg2 are calculated from the nominal sample point and tseg
 *
 * @param[in] btc          the bittiming const
 * @param[in] spt_nominal  the nominal sample point
 * @param[in] tseg         number of tq in the nbt minus the SYNC_SEG
 * @param[out] p_tseg1     number of tq in tseg1 (PHASE_SEG_1 + PROP_SEG)
 * @param[out] p_tseg2     number of tq in tseg2 (PHASE_SEG_2)
 * @param[out] p_spt_error (optional) the sample point difference between @p spt_nominal
 *                          and computed sample point from @p tseg1 and @p tseg2
 *
 * @return the computed sample point from @p tseg1 and @p tseg2
 */
static uint32_t update_sample_point(const struct can_bittiming_const *btc, uint32_t spt_nominal,
                                    uint32_t tseg, uint32_t *p_tseg1, uint32_t *p_tseg2, uint32_t *p_spt_error)
{
    uint32_t best_spt = 0;
    uint32_t min_spt_error = UINT32_MAX;

    for (int i = 0; i <= 1; i++) {
        uint32_t tseg1;
        uint32_t tseg2;
        uint32_t spt;
        uint32_t spt_error;

        tseg2 = tseg + CAN_SYNC_SEG - (spt_nominal * (tseg + CAN_SYNC_SEG)) / 1000 - i;
        tseg2 = clamp(tseg2, btc->tseg2_min, btc->tseg2_max);
        tseg1 = tseg - tseg2;
        if (tseg1 > btc->tseg1_max) {
            tseg1 = btc->tseg1_max;
            tseg2 = tseg - tseg1;
        }

        spt = 1000 * (tseg1 + CAN_SYNC_SEG) / (tseg + CAN_SYNC_SEG);
        spt_error = max(spt, spt_nominal) - min(spt, spt_nominal);
        if (spt <= spt_nominal && spt_error < min_spt_error) {
            best_spt = spt;
            min_spt_error = spt_error;
            *p_tseg1 = tseg1;
            *p_tseg2 = tseg2;
        }

        if (p_spt_error) {
            *p_spt_error = min_spt_error;
        }
        DEBUG("tseg1=%" PRIu32 ", tseg2=%" PRIu32 ", spt_error=%" PRIu32 "\n",
              tseg1, tseg2, spt_error);
    }

    return best_spt;
}

/*
 *                   Nominal bit time (nbt) composed of 8 time quantum (tq)
 * |<------------------------------------------------------------------------------------->|
 * |                                                                                       |
 * +----------+----------+-------------------------------------------+---------------------+
 * | SYNC_SEG | PROP_SEG |                PHASE_SEG_1                |     PHASE_SEG_2     |
 * +----------+----------+-------------------------------------------+---------------------+
 * |                                                                 ^                     |
 * |                                                    Sample point | at 75%              |
 * |----------|----------|----------|----------|----------|----------|----------|----------|
 * |   Time quanta                                                 6 | 2                   |
 *
 * Synchronization segment = always 1 tq
 *                SYNC_SEG + PROP_SEG + PHASE_SEG1
 * Sample point = --------------------------------
 *                             nbt
 *
 * tseg1 = PROP_SEG + PHASE_SEG_1
 * tseg2 = PHASE_SEG_2
 * tseg = tseg1 + tseg2
 * nbt = tseg + SYNC_SEG
 *
 */
int can_device_calc_bittiming(uint32_t clock, const struct can_bittiming_const *timing_const,
                              struct can_bittiming *timing)
{
    uint32_t spt; /* nominal sample point, in one-tenth of a percent */
    uint32_t spt_error;
    uint32_t min_spt_error = UINT32_MAX;
    uint32_t best_brp = 0;
    uint32_t tseg = 0;
    uint32_t tseg1 = 0;
    uint32_t tseg2 = 0;
    uint32_t best_tseg = 0;
    uint32_t rate_error;
    uint32_t min_rate_error;

    assert((timing != NULL) && (timing->bitrate != 0));
    assert(timing_const != NULL);

    if (timing->sample_point) {
        spt = timing->sample_point;
    }
    else {
        /* Use recommended sample points */
        /* See CiA 301 (https://www.can-cia.org/standardization/technical-documents/) */
        /* 87.5% is recommended from 10kbit/s to 1Mbit/s */
        spt = 875;
    }
    rate_error = min_rate_error = timing->bitrate;

    DEBUG("init_bittiming: rate=%" PRIu32 ", clock=%" PRIu32 ", spt=%" PRIu32 "\n",
          timing->bitrate, clock, timing->sample_point);

    /* Starting from higher tq per nbt */
    for (tseg = timing_const->tseg1_max + timing_const->tseg2_max;
         tseg >= timing_const->tseg1_min + timing_const->tseg2_min; tseg--) {
        uint32_t nbt = tseg + CAN_SYNC_SEG;

        /* theoretical brp */
        uint32_t brp = clock / (timing->bitrate * nbt);
        /* brp according to brp_inc */
        brp = (brp / timing_const->brp_inc) * timing_const->brp_inc;

        DEBUG("tsegall=%" PRIu32 ", brp=%" PRIu32 "\n", nbt, brp);

        if (brp < timing_const->brp_min || brp > timing_const->brp_max) {
            /* Invalid brp */
            DEBUG("invalid brp\n");
            continue;
        }
        /* current bitrate */
        uint32_t rate = clock / (brp * nbt);
        rate_error = max(timing->bitrate, rate) - min(timing->bitrate, rate);
        if (rate_error > min_rate_error) {
            DEBUG("timing->rate=%" PRIu32 ", rate=%" PRIu32 ", rate_error=%" PRIu32 " > min_rate_error=%" PRIu32 ", continuing\n",
                  timing->bitrate, rate, rate_error, min_rate_error);
            continue;
        }

        if (rate_error < min_rate_error) {
            min_spt_error = UINT32_MAX;
        }

        update_sample_point(timing_const, spt, tseg, &tseg1, &tseg2, &spt_error);
        if (spt_error > min_spt_error) {
            DEBUG("spt_error=%" PRIu32 " > min_spt_error=%" PRIu32 ", continuing\n",
                  spt_error, min_spt_error);
            continue;
        }

        min_spt_error = spt_error;
        min_rate_error = rate_error;
        best_tseg = tseg;
        best_brp = brp;

        DEBUG("rate_error=%" PRIu32 ", spt_error=%" PRIu32 "\n", rate_error, spt_error);

        if (rate_error == 0 && spt_error == 0) {
            break;
        }
    }

    DEBUG("computed values: min_rate_error=%" PRIu32 ", min_spt_error=%" PRIu32 "\n", min_rate_error, min_spt_error);

    if (min_rate_error) {
        rate_error = min_rate_error * 1000 / timing->bitrate;
        if (rate_error > CAN_MAX_RATE_ERROR) {
            return -1;
        }
    }

    timing->sample_point = update_sample_point(timing_const, spt,
                                               best_tseg, &tseg1, &tseg2, NULL);

    timing->prop_seg = tseg1 / 2;
    timing->phase_seg1 = tseg1 - timing->prop_seg;
    timing->phase_seg2 = tseg2;

    /* this paper http://www.oertel-halle.de/files/cia99paper.pdf might help to understand SJW */
    if (!timing->sjw) {
        if (!timing_const->sjw_max) {
            /* fallback if no device max value is known */
            timing->sjw = SJW;
        }
        else {
            /* try to find the max_value start at max */
            timing->sjw = timing_const->sjw_max;
        }
    }
    if (timing->sjw > timing->phase_seg2) {
        /* SJW shall not be bigger than phase segment 2 */
        timing->sjw = timing->phase_seg2;
    }
    if (timing->sjw > timing->phase_seg1) {
        /* SJW shall not be bigger than phase segment 1 */
        timing->sjw = timing->phase_seg1;
    }
    timing->brp = best_brp;

    timing->bitrate = clock / (timing->brp * (CAN_SYNC_SEG + tseg1 + tseg2));

    DEBUG("bitrate=%" PRIu32 ", sample_point=%" PRIu32 ", brp=%" PRIu32 ", prop_seg=%" PRIu32
          ", phase_seg1=%" PRIu32 ", phase_seg2=%" PRIu32 "\n", timing->bitrate, timing->sample_point,
          timing->brp, timing->prop_seg, timing->phase_seg1, timing->phase_seg2);

    return 0;
}
