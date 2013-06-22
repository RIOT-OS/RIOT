/*
 * 6lowpan implementation
 *
 * Copyright (C) 2013  INRIA.
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 *
 * @ingroup sixlowpan
 * @{
 * @file    sixlowpan.c
 * @brief   6lowpan functions 
 * @author  Stephan Zeisberg <zeisberg@mi.fu-berlin.de>
 * @author  Martin Lenders <mlenders@inf.fu-berlin.de>
 * @author  Oliver Gesch <oliver.gesch@googlemail.com>
 * @author  Eric Engel <eric.engel@fu-berlin.de>
 * @}
 */

#include "sixlowip.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#include "vtimer.h"
#include "timex.h"
#include "debug.h"
#include "thread.h"
#include "mutex.h"
#include "hwtimer.h"
#include "rtc.h"
#include "msg.h"
#include "sixlowmac.h"
#include "sixlowpan.h"
#include "sixlowborder.h"
#include "sixlowip.h"
#include "sixlownd.h"
#include "transceiver.h"
#include "ieee802154_frame.h"
#include "sys/net/destiny/in.h"
#include "sys/net/net_help/net_help.h"

uint16_t packet_length;
uint8_t packet_dispatch;
uint16_t tag;
uint8_t header_size = 0;
uint8_t max_frame = 0;
uint8_t max_frag_initial = 0;
uint8_t position;
uint8_t max_frag;

struct ipv6_hdr_t *ipv6_buf;

/* length of compressed packet */
uint16_t comp_len;
uint8_t frag_size;
uint8_t reas_buf[512];
uint8_t comp_buf[512];
uint8_t byte_offset;
uint8_t first_frag = 0;
lowpan_reas_buf_t *head = NULL;
lowpan_reas_buf_t *packet_fifo = NULL;
mutex_t fifo_mutex;

unsigned int ip_process_pid;
unsigned int nd_nbr_cache_rem_pid = 0;
unsigned int contexts_rem_pid = 0;
unsigned int transfer_pid = 0;

iface_t iface;
ipv6_addr_t lladdr;
ieee_802154_long_t laddr;
mutex_t buf_mutex;
mutex_t lowpan_context_mutex;

char ip_process_buf[IP_PROCESS_STACKSIZE];
char nc_buf[NC_STACKSIZE];
char con_buf[CON_STACKSIZE];
char lowpan_transfer_buf[LOWPAN_TRANSFER_BUF_STACKSIZE];
lowpan_context_t contexts[LOWPAN_CONTEXT_MAX];
uint8_t context_len = 0;
uint8_t static_route = 0;
uint16_t local_address = 0;

void lowpan_context_auto_remove(void);

/* deliver packet to mac*/
void lowpan_init(ieee_802154_long_t *addr, uint8_t *data)
{
    uint8_t mcast = 0;

    ipv6_buf = (ipv6_hdr_t *) data;

    memcpy(&laddr.uint8[0], &addr->uint8[0], 8);

    if(ipv6_prefix_mcast_match(&ipv6_buf->destaddr)) {
        /* send broadcast */
        mcast = 1;
    }

    lowpan_iphc_encoding(&laddr, ipv6_buf, data);
    data = &comp_buf[0];
    packet_length = comp_len;

    if(static_route == 1) {
        if(laddr.uint8[7] < local_address) {
            laddr.uint8[7] = local_address - 1;
        }
        else {
            laddr.uint8[7] = local_address + 1;
        }
    }

    /* check if packet needs to be fragmented */
    if(packet_length + header_size > PAYLOAD_SIZE - IEEE_802154_MAX_HDR_LEN) {
        uint8_t fragbuf[packet_length + header_size];
        uint8_t remaining;
        uint8_t i = 2;
        /* first fragment */
        max_frame = PAYLOAD_SIZE - IEEE_802154_MAX_HDR_LEN;
        max_frag_initial = ((max_frame - 4 - header_size) / 8) * 8;

        memcpy(fragbuf + 4, data, max_frag_initial);

        fragbuf[0] = (((0xc0 << 8) | packet_length) >> 8) & 0xff;
        fragbuf[1] = ((0xc0 << 8) | packet_length) & 0xff;
        fragbuf[2] = (tag >> 8) & 0xff;
        fragbuf[3] = tag & 0xff;

        send_ieee802154_frame(&laddr, (uint8_t *)&fragbuf,
                              max_frag_initial + header_size + 4, mcast);
        /* subsequent fragments */
        position = max_frag_initial;
        max_frag = ((max_frame - 5) / 8) * 8;

        data += position;

        while(packet_length - position > max_frame - 5) {
            memset(&fragbuf, 0, packet_length + header_size);
            memcpy(fragbuf + 5, data, max_frag);

            fragbuf[0] = (((0xe0 << 8) | packet_length) >> 8) & 0xff;
            fragbuf[1] = ((0xe0 << 8) | packet_length) & 0xff;
            fragbuf[2] = (tag >> 8) & 0xff;
            fragbuf[3] = tag & 0xff;
            fragbuf[4] = position / 8;

            send_ieee802154_frame(&laddr, (uint8_t *)&fragbuf, max_frag + 5,
                                  mcast);
            data += max_frag;
            position += max_frag;

            i++;
        }

        remaining = packet_length - position;

        memset(&fragbuf, 0, packet_length + header_size);
        memcpy(fragbuf + 5, data, remaining);

        fragbuf[0] = (((0xe0 << 8) | packet_length) >> 8) & 0xff;
        fragbuf[1] = ((0xe0 << 8) | packet_length) & 0xff;
        fragbuf[2] = (tag >> 8) & 0xff;
        fragbuf[3] = tag & 0xff;
        fragbuf[4] = position / 8;

        send_ieee802154_frame(&laddr, (uint8_t *)&fragbuf, remaining + 5, mcast);
    }
    else {
        send_ieee802154_frame(&laddr, data, packet_length, mcast);
    }

    tag++;
}

void printLongLocalAddr(ieee_802154_long_t *saddr)
{
    printf("%02x%02x:%02x%02x:%02x%02x:%02x%02x\n",
           ((uint8_t *)saddr)[0], ((uint8_t *)saddr)[1], ((uint8_t *)saddr)[2],
           ((uint8_t *)saddr)[3], ((uint8_t *)saddr)[4], ((uint8_t *)saddr)[5],
           ((uint8_t *)saddr)[6], ((uint8_t *)saddr)[7]);
}

void printReasBuffers(void)
{
    lowpan_reas_buf_t *temp_buffer;
    lowpan_interval_list_t *temp_interval;
    temp_buffer = head;

    printf("\n\n--- Reassembly Buffers ---\n");

    while(temp_buffer != NULL) {
        printLongLocalAddr(&temp_buffer->s_laddr);
        printf("Ident.: %i, Packet Size: %i/%i, Timestamp: %li\n",
               temp_buffer->ident_no, temp_buffer->current_packet_size,
               temp_buffer->packet_size, temp_buffer->timestamp);
        temp_interval = temp_buffer->interval_list_head;

        while(temp_interval != NULL) {
            printf("\t%i - %i\n", temp_interval->start, temp_interval->end);
            temp_interval = temp_interval->next;
        }

        temp_buffer = temp_buffer->next;
    }
}

