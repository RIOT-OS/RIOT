/*
 * Copyright (C) 2017 Fundacion Inria Chile
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    pkg_openthread_cli   OpenThread
 * @ingroup     pkg_openthread
 * @brief       An open source implementation of Thread stack
 * @see         https://github.com/openthread/openthread
 *
 * Thread is a mesh oriented network stack running for IEEE802.15.4 networks.
 *
 * The RIOT port allows to directly call OpenThread API functions using
 * @ref sys_event. For example:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * #include "ot.h"
 * #include "openthread/thread.h"
 *
 * static void _panid_handler(event_t *event);
 * static event_t event_panid = {
 *     .handler = _panid_handler
 * };
 *
 * static void _panid_handler(event_t *event)
 * {
 *     (void) event;
 *     uint16_t panid = otLinkGetPanId(openthread_get_instance());
 *     do_something_with_panid(panid);
 * }
 *
 * int main(void)
 * {
 *     event_post(openthread_get_evq(), &event_panid);
 *     return 0;
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @see https://openthread.io/releases/thread-reference-20180619
 *
 * @{
 *
 * @file
 *
 * @author      Jose Ignacio Alamos <jialamos@uc.cl>
 * @author      Baptiste Clenet <bapclenet@gmail.com>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "net/netopt.h"
#include "net/ieee802154.h"
#include "net/ethernet.h"
#include "net/netdev.h"
#include "thread.h"
#include "openthread/instance.h"
#include "event.h"

/**
 * @name    Openthread constants
 * @{
 */
/** @brief   number of serial reception buffer */
#define OPENTHREAD_NUMBER_OF_SERIAL_BUFFER                  (1U)
/** @brief   sizeof in bytes the two first members of she serial structure */
#define OPENTHREAD_SIZEOF_LENGTH_AND_FREEBUFF               (4U)
/** @brief   sizeof the serial buffer */
#define OPENTHREAD_SERIAL_BUFFER_SIZE                       OPENTHREAD_SIZEOF_LENGTH_AND_FREEBUFF + 100
/** @brief   sizeof the spinel payload data */
#define OPENTHREAD_SERIAL_BUFFER__PAYLOAD_SIZE              OPENTHREAD_SERIAL_BUFFER_SIZE - OPENTHREAD_SIZEOF_LENGTH_AND_FREEBUFF
/** @brief   error when no more buffer available */
#define OPENTHREAD_ERROR_NO_EMPTY_SERIAL_BUFFER             -1
/** @brief   serial buffer ready to use */
#define OPENTHREAD_SERIAL_BUFFER_STATUS_FREE                (0x0001)
/** @brief   serial buffer ready for processing */
#define OPENTHREAD_SERIAL_BUFFER_STATUS_READY_TO_PROCESS    (0x0002)
/** @brief   serial buffer payload full */
#define OPENTHREAD_SERIAL_BUFFER_STATUS_FULL                (0x0004)
/** @brief   Max length for IEEE802154 frame */
#define IEEE802154_MAX_LENGTH                               (127U)
/** @brief   Max length for a netdev buffer  */
#define OPENTHREAD_NETDEV_BUFLEN                            (IEEE802154_MAX_LENGTH)
/** @} */

/**
 * @brief   Struct containing a serial message
 */
typedef struct {
    uint16_t length;                                        /**< length of the message */
    uint16_t serial_buffer_status;                          /**< status of the buffer */
    uint8_t buf[OPENTHREAD_SERIAL_BUFFER__PAYLOAD_SIZE];    /**< buffer containing the message */
} serial_msg_t;

/**
 * @brief Gets packet from driver and tells OpenThread about the reception.
 *
 * @param[in]  aInstance          pointer to an OpenThread instance
 * @param[in]  dev                pointer to a netdev instance
 */
void recv_pkt(otInstance *aInstance, netdev_t *dev);

/**
 * @brief   Inform OpenThread when tx is finished
 *
 * @param[in]  aInstance          pointer to an OpenThread instance
 * @param[in]  dev                pointer to a netdev interface
 * @param[in]  event              just occurred netdev event
 */
void send_pkt(otInstance *aInstance, netdev_t *dev, netdev_event_t event);

/**
 * @brief Get OpenThread event queue
 *
 * @return pointer to the event queue
 */
event_queue_t *openthread_get_evq(void);

/**
 * @brief Get pointer to the OpenThread instance
 *
 * @return pointer to the OpenThread instance
 */
otInstance* openthread_get_instance(void);

/**
 * @brief   Bootstrap OpenThread
 */
void openthread_bootstrap(void);

/**
 * @brief   Init OpenThread radio
 *
 * @param[in]  dev                pointer to a netdev interface
 * @param[in]  tb                 pointer to the TX buffer designed for OpenThread
 * @param[in]  rb                 pointer to the RX buffer designed for Open_Thread
 */
void openthread_radio_init(netdev_t *dev, uint8_t *tb, uint8_t *rb);

/**
 * @brief   Starts OpenThread thread.
 *
 * @param[in]  stack              pointer to the stack designed for OpenThread
 * @param[in]  stacksize          size of the stack
 * @param[in]  priority           priority of the OpenThread stack
 * @param[in]  name               name of the OpenThread stack
 * @param[in]  netdev             pointer to the netdev interface
 *
 * @return  PID of OpenThread thread
 * @return  -EINVAL if there was an error creating the thread
 */
int openthread_netdev_init(char *stack, int stacksize, char priority, const char *name, netdev_t *netdev);

/**
 * @brief   Init OpenThread random
 */
void ot_random_init(void);

#ifdef __cplusplus
}
#endif

/** @} */
