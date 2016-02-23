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
 * @{
 *
 * @file
 * @brief       implementation of the South Bound App protocol for CC3200
 *
 */

#ifndef SYS_INCLUDE_NET_SBAPP_H_
#define SYS_INCLUDE_NET_SBAPP_H_

#include "kernel.h"
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/netapi.h"
#include "net/gnrc/netreg.h"
#include "net/gnrc/netif/hdr.h"
#include "socket.h"

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


/**
 * @brief network driver settings
 */
#define SBAPI_SMARTCONFIG   (0x1)
#define SBAPI_DEFAULT_RESET (0x2)

// 2 bits flag, numbers of connections attempt before invoking smartconfig if
// enabled
#define SBAPI_RETRIES         (2 << 2)
#define SBAPI_DELETE_PROFILES (0x10)

#define SBAPI_DEFAULT_CFG   (SBAPI_RETRIES|SBAPI_SMARTCONFIG)


/**
 * @brief configuration key items
 */
#define SBAPI_SSID      (0x01)
#define SBAPI_PWD       (0x02)


/**
 * @brief error types
 */
#define SBAPI_RIOT_KERNEL_PANIC (-1)
#define SBAPI_CONNECTION_ERR    (-2)


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
int sbapp_init(uint32_t options);


/**
 * @brief close the connection with the network processor
 */
int sbapp_stop(void);


/**
 * @brief
 */
int sbapp_configure(int32_t options);

/**
 * @brief set wifi credentials
 */
void sbapp_set_security(const char* ssid, const char* password);

/**
 * @brief connect to a TCP or UDP server
 *
 * @param[in] conn_type  TCP or UDP
 * @param[in] server the hostname or ip address of the server
 * @param[in] port remote port
 * @param[in] local_port  local port, if zero a random port is assigned
 *
 * @return the connection handle
 */
sbh_t sbapp_connect(uint8_t conn_type, uint32_t remote_ip, uint16_t port,
		uint16_t local_port, kernel_pid_t pid);


/**
 *  @brief send data
 *
 *  @param[in] fd the connection handle
 *              (or think it is a file descriptor if you prefer)
 *
 *  @return 1 on success -1 on error
 */
int sbapp_send(sbh_t fd, void* data, size_t len);


/**
 *  @brief send a datagram packet to addr destination
 *
 *  @param[in] data  the payload
 *  @param[in] len   the payload length
 *  @param[in] addr  the destination address
 *
 *  @return the number of transmitted bytes or -1 if an error occurred
 */
int sbapp_sendto(sbh_t fd, void* data, size_t len, sockaddr_in addr);


/**
 *
 */
uint32_t net_atoi(const char* name);


#ifdef __cplusplus
}
#endif


#endif /* SYS_INCLUDE_NET_SBAPP_H_ */
/** @} */