void printFIFOBuffers(void)
{
    lowpan_reas_buf_t *temp_buffer;
    lowpan_interval_list_t *temp_interval;
    temp_buffer = packet_fifo;

    printf("\n\n--- Reassembly Buffers ---\n");

    while(temp_buffer != NULL) {
        printLongLocalAddr(&temp_buffer->s_laddr);
        printf("Ident.: %i, Packet Size: %i/%i, Timestamp: %li\n",
               temp_buffer->ident_no, temp_buffer->current_packet_size, 
               temp_buffer->packet_size, temp_buffer->timestamp);
        temp_interval = temp_buffer->interval_list_head;

        while(temp_interval != NULL) {
            printf("\t%i - %i\n", temp_interval->start, temp_interval->end);
            temp_interval = temp_interval->next;
        }

        temp_buffer = temp_buffer->next;
    }
}

void lowpan_transfer(void)
{
    msg_t m_recv, m_send;
    ipv6_hdr_t *ipv6_buf;
    lowpan_reas_buf_t *current_buf;
    uint8_t gotosleep;

    while(1) {

        gotosleep = 1;
        mutex_lock(&fifo_mutex);
        current_buf = packet_fifo;

        if(current_buf != NULL) {
            mutex_unlock(&fifo_mutex, 0);

            if((current_buf->packet)[0] == LOWPAN_IPV6_DISPATCH) {
                ipv6_buf = get_ipv6_buf();
                memcpy(ipv6_buf, (current_buf->packet) + 1, current_buf->packet_size - 1);
                m_send.content.ptr = (char *)ipv6_buf;
                packet_length = current_buf->packet_size - 1;
                msg_send_receive(&m_send, &m_recv, ip_process_pid);
            }
            else if(((current_buf->packet)[0] & 0xe0) == LOWPAN_IPHC_DISPATCH) {
                lowpan_iphc_decoding(current_buf->packet,
                                     current_buf->packet_size,
                                     current_buf->s_laddr,
                                     current_buf->d_laddr);

                ipv6_buf = get_ipv6_buf();
                m_send.content.ptr = (char *) ipv6_buf;
                msg_send_receive(&m_send, &m_recv, ip_process_pid);
            }
            else {
                //				printf("ERROR: packet with unknown dispatch received\n");
            }

            collect_garbage_fifo(current_buf);
            gotosleep = 0;
        }


        if(gotosleep == 1) {
            mutex_unlock(&fifo_mutex, 0);
            thread_sleep();
        }
    }
}

uint8_t ll_get_addr_match(ieee_802154_long_t *src, ieee_802154_long_t *dst)
{
    uint8_t val = 0, xor;

    for(int i = 0; i < 8; i++) {
        /* if bytes are equal add 8 */
        if(src->uint8[i] == dst->uint8[i]) {
            val += 8;
        }
        else {
            xor = src->uint8[i] ^ dst->uint8[i];

            /* while bits from byte equal add 1 */
            for(int j = 0; j < 8; j++) {
                if((xor & 0x80) == 0) {
                    val++;
                    xor = xor << 1;
                }
                else {
                    break;
                }
            }
        }
    }

    return val;
}

lowpan_reas_buf_t *new_packet_buffer(uint16_t datagram_size,
                                     uint16_t datagram_tag,
                                     ieee_802154_long_t *s_laddr,
                                     ieee_802154_long_t *d_laddr,
                                     lowpan_reas_buf_t *current_buf,
                                     lowpan_reas_buf_t *temp_buf)
{
    lowpan_reas_buf_t *new_buf = NULL;

    /* Allocate new memory for a new packet to be reassembled */
    new_buf = malloc(sizeof(lowpan_reas_buf_t));

    if(new_buf != NULL) {
        init_reas_bufs(new_buf);

        new_buf->packet = malloc(datagram_size);

        if(new_buf->packet != NULL) {
            memcpy(&new_buf->s_laddr, s_laddr, SIXLOWPAN_IPV6_LL_ADDR_LEN);
            memcpy(&new_buf->d_laddr, d_laddr, SIXLOWPAN_IPV6_LL_ADDR_LEN);

            new_buf->ident_no = datagram_tag;
            new_buf->packet_size = datagram_size;

            timex_t now;
            vtimer_now(&now);
            new_buf->timestamp = now.microseconds;

            if((current_buf == NULL) && (temp_buf == NULL)) {
                head = new_buf;
            }
            else {
                temp_buf->next = new_buf;
            }

            return new_buf;
        }
        else {
            return NULL;
        }
    }
    else {
        return NULL;
    }
}

lowpan_reas_buf_t *get_packet_frag_buf(uint16_t datagram_size,
                                       uint16_t datagram_tag,
                                       ieee_802154_long_t *s_laddr,
                                       ieee_802154_long_t *d_laddr)
{
    lowpan_reas_buf_t *current_buf = NULL, *temp_buf = NULL;
    current_buf = head;

    while(current_buf != NULL) {
        if(((ll_get_addr_match(&current_buf->s_laddr, s_laddr)) == 64) &&
           ((ll_get_addr_match(&current_buf->d_laddr, d_laddr)) == 64) &&
           (current_buf->packet_size == datagram_size) &&
           (current_buf->ident_no == datagram_tag) &&
           current_buf->interval_list_head != NULL) {
            /* Found buffer for current packet fragment */
            timex_t now;
            vtimer_now(&now);
            current_buf->timestamp = now.microseconds;
            return current_buf;
        }

        temp_buf = current_buf;
        current_buf = current_buf->next;
    }

    return new_packet_buffer(datagram_size, datagram_tag, s_laddr, d_laddr,
                             current_buf, temp_buf);
}

uint8_t isInInterval(uint8_t start1, uint8_t end1, uint8_t start2, uint8_t end2)
{
    /* 1: Interval 1 and 2 are the same or overlapping */
    /* 0: Interval 1 and 2 are not overlapping or the same */

    if(((start1 < start2) && (start2 <= end1)) ||
       ((start2 < start1) && (start1 <= end2)) ||
       ((start1 == start2) && (end1 == end2))) {
        return 1;
    }
    else {
        return 0;
    }
}

uint8_t handle_packet_frag_interval(lowpan_reas_buf_t *current_buf,
                                    uint8_t datagram_offset,  uint8_t frag_size)
{
    /* 0: Error, discard fragment */
    /* 1: Finished correctly */
    lowpan_interval_list_t *temp_interval = NULL, *current_interval = NULL, *new_interval = NULL;
    current_interval = current_buf->interval_list_head;

    while(current_interval != NULL) {
        if(isInInterval(current_interval->start, current_interval->end, datagram_offset, datagram_offset + frag_size) == 1) {
            /* Interval is overlapping or the same as one of a previous fragment, discard fragment */
            return 0;
        }

        temp_interval = current_interval;
        current_interval = current_interval->next;
    }

    new_interval = malloc(sizeof(lowpan_interval_list_t));

    if(new_interval != NULL) {
        new_interval->start = datagram_offset;
        new_interval->end = datagram_offset + frag_size - 1;
        new_interval->next = NULL;

        if((current_interval == NULL) && (temp_interval == NULL)) {
            current_buf->interval_list_head = new_interval;
        }
        else {
            temp_interval->next = new_interval;
        }

        return 1;
    }

    return 0;
}

