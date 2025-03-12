/*
 * Copyright (C) 2016 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @ingroup drivers_candev_linux
 * @brief   Implementation of simulated CAN controller driver using SocketCAN on Linux
 * @author  Hermann Lelong <hermann@otakeys.com>
 * @author  Aurelien Gonce <aurelien.gonce@altran.com>
 * @author  Vincent Dupont <vincent@otakeys.com>
 */

#if !defined(__linux__)
#error "Usage of periph_can on RIOT native requires libsocketcan. Currently \
        this is only available on Linux"
#else

#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include <linux/can/raw.h>
#include <linux/can/error.h>

#include "native_internal.h"
#include "can/device.h"
#include "candev_linux.h"
#include "thread.h"
#include "mutex.h"
#include "async_read.h"
#include "sched.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static int _init(candev_t *candev);
static int _send(candev_t *candev, const can_frame_t *frame);
static void _isr(candev_t *candev);
static int _set(candev_t *candev, canopt_t opt, void *value, size_t value_len);
static int _get(candev_t *candev, canopt_t opt, void *value, size_t max_len);
static int _abort(candev_t *candev, const can_frame_t *frame);
static int _set_filter(candev_t *candev, const struct can_filter *filter);
static int _remove_filter(candev_t *candev, const struct can_filter *filter);
static int _power_up(candev_t *candev);
static int _power_down(candev_t *candev);

static int _set_bittiming(can_t *dev, struct can_bittiming *bittiming);

static const candev_driver_t candev_linux_driver = {
    .send = _send,
    .init = _init,
    .isr = _isr,
    .get = _get,
    .set = _set,
    .abort = _abort,
    .set_filter = _set_filter,
    .remove_filter = _remove_filter,
};

static candev_event_t _can_error_to_can_evt(can_frame_t can_frame_err);
static void _callback_can_sigio(int sock, void *arg);

can_conf_t candev_conf[CAN_DLL_NUMOF] = {
#if CAN_DLL_NUMOF >= 1
   {
        .interface_name = "vcan0",
   },
#endif
#if CAN_DLL_NUMOF >= 2
   {
        .interface_name = "vcan1",
   }
#endif
};

int can_init(can_t *dev, const can_conf_t *conf)
{
    memset(dev, 0, sizeof(can_t));
    dev->candev.driver = &candev_linux_driver;
    dev->conf = conf;
    dev->candev.bittiming.bitrate = CANDEV_LINUX_DEFAULT_BITRATE;
    dev->candev.bittiming.sample_point = CANDEV_LINUX_DEFAULT_SPT;

    return 0;
}

static candev_event_t _can_error_to_can_evt(can_frame_t can_frame_err)
{
    candev_event_t can_evt = CANDEV_EVENT_NOEVENT;
    can_err_mask_t can_err_type = can_frame_err.can_id & CAN_ERR_MASK;

    if (can_err_type & CAN_ERR_TX_TIMEOUT) {
        can_evt = CANDEV_EVENT_TX_ERROR;
    }
    else if (can_err_type & CAN_ERR_CRTL) {
        switch (can_frame_err.data[1]) {
        case CAN_ERR_CRTL_RX_OVERFLOW:
            can_evt = CANDEV_EVENT_RX_ERROR;
            break;
        case CAN_ERR_CRTL_TX_OVERFLOW:
            can_evt = CANDEV_EVENT_TX_ERROR;
            break;
        case CAN_ERR_CRTL_RX_PASSIVE:
        case CAN_ERR_CRTL_TX_PASSIVE:
            can_evt = CANDEV_EVENT_ERROR_PASSIVE;
            break;
        case CAN_ERR_CRTL_RX_WARNING:
        case CAN_ERR_CRTL_TX_WARNING:
            can_evt = CANDEV_EVENT_ERROR_WARNING;
            break;
        }
    }
    else if (can_err_type & CAN_ERR_BUSOFF) {
        can_evt = CANDEV_EVENT_BUS_OFF;
    }

    return can_evt;
}

static void _callback_can_sigio(int sockfd, void *arg)
{
    (void) sockfd;
    can_t *dev = (can_t *) arg;

    if (dev->candev.event_callback) {
        dev->candev.event_callback(&dev->candev, CANDEV_EVENT_ISR, NULL);
    }

    native_async_read_continue(sockfd);

    if (sched_context_switch_request) {
        thread_yield_higher();
    }
}

