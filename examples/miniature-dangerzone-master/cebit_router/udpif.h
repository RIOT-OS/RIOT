/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cebit_demo
 * @{
 *
 * @file        udp.h
 * @brief       CeBIT 2014 demo application - router node
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#ifndef __UDP_H
#define __UDP_H


/**
 * @brief UDP shell command handler for starting a UDP server
 */
void udpif_shell_server(int argc, char **argv);

/**
 * @brief UDP shell command handler for sending data
 */
void udpif_shell_send(int argc, char **argv);

/**
 * @brief Send data via UDP to given local address
 */
int udpif_send(uint16_t dst_addr, uint16_t port, char *data, int length);

/**
 * @brief Start a UDP server on the designated port
 */
void udpif_start_server(uint16_t port, void(*ondata)(uint16_t src_addr, char *data, int length));

/**
 * @brief Get a full IPv6 address from local address
 */
void udpif_get_ipv6_address(ipv6_addr_t *addr, uint16_t local_addr);


#endif /* __UDP_H */