lowpan_reas_buf_t *collect_garbage_fifo(lowpan_reas_buf_t *current_buf)
{
    lowpan_interval_list_t *temp_list, *current_list;
    lowpan_reas_buf_t *temp_buf, *my_buf, *return_buf;

    mutex_lock(&fifo_mutex);

    temp_buf = packet_fifo;
    my_buf = temp_buf;

    if(packet_fifo == current_buf) {
        packet_fifo = current_buf->next;
        return_buf = packet_fifo;
    }
    else {
        while(temp_buf != current_buf) {
            my_buf = temp_buf;
            temp_buf = temp_buf->next;
        }

        my_buf->next = current_buf->next;

        return_buf = my_buf->next;
    }

    mutex_unlock(&fifo_mutex, 0);

    current_list = current_buf->interval_list_head;
    temp_list = current_list;

    while(current_list != NULL) {
        temp_list = current_list->next;
        free(current_list);
        current_list = temp_list;
    }

    free(current_buf->packet);
    free(current_buf);

    return return_buf;
}

lowpan_reas_buf_t *collect_garbage(lowpan_reas_buf_t *current_buf)
{
    lowpan_interval_list_t *temp_list, *current_list;
    lowpan_reas_buf_t *temp_buf, *my_buf, *return_buf;

    temp_buf = head;
    my_buf = temp_buf;

    if(head == current_buf) {
        head = current_buf->next;
        return_buf = head;
    }
    else {
        while(temp_buf != current_buf) {
            my_buf = temp_buf;
            temp_buf = temp_buf->next;
        }

        my_buf->next = current_buf->next;

        return_buf = my_buf->next;
    }

    current_list = current_buf->interval_list_head;
    temp_list = current_list;

    while(current_list != NULL) {
        temp_list = current_list->next;
        free(current_list);
        current_list = temp_list;
    }

    free(current_buf->packet);
    free(current_buf);

    return return_buf;
}

void handle_packet_fragment(uint8_t *data, uint8_t datagram_offset, 
                            uint16_t datagram_size, uint16_t datagram_tag,
                            ieee_802154_long_t *s_laddr,
                            ieee_802154_long_t *d_laddr, uint8_t hdr_length,
                            uint8_t frag_size)
{
    lowpan_reas_buf_t *current_buf;
    /* Is there already a reassembly buffer for this packet fragment? */
    current_buf = get_packet_frag_buf(datagram_size, datagram_tag, s_laddr, d_laddr);

    if((current_buf != NULL) && (handle_packet_frag_interval(current_buf,
                                                             datagram_offset,
                                                             frag_size) == 1)) {
        /* Copy fragment bytes into corresponding packet space area */
        memcpy(current_buf->packet + datagram_offset, data + hdr_length, frag_size);
        current_buf->current_packet_size += frag_size;

        if(current_buf->current_packet_size == current_buf->packet_size) {
            add_fifo_packet(current_buf);

            if(thread_getstatus(transfer_pid) == STATUS_SLEEPING) {
                thread_wakeup(transfer_pid);
            }
        }
    }
    else {
        /* No memory left or duplicate */
        if(current_buf == NULL) {
            printf("ERROR: no memory left!\n");
        }
        else {
            printf("ERROR: duplicate fragment!\n");
        }
    }
}

void check_timeout(void)
{
    lowpan_reas_buf_t *temp_buf, *smallest_time = NULL;
    long cur_time;
    int count = 0;

    timex_t now;
    vtimer_now(&now);
    cur_time = now.microseconds;
    temp_buf = head;

    while(temp_buf != NULL) {
        if((cur_time - temp_buf->timestamp) >= LOWPAN_REAS_BUF_TIMEOUT) {
            printf("TIMEOUT! cur_time: %li, temp_buf: %li\n", cur_time,
                   temp_buf->timestamp);
            temp_buf = collect_garbage(temp_buf);
        }
        else {
            if(smallest_time == NULL) {
                smallest_time = temp_buf;
            }
            else if(temp_buf->timestamp < smallest_time->timestamp) {
                smallest_time = temp_buf;
            }

            temp_buf = temp_buf->next;
            count++;
        }
    }

    if((count > 10) && (smallest_time != NULL)) {
        collect_garbage(smallest_time);
    }
}

void add_fifo_packet(lowpan_reas_buf_t *current_packet)
{
    lowpan_reas_buf_t *temp_buf, *my_buf;

    if(head == current_packet) {
        head = current_packet->next;
    }
    else {
        temp_buf = head;

        while(temp_buf != current_packet) {
            my_buf = temp_buf;
            temp_buf = temp_buf->next;
        }

        my_buf->next = current_packet->next;
    }

    mutex_lock(&fifo_mutex);

    if(packet_fifo == NULL) {
        packet_fifo = current_packet;
    }
    else {
        temp_buf = packet_fifo;

        while(temp_buf != NULL) {
            my_buf = temp_buf;
            temp_buf = temp_buf->next;
        }

        my_buf->next = current_packet;
    }

    mutex_unlock(&fifo_mutex, 0);
    current_packet->next = NULL;
}

void lowpan_read(uint8_t *data, uint8_t length, ieee_802154_long_t *s_laddr,
                 ieee_802154_long_t *d_laddr)
{
    /* check if packet is fragmented */
    uint8_t hdr_length = 0;
    uint8_t datagram_offset = 0;
    uint16_t datagram_size = 0;
    uint16_t datagram_tag = 0;

    check_timeout();

    /* Fragmented Packet */
    if(((data[0] & 0xf8) == (0xc0)) || ((data[0] & 0xf8) == (0xe0))) {
        /* get 11-bit from first 2 byte*/
        datagram_size = (((uint16_t)(data[0] << 8)) | data[1]) & 0x07ff;

        /* get 16-bit datagram tag */
        datagram_tag = (((uint16_t)(data[2] << 8)) | data[3]);

        switch(data[0] & 0xf8) {
                /* First Fragment */
            case(0xc0): {
                datagram_offset = 0;
                hdr_length += 4;
                break;
            }

            /* Subsequent Fragment */
            case(0xe0): {
                datagram_offset = data[4];
                hdr_length += 5;
                break;
            }
        }

        frag_size = length - hdr_length;
        byte_offset = datagram_offset * 8;

        if((frag_size % 8) != 0) {
            if((byte_offset + frag_size) != datagram_size) {
                printf("ERROR: received invalid fragment\n");
                return;
            }
        }

        handle_packet_fragment(data, byte_offset, datagram_size, datagram_tag,
                               s_laddr, d_laddr, hdr_length, frag_size);
    }
    /* Regular Packet */
    else {
        lowpan_reas_buf_t *current_buf = get_packet_frag_buf(length, 0, s_laddr,
                                                             d_laddr);
        /* Copy packet bytes into corresponding packet space area */
        memcpy(current_buf->packet, data, length);
        current_buf->current_packet_size += length;
        add_fifo_packet(current_buf);

        if(thread_getstatus(transfer_pid) == STATUS_SLEEPING) {
            thread_wakeup(transfer_pid);
        }
    }

}

void lowpan_ipv6_set_dispatch(uint8_t *data)
{
    memmove(data + 1, data, packet_length);
    data[0] = LOWPAN_IPV6_DISPATCH;
    packet_length++;
}

