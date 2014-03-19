/*
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    net_rpl RPL
 * @ingroup     net
 * @brief       Routing Protocol for Low power and Lossy Networks
 * @{
 *
 * @file        rpl.h
 * @brief       RPL header
 *
 * @author      Eric Engel <eric.engel@fu-berlin.de>
 */

#ifndef __RPL_H
#define __RPL_H

#include <string.h>
#include <stdint.h>
#include <vtimer.h>
#include <mutex.h>
#include <transceiver.h>
#include "ipv6.h"
#include "forwarding_tables.h"
#include "rpl_dodag.h"
#include "rpl_of_manager.h"


#undef CC1100_RADIO_MODE
#define CC1100_RADIO_MODE CC1100_MODE_WOR

#define RPL_PKT_RECV_BUF_SIZE 16
#define RPL_PROCESS_STACKSIZE KERNEL_CONF_STACKSIZE_DEFAULT

uint8_t rpl_init(int if_id);
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
void rpl_init_root(void);
void rpl_send(ipv6_addr_t *destination, uint8_t *payload, uint16_t p_len, uint8_t next_header);

/** @} */
#endif /* __RPL_H */
