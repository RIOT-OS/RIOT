/*
 * SPDX-FileCopyrightText: 2016 Martine Lenders <mlenders@inf.fu-berlin.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief   Definitions for tests/lwip/
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <stdint.h>
#include <sys/types.h>

#include "kernel_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

#if IS_USED(MODULE_LWIP_IPV6)
#include "net/ipv6.h"
#define SOCK_IP_EP_ANY  SOCK_IPV6_EP_ANY
#elif IS_USED(MODULE_LWIP_IPV4)
#include "net/ipv4.h"
#define SOCK_IP_EP_ANY  SOCK_IPV4_EP_ANY
#endif

/**
 * @brief   Application configuration
 * @{
 */
#define SOCK_INBUF_SIZE         (256)
#define SERVER_MSG_QUEUE_SIZE   (8)
#define SERVER_BUFFER_SIZE      (64)
/**
 * @}
 */

#ifdef MODULE_SOCK_IP
/**
 * @brief   Raw IP shell command
 *
 * @param[in] argc  number of arguments
 * @param[in] argv  array of arguments
 *
 * @return  0 on success
 * @return  other on error
 */
int ip_cmd(int argc, char **argv);
#endif

#ifdef MODULE_SOCK_TCP
/**
 * @brief   TCP IP shell command
 *
 * @param[in] argc  number of arguments
 * @param[in] argv  array of arguments
 *
 * @return  0 on success
 * @return  other on error
 */
int tcp_cmd(int argc, char **argv);
#endif

#ifdef MODULE_SOCK_UDP
/**
 * @brief   UDP IP shell command
 *
 * @param[in] argc  number of arguments
 * @param[in] argv  array of arguments
 *
 * @return  0 on success
 * @return  other on error
 */
int udp_cmd(int argc, char **argv);
#endif

#ifdef __cplusplus
}
#endif

/** @} */