/* draft-ietf-6lowpan-hc-13#section-3.1 */
void lowpan_iphc_encoding(ieee_802154_long_t *dest, ipv6_hdr_t *ipv6_buf_extra,
                          uint8_t *ptr)
{
    ipv6_buf = ipv6_buf_extra;

    uint16_t payload_length = ipv6_buf->length;
    uint8_t lowpan_iphc[2];
    uint8_t *ipv6_hdr_fields = &comp_buf[2];
    lowpan_context_t *con = NULL;
    uint16_t hdr_pos = 0;
    uint8_t tc;

    memset(&lowpan_iphc, 0, 2);

    /* set iphc dispatch */
    lowpan_iphc[0] = LOWPAN_IPHC_DISPATCH;

    /* TF: Traffic Class, Flow Label:
     * first we need to change DSCP and ECN because in 6lowpan-nd-13 these
     * fields are reverse, the original order is DSCP/ECN (rfc 3168) */
    tc = (ipv6_buf->version_trafficclass << 4) |
         (ipv6_buf->trafficclass_flowlabel >> 4);
    tc = (tc >> 2) | (tc << 6);

    if((ipv6_buf->flowlabel == 0) &&
       (ipv6_buf->trafficclass_flowlabel & 0x0f) == 0) {
        /* flowlabel is elided */
        lowpan_iphc[0] |= LOWPAN_IPHC_FL_C;

        if(((ipv6_buf->version_trafficclass & 0x0f) == 0) &&
           ((ipv6_buf->trafficclass_flowlabel & 0xf0) == 0)) {
            /* traffic class is elided */
            lowpan_iphc[0] |= LOWPAN_IPHC_TC_C;
        }
        else {
            /* ECN + DSCP (1 byte), Flow Label is elided */
            ipv6_hdr_fields[hdr_pos] = tc;
            hdr_pos++;
        }
    }
    else {
        /* flowlabel not compressible */
        if(((ipv6_buf->version_trafficclass & 0x0f) == 0) &&
           ((ipv6_buf->trafficclass_flowlabel & 0xf0) == 0)) {
            /* traffic class is elided */
            lowpan_iphc[0] |= LOWPAN_IPHC_TC_C;
            /* ECN + 2-bit Pad + Flow Label (3 bytes), DSCP is elided */
            ipv6_hdr_fields[hdr_pos] = ((tc & 0xc0) |
                                        (ipv6_buf->trafficclass_flowlabel & 0x0f));
            memcpy(&(ipv6_hdr_fields[hdr_pos]), &ipv6_buf->flowlabel , 2);
            hdr_pos += 3;
        }
        else {
            /* ECN + DSCP + 4-bit Pad + Flow Label (4 bytes) */
            memcpy(&ipv6_hdr_fields[hdr_pos], &ipv6_buf->version_trafficclass, 4);
            ipv6_hdr_fields[hdr_pos] = tc;
            hdr_pos += 4;
        }
    }

    /* NH: Next Header:
     * TODO: NHC */
    ipv6_hdr_fields[hdr_pos] = ipv6_buf->nextheader;
    hdr_pos++;

    /* HLIM: Hop Limit: */
    switch(ipv6_buf->hoplimit) {
        case(1): {
            /* 01: The Hop Limit field is compressed and the hop limit is 1. */
            lowpan_iphc[0] |= 0x01;
            break;
        }

        case(64): {
            /* 10: The Hop Limit field is compressed and the hop limit is 64. */
            lowpan_iphc[0] |= 0x02;
            break;
        }

        case(255): {
            /* 11: The Hop Limit field is compressed and the hop limit is 255. */
            lowpan_iphc[0] |= 0x03;
            break;
        }

        default: {
            ipv6_hdr_fields[hdr_pos] = ipv6_buf->hoplimit;
            hdr_pos++;
            break;
        }
    }

    mutex_lock(&lowpan_context_mutex);

    /* CID: Context Identifier Extension: */
    if((lowpan_context_lookup(&ipv6_buf->srcaddr) != NULL) ||
       (lowpan_context_lookup(&ipv6_buf->destaddr) != NULL)) {
        lowpan_iphc[1] |= LOWPAN_IPHC_CID;
        memmove(&ipv6_hdr_fields[1], &ipv6_hdr_fields[0], hdr_pos);
        hdr_pos++;
    }

    /* SAC: Source Address Compression */
    if(ipv6_addr_unspec_match(&(ipv6_buf->srcaddr))) {
        /* SAC = 1 and SAM = 00 */
        lowpan_iphc[1] |= LOWPAN_IPHC_SAC;
    }
    else if((con = lowpan_context_lookup(&ipv6_buf->srcaddr)) != NULL) {
        /* 1: Source address compression uses stateful, context-based
         *    compression. */
        lowpan_iphc[1] |= LOWPAN_IPHC_SAC;
        ipv6_hdr_fields[0] |= (con->num << 4);

        if(memcmp(&(ipv6_buf->srcaddr.uint8[8]), &(iface.laddr.uint8[0]), 8) == 0) {
            /* 0 bits. The address is derived using context information
             * and possibly the link-layer addresses.*/
            lowpan_iphc[1] |= 0x30;
        }
        else if((ipv6_buf->srcaddr.uint16[4] == 0) &&
                (ipv6_buf->srcaddr.uint16[5] == 0) &&
                (ipv6_buf->srcaddr.uint16[6] == 0) &&
                ((ipv6_buf->srcaddr.uint8[14]) & 0x80) == 0) {
            /* 49-bit of interface identifier are 0, so we can compress
             * source address-iid to 16-bit */
            memcpy(&ipv6_hdr_fields[hdr_pos], &ipv6_buf->srcaddr.uint16[7], 2);
            hdr_pos += 2;
            /* 16 bits. The address is derived using context information
             * and the 16 bits carried inline. */
            lowpan_iphc[1] |= 0x20;
        }
        else {
            memcpy(&ipv6_hdr_fields[hdr_pos], &(ipv6_buf->srcaddr.uint16[4]), 8);
            hdr_pos += 8;
            /* 64 bits. The address is derived using context information
             * and the 64 bits carried inline. */
            lowpan_iphc[1] |= 0x10;
        }
    }
    else if(ipv6_prefix_ll_match(&ipv6_buf->srcaddr)) {
        /* 0: Source address compression uses stateless compression.*/
        if(memcmp(&(ipv6_buf->srcaddr.uint8[8]), &(iface.laddr.uint8[0]), 8) == 0) {
            /* 0 bits. The address is derived using context information
             * and possibly the link-layer addresses.*/
            lowpan_iphc[1] |= 0x30;
        }
        else if((ipv6_buf->srcaddr.uint16[4] == 0) &&
                (ipv6_buf->srcaddr.uint16[5] == 0) &&
                (ipv6_buf->srcaddr.uint16[6] == 0) &&
                ((ipv6_buf->srcaddr.uint8[14]) & 0x80) == 0) {
            /* 49-bit of interface identifier are 0, so we can compress
             * source address-iid to 16-bit */
            memcpy(&ipv6_hdr_fields[hdr_pos], &ipv6_buf->srcaddr.uint16[7], 2);
            hdr_pos += 2;
            /* 16 bits. The address is derived using context information
             * and the 16 bits carried inline. */
            lowpan_iphc[1] |= 0x20;
        }
        else {
            memcpy(&ipv6_hdr_fields[hdr_pos], &(ipv6_buf->srcaddr.uint16[4]), 8);
            hdr_pos += 8;
            /* 64 bits. The address is derived using context information
             * and the 64 bits carried inline. */
            lowpan_iphc[1] |= 0x10;
        }
    }
    else {
        /* full address carried inline */
        memcpy(&ipv6_hdr_fields[hdr_pos], &(ipv6_buf->srcaddr.uint8[0]), 16);
        hdr_pos += 16;
    }

    /* M: Multicast Compression */
    if(ipv6_prefix_mcast_match(&ipv6_buf->destaddr)) {
        /* 1: Destination address is a multicast address. */
        lowpan_iphc[1] |= LOWPAN_IPHC_M;

        /* just another cool if condition */
        if((ipv6_buf->destaddr.uint8[1] == 2) &&
           (ipv6_buf->destaddr.uint16[1] == 0) &&
           (ipv6_buf->destaddr.uint16[2] == 0) &&
           (ipv6_buf->destaddr.uint16[3] == 0) &&
           (ipv6_buf->destaddr.uint16[4] == 0) &&
           (ipv6_buf->destaddr.uint16[5] == 0) &&
           (ipv6_buf->destaddr.uint16[6] == 0) &&
           (ipv6_buf->destaddr.uint8[14] == 0)) {
            /* 11: 8 bits. The address takes the form FF02::00XX. */
            lowpan_iphc[1] |= 0x03;
            ipv6_hdr_fields[hdr_pos] = ipv6_buf->destaddr.uint8[15];
            hdr_pos++;
        }
        else if((ipv6_buf->destaddr.uint16[1] == 0) &&
                (ipv6_buf->destaddr.uint16[2] == 0) &&
                (ipv6_buf->destaddr.uint16[3] == 0) &&
                (ipv6_buf->destaddr.uint16[4] == 0) &&
                (ipv6_buf->destaddr.uint16[5] == 0) &&
                (ipv6_buf->destaddr.uint8[12] == 0)) {
            /* 10: 32 bits. The address takes the form FFXX::00XX:XXXX. */
            lowpan_iphc[1] |= 0x02;
            /* copy second and last 3 byte */
            ipv6_hdr_fields[hdr_pos] = ipv6_buf->destaddr.uint8[1];
            hdr_pos++;
            memcpy(&ipv6_hdr_fields[hdr_pos], &ipv6_buf->destaddr.uint8[13], 3);
            hdr_pos += 3;
        }
        else if((ipv6_buf->destaddr.uint16[1] == 0) &&
                (ipv6_buf->destaddr.uint16[2] == 0) &&
                (ipv6_buf->destaddr.uint16[3] == 0) &&
                (ipv6_buf->destaddr.uint16[4] == 0) &&
                (ipv6_buf->destaddr.uint8[10] == 0)) {
            /* 01: 48 bits.  The address takes the form FFXX::00XX:XXXX:XXXX */
            lowpan_iphc[1] |= 0x01;
            /* copy second and last 5 byte */
            ipv6_hdr_fields[hdr_pos] = ipv6_buf->destaddr.uint8[1];
            hdr_pos++;
            memcpy(&ipv6_hdr_fields[hdr_pos], &ipv6_buf->destaddr.uint8[11], 5);
            hdr_pos += 5;
        }
        else {
            memcpy(&ipv6_hdr_fields[hdr_pos], &ipv6_buf->destaddr.uint8[0], 16);
            hdr_pos += 16;
        }
    }
    else {
        /* 0: Destination address is not a multicast address. */
        if((con = lowpan_context_lookup(&ipv6_buf->destaddr)) != NULL) {
            /* 1: Destination address compression uses stateful, context-based
             * compression. */
            lowpan_iphc[1] |= LOWPAN_IPHC_DAC;
            ipv6_hdr_fields[0] = con->num;

            if(memcmp(&(ipv6_buf->destaddr.uint8[8]), &(dest->uint8[0]), 8) == 0) {
                /* 0 bits. The address is derived using context information
                 * and possibly the link-layer addresses.*/
                lowpan_iphc[1] |= 0x03;
            }
            else if((ipv6_buf->destaddr.uint16[4] == 0) &&
                    (ipv6_buf->destaddr.uint16[5] == 0) &&
                    (ipv6_buf->destaddr.uint16[6] == 0) &&
                    ((ipv6_buf->destaddr.uint8[14]) & 0x80) == 0) {
                /* 49-bit of interface identifier are 0, so we can compress
                 * source address-iid to 16-bit */
                memcpy(&ipv6_hdr_fields[hdr_pos], &ipv6_buf->destaddr.uint16[7], 2);
                hdr_pos += 2;
                /* 16 bits. The address is derived using context information
                 * and the 16 bits carried inline. */
                lowpan_iphc[1] |= 0x02;
            }
            else {
                memcpy(&ipv6_hdr_fields[hdr_pos], &(ipv6_buf->destaddr.uint16[4]), 8);
                hdr_pos += 8;
                /* 64 bits. The address is derived using context information
                * and the 64 bits carried inline. */
                lowpan_iphc[1] |= 0x01;
            }
        }
        else if(ipv6_prefix_ll_match(&ipv6_buf->destaddr)) {
            if(memcmp(&(ipv6_buf->destaddr.uint8[8]), &(dest->uint8[0]), 8) == 0) {
                /* 0 bits. The address is derived using context information
                * and possibly the link-layer addresses.*/
                lowpan_iphc[1] |= 0x03;
            }
            else if((ipv6_buf->destaddr.uint16[4] == 0) &&
                    (ipv6_buf->destaddr.uint16[5] == 0) &&
                    (ipv6_buf->destaddr.uint16[6] == 0) &&
                    ((ipv6_buf->destaddr.uint8[14]) & 0x80) == 0) {
                /* 49-bit of interface identifier are 0, so we can compress
                 * source address-iid to 16-bit */
                memcpy(&ipv6_hdr_fields[hdr_pos], &ipv6_buf->destaddr.uint16[7], 2);
                hdr_pos += 2;
                /* 16 bits. The address is derived using context information
                 * and the 16 bits carried inline. */
                lowpan_iphc[1] |= 0x02;
            }
            else {
                memcpy(&ipv6_hdr_fields[hdr_pos], &(ipv6_buf->destaddr.uint16[4]), 8);
                hdr_pos += 8;
                /* 64 bits. The address is derived using context information
                 * and the 64 bits carried inline. */
                lowpan_iphc[1] |= 0x01;
            }
        }
        else {
            memcpy(&ipv6_hdr_fields[hdr_pos], &(ipv6_buf->destaddr.uint8[0]), 16);
            hdr_pos += 16;
        }
    }

    mutex_unlock(&lowpan_context_mutex, 0);

    comp_buf[0] = lowpan_iphc[0];
    comp_buf[1] = lowpan_iphc[1];

    /*uint8_t *ptr;
    if (ipv6_buf->nextheader == IPPROTO_TCP)
    	{
    	ptr = get_payload_buf_send(ipv6_ext_hdr_len);
    	}
    else
    	{
    	ptr = get_payload_buf(ipv6_ext_hdr_len);
    	}
    */
    memcpy(&ipv6_hdr_fields[hdr_pos], &ptr[IPV6_HDR_LEN], ipv6_buf->length);

    comp_len = 2 + hdr_pos + payload_length;
}

