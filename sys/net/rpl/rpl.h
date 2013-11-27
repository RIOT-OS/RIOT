/**
 * RPL constants and prototypes
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @defgroup 	net_rpl RPL
 * @ingroup 	net
 * @brief		Routing Protocol for Low power and Lossy Networks
 * @{
 * @file    rpl.h
 * @brief   RPL header
 * @author  Eric Engel <eric.engel@fu-berlin.de>
 * @}
 */

#include <string.h>
#include <stdint.h>
#include <vtimer.h>
#include <mutex.h>
#include <transceiver.h>
#include "ipv6.h"
#include "rpl_dodag.h"

#undef CC1100_RADIO_MODE
#define CC1100_RADIO_MODE CC1100_MODE_WOR

#define RPL_PKT_RECV_BUF_SIZE 20
#define RPL_PROCESS_STACKSIZE KERNEL_CONF_STACKSIZE_DEFAULT

uint8_t rpl_init(transceiver_type_t trans, uint16_t rpl_address);
void rpl_init_root(void);
rpl_of_t *rpl_get_of_for_ocp(uint16_t ocp);

void send_DIO(ipv6_addr_t *destination);
void send_DIS(ipv6_addr_t *destination);
void send_DAO(ipv6_addr_t *destination, uint8_t lifetime, bool default_lifetime, uint8_t start_index);
void send_DAO_ACK(ipv6_addr_t *destination);
void rpl_process(void);
void recv_rpl_dio(void);
void recv_rpl_dis(void);
void recv_rpl_dao(void);
void recv_rpl_dao_ack(void);
void rpl_send(ipv6_addr_t *destination, uint8_t *payload, uint16_t p_len, uint8_t next_header);
ipv6_addr_t *rpl_get_next_hop(ipv6_addr_t *addr);
void rpl_add_routing_entry(ipv6_addr_t *addr, ipv6_addr_t *next_hop, uint16_t lifetime);
void rpl_del_routing_entry(ipv6_addr_t *addr);
rpl_routing_entry_t *rpl_find_routing_entry(ipv6_addr_t *addr);
void rpl_clear_routing_table(void);
rpl_routing_entry_t *rpl_get_routing_table(void);
