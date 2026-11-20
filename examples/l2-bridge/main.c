/*
 * Copyright (C) 2024 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */


#include "net/ethernet.h"
#include "net/netdev.h"

#include "event/callback.h"
#include "event/thread.h"

#include "shell.h"

#define ENABLE_DEBUG 0
#include "debug.h"

typedef struct _bridge_port bridge_port_t;
struct _bridge_port {
    netdev_t *dev;                  /**< netdev associated with port */
    event_callback_t irq_handler;   /**< interrupt handler event     */
    bridge_port_t *next;            /**< port list node              */
#if ENABLE_DEBUG
    const char *name;               /**< port name (for debugging)   */
#endif
};

static inline const char *_port_name(const bridge_port_t *port)
{
#if ENABLE_DEBUG
    return port->name;
#else
    (void)port;
    return NULL;
#endif
}

static void _handle_irq(void *ctx)
{
    netdev_t *dev = ctx;
    dev->driver->isr(dev);
}

static void _event_cb(netdev_t *dev, netdev_event_t event)
{
    bridge_port_t *port = dev->context;

    switch (event) {
    case NETDEV_EVENT_ISR:
        event_post(EVENT_PRIO_HIGHEST, &port->irq_handler.super);
        break;
    case NETDEV_EVENT_RX_COMPLETE:
        DEBUG("%s: RX frame\n", _port_name(port));

        static uint8_t frame_buf[ETHERNET_FRAME_LEN];
        iolist_t frame = {
            .iol_base = frame_buf,
        };

        frame.iol_len = dev->driver->recv(dev, frame_buf, sizeof(frame_buf), NULL);
        if ((int)frame.iol_len <= 0) {
            DEBUG("%s: recv returned %d\n", _port_name(port), (int)frame.iol_len);
            break;
        }

        for (bridge_port_t *next_port = port->next;
             next_port != port; next_port = next_port->next) {
            assert(next_port);
            netdev_t *portdev = next_port->dev;
            DEBUG("%s: send %u byte frame\n", _port_name(next_port), frame.iol_len);
            portdev->driver->send(portdev, &frame);
        }

        break;
    default:
        break;
    }
}

static bridge_port_t *_init_netdev(bridge_port_t *port, netdev_t *dev,
                                   bridge_port_t *chain, const char *name)
{
    port->dev = dev;
#if ENABLE_DEBUG
    port->name = name;
#else
    (void)name;
#endif

    event_callback_init(&port->irq_handler, _handle_irq, dev);
    dev->event_callback = _event_cb;
    dev->context = port;

    dev->driver->init(dev);

    const netopt_enable_t enable = NETOPT_ENABLE;
    while (-EBUSY == (dev->driver->set(dev, NETOPT_PROMISCUOUSMODE,
                                       &enable, sizeof(enable)))) {}
    if (chain) {
        bridge_port_t *tmp = chain->next;
        chain->next = port;
        port->next = tmp;
    } else {
        port->next = port;
    }

    return port;
}

#define _SKIP_INIT(name)                                        \
static bridge_port_t *_init_ ## name (bridge_port_t *prev)      \
{                                                               \
    return prev;                                                \
}

#ifdef MODULE_ATWINC15X0
#include "atwinc15x0.h"
#include "atwinc15x0_params.h"

#define ATWINC15X0_NUM ARRAY_SIZE(atwinc15x0_params)

static bridge_port_t *_init_atwinc15x0(bridge_port_t *prev)
{
    static atwinc15x0_t dev[ATWINC15X0_NUM];
    static bridge_port_t port[ATWINC15X0_NUM];

    for (unsigned i = 0; i < ATWINC15X0_NUM; i++) {
        atwinc15x0_setup(&dev[i], &atwinc15x0_params[i], i);
        prev = _init_netdev(&port[i], &dev[i].netdev, prev, "ATWINC15X0");
    }

    return prev;
}
#else
_SKIP_INIT(atwinc15x0)
#endif

#ifdef MODULE_DOSE
#include "dose.h"
#include "dose_params.h"

#define DOSE_NUM ARRAY_SIZE(dose_params)

static bridge_port_t *_init_dose(bridge_port_t *prev)
{
    static dose_t dev[DOSE_NUM];
    static bridge_port_t port[DOSE_NUM];

    for (unsigned i = 0; i < DOSE_NUM; i++) {
        dose_setup(&dev[i], &dose_params[i], i);
        prev = _init_netdev(&port[i], &dev[i].netdev, prev, "DOSE");
    }

    return prev;
}
#else
_SKIP_INIT(dose)
#endif

#ifdef MODULE_ETHOS
#include "ethos.h"
#include "ethos_params.h"

#define ETHOS_NUM ARRAY_SIZE(ethos_params)

static bridge_port_t *_init_ethos(bridge_port_t *prev)
{
    static ethos_t dev[ETHOS_NUM];
    static bridge_port_t port[ETHOS_NUM];
    static uint8_t inbuf[ETHOS_NUM][2048];

    for (unsigned i = 0; i < ETHOS_NUM; i++) {
        ethos_setup(&dev[i], &ethos_params[i], i, &inbuf[i], sizeof(inbuf[i]));
        prev = _init_netdev(&port[i], &dev[i].netdev, prev, "ETHOS");
    }

    return prev;
}
#else
_SKIP_INIT(ethos)
#endif

#ifdef MODULE_USBUS_CDC_ECM
#include "usb/usbus/cdc/ecm.h"
static bridge_port_t *_init_cdcecm(bridge_port_t *prev)
{
    /* global cdc ecm object, declared in the usb auto init file */
    extern usbus_cdcecm_device_t cdcecm;
    extern void cdcecm_netdev_setup(usbus_cdcecm_device_t *cdcecm);

    static bridge_port_t port;
    cdcecm_netdev_setup(&cdcecm);
    return _init_netdev(&port, &cdcecm.netdev, prev, "CDC ECM");
}
#else
_SKIP_INIT(cdcecm)
#endif

#ifdef MODULE_SAM0_ETH
#include "sam0_eth_netdev.h"
static bridge_port_t *_init_sam0_eth(bridge_port_t *prev)
{
    static netdev_t sam0eth;
    static bridge_port_t port;

    sam0_eth_setup(&sam0eth);
    return _init_netdev(&port, &sam0eth, prev, "SAM0 ETH");
}
#else
_SKIP_INIT(sam0_eth)
#endif

int main(void)
{
    bridge_port_t *root = NULL;
    root = _init_atwinc15x0(root);
    root = _init_cdcecm(root);
    root = _init_dose(root);
    root = _init_ethos(root);
    root = _init_sam0_eth(root);

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
