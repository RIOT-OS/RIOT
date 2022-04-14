/*
 * Copyright (C) 2016 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_mcp2515
 * @{
 *
 * @file
 * @brief       Implementation of the CAN controller driver
 *
 * @author      Toon Stegen <toon.stegen@altran.com>
 * @author      Vincent Dupont <vincent@otakeys.com>
 * @author      Wouter Symons <wosym@airsantelmo.com>
 * @}
 */

#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>

#include "candev_mcp2515.h"
#include "can/common.h"
#include "can/device.h"
#include "mcp2515.h"
#include "mutex.h"
#include "periph_conf.h"
#include "thread.h"
#include "sched.h"
#include "ztimer.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static mutex_t _mcp_mutex;
static int _neednewisr = 0;

static int _init(candev_t *candev);
static int _send(candev_t *candev, const struct can_frame *frame);
static void _isr(candev_t *candev);
static int _set(candev_t *candev, canopt_t opt, void *value, size_t value_len);
static int _get(candev_t *candev, canopt_t opt, void *value, size_t max_len);
static int _abort(candev_t *candev, const struct can_frame *frame);
static int _set_filter(candev_t *dev, const struct can_filter *filter);
static int _remove_filter(candev_t *dev, const struct can_filter *filter);

static void _irq_rx(candev_mcp2515_t *dev, int handle);
static void _irq_tx(candev_mcp2515_t *dev, int handle);
static void _irq_error(candev_mcp2515_t *dev);
static void _irq_message_error(candev_mcp2515_t *dev);
static void _irq_wakeup(const candev_mcp2515_t *dev);
static void _send_event(const candev_mcp2515_t *dev, candev_event_t event,
                        void *arg);

static const candev_driver_t candev_mcp2515_driver = {
    .send = _send,
    .init = _init,
    .isr = _isr,
    .get = _get,
    .set = _set,
    .abort = _abort,
    .set_filter = _set_filter,
    .remove_filter = _remove_filter,
};

static const struct can_bittiming_const bittiming_const = {
    .tseg1_min = 3,     /**< Time segment 1 = prop_seg + phase_seg1, min value */
    .tseg1_max = 16,    /**< Time segment 1, max value */
    .tseg2_min = 2,     /**< Time segment 2 = phase_seg2, min value */
    .tseg2_max = 8,     /**< Time segment 2, max value */
    .sjw_max = 4,       /**< Synchronisation jump width */
    .brp_min = 1,       /**< Bit-rate prescaler, min value */
    .brp_max = 64,      /**< Bit-rate prescaler, max value */
    .brp_inc = 1,       /**< Bit-rate prescaler, increment */
};

static inline int _max_filters(int mailbox)
{
    return mailbox == 0 ? MCP2515_FILTERS_MB0 : MCP2515_FILTERS_MB1;
}

void candev_mcp2515_init(candev_mcp2515_t *dev,
                         const candev_mcp2515_conf_t *conf)
{
    memset(dev, 0, sizeof(*dev));
    dev->candev.driver = &candev_mcp2515_driver;

    struct can_bittiming timing = { .bitrate = CANDEV_MCP2515_DEFAULT_BITRATE,
                                    .sample_point =
                                        CANDEV_MCP2515_DEFAULT_SPT };

    /* f_quantum = f_osc / 2 */
    can_device_calc_bittiming(conf->clk / 2, &bittiming_const, &timing);

    memcpy(&dev->candev.bittiming, &timing, sizeof(timing));
    /* configure filters to be closed */
    for (int mailbox = 0; mailbox < MCP2515_RX_MAILBOXES; mailbox++) {
        dev->masks[mailbox] = 0;
        for (int filter_id = 0; filter_id < MCP2515_FILTERS_MB1; filter_id++) {
            dev->filter_ids[mailbox][filter_id] = 0;
        }
    }

    dev->conf = conf;
}

static void _mcp2515_irq_handler(void *arg)
{
    candev_mcp2515_t *candev = (candev_mcp2515_t *)arg;

    _send_event(candev, CANDEV_EVENT_ISR, NULL);
}

