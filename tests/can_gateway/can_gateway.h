/*
 * Copyright (C) 2020 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @brief       A gateway-function between CAN and Ethernet or SOME/IP (IPv4 or IPv6) for RIOT
 *
 * @{
 *
 * @file
 * @author  Jannes Volkens <jannes.volkens@haw-hamburg.de>
 */

#ifndef CAN_GATEWAY_H
#define CAN_GATEWAY_H

#include "can/conn/raw.h"
#include "net/someip.h"
#include "net/sock/udp.h"
#include "od.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define GATEWAY_TEST_PIN  GPIO_PIN(PORT_E, 0)

#define SOMEIP_CLIENT_ID 0x00           /**< SOME/IP Client ID*/
#define SOMEIP_SESSION_ID 0x00          /**< SOME/IP Session ID*/

#define SOMEIP_PROTOCOL_VERSION 0x01    /**< SOME/IP Protocol Version*/
#define SOMEIP_INTERFACE_VERSION 0x03   /**< SOME/IP Interface Version */
#define SOMEIP_MESSAGE_TYPE 0x02        /**< SOME/IP Message Type*/
#define SOMEIP_RETURN_CODE 0x00         /**< SOME/IP Return Code*/

struct interfaces {
    #ifndef GATEWAY_OPERATING_MODE
    ipv6_addr_t ipv6_addr;
    uint8_t eth_send;
    #else
    ipv4_addr_t ipv4_addr;
    #endif
    uint16_t dst_port;
    int8_t dst_ifnum;
    uint8_t src_ifnum;
};

struct id {
    uint32_t can_id;
    struct interfaces interface_information;
    struct id *next;
};

int can_gateway_handler(int argc, char **argv);
kernel_pid_t init_gateway(void);

#ifdef __cplusplus
}
#endif

#endif /* CAN_GATEWAY_H */
/** @} */
