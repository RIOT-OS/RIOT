/**
 * 6lowpan border router flow control
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 * @ingroup sixlowpan
 * @{
 * @file    flowcontrol.h
 * @brief   data structs for border router flowcontrol
 * @author  Martin Lenders <mlenders@inf.fu-berlin.de>
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 * @}
 */

#ifndef _SIXLOWPAN_FLOWCONTROL_H
#define _SIXLOWPAN_FLOWCONTROL_H

#include <stdint.h>

#include "vtimer.h"

#include "semaphore.h"
#include "ip.h"
#include "border.h"
#include "bordermultiplex.h"

#ifdef __cplusplus
extern "C" {
#endif

/* packet types for flowcontrol */
#define BORDER_PACKET_ACK_TYPE    (1)

/* configuration types for flowcontrol */
#define BORDER_CONF_SYN           (0)
#define BORDER_CONF_SYNACK        (1)

#define BORDER_SWS                (1)
#define BORDER_RWS                (1)
#define BORDER_SL_TIMEOUT         (500) // microseconds, maybe smaller

#define SENDING_SLOT_STACK_SIZE     (MINIMUM_STACK_SIZE + 256)

typedef struct {
    /* Sender state */
    uint8_t last_ack;
    uint8_t last_frame;
    sem_t send_win_not_full;
    struct send_slot {
        vtimer_t timeout;
        uint8_t frame[BORDER_BUFFER_SIZE];
        size_t frame_len;
    } send_win[BORDER_SWS];

    /* Receiver state */
    uint8_t next_exp;
    struct recv_slot {
        int8_t received;
        uint8_t frame[BORDER_BUFFER_SIZE];
        size_t frame_len;
    } recv_win[BORDER_RWS];
} flowcontrol_stat_t;

typedef struct __attribute__((packed)) {
    uint8_t empty;
    uint8_t type;
    uint8_t next_seq_num;
    uint8_t conftype;
    uint8_t next_exp;
    ipv6_addr_t addr;
} border_syn_packet_t;

ipv6_addr_t flowcontrol_init(void);
void flowcontrol_send_over_uart(border_packet_t *packet, int len);
void flowcontrol_deliver_from_uart(border_packet_t *packet, int len);

#ifdef __cplusplus
}
#endif

#endif /* _SIXLOWPAN_FLOWCONTROL_H*/