static int _init(candev_t *candev)
{
    int res = 0;
    candev_mcp2515_t *dev = container_of(candev, candev_mcp2515_t, candev);

    memset(dev->tx_mailbox, 0, sizeof(dev->tx_mailbox));

    mcp2515_init(dev, _mcp2515_irq_handler);
    mcp2515_reset(dev);
    mcp2515_set_mode(dev, MODE_CONFIG);
    mcp2515_configure_bittiming(dev);
    mcp2515_init_irqs(dev);

    if (mutex_trylock(&_mcp_mutex)) {
        /* configure filters to be closed */
        for (int mailbox = 0; mailbox < MCP2515_RX_MAILBOXES; mailbox++) {
            mcp2515_set_mask(dev, mailbox, dev->masks[mailbox]);
            for (int filter = 0; filter < _max_filters(mailbox); filter++) {
                mcp2515_set_filter(dev, mailbox * MCP2515_FILTERS_MB0 + filter,
                                   dev->filter_ids[mailbox][filter]);
            }
        }
        res = mcp2515_set_mode(dev, MODE_NORMAL);
        mutex_unlock(&_mcp_mutex);
    }
    else {
        /* locking failed */
        DEBUG("failed to lock mutex_init");
        return -1;
    }
    return res;
}

static int _send(candev_t *candev, const struct can_frame *frame)
{
    candev_mcp2515_t *dev = container_of(candev, candev_mcp2515_t, candev);
    int box;
    int ret = 0;
    enum mcp2515_mode mode;

    if (mutex_trylock(&_mcp_mutex)) {
        mode = mcp2515_get_mode(dev);
        mutex_unlock(&_mcp_mutex);
    }
    else {
        DEBUG("failed to lock mutex_send\n");
        return -1;
    }
    if (mode != MODE_NORMAL && mode != MODE_LOOPBACK) {
        return -EINVAL;
    }

    DEBUG("Inside mcp2515 send\n");

    for (box = 0; box < MCP2515_TX_MAILBOXES; box++) {
        if (dev->tx_mailbox[box] == NULL) {
            break;
        }
    }

    if (box == MCP2515_TX_MAILBOXES) {
        return -EBUSY;
    }

    dev->tx_mailbox[box] = frame;

    if (mutex_trylock(&_mcp_mutex)) {
        ret = mcp2515_send(dev, frame, box);
        mutex_unlock(&_mcp_mutex);
    }
    else {
        DEBUG("send_failed to lock mutex\n");
        return -1;
    }
    if (ret < 0) {
        return -1;
    }

    if (_neednewisr) {
        DEBUG("Calling _isr() again on request\n");
        _isr(candev);
        _neednewisr = 0;
    }

    return box;
}

static int _abort(candev_t *candev, const struct can_frame *frame)
{
    candev_mcp2515_t *dev = container_of(candev, candev_mcp2515_t, candev);
    int box;

    DEBUG("Inside mcp2515 abort\n");

    for (box = 0; box < MCP2515_TX_MAILBOXES; box++) {
        if (dev->tx_mailbox[box] == frame) {
            break;
        }
    }

    if (box == MCP2515_TX_MAILBOXES) {
        return -EBUSY;
    }

    if (mutex_trylock(&_mcp_mutex)) {
        mcp2515_abort(dev, box);
        mutex_unlock(&_mcp_mutex);
    }
    else {
        DEBUG("abort_Failed to lock mutex\n");
        return -1;
    }
    dev->tx_mailbox[box] = NULL;

    return 0;
}

static void _isr(candev_t *candev)
{
    uint8_t flag;
    candev_mcp2515_t *dev = container_of(candev, candev_mcp2515_t, candev);

    if (mutex_trylock(&_mcp_mutex)) {
        flag = mcp2515_get_irq(dev);
        mcp2515_clear_irq(dev, flag & ~(INT_RX0 | INT_RX1));
        mutex_unlock(&_mcp_mutex);
    }
    else {
        DEBUG("isr: Failed to lock mutex\n");
        _neednewisr = 1;
        return;
    }

    while ((flag)) {
        if (flag & INT_WAKEUP) {
            if (dev->wakeup_src == MCP2515_WKUP_SRC_INTERNAL) {
                dev->wakeup_src = 0;
            }
            else {
                _irq_wakeup(dev);
            }
        }
        if (flag & INT_ERROR) {
            _irq_error(dev);
        }
        if (flag & INT_RX0) {
            _irq_rx(dev, 0);
        }
        if (flag & INT_RX1) {
            _irq_rx(dev, 1);
        }
        if (flag & INT_TX0) {
            _irq_tx(dev, 0);
        }
        if (flag & INT_TX1) {
            _irq_tx(dev, 1);
        }
        if (flag & INT_TX2) {
            _irq_tx(dev, 2);
        }
        if (flag & INT_MESSAGE_ERROR) {
            _irq_message_error(dev);
        }
        if (mutex_trylock(&_mcp_mutex)) {
            flag = mcp2515_get_irq(dev);
            mcp2515_clear_irq(dev, flag & ~(INT_RX0 | INT_RX1));
            mutex_unlock(&_mcp_mutex);
        }
        else {
            DEBUG("isr2: Failed to lock mutex\n");
            _neednewisr = 1;
            return;
        }
    }
}