static int _init(candev_t *candev)
{
    struct sockaddr_can addr;
    struct ifreq ifr;
    int ret;

    DEBUG("Will start linux CAN init\n");
    can_t *dev = (can_t *)candev;

    if ((strlen(dev->conf->interface_name) == 0)
            || (strlen(dev->conf->interface_name) > CAN_MAX_SIZE_INTERFACE_NAME)) {
        real_printf("Error: Invalid can iface, too short or too long \n");
        return -1;
    }

    dev->sock = real_socket(PF_CAN, SOCK_RAW, CAN_RAW);

    if (dev->sock < 0) {
        real_printf("CAN config KO, socket nr = %i \n", dev->sock);
        return -1;
    }

    can_err_mask_t err_mask = CAN_ERR_TX_TIMEOUT |
                              CAN_ERR_BUSOFF |
                              CAN_ERR_CRTL;
    ret = real_setsockopt(dev->sock, SOL_CAN_RAW,
#ifdef MODULE_FDCAN
                          CAN_RAW_FD_FRAMES |
#endif
                          CAN_RAW_ERR_FILTER,
                          &err_mask, sizeof(err_mask));

    if (ret < 0) {
        real_printf("Error: setsockopt failed\n");
        real_close(dev->sock);
        return -1;
    }

    strcpy(ifr.ifr_name, dev->conf->interface_name);
    ret = real_ioctl(dev->sock, SIOCGIFINDEX, &ifr);

    if (ret < 0) {
        real_printf("Error: Invalid can iface %s\n", dev->conf->interface_name);
        real_close(dev->sock);
        return -1;
    }

    native_async_read_setup();
    /* This func will also automatically configure socket to be asynchronous */
    /* and to activate SIGIO */
    native_async_read_add_handler(dev->sock, (void *) dev, _callback_can_sigio);

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    real_bind(dev->sock, (struct sockaddr *)&addr, sizeof(addr));

    /* Only set bitrate on real can interfaces.
     * Not supported on virtual can interfaces ("vcanX") */
    if (strncmp(dev->conf->interface_name, "can", strlen("can"))) {
        DEBUG("not setting bitrate on virtual can interface %s\n", dev->conf->interface_name);
    }
    else {
        _set_bittiming(dev, &candev->bittiming);
    }

    DEBUG("CAN linux device ready\n");

    return 0;
}

static int _send(candev_t *candev, const can_frame_t *frame)
{
    int nbytes;
    can_t *dev = (can_t *)candev;

    nbytes = real_write(dev->sock, frame, sizeof(can_frame_t));

    if (nbytes < frame->len) {
        real_printf("CAN write op failed, nbytes=%i\n", nbytes);
        return -1;
    }

    if (dev->candev.event_callback) {
        dev->candev.event_callback(&dev->candev, CANDEV_EVENT_TX_CONFIRMATION, (void *)frame);
    }

    return 0;
}

static void _isr(candev_t *candev)
{
    int nbytes;
    can_frame_t rcv_frame;
    can_t *dev = (can_t *)candev;

    if (dev == NULL) {
        return;
    }

    DEBUG("candev_native _isr: CAN SIGIO interrupt received, sock = %i\n", dev->sock);
    nbytes = real_read(dev->sock, &rcv_frame, sizeof(can_frame_t));

    if (nbytes < 0) {   /* SIGIO signal was probably due to an error with the socket */
        DEBUG("candev_native _isr: read: error during read\n");
        return;
    }

    if (nbytes < (int)sizeof(can_frame_t)) {
        DEBUG("candev_native _isr: read: incomplete CAN frame\n");
        return;
    }

    if (rcv_frame.can_id & CAN_ERR_FLAG) {
        DEBUG("candev_native _isr: error frame\n");
        candev_event_t evt = _can_error_to_can_evt(rcv_frame);
        if ((evt != CANDEV_EVENT_NOEVENT) && (dev->candev.event_callback)) {
            dev->candev.event_callback(&dev->candev, evt, NULL);
        }
        return;
    }

    if (rcv_frame.can_id & CAN_RTR_FLAG) {
        DEBUG("candev_native _isr: rtr frame\n");
        return;
    }

    if (dev->candev.event_callback) {
        DEBUG("candev_native _isr: calling event callback\n");
        dev->candev.event_callback(&dev->candev, CANDEV_EVENT_RX_INDICATION, &rcv_frame);
    }

}