void lowpan_iphc_decoding(uint8_t *data, uint8_t length,
                          ieee_802154_long_t *s_laddr,
                          ieee_802154_long_t *d_laddr)
{
    uint8_t hdr_pos = 0;
    uint8_t *ipv6_hdr_fields = data;
    uint8_t lowpan_iphc[2];
    uint8_t cid = 0;
    uint8_t sci = 0;
    uint8_t dci = 0;

    uint8_t ll_prefix[2] = {0xfe, 0x80};
    uint8_t m_prefix[2] = {0xff, 0x02};
    lowpan_context_t *con = NULL;

    ipv6_buf = get_ipv6_buf();

    lowpan_iphc[0] = ipv6_hdr_fields[0];
    lowpan_iphc[1] = ipv6_hdr_fields[1];
    hdr_pos += 2;

    /* first check if CID flag is set */
    if(lowpan_iphc[1] & LOWPAN_IPHC_CID) {
        hdr_pos++;
        cid = 1;
    }

    /* TF: Traffic Class, Flow Label: */
    if(lowpan_iphc[0] & LOWPAN_IPHC_FL_C) {
        /* flowlabel is elided */
        if(lowpan_iphc[0] & LOWPAN_IPHC_TC_C) {
            /* traffic class is elided */
            ipv6_buf->version_trafficclass = 0x60;
            ipv6_buf->trafficclass_flowlabel = 0;
            ipv6_buf->flowlabel = 0;
        }
        else {
            /* toogle ecn/dscp order */
            ipv6_buf->version_trafficclass = 0x60 | (0x0f &
                                             (ipv6_hdr_fields[hdr_pos] >> 2));
            ipv6_buf->trafficclass_flowlabel = ((ipv6_hdr_fields[hdr_pos] >> 2) & 0x30) |
                                               ((ipv6_hdr_fields[hdr_pos] << 6) & 0xc0);
            ipv6_buf->flowlabel = 0;
            hdr_pos += 3;
        }
    }
    else {
        /* flowlabel carried inline */
        if(lowpan_iphc[0] & LOWPAN_IPHC_TC_C) {
            /* traffic class is elided */
            ipv6_buf->version_trafficclass = 0x60;
            /* ecn + 4 bit flowlabel*/
            ipv6_buf->trafficclass_flowlabel = ((ipv6_hdr_fields[hdr_pos] >> 2) & 0x30) |
                                               (ipv6_hdr_fields[hdr_pos] & 0x0f);
            hdr_pos++;
            /* copy 2byte flowlabel */
            memcpy(&ipv6_buf->flowlabel, &ipv6_hdr_fields[hdr_pos], 2);
            hdr_pos += 2;
        }
        else {
            ipv6_buf->version_trafficclass = 0x60 | (0x0f &
                                             (ipv6_hdr_fields[hdr_pos] >> 2));
            ipv6_buf->trafficclass_flowlabel = ((ipv6_hdr_fields[hdr_pos] >> 2) & 0x30) |
                                               (ipv6_hdr_fields[hdr_pos] & 0x0f) |
                                               (ipv6_hdr_fields[hdr_pos + 1] & 0x0f);
            hdr_pos += 2;
            memcpy(&ipv6_buf->trafficclass_flowlabel,
                   &ipv6_hdr_fields[hdr_pos], 2);
            hdr_pos += 2;
        }
    }

    /* NH: Next Header: */
    if(lowpan_iphc[0] & LOWPAN_IPHC_NH) {
        // TODO: next header decompression
    }
    else {
        ipv6_buf->nextheader = ipv6_hdr_fields[hdr_pos];
        hdr_pos++;
    }

    /* HLIM: Hop Limit: */
    if(lowpan_iphc[0] & 0x03) {
        switch(lowpan_iphc[0] & 0x03) {
            case(0x01): {
                ipv6_buf->hoplimit = 1;
                break;
            }

            case(0x02): {
                ipv6_buf->hoplimit = 64;
                break;
            }

            case(0x03): {
                ipv6_buf->hoplimit = 255;
                break;
            }

            default:
                break;
        }
    }
    else {
        ipv6_buf->hoplimit = ipv6_hdr_fields[hdr_pos];
        hdr_pos++;
    }

    /* CID: Context Identifier Extension: + SAC: Source Address Compression */
    if(lowpan_iphc[1] & LOWPAN_IPHC_SAC) {
        /* 1: Source address compression uses stateful, context-based
         * compression.*/
        if(cid) {
            sci = ipv6_hdr_fields[3] >> 4;
        }

        mutex_lock(&lowpan_context_mutex);

        /* check context number */
        if(((lowpan_iphc[1] & LOWPAN_IPHC_SAM) >> 4) & 0x03) {
            con = lowpan_context_num_lookup(sci);
        }

        if(con == NULL) {
            printf("ERROR: context not found\n");
            return;
        }

        switch(((lowpan_iphc[1] & LOWPAN_IPHC_SAM) >> 4) & 0x03) {
            case(0x01): {
                /* 64-bits */
                memcpy(&(ipv6_buf->srcaddr.uint8[8]), &ipv6_hdr_fields[hdr_pos], 8);
                /* By draft-ietf-6lowpan-hc-15 3.1.1. Bits covered by context
                 * information are always used. */
                memcpy(&(ipv6_buf->srcaddr.uint8[0]), &con->prefix, con->length);
                hdr_pos += 8;
                break;
            }

            case(0x02): {
                /* 16-bits */
                memset(&(ipv6_buf->srcaddr.uint8[8]), 0, 6);
                memcpy(&(ipv6_buf->srcaddr.uint8[14]), &ipv6_hdr_fields[hdr_pos], 2);
                /* By draft-ietf-6lowpan-hc-15 3.1.1. Bits covered by context
                 * information are always used. */
                memcpy(&(ipv6_buf->srcaddr.uint8[0]), &con->prefix, con->length);
                hdr_pos += 2;
                break;
            }

            case(0x03): {
                /* 0-bits */
                memset(&(ipv6_buf->srcaddr.uint8[8]), 0, 8);
                memcpy(&(ipv6_buf->srcaddr.uint8[8]), &s_laddr->uint8[0], 8);
                /* By draft-ietf-6lowpan-hc-15 3.1.1. Bits covered by context
                 * information are always used. */
                memcpy(&(ipv6_buf->srcaddr.uint8[0]), &con->prefix, con->length);
                break;
            }

            default: {
                /* unspecified address */
                memset(&(ipv6_buf->srcaddr.uint8[0]), 0, 16);
                break;
            }
        }

        mutex_unlock(&lowpan_context_mutex, 0);
    }
    else {
        switch(((lowpan_iphc[1] & LOWPAN_IPHC_SAM) >> 4) & 0x03) {
            case(0x01): {
                /* 64-bits */
                memcpy(&(ipv6_buf->srcaddr.uint8[0]), &ll_prefix[0], 2);
                memset(&(ipv6_buf->srcaddr.uint8[2]), 0, 6);
                memcpy(&(ipv6_buf->srcaddr.uint8[8]), &ipv6_hdr_fields[hdr_pos], 8);
                hdr_pos += 8;
                break;
            }

            case(0x02): {
                /* 16-bits */
                memcpy(&(ipv6_buf->srcaddr.uint8[0]), &ll_prefix[0], 2);
                memset(&(ipv6_buf->srcaddr.uint8[2]), 0, 12);
                memcpy(&(ipv6_buf->srcaddr.uint8[14]), &ipv6_hdr_fields[hdr_pos], 2);
                hdr_pos += 2;
                break;
            }

            case(0x03): {
                /* 0-bits */
                memcpy(&(ipv6_buf->srcaddr.uint8[0]), &ll_prefix[0], 2);
                memset(&(ipv6_buf->srcaddr.uint8[8]), 0, 14);
                memcpy(&(ipv6_buf->srcaddr.uint8[8]), &s_laddr->uint8[0], 8);
                break;
            }

            default: {
                /* full address carried inline */
                memcpy(&(ipv6_buf->srcaddr.uint8[0]),
                       &ipv6_hdr_fields[hdr_pos], 16);
                hdr_pos += 16;
                break;
            }
        }
    }

    /* M: Multicast Compression + DAC: Destination Address Compression */
    if(lowpan_iphc[1] & LOWPAN_IPHC_M) {
        /* 1: Destination address is a multicast address. */
        if(lowpan_iphc[1] & LOWPAN_IPHC_DAC) {
            /* 1: Destination address compression uses stateful, context-based
             * compression.
             * If M=1 and DAC=1: */
            if(cid) {
                dci = ipv6_hdr_fields[3] & 0x0f;
            }

            mutex_lock(&lowpan_context_mutex);

            if((lowpan_iphc[1] & LOWPAN_IPHC_DAM) & 0x03) {
                con = lowpan_context_num_lookup(dci);
            }

            if(con == NULL) {
                printf("ERROR: context not found\n");
                return;
            }

            // TODO:
            mutex_unlock(&lowpan_context_mutex, 0);
        }
        else {
            /* If M=1 and DAC=0: */
            switch(lowpan_iphc[1] & 0x03) {
                case(0x01): {
                    m_prefix[1] = ipv6_hdr_fields[hdr_pos];
                    hdr_pos++;
                    break;
                }

                case(0x02): {
                    m_prefix[1] = ipv6_hdr_fields[hdr_pos];
                    hdr_pos++;
                    break;
                }

                default:
                    break;
            }

            switch(lowpan_iphc[1] & 0x03) {
                case(0x01): {
                    memcpy(&(ipv6_buf->destaddr.uint8[0]), &m_prefix[0], 2);
                    memset(&(ipv6_buf->destaddr.uint8[2]), 0, 9);
                    memcpy(&(ipv6_buf->destaddr.uint8[11]), &ipv6_hdr_fields[hdr_pos], 5);
                    hdr_pos += 5;
                    break;
                }

                case(0x02): {
                    memcpy(&(ipv6_buf->destaddr.uint8[0]), &m_prefix[0], 2);
                    memset(&(ipv6_buf->destaddr.uint8[2]), 0, 11);
                    memcpy(&(ipv6_buf->destaddr.uint8[13]), &ipv6_hdr_fields[hdr_pos], 3);
                    hdr_pos += 3;
                    break;
                }

                case(0x03): {
                    memcpy(&(ipv6_buf->destaddr.uint8[0]), &m_prefix[0], 2);
                    memset(&(ipv6_buf->destaddr.uint8[2]), 0, 13);
                    memcpy(&(ipv6_buf->destaddr.uint8[15]), &ipv6_hdr_fields[hdr_pos], 1);
                    hdr_pos++;
                    break;
                }

                default: {
                    memcpy(&(ipv6_buf->destaddr.uint8[0]), &ipv6_hdr_fields[hdr_pos], 16);
                    break;
                }
            }
        }
    }
    else {
        if(lowpan_iphc[1] & LOWPAN_IPHC_DAC) {
            /* 1: Destination address compression uses stateful, context-based
             * compression.
             * If M=1 and DAC=1: */
            if(cid) {
                dci = ipv6_hdr_fields[3] & 0x0f;
            }

            mutex_lock(&lowpan_context_mutex);

            if((lowpan_iphc[1] & LOWPAN_IPHC_DAM) & 0x03) {
                con = lowpan_context_num_lookup(dci);
            }

            if(con == NULL) {
                printf("ERROR: context not found\n");
                return;
            }

            switch((lowpan_iphc[1] & LOWPAN_IPHC_DAM) & 0x03) {
                case(0x01): {
                    memcpy(&(ipv6_buf->destaddr.uint8[8]), &ipv6_hdr_fields[hdr_pos], 8);
                    /* By draft-ietf-6lowpan-hc-15 3.1.1. Bits covered by context information are always used. */
                    memcpy(&(ipv6_buf->srcaddr.uint8[0]), &con->prefix, con->length);
                    hdr_pos += 8;
                    break;
                }

                case(0x02): {
                    memset(&(ipv6_buf->destaddr.uint8[8]), 0, 6);
                    memcpy(&(ipv6_buf->destaddr.uint8[14]), &ipv6_hdr_fields[hdr_pos], 2);
                    /* By draft-ietf-6lowpan-hc-15 3.1.1. Bits covered by context information are always used. */
                    memcpy(&(ipv6_buf->srcaddr.uint8[0]), &con->prefix, con->length);
                    hdr_pos += 2;
                    break;
                }

                case(0x03): {
                    memset(&(ipv6_buf->destaddr.uint8[0]), 0, 8);
                    memcpy(&(ipv6_buf->destaddr.uint8[8]), &d_laddr->uint8[0], 8);
                    /* By draft-ietf-6lowpan-hc-15 3.1.1. Bits covered by context information are always used. */
                    memcpy(&(ipv6_buf->srcaddr.uint8[0]), &con->prefix, con->length);
                    break;
                }

                default:
                    break;
            }

            mutex_unlock(&lowpan_context_mutex, 0);
        }
        else {
            switch((lowpan_iphc[1] & LOWPAN_IPHC_DAM) & 0x03) {
                case(0x01): {
                    memcpy(&(ipv6_buf->destaddr.uint8[0]), &ll_prefix[0], 2);
                    memset(&(ipv6_buf->destaddr.uint8[2]), 0, 6);
                    memcpy(&(ipv6_buf->destaddr.uint8[8]),
                           &ipv6_hdr_fields[hdr_pos], 8);
                    hdr_pos += 8;
                    break;
                }

                case(0x02): {
                    memcpy(&(ipv6_buf->destaddr.uint8[0]), &ll_prefix[0], 2);
                    memset(&(ipv6_buf->destaddr.uint8[2]), 0, 12);
                    memcpy(&(ipv6_buf->destaddr.uint8[14]),
                           &ipv6_hdr_fields[hdr_pos], 2);
                    hdr_pos += 2;
                    break;
                }

                case(0x03): {
                    memcpy(&(ipv6_buf->destaddr.uint8[0]), &ll_prefix, 2);
                    memset(&(ipv6_buf->destaddr.uint8[2]), 0, 14);
                    memcpy(&(ipv6_buf->destaddr.uint8[8]), &d_laddr->uint8[0], 8);
                    break;
                }

                default: {
                    memcpy(&(ipv6_buf->destaddr.uint8[0]),
                           &ipv6_hdr_fields[hdr_pos], 16);
                    hdr_pos += 16;
                    break;
                }
            }
        }
    }

    uint8_t *ptr = get_payload_buf(ipv6_ext_hdr_len);

    memcpy(ptr, &ipv6_hdr_fields[hdr_pos], length - hdr_pos);

    /* ipv6 length */
    ipv6_buf->length = length - hdr_pos;
    packet_length = IPV6_HDR_LEN + ipv6_buf->length;
}

