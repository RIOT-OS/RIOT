/*
 * Copyright (C) 2015 Attilio Dona'
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @author      Attilio Dona'
 *
 */

/**
 * @defgroup    net_gnrc_nbapp NBAPP
 * @ingroup     net_gnrc
 * @brief       GNRC's implementation of the South Bound App protocol
 *
 */

#ifndef SYS_INCLUDE_NET_SBAPP_H_
#define SYS_INCLUDE_NET_SBAPP_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Default message queue size for the NBAPP thread
 */
#ifndef GNRC_NBAPP_MSG_QUEUE_SIZE
#define GNRC_NBAPP_MSG_QUEUE_SIZE (8U)
#endif

/**
 * @brief   Priority of the NBAPP thread
 */
#ifndef GNRC_NBAPP_PRIO
#define GNRC_NBAPP_PRIO           (THREAD_PRIORITY_MAIN - 2)
#endif

/**
 * @brief   Default stack size to use for the UDP thread
 */
#ifndef GNRC_NBAPP_STACK_SIZE
#define GNRC_NBAPP_STACK_SIZE     (THREAD_STACKSIZE_DEFAULT)
#endif

/**
 * south bound app API
 */

/**
 *  @brief setup ip connection with an AP through the 802.11 interface
 *
 *  @return  PID of the UDP thread
 *  @return  negative value on error
 */
int sbapp_init(void);

/**
 * @brief tcp connect to a tcp server and tcpport @p port)
 *
 * @param[in] server the hostname or ip address of the server
 * @param[in] port
 *
 * @return the demux context
 */
uint32_t sbapp_connect(const char* server, uint16_t port);


#ifdef __cplusplus
}
#endif


#endif /* SYS_INCLUDE_NET_SBAPP_H_ */
