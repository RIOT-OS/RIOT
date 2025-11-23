/*
 * SPDX-FileCopyrightText: 2015 INRIA
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    net_packet Packet interface on device level
 * @ingroup     net
 * @brief       Packet address family definitions
 * @{
 *
 * @file
 * @brief   Defines the struct for @ref AF_PACKET sockets
 *
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Link-Layer socket descriptor
 */
struct sockaddr_ll {
    unsigned short sll_family;   /**< Always AF_PACKET */
    unsigned short sll_protocol; /**< Physical-layer protocol */
    int            sll_ifindex;  /**< Interface number */
    unsigned short sll_hatype;   /**< ARP hardware type */
    unsigned char  sll_pkttype;  /**< Packet type */
    unsigned char  sll_halen;    /**< Length of address */
    unsigned char  sll_addr[8];  /**< Physical-layer address */
};

#ifdef __cplusplus
}
#endif

/**
 * @}
 */