uint8_t lowpan_context_len()
{
    return context_len;
}

void lowpan_context_remove(uint8_t num)
{
    int i, j;

    for(i = 0; i < LOWPAN_CONTEXT_MAX; i++) {
        if(contexts[i].num == num) {
            context_len--;
            break;
        }
    }

    abr_remove_context(num);

    for(j = i; j < LOWPAN_CONTEXT_MAX; j++) {
        contexts[j] = contexts[j + 1];
    }
}

lowpan_context_t *lowpan_context_update(uint8_t num, const ipv6_addr_t *prefix,
                                        uint8_t length, uint8_t comp,
                                        uint16_t lifetime)
{
    lowpan_context_t *context;

    if(lifetime == 0) {
        lowpan_context_remove(num);
        return NULL;
    }

    if(context_len == LOWPAN_CONTEXT_MAX) {
        return NULL;
    }

    context = lowpan_context_num_lookup(num);

    if(context == NULL) {
        context = &(contexts[context_len++]);
    }

    context->num = num;
    memset((void *)(&context->prefix), 0, 16);
    // length in bits
    memcpy((void *)(&context->prefix), (void *)prefix, length / 8);
    context->length = length;
    context->comp = comp;
    context->lifetime = lifetime;
    return context;
}

lowpan_context_t *lowpan_context_get()
{
    return contexts;
}

