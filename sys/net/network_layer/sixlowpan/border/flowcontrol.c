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
 * @file    flowcontrol.c
 * @brief   flowcontrol for constraint node border router implementation
 * @author  Martin Lenders <mlenders@inf.fu-berlin.de>
 * @author  Oliver Hahm <oliver.hahm@inria.fr>
 * @}
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vtimer.h"
#include "thread.h"
#include "semaphore.h"
#include "bordermultiplex.h"
#include "flowcontrol.h"


static int set_timeout(vtimer_t *timeout, timex_t val, void *args);
static void sending_slot(void);

char sending_slot_stack[SENDING_SLOT_STACK_SIZE];
unsigned int sending_slot_pid;

flowcontrol_stat_t slwin_stat;
sem_t connection_established;
int16_t synack_seqnum = -1;

ipv6_addr_t init_threeway_handshake(void)
{
    border_syn_packet_t *syn;
    msg_t m;
    m.content.ptr = NULL;
    msg_send(&m, border_get_serial_reader(), 1);
    msg_receive(&m);

    syn = (border_syn_packet_t *)m.content.ptr;
    border_conf_header_t *synack = (border_conf_header_t *)get_serial_out_buffer(0);
    ipv6_addr_t addr;
    memcpy(&addr, &(syn->addr), sizeof(ipv6_addr_t));

    slwin_stat.next_exp = syn->next_seq_num;
    slwin_stat.last_frame = syn->next_exp - 1;
    slwin_stat.last_ack = slwin_stat.last_frame;

    synack->empty = 0;
    synack->type = BORDER_PACKET_CONF_TYPE;
    synack->conftype = BORDER_CONF_SYNACK;

    sending_slot_pid = thread_create(sending_slot_stack, SENDING_SLOT_STACK_SIZE, PRIORITY_MAIN - 1, CREATE_SLEEPING, sending_slot, "sending slot");
    flowcontrol_send_over_uart((border_packet_t *)synack, sizeof(border_conf_header_t));

    synack_seqnum = synack->seq_num;

    return addr;
}

ipv6_addr_t flowcontrol_init(void)
{
    int i;

    sem_init(&slwin_stat.send_win_not_full, 0, BORDER_SWS);

    for (i = 0; i < BORDER_SWS; i++) {
        slwin_stat.send_win[i].frame_len = 0;
    }

    memset(&slwin_stat.send_win, 0, sizeof(struct send_slot) * BORDER_SWS);

    for (i = 0; i < BORDER_RWS; i++) {
        slwin_stat.recv_win[i].received = 0;
        slwin_stat.recv_win[i].frame_len = 0;
    }

    memset(&slwin_stat.recv_win, 0, sizeof(struct recv_slot) * BORDER_RWS);

    return init_threeway_handshake();
}

static void sending_slot(void)
{
    msg_t m;
    uint8_t seq_num;
    struct send_slot *slot;
    border_packet_t *tmp;

    while (1) {
        msg_receive(&m);
        seq_num = *((uint8_t *)m.content.ptr);
        slot = &(slwin_stat.send_win[seq_num % BORDER_SWS]);
        tmp = (border_packet_t *)slot->frame;

        if (seq_num == tmp->seq_num) {
            writepacket(slot->frame, slot->frame_len);

            if (set_timeout(&slot->timeout, timex_set(0, BORDER_SL_TIMEOUT), (void *)m.content.ptr) != 0) {
                printf("ERROR: Error invoking timeout timer\n");
            }
        }
    }
}

static int set_timeout(vtimer_t *timeout, timex_t val, void *args)
{
    vtimer_remove(timeout);

    timex_normalize(&val);
    return vtimer_set_msg(timeout, val, sending_slot_pid, args);
}

static int in_window(uint8_t seq_num, uint8_t min, uint8_t max)
{
    uint8_t pos = seq_num - min;
    uint8_t maxpos = max - min + 1;
    return (pos < maxpos);
}

void flowcontrol_send_over_uart(border_packet_t *packet, int len)
{
    struct send_slot *slot;
    uint8_t args[] = {packet->seq_num};

    sem_wait(&(slwin_stat.send_win_not_full));
    packet->seq_num = ++slwin_stat.last_frame;
    slot = &(slwin_stat.send_win[packet->seq_num % BORDER_SWS]);
    memcpy(slot->frame, (uint8_t *)packet, len);
    slot->frame_len = len;

    if (set_timeout(&slot->timeout, timex_set(0, BORDER_SL_TIMEOUT * 1000), (void *)args) != 0) {
        printf("ERROR: Error invoking timeout timer\n");
        return;
    }

    writepacket((uint8_t *)packet, len);
}

void send_ack(uint8_t seq_num)
{
    border_packet_t *packet = (border_packet_t *)get_serial_out_buffer(0);
    packet->empty = 0;
    packet->type = BORDER_PACKET_ACK_TYPE;
    packet->seq_num = seq_num;
    writepacket((uint8_t *)packet, sizeof(border_packet_t));
}

void flowcontrol_deliver_from_uart(border_packet_t *packet, int len)
{
    if (packet->type == BORDER_PACKET_ACK_TYPE) {
        if (in_window(packet->seq_num, slwin_stat.last_ack + 1, slwin_stat.last_frame)) {
            if (synack_seqnum == packet->seq_num) {
                synack_seqnum = -1;
                sem_post(&connection_established);
            }

            do {
                struct send_slot *slot;
                slot = &(slwin_stat.send_win[++slwin_stat.last_ack % BORDER_SWS]);
                vtimer_remove(&slot->timeout);
                memset(&slot->frame, 0, BORDER_BUFFER_SIZE);
                sem_post(&slwin_stat.send_win_not_full);
            }
            while (slwin_stat.last_ack != packet->seq_num);
        }
    }
    else {
        struct recv_slot *slot;

        slot = &(slwin_stat.recv_win[packet->seq_num % BORDER_RWS]);

        if (!in_window(packet->seq_num,
                       slwin_stat.next_exp,
                       slwin_stat.next_exp + BORDER_RWS - 1)) {
            return;
        }

        memcpy(slot->frame, (uint8_t *)packet, len);
        slot->received = 1;

        if (packet->seq_num == slwin_stat.next_exp) {
            while (slot->received) {
                demultiplex((border_packet_t *)slot->frame);
                memset(&slot->frame, 0, BORDER_BUFFER_SIZE);
                slot->received = 0;
                slot = &slwin_stat.recv_win[++(slwin_stat.next_exp) % BORDER_RWS];
            }
        }

        send_ack(slwin_stat.next_exp - 1);
    }
}
