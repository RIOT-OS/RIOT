/*
 * Copyright (C) 2017 Fundación Inria Chile
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
 * @{
 *
 * @file
 *
 * @author  JosÃ© Ignacio Alamos <jialamos@uc.cl>
 */

#ifndef OT_H
#define OT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "net/netopt.h"
#include "net/ieee802154.h"
#include "net/ethernet.h"
#include "net/gnrc/netdev.h"
#include "thread.h"
#include "openthread/types.h"

#define OPENTHREAD_XTIMER_MSG_TYPE_EVENT (0x2235)   /**< xtimer message receiver event*/
#define OPENTHREAD_NETDEV_MSG_TYPE_EVENT (0x2236)   /**< message received from driver */
#define OPENTHREAD_SERIAL_MSG_TYPE_EVENT (0x2237)   /**< event indicating a serial (UART) message was sent to OpenThread */
#define OPENTHREAD_MSG_TYPE_RECV (0x2238)           /**< event for frame reception */
#define OPENTHREAD_JOB_MSG_TYPE_EVENT (0x2240)      /**< event indicating an OT_JOB message */

typedef void OT_JOB;

/**
 * @brief   Struct containing a serial message
 *
 */
typedef struct {
    void *buf;  /**< buffer containing the message */
    size_t len; /**< length of the message */
} serial_msg_t;

/**
 * @brief   Struct containing an OpenThread job
 */
typedef struct {
    void (*function)(otInstance*, void*);   /**< function to be called when executing job */
    void *context;                          /**< context for the job **/
} ot_job_t;

/**
 * @brief Gets packet from driver and tells OpenThread about the reception.
 *
 * @param[in]  aInstance          pointer to an OpenThread instance
 */
void recv_pkt(otInstance *aInstance, netdev_t *dev);

/**
 * @brief Gets packet from driver and tells OpenThread about the reception.
 *
 * @param[in]  aInstance          pointer to an OpenThread instance
 */

/**
 * @brief   Inform OpenThread when tx is finished
 *
 * @param[in]  aInstance          pointer to an OpenThread instance
 * @param[in]  dev                pointer to a netdev interface
 * @param[in]  event              just occurred netdev event
 */
void send_pkt(otInstance *aInstance, netdev_t *dev, netdev_event_t event);

/**
 * @brief   Bootstrap OpenThread
 */
void openthread_bootstrap(void);

/**
 * @brief   Init OpenThread radio
 *
 * @param[in]  dev                pointer to a netdev interface
 * @param[in]  tb                 pointer to the TX buffer designed for OpenThread
 * @param[in]  event              pointer to the RX buffer designed for Open_Thread
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
 * @brief   get PID of OpenThread thread.
 *
 * @return  PID of OpenThread thread
 */
kernel_pid_t openthread_get_pid(void);

/**
 * @brief   Init OpenThread random
 */
void ot_random_init(void);

#if defined(MODULE_OPENTHREAD_CLI) || defined(MODULE_OPENTHREAD_NCP)
/*
 * @brief   Run OpenThread UART simulator (stdio)
 */
void openthread_uart_run(void);

#endif

/**
 * @brief   Execute OpenThread job in same thread as OT core (dute to concurrency).
 *
 * @note    An OpenThread job allows direct calls to OpenThread API (otXXX functions) without worrying about concurrency
 * issues. All API calls should be made in OT_JOB type functions.
 *
 * @param[in]   job      callback pointer to an OpenThread job function
 * @param[in]   context     context for the job
 */
void ot_exec_job(OT_JOB (*job)(otInstance*, void*), void *context);

#ifdef __cplusplus
}
#endif

#endif
/** @} */
