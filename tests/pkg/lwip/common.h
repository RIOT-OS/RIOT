/*
 * Copyright (C) 2016 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief   Definitions for tests/lwip/
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef COMMON_H
#define COMMON_H

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

/**
 * @brief   Converts hex string to byte array.
 *
 * @param[out] out  Resulting byte array
 * @param[in] in    `\0` terminated string. Non-hex characters (all except 0-9, a-f, A-F)
 *                  will be ignored.
 *
 * @return  Length of @p out.
 */
size_t hex2ints(uint8_t *out, const char *in);

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
#endif /* COMMON_H */
