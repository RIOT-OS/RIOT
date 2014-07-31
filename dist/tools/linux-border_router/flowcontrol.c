/*
 * Copyright (C) 2014 Freie Universität Berlin.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <pthread.h>
#include <semaphore.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#ifdef BORDER_TESTING
#include "testing.h"
#endif

#include "flowcontrol.h"
#include "multiplex.h"

flowcontrol_stat_t slwin_stat;
uint8_t connection_established;

void *resend_thread_f(void *args)
{
    uint8_t seq_num = *((uint8_t *)args);
    struct send_slot *slot = &(slwin_stat.send_win[seq_num % BORDER_SWS]);

    while (1) {
        usleep(BORDER_SL_TIMEOUT);

        if (seq_num == ((border_packet_t *)(slot->frame))->seq_num) {
            writepacket(slot->frame, slot->frame_len);
        }
        else {
            return NULL;
        }
    }
}

void init_threeway_handshake(const struct in6_addr *addr)
{
    border_syn_packet_t *syn = (border_syn_packet_t *)get_serial_out_buffer(0);
    syn->empty = 0;
    syn->type = BORDER_PACKET_CONF_TYPE;
    syn->next_seq_num = slwin_stat.last_frame + 1;
    syn->conftype = BORDER_CONF_SYN;
    syn->next_exp = slwin_stat.next_exp;
    memcpy(&(syn->addr), addr, 16);

    do {
        writepacket((uint8_t *)syn, sizeof(border_syn_packet_t));
        usleep(BORDER_SL_TIMEOUT);
    }
    while (!connection_established);
}

void signal_connection_established(void)
{
    connection_established = 1;
}

void flowcontrol_init(const struct in6_addr *addr)
{
    int i;
    slwin_stat.last_frame = 0xFF;
    slwin_stat.last_ack = slwin_stat.last_frame;
    connection_established = 0;

    sem_init(&slwin_stat.send_win_not_full, 0, BORDER_SWS);

    for (i = 0; i < BORDER_SWS; i++) {
        slwin_stat.send_win[i].frame_len = 0;
    }

    memset(&slwin_stat.send_win, 0, sizeof(struct send_slot) * BORDER_SWS);

    slwin_stat.next_exp = 0;

    for (i = 0; i < BORDER_RWS; i++) {
        slwin_stat.recv_win[i].received = 0;
        slwin_stat.recv_win[i].frame_len = 0;
    }

    memset(&slwin_stat.recv_win, 0, sizeof(struct recv_slot) * BORDER_RWS);

    init_threeway_handshake(addr);
}

void flowcontrol_destroy(void)
{
    sem_destroy(&slwin_stat.send_win_not_full);
}

static int in_window(uint8_t seq_num, uint8_t min, uint8_t max)
{
    uint8_t pos = seq_num - min;
    uint8_t maxpos = max - min + 1;
    return pos < maxpos;
}

void send_ack(uint8_t seq_num)
{
    border_packet_t *packet = (border_packet_t *)get_serial_out_buffer(0);
    packet->empty = 0;
    packet->type = BORDER_PACKET_ACK_TYPE;
    packet->seq_num = seq_num;
    writepacket((uint8_t *)packet, sizeof(border_packet_t));
}

void flowcontrol_send_over_tty(border_packet_t *packet, int len)
{
    struct send_slot *slot;
    uint8_t args[] = {packet->seq_num};

    sem_wait(&(slwin_stat.send_win_not_full));
    packet->seq_num = ++slwin_stat.last_frame;
    slot = &(slwin_stat.send_win[packet->seq_num % BORDER_SWS]);
    memcpy(slot->frame, (uint8_t *)packet, len);
    slot->frame_len = len;
    pthread_create(&slot->resend_thread, NULL, resend_thread_f, (void *)args);
#ifdef BORDER_TESTING
    testing_start(packet->seq_num);
#endif
    writepacket((uint8_t *)packet, len);
}

void flowcontrol_deliver_from_tty(const border_packet_t *packet, int len)
{
    if (packet->type == BORDER_PACKET_ACK_TYPE) {
        if (in_window(packet->seq_num, slwin_stat.last_ack + 1, slwin_stat.last_frame)) {
            do {
                struct send_slot *slot;
                slot = &(slwin_stat.send_win[++slwin_stat.last_ack % BORDER_SWS]);
#ifdef BORDER_TESTING
                testing_stop(slwin_stat.last_ack);
#endif
                pthread_cancel(slot->resend_thread);
                memset(&slot->frame, 0, BUFFER_SIZE);
                slot->frame_len = 0;
                sem_post(&slwin_stat.send_win_not_full);
            }
            while (slwin_stat.last_ack != packet->seq_num);
        }
    }
    else {
        struct recv_slot *slot;

        slot = &slwin_stat.recv_win[packet->seq_num % BORDER_RWS];

        if (!in_window(packet->seq_num,
                       slwin_stat.next_exp,
                       slwin_stat.next_exp + BORDER_RWS - 1)) {
            return;
        }

        memcpy(slot->frame, (uint8_t *)packet, len);
        slot->received = 1;

        if (packet->seq_num == slwin_stat.next_exp) {
            while (slot->received) {
                demultiplex((border_packet_t *)slot->frame, slot->frame_len);
                memset(&slot->frame, 0, BUFFER_SIZE);
                slot->received = 0;
                slot = &slwin_stat.recv_win[++slwin_stat.next_exp % BORDER_RWS];
            }
        }

        send_ack(slwin_stat.next_exp - 1);
    }
}