lowpan_context_t *lowpan_context_lookup(ipv6_addr_t *addr)
{
    int i;

    lowpan_context_t *context = NULL;

    for(i = 0; i < lowpan_context_len(); i++) {
        if(contexts[i].length > 0 && memcmp((void *)addr, &(contexts[i].prefix),
                                            contexts[i].length) == 0) {
            /* longer prefixes are always prefered */
            if(context == NULL || context->length < contexts[i].length) {
                context = &contexts[i];
            }
        }
    }

    return context;
}

lowpan_context_t *lowpan_context_num_lookup(uint8_t num)
{
    int i;

    for(i = 0; i < lowpan_context_len(); i++) {
        if(contexts[i].num == num) {
            return &contexts[i];
        }
    }

    return NULL;
}

void lowpan_context_auto_remove(void)
{
    timex_t minute = timex_set(60, 0);
    int i;
    int8_t to_remove[LOWPAN_CONTEXT_MAX];
    int8_t to_remove_size;

    while(1) {
        vtimer_sleep(minute);
        to_remove_size = 0;
        mutex_lock(&lowpan_context_mutex);

        for(i = 0; i < lowpan_context_len(); i++) {
            if(--(contexts[i].lifetime) == 0) {
                to_remove[to_remove_size++] = contexts[i].num;
            }
        }

        for(i = 0; i < to_remove_size; i++) {
            lowpan_context_remove(to_remove[i]);
        }

        mutex_unlock(&lowpan_context_mutex, 0);
    }
}