static int _set(candev_t *candev, canopt_t opt, void *value, size_t value_len)
{
    candev_mcp2515_t *dev = container_of(candev, candev_mcp2515_t, candev);
    int res = 0;

    DEBUG("Inside mcp2515 set opt=%d\n", opt);
    switch (opt) {
    case CANOPT_BITTIMING:       /**< bit timing parameter */
        if (value_len < sizeof(candev->bittiming)) {
            res = -EOVERFLOW;
        }
        else {
            memcpy(&candev->bittiming, value, sizeof(candev->bittiming));
            res = _init(candev);
            if (res == 0) {
                res = sizeof(candev->bittiming);
            }

        }
        break;
    case CANOPT_STATE:
        if (value_len < sizeof(uint8_t)) {
            res = -EOVERFLOW;
        }
        else {
            switch (*((canopt_state_t *)value)) {
            case CANOPT_STATE_LISTEN_ONLY:
                if (mutex_trylock(&_mcp_mutex)) {
                    res = mcp2515_set_mode(dev, MODE_LISTEN_ONLY);
                    mutex_unlock(&_mcp_mutex);
                }
                else {
                    DEBUG("set1_Failed to lock mutex\n");
                    return -1;
                }
                break;
            case CANOPT_STATE_OFF:
            case CANOPT_STATE_SLEEP:
                if (mutex_trylock(&_mcp_mutex)) {
                    res = mcp2515_set_mode(dev, MODE_SLEEP);
                    mutex_unlock(&_mcp_mutex);
                }
                else {
                    DEBUG("set2_Failed to lock mutex\n");
                    return -1;
                }
                break;
            case CANOPT_STATE_ON:
                if (mutex_trylock(&_mcp_mutex)) {
                    res = mcp2515_set_mode(dev, MODE_NORMAL);
                    mutex_unlock(&_mcp_mutex);
                }
                else {
                    DEBUG("set3_Failed to lock mutex\n");
                    return -1;
                }
                break;
            default:
                res = -ENOTSUP;
                break;
            }
        }
        break;
    default:
        res = -ENOTSUP;
    }

    return res;
}

static int _get(candev_t *candev, canopt_t opt, void *value, size_t max_len)
{
    candev_mcp2515_t *dev = container_of(candev, candev_mcp2515_t, candev);
    int res = 0;

    DEBUG("Inside mcp2515 get opt=%d\n", opt);
    switch (opt) {
    case CANOPT_BITTIMING:
        if (max_len < sizeof(candev->bittiming)) {
            res = -EOVERFLOW;
        }
        else {
            memcpy(value, &candev->bittiming, sizeof(candev->bittiming));
            res = sizeof(candev->bittiming);
        }
        break;
    case CANOPT_RX_FILTERS:      /**< rx filters */
        if (max_len % sizeof(struct can_filter) != 0) {
            res = -EOVERFLOW;
        }
        else {
            /* Not implemented (yet...) */
            res = -ENOTSUP;
        }
        break;
    case CANOPT_BITTIMING_CONST:
        if (max_len < sizeof(bittiming_const)) {
            res = -EOVERFLOW;
        }
        else {
            memcpy(value, &bittiming_const, sizeof(bittiming_const));
            res = sizeof(bittiming_const);
        }
        break;
    case CANOPT_CLOCK:
        if (max_len < sizeof(uint32_t)) {
            res = -EOVERFLOW;
        }
        else {
            *((uint32_t *)value) = (uint32_t)(dev->conf->clk / 2);
            res = sizeof(uint32_t);
        }
        break;
    default:
        res = -ENOTSUP;
        break;
    }

    return res;
}

