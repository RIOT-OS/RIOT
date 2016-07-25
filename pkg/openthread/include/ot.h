/*
 * Copyright (C) 2016 José Ignacio Alamos
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    pkg_openthread_cli   OpenThread
 * @ingroup     pkg_openthread
 * @brief       An open source implementation of Thread stack
 * @see         https://github.com/openthread/openthread
 *
 * Thread if a mesh oriented network stack running for IEEE802.15.4 networks.
 * It can be used with the @ref conn.
 * @{
 *
 * @file
 *
 * @author  José Ignacio Alamos <jialamos@uc.cl>
 */

#ifndef OT_H
#define OT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "net/netopt.h"
#include "net/ieee802154.h"
#include "net/ethernet.h"
#include "net/netdev2.h"
#include "thread.h"

#define OPENTHREAD_XTIMER_MSG_TYPE_EVENT (0x2235) /**< xtimer message receiver event*/
#define OPENTHREAD_NETDEV2_MSG_TYPE_EVENT (0x2236) /**< message received from driver */
#define OPENTHREAD_SERIAL_MSG_TYPE_EVENT (0x2237) /**< event indicating a serial (UART) message was sent to OpenThread */
#define OPENTHREAD_MSG_TYPE_RECV (0x2238) /**< event for frame reception */

/**
 *  * @brief   Struct containing a serial message
 *  
 */
typedef struct {
	void *buf; /**< buffer containing the message */
	size_t len; /**< length of the message */
} serial_msg_t;

void recv_pkt(netdev2_t *dev);
void send_pkt(netdev2_t *dev, netdev2_event_t event);
void openthread_bootstrap(void);
void openthread_radio_init(netdev2_t *dev, uint8_t *tb, uint8_t *rb);
void *_openthread_event_loop(void *arg);
int openthread_netdev2_init(char *stack, int stacksize, char priority, const char *name, netdev2_t *netdev);

void _event_cb(netdev2_t *dev, netdev2_event_t event);
kernel_pid_t openthread_get_pid(void);
void begin_mutex(void);
void end_mutex(void);
void ot_random_init(void);
#ifdef __cplusplus
}
#endif

#endif
/** @} */