void init_reas_bufs(lowpan_reas_buf_t *buf)
{
    memset(&buf->s_laddr, 0, SIXLOWPAN_IPV6_LL_ADDR_LEN);
    memset(&buf->d_laddr, 0, SIXLOWPAN_IPV6_LL_ADDR_LEN);
    buf->ident_no = 			0;
    buf->timestamp =			0;
    buf->packet_size =			0;
    buf->current_packet_size = 	0;
    buf->packet = 				NULL;
    buf->interval_list_head	=	NULL;
    buf->next = 				NULL;
}

void sixlowpan_init(transceiver_type_t trans, uint8_t r_addr, int as_border)
{
    ipv6_addr_t tmp;
    /* init mac-layer and radio transceiver */
    sixlowmac_init(trans);

    rtc_init();
    rtc_enable();

    /* init interface addresses */
    memset(&iface, 0, sizeof(iface_t));
    set_radio_address(r_addr);
    init_802154_short_addr(&(iface.saddr));
    init_802154_long_addr(&(iface.laddr));
    /* init global buffer mutex */
    mutex_init(&buf_mutex);

    /* init lowpan context mutex */
    mutex_init(&lowpan_context_mutex);

    /* init packet_fifo mutex */
    mutex_init(&fifo_mutex);

    local_address = r_addr;

    /* init link-local address */
    ipv6_set_ll_prefix(&lladdr);

    memcpy(&(lladdr.uint8[8]), &(iface.laddr.uint8[0]), 8);
    ipv6_iface_add_addr(&lladdr, ADDR_STATE_PREFERRED, 0, 0,
                        ADDR_TYPE_LINK_LOCAL);
    ipv6_set_loaddr(&tmp);
    ipv6_iface_add_addr(&tmp, ADDR_STATE_PREFERRED, 0, 0,
                        ADDR_TYPE_LOOPBACK);
    ipv6_set_all_nds_mcast_addr(&tmp);
    ipv6_iface_add_addr(&tmp, ADDR_STATE_PREFERRED, 0, 0,
                        ADDR_TYPE_LOOPBACK);

    ipv6_iface_add_addr(&lladdr, ADDR_STATE_PREFERRED, 0, 0,
                        ADDR_CONFIGURED_AUTO);

    if(as_border) {
        ip_process_pid = thread_create(ip_process_buf, IP_PROCESS_STACKSIZE,
                                       PRIORITY_MAIN - 1, CREATE_STACKTEST,
                                       border_process_lowpan,
                                       "border_process_lowpan");
    }
    else {
        ip_process_pid = thread_create(ip_process_buf, IP_PROCESS_STACKSIZE,
                                       PRIORITY_MAIN - 1, CREATE_STACKTEST,
                                       ipv6_process, "ip_process");
    }

    nd_nbr_cache_rem_pid = thread_create(nc_buf, NC_STACKSIZE,
                                         PRIORITY_MAIN - 1, CREATE_STACKTEST,
                                         nbr_cache_auto_rem, "nbr_cache_rem");
    contexts_rem_pid = thread_create(con_buf, CON_STACKSIZE,
                                     PRIORITY_MAIN + 1, CREATE_STACKTEST,
                                     lowpan_context_auto_remove, "lowpan_context_rem");
    transfer_pid = thread_create(lowpan_transfer_buf, LOWPAN_TRANSFER_BUF_STACKSIZE,
                                 PRIORITY_MAIN - 1, CREATE_STACKTEST,
                                 lowpan_transfer, "lowpan_transfer");
}

void sixlowpan_adhoc_init(transceiver_type_t trans, ipv6_addr_t *prefix,
                          uint8_t r_addr)
{
    /* init network prefix */
    ipv6_set_prefix(prefix, prefix);
    plist_add(prefix, 64, OPT_PI_VLIFETIME_INFINITE, 0, 1, OPT_PI_FLAG_A);
    ipv6_init_iface_as_router();
    sixlowpan_init(trans, r_addr, 0);
}