static int _set_filter(candev_t *dev, const struct can_filter *filter)
{
    DEBUG("inside _set_filter of MCP2515\n");
    bool filter_added = true;
    struct can_filter f = *filter;
    int res = -1;
    enum mcp2515_mode mode;

    candev_mcp2515_t *dev_mcp = container_of(dev, candev_mcp2515_t, candev);

    if (mutex_trylock(&_mcp_mutex)) {
        mode = mcp2515_get_mode(dev_mcp);
        res = mcp2515_set_mode(dev_mcp, MODE_CONFIG);

        mutex_unlock(&_mcp_mutex);
    }
    else {
        DEBUG("setfilt_Failed to lock mutex\n");
        return -1;
    }

    if (res != MODE_CONFIG) {
        return -1;
    }

    if ((f.can_id & CAN_EFF_FLAG) == CAN_EFF_FLAG) {
        f.can_mask &= CAN_EFF_MASK;
    }
    else {
        f.can_mask &= CAN_SFF_MASK;
    }

    /* Browse on each mailbox to find an empty space */
    int mailbox_index = 0;

    while (mailbox_index < MCP2515_RX_MAILBOXES && !filter_added) {
        /* mask unused */
        if (dev_mcp->masks[mailbox_index] == 0) {
            /* set mask */
            mcp2515_set_mask(dev_mcp, mailbox_index, f.can_mask);
            /* set filter */
            mcp2515_set_filter(dev_mcp, MCP2515_FILTERS_MB0 * mailbox_index,
                               f.can_id);

            /* save filter */
            dev_mcp->masks[mailbox_index] = f.can_mask;
            dev_mcp->filter_ids[mailbox_index][0] = f.can_id;

            /* function succeeded */
            filter_added = true;
        }

        /* mask existed and same mask */
        else if (dev_mcp->masks[mailbox_index] == f.can_mask) {
            /* find en empty space if it exist */
            int filter_pos = 1; /* first one is already filled */
            /* stop at the end of mailbox or an empty space found */
            while (filter_pos < _max_filters(mailbox_index) &&
                   dev_mcp->filter_ids[mailbox_index][filter_pos] != 0) {
                filter_pos++;
            }

            /* an empty space is found */
            if (filter_pos < _max_filters(mailbox_index)) {
                /* set filter on this memory space */
                if (mutex_trylock(&_mcp_mutex)) {
                    mcp2515_set_filter(dev_mcp,
                                       MCP2515_FILTERS_MB0 * mailbox_index + filter_pos,
                                       f.can_id);
                    mutex_unlock(&_mcp_mutex);
                }
                else {
                    DEBUG("setfilt2_Failed to lock mutex");
                    return -1;
                }

                /* save filter */
                dev_mcp->filter_ids[mailbox_index][filter_pos] = f.can_id;

                /* function succeeded */
                filter_added = true;
            }
        }
        mailbox_index++;
    }

    if (mutex_trylock(&_mcp_mutex)) {
        mcp2515_set_mode(dev_mcp, mode);
        mutex_unlock(&_mcp_mutex);
    }
    else {
        DEBUG("setfilt3_Failed to lock mutex");
        return -1;
    }

    return filter_added;
}

static int _remove_filter(candev_t *dev, const struct can_filter *filter)
{
    DEBUG("inside _remove_filter of MCP2515\n");
    bool filter_removed;
    struct can_filter f = *filter;
    int res = 0;
    enum mcp2515_mode mode;

    candev_mcp2515_t *dev_mcp = container_of(dev, candev_mcp2515_t, candev);

    if (mutex_trylock(&_mcp_mutex)) {
        mode = mcp2515_get_mode(dev_mcp);
        res = mcp2515_set_mode(dev_mcp, MODE_CONFIG);
        mutex_unlock(&_mcp_mutex);
    }
    else {
        DEBUG("remfilt_Failed to lock mutex\n");
        return -1;
    }

    if (res < 0) {
        return -1;
    }

    if ((f.can_id & CAN_EFF_FLAG) == CAN_EFF_FLAG) {
        f.can_mask &= CAN_EFF_MASK;
    }
    else {
        f.can_mask &= CAN_SFF_MASK;
    }

    int mailbox_index = 0;

    /* Browse on each mailbox to find the right filter id */
    while (mailbox_index < MCP2515_RX_MAILBOXES && !filter_removed) {
        /* same mask */
        if (dev_mcp->masks[mailbox_index] == f.can_mask) {
            int filter_pos = 0;
            /* stop at the end of mailbox or filter_id found */
            while (filter_pos < _max_filters(mailbox_index) &&
                   dev_mcp->filter_ids[mailbox_index][filter_pos] != f.can_id) {
                filter_pos++;
            }

            /* filter id found */
            if (filter_pos < _max_filters(mailbox_index)) {
                /* remove filter */
                if (mutex_trylock(&_mcp_mutex)) {
                    mcp2515_set_filter(dev_mcp,
                                       MCP2515_FILTERS_MB0 * mailbox_index + filter_pos,
                                       CAN_EFF_MASK);
                    mutex_unlock(&_mcp_mutex);
                }
                else {
                    DEBUG("remfilt2_Failed to lock mutex\n");
                    return -1;
                }
                /* save modification */
                dev_mcp->filter_ids[mailbox_index][filter_pos] = 0;

                /* check mailbox empty */
                int nb_item = 0;
                for (int i = 0; i < _max_filters(mailbox_index); i++) {
                    if (dev_mcp->filter_ids[mailbox_index][i] == 0) {
                        nb_item++;
                    }
                }

                /* mailbox empty */
                if (nb_item == _max_filters(mailbox_index)) {
                    /* remove mask */
                    mcp2515_set_mask(dev_mcp, mailbox_index, CAN_EFF_MASK);
                    /* save modification */
                    dev_mcp->masks[mailbox_index] = 0;
                }

                filter_removed = 0;
            }
        }
        mailbox_index++;
    }
    if (mutex_trylock(&_mcp_mutex)) {
        mcp2515_set_mode(dev_mcp, mode);
        mutex_unlock(&_mcp_mutex);
    }
    else {
        DEBUG("remfilt3_Failed to lock mutex\n");
        return -1;
    }

    return filter_removed;
}

