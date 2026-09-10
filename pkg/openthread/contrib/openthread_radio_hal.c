/*
 * SPDX-FileCopyrightText: 2017 Fundacion Inria Chile
 * SPDX-FileCopyrightText: 2026 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 * @ingroup     net
 * @file
 * @brief       Radio HAL adoption for OpenThread
 *
 * @author      Jose Ignacio Alamos <jialamos@uc.cl>
 * @author      Baptiste Clenet <bapclenet@gmail.com>
 * @author      Moritz Voigt <moritz.voigt@mailbox.tu-dresden.de>
 * @}
 */

#define OT_NETWORK_KEY_SIZE 16

#include <ctype.h>
#include <errno.h>
#include <string.h>
#include "msg.h"
#include "openthread/instance.h"
#include "openthread/ip6.h"
#include "openthread/platform/alarm-milli.h"
#include "openthread/thread.h"
#include "random.h"
#include "ot.h"
#include "event.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static otInstance *sInstance;   /**< global OpenThread instance */
static ieee802154_dev_t *_dev;  /**< radio hal descriptor for OpenThread */
static event_queue_t ev_queue;  /**< the event queue for OpenThread */
static uint8_t *_tx_is_ack;

static int bytes_from_str(uint8_t *buf, int buf_len, const char *src)
{
    size_t i;
    size_t src_len = strlen(src);
    char *endptr;

    for (i = 0U; i < src_len; i++) {
        if (!isxdigit((unsigned char)src[i]) &&
            src[i] != ':') {
            return -EINVAL;
        }
    }

    (void)memset(buf, 0, buf_len);

    for (i = 0U; i < (size_t)buf_len; i++) {
        buf[i] = (uint8_t)strtol(src, &endptr, 16);
        src = ++endptr;
    }

    return 0;
}

static void _ev_recv_handler(event_t *event)
{
    (void)event;
    recv_pkt(sInstance);
}

static event_t ev_recv = {
    .handler = _ev_recv_handler
};

static void _ev_process_tx_done_handler(event_t *event)
{
    (void)event;
    process_tx_done(sInstance);
}

static event_t _ev_process_tx_done = {
    .handler = _ev_process_tx_done_handler
};

event_queue_t *openthread_get_evq(void)
{
    return &ev_queue;
}

otInstance *openthread_get_instance(void)
{
    return sInstance;
}

static void _hal_radio_cb(ieee802154_dev_t *dev, ieee802154_trx_ev_t status)
{
    switch (status) {
    case IEEE802154_RADIO_CONFIRM_TX_DONE:
        if (*_tx_is_ack == 1) {
            ieee802154_radio_confirm_transmit(dev, NULL);
            *_tx_is_ack = 0;
            break;
        }
        event_post(&ev_queue, &_ev_process_tx_done);
        break;
    case IEEE802154_RADIO_INDICATION_CRC_ERROR:
        /* Just drop the packet */
        while (ieee802154_radio_set_idle(dev, false) < 0) {}
        ieee802154_radio_read(dev, NULL, 0, NULL);
        break;
    case IEEE802154_RADIO_INDICATION_RX_DONE:
        event_post(&ev_queue, &ev_recv);
        break;
    default:
        break;
    }
}

static void _openthread_manual_config(otInstance *sInstance)
{
    /* Init default parameters */
    otPanId panid = OPENTHREAD_PANID;
    uint8_t channel = OPENTHREAD_CHANNEL;
    char *networkkey = OPENTHREAD_NETWORK_KEY;
    otNetworkKey otNetKey;

    otLinkSetChannel(sInstance, channel);
    otLinkSetPanId(sInstance, panid);

    bytes_from_str(otNetKey.m8, OT_NETWORK_KEY_SIZE, networkkey);
    otThreadSetNetworkKey(sInstance, &otNetKey);

    /* Init optional parameters */
#ifdef OPENTHREAD_NETWORK_NAME
    char *networkname = OPENTHREAD_NETWORK_NAME;
    otThreadSetNetworkName(sInstance, networkname);
#endif

#ifdef OPENTHREAD_MESH_PREFIX
    otMeshLocalPrefix otMeshLocalPrefix;
    char *meshprefix = OPENTHREAD_MESH_PREFIX;
    bytes_from_str(otMeshLocalPrefix.m8, OT_MESH_LOCAL_PREFIX_SIZE, meshprefix);
    otThreadSetMeshLocalPrefix(sInstance, &otMeshLocalPrefix);
#endif

#ifdef OPENTHREAD_EXT_PANID
    otExtendedPanId otExtPanId;
    char *extpanid = OPENTHREAD_EXT_PANID;
    bytes_from_str(otExtPanId.m8, OT_EXT_PAN_ID_SIZE, extpanid);
    otThreadSetExtendedPanId(sInstance, &otExtPanId);
#endif
}

static void *_openthread_event_loop(void *arg)
{
    otError error;

    _dev = arg;
    _dev->cb = _hal_radio_cb;

    event_queue_init(&ev_queue);

    /* Init OpenThread Instance */
    sInstance = otInstanceInitSingle();

#if defined(MODULE_OPENTHREAD_CLI_FTD) || defined(MODULE_OPENTHREAD_CLI_MTD)
    ot_shell_init(sInstance);
#endif

    /* Bring up the IPv6 interface  */
    error = otIp6SetEnabled(sInstance, true);

    /* Manually configure OpenThread requires -DOT_OPERATIONAL_DATASET_AUTO_INIT=1 */
    _openthread_manual_config(sInstance);

    /* Start Thread protocol operation */
    error = otThreadSetEnabled(sInstance, true);
    if (error != OT_ERROR_NONE) {
        printf("pkg/openthread: Error in initialization\n");
    }

#if OPENTHREAD_ENABLE_DIAG
    diagInit(sInstance);
#endif

    while (1) {
        event_loop(&ev_queue);
    }

    return NULL;
}

/* starts OpenThread thread */
int openthread_hal_init(char *stack, int stacksize, char priority,
                        const char *name, ieee802154_dev_t *dev, uint8_t *tx_is_ack)
{
    _tx_is_ack = tx_is_ack;
    if (thread_create(stack, stacksize,
                      priority, 0,
                      _openthread_event_loop, dev, name) < 0) {
        return -EINVAL;
    }

    return 0;
}
