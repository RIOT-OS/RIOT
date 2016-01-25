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
 * @ingroup     cpu_cc3200
 *
 * @file
 * @brief       implementation of the South Bound App protocol for CC3200
 *
 */

#ifndef SYS_INCLUDE_NET_SBAPP_H_
#define SYS_INCLUDE_NET_SBAPP_H_

#include "kernel.h"

#ifdef __cplusplus
extern "C" {
#endif


enum conn_type_t {
	TCP = 1,
	UDP
};

/**
 * @brief   Default message queue size for the NBAPP thread
 */
#ifndef SBAPP_MSG_QUEUE_SIZE
#define SBAPP_MSG_QUEUE_SIZE (8U)
#endif

/**
 * @brief   Priority of the NBAPP thread
 */
#ifndef SBAPP_PRIO
#define SBAPP_PRIO           (THREAD_PRIORITY_MAIN - 2)
#endif

/**
 * @brief   Default stack size to use for the UDP thread
 */
#ifndef SBAPP_STACK_SIZE
#define SBAPP_STACK_SIZE     (THREAD_STACKSIZE_DEFAULT)
#endif


/**
 * @brief @ref core_msg type for passing a @ref net_gnrc_pkt from sbapp up the app
 */
#define SBAPI_MSG_TYPE_RCV        (0x0301)

/**
 * @brief @ref core_msg type for passing a @ref net_gnrc_pkt to sbapp adapter
 */
#define SBAPI_MSG_TYPE_SND        (0x0302)

/**
 * @brief   @ref core_msg type for setting options of network modules
 */
#define SBAPI_MSG_TYPE_SET        (0x0303)

/**
 * @brief   @ref core_msg type for getting options from network modules
 */
#define SBAPI_MSG_TYPE_GET        (0x0304)

/**
 * @brief detected an error condition on the network channel (currently a tcp socket)
 */
#define SBAPI_MSG_TYPE_ERR             (0x0306)


/**
 * @brief the connection handler exited because of an error
 */
#define SBAPI_MSG_TYPE_HANDLER_EXIT    (0x307)


#define SBAPI_SEND_FAILED 1


typedef void* sbh_t;

/**
 * south bound app API methods
 */

/**
 *  @brief initialize an ip connection with a 802.11 AP
 *
 *  @return  PID of the UDP thread
 *  @return  negative value on error
 */
int sbapp_init(void);

/**
 * @brief connect to a TCP or UDP server
 *
 * @param[in] conn_type  TCP or UDP
 * @param[in] server the hostname or ip address of the server
 * @param[in] port
 *
 * @return the connection handle
 */
sbh_t sbapp_connect(uint8_t conn_type,
		const char* server, uint16_t port, kernel_pid_t pid);


/**
 *  @brief send data
 *
 *  @param[in] fd the connection handle
 *              (or think it is a file descriptor if you prefer)
 */
int sbapp_send(sbh_t fd, void* data, size_t len);


#ifdef __cplusplus
}
#endif


#endif /* SYS_INCLUDE_NET_SBAPP_H_ */