static void _irq_rx(candev_mcp2515_t *dev, int box)
{
    DEBUG("Inside mcp2515 rx irq, box=%d\n", box);

    if (mutex_trylock(&_mcp_mutex)) {
        mcp2515_receive(dev, &dev->rx_buf[box], box);
        mutex_unlock(&_mcp_mutex);
    }
    else {
        DEBUG("irqrx_Failed to lock mutex\n");
        return;
    }

    _send_event(dev, CANDEV_EVENT_RX_INDICATION, &dev->rx_buf[box]);
}

static void _irq_tx(candev_mcp2515_t *dev, int box)
{
    DEBUG("Inside mcp2515 tx irq\n");
    const struct can_frame *frame = dev->tx_mailbox[box];

    dev->tx_mailbox[box] = NULL;

    _send_event(dev, CANDEV_EVENT_TX_CONFIRMATION, (void *)frame);
}

static void _irq_error(candev_mcp2515_t *dev)
{
    uint8_t err;

    DEBUG("Inside mcp2515 error irq\n");

    if (mutex_trylock(&_mcp_mutex)) {
        err = mcp2515_get_errors(dev);
        mutex_unlock(&_mcp_mutex);
    }
    else {
        DEBUG("erqerr_Failed to lock mutex\n");
        return;
    }

    if (err & (ERR_WARNING | ERR_RX_WARNING | ERR_TX_WARNING)) {
        DEBUG("Error Warning\n");
        _send_event(dev, CANDEV_EVENT_ERROR_WARNING, NULL);
    }
    else if (err & (ERR_RX_PASSIVE | ERR_TX_PASSIVE)) {
        DEBUG("Error Passive\n");
        _send_event(dev, CANDEV_EVENT_ERROR_PASSIVE, NULL);
    }
    else if (err & ERR_TX_BUS_OFF) {
        DEBUG("Bus Off\n");
        _send_event(dev, CANDEV_EVENT_BUS_OFF, NULL);
    }
    else if (err & (ERR_RX_0_OVERFLOW | ERR_RX_1_OVERFLOW)) {
        DEBUG("RX overflow\n");
        _send_event(dev, CANDEV_EVENT_RX_ERROR, NULL);
    }
}

static void _irq_message_error(candev_mcp2515_t *dev)
{
    int box;

    DEBUG("Inside mcp2515 message error irq\n");

    for (box = 0; box < MCP2515_TX_MAILBOXES; box++) {
        if (mcp2515_tx_err_occurred(dev, box)) {
            DEBUG("Box: %d\n", box);

            if (mutex_trylock(&_mcp_mutex)) {
                mcp2515_abort(dev, box);
                mutex_unlock(&_mcp_mutex);
            }
            else {
                DEBUG("irqmsg_Failed to lock mutex\n");
                return;
            }
            _send_event(dev, CANDEV_EVENT_TIMEOUT_TX_CONF,
                        NULL);

        }
    }
}

static void _irq_wakeup(const candev_mcp2515_t *dev)
{
    DEBUG("Inside mcp2515 wakeup irq\n");

    _send_event(dev, CANDEV_EVENT_WAKE_UP, NULL);
}

static void _send_event(const candev_mcp2515_t *dev, candev_event_t event,
                        void *arg)
{
    candev_t *candev = (candev_t *)dev;

    if (candev->event_callback) {
        candev->event_callback(candev, event, arg);
    }
}