static int _set_bittiming(can_t *dev, struct can_bittiming *bittiming)
{
    int res;

    dev->candev.bittiming = *bittiming;

    DEBUG("bitrate = %d, brp= %d, phase_seg1 = %d, phase_seg2 = %d, sjw = %d\n",
          dev->candev.bittiming.bitrate, dev->candev.bittiming.brp,
          dev->candev.bittiming.phase_seg1, dev->candev.bittiming.phase_seg2,
          dev->candev.bittiming.sjw);

    /* bitrate setting */
    DEBUG("_set: setting %s down\n", dev->conf->interface_name);
    res = can_do_stop(dev->conf->interface_name);
    if (res < 0) {
        return res;
    }
    DEBUG("_set: setting bittiming to %s\n", dev->conf->interface_name);
    res = can_set_bitrate(dev->conf->interface_name, dev->candev.bittiming.bitrate);
    can_get_bittiming(dev->conf->interface_name, &dev->candev.bittiming);
    DEBUG("_set: setting %s up\n", dev->conf->interface_name);
    can_do_start(dev->conf->interface_name);

    return res;
}

static int _set(candev_t *candev, canopt_t opt, void *value, size_t value_len)
{
    can_t *dev = (can_t *) candev;
    int res = 0;

    switch (opt) {
    case CANOPT_BITTIMING:
        DEBUG("candev_linux: CANOPT_BITTIMING\n");

        /* Only set bitrate on real can interfaces.
         * Not supported on virtual can interfaces ("vcanX") */
        if (strncmp(dev->conf->interface_name, "can", strlen("can"))) {
            DEBUG("candev_native: _set: error interface is not real can\n");
            return -EINVAL;
        }

        if (value == NULL) {
            DEBUG("candev_native: _set: error value NULL\n");
            return -EOVERFLOW;
        }

        if (value_len < sizeof(struct can_bittiming)) {
            DEBUG("candev_native: _set: error size of bittiming\n");
            return -EOVERFLOW;
        }

        res = _set_bittiming(dev, value);

        break;
    case CANOPT_STATE:
        switch (*((canopt_state_t *)value)) {
            case CANOPT_STATE_SLEEP:
            case CANOPT_STATE_OFF:
                _power_down(candev);
                break;
            default:
                _power_up(candev);
                break;
        }
        break;
    default:
        DEBUG("CAN set, not supported opt\n");
        res = -ENOTSUP;
        break;
    }

    return res;
}

static int _get(candev_t *candev, canopt_t opt, void *value, size_t max_len)
{
    can_t *dev = (can_t *) candev;
    int res = 0;

    switch (opt) {
    case CANOPT_BITTIMING:
        if (max_len < sizeof(struct can_bittiming)) {
            res = -EOVERFLOW;
            break;
        }
        if (value == NULL) {
            res = -EINVAL;
            break;
        }
        if (can_get_bittiming(dev->conf->interface_name, value) == 0) {
            res = sizeof(struct can_bittiming);
        }
        else {
            res = -ENOTSUP;
        }
        break;
    case CANOPT_BITTIMING_CONST:
        if (max_len < sizeof(struct can_bittiming_const)) {
            res = -EOVERFLOW;
            break;
        }
        if (value == NULL) {
            res = -EINVAL;
            break;
        }
        if (can_get_bittiming_const(dev->conf->interface_name, value) == 0) {
            res = sizeof(struct can_bittiming_const);
        }
        else {
            res = -ENOTSUP;
        }
        break;
    case CANOPT_CLOCK:
        if (max_len < sizeof(uint32_t)) {
            res = -EOVERFLOW;
            break;
        }
        if (value == NULL) {
            res = -EINVAL;
            break;
        }
        {
        struct can_clock clock;
        if (can_get_clock(dev->conf->interface_name, &clock) == 0) {
            *((uint32_t *)value) = clock.freq;
            res = sizeof(uint32_t);
        }
        else {
            res = -ENOTSUP;
        }
        }
        break;
    case CANOPT_TEC:
    case CANOPT_REC:
        if (max_len < sizeof(uint16_t)) {
            res = -EOVERFLOW;
            break;
        }
        if (value == NULL) {
            res = -EINVAL;
            break;
        }
        {
        struct can_berr_counter bc;
        if (can_get_berr_counter(dev->conf->interface_name, &bc) == 0) {
            if (opt == CANOPT_TEC) {
                *((uint16_t *)value) = bc.txerr;
            }
            else {
                *((uint16_t *)value) = bc.rxerr;
            }
            res = sizeof(uint16_t);
        }
        else {
            res = -ENOTSUP;
        }
        }
        break;
    case CANOPT_RX_FILTERS: {
        if (max_len % sizeof(struct can_filter) != 0) {
            res = -EOVERFLOW;
            break;
        }
        struct can_filter *list = value;
        size_t i;
        for (i = 0; (i < CANDEV_LINUX_MAX_FILTERS_RX)
             && (dev->filters[i].can_id != 0)
             && (i < (max_len / sizeof(struct can_filter))); i++) {
            list[i] = dev->filters[i];
        }
        res = i * sizeof(struct can_filter);
        break; }
    default:
        DEBUG("CAN get, not supported op\n");
        res = -ENOTSUP;
        break;
    }

    return res;
}

static int _set_filter(candev_t *candev, const struct can_filter *filter)
{
    can_t *dev = (can_t *)candev;

    if (filter == NULL) {
        DEBUG("candev_native: _set_filter: error filter NULL\n");
        return -EOVERFLOW;
    }

    DEBUG("candev_native: _set_filter: candev=%p, filter: f=%x m=%x on sock: %i\n",
          (void *)candev, filter->can_id, filter->can_mask, dev->sock
    );
    uint32_t i;
    for (i = 0; i < CANDEV_LINUX_MAX_FILTERS_RX; i++) {
        if (dev->filters[i].can_id == filter->can_id) {
            DEBUG("candev_native: _set_filter: filter already set\n");
            return 0;
        }
        else if (dev->filters[i].can_id == 0) {
            break;
        }
    }
    if (i == CANDEV_LINUX_MAX_FILTERS_RX) {
        DEBUG("candev_native: _set_filter: no more filters available\n");
        return -EOVERFLOW;
    }
    for (i = 0; i < CANDEV_LINUX_MAX_FILTERS_RX; i++) {
        if (dev->filters[i].can_id == 0) {
            /* Only 29 bits must be used for masks in SocketCAN */
            dev->filters[i] = *filter;
            dev->filters[i].can_mask &= CAN_EFF_MASK;
            DEBUG("candev_native: _set_filter: filter:ID=0x%x\n", filter->can_id);
            DEBUG("candev_native: _set_filter: mask=0x%x\n", filter->can_mask);
            break;
        }
    }
    i++;
    DEBUG("%" PRIu32 " filters will be set\n", i);
    real_setsockopt(dev->sock, SOL_CAN_RAW, CAN_RAW_FILTER, dev->filters,
                    sizeof(struct can_filter) * i);

    return i;
}

static int _remove_filter(candev_t *candev, const struct can_filter *filter)
{
    can_t *dev = (can_t *)candev;

    if (filter == NULL) {
        DEBUG("candev_native: _remove_filter: error filter NULL\n");
        return -EOVERFLOW;
    }

    uint32_t i;
    for (i = 0; i < CANDEV_LINUX_MAX_FILTERS_RX; i++) {
        if ((dev->filters[i].can_id == filter->can_id )
                && (dev->filters[i].can_mask == (filter->can_mask & CAN_EFF_MASK))) {
            if (i < CANDEV_LINUX_MAX_FILTERS_RX - 1) {
                memmove(&dev->filters[i], &dev->filters[i + 1],
                        sizeof(dev->filters[i]) * (CANDEV_LINUX_MAX_FILTERS_RX - i - 1));
            }
            dev->filters[CANDEV_LINUX_MAX_FILTERS_RX - 1].can_id = 0;
            break;

        }
        else if (dev->filters[i].can_id == 0) {
            DEBUG("candev_native: _remove_filter: error filter not found\n");
            return -EOVERFLOW;
        }
    }

    if (i == CANDEV_LINUX_MAX_FILTERS_RX) {
        DEBUG("candev_native: _remove_filter: error filter not found\n");
        return -EOVERFLOW;
    }

    for (i = 0; i < CANDEV_LINUX_MAX_FILTERS_RX; i++) {
        if (dev->filters[i].can_id == 0) {
            break;
        }
    }

    DEBUG("%" PRIu32 " filters will be set\n", i);
    real_setsockopt(dev->sock, SOL_CAN_RAW, CAN_RAW_FILTER, dev->filters,
                    sizeof(struct can_filter) * i);

    return 0;
}

static int _abort(candev_t *candev, const can_frame_t *frame)
{
    (void)frame;
    (void)candev;

    return 0;
}

static int _power_down(candev_t *candev)
{
    (void)candev;

    return 0;
}

static int _power_up(candev_t *candev)
{
    (void)candev;

    return 0;
}
#endif   /* defined(__linux__) */
