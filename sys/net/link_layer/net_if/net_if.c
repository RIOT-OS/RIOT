/*
 * Copyright (C) 2013  Freie Universität Berlin.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup net_if
 * @{
 * @file    net_if.c
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#include <string.h>

#include "clist.h"
#include "ieee802154_frame.h"
#include "msg.h"
#include "mutex.h"
#include "transceiver.h"

#include "net_if.h"
#include "net_help.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

net_if_t interfaces[NET_IF_MAX];

#if ENABLE_DEBUG
void print_addr_hex(net_if_addr_t *addr)
{
    int i;
    DEBUG("0x");

    for (i = 0; i < addr->addr_len; i++) {
        DEBUG("%02x", ((char *)addr->addr_data)[i]);
    }

    DEBUG("\n");
}
#endif

uint8_t net_if_hex_to_dec(char c)
{
    if (c >= '0' && c <= '9') {
        return (uint8_t)(c - '0');
    }
    else if (c >= 'A' && c <= 'F') {
        return (uint8_t)(c - 55);
    }
    else if (c >= 'a' && c <= 'f') {
        return (uint8_t)(c - 87);
    }
    else {
        return 0xff;
    }
}

void net_if_hex_to_eui64(net_if_eui64_t *eui64, const char *eui64_str)
{
    int i;
    const char *eui64_rev = &eui64_str[strlen(eui64_str) - 1];
    eui64->uint64 = 0;

    for (i = 7; i >= 0 || eui64_rev >= eui64_str; i--) {
        uint8_t digit;

        while ((digit = net_if_hex_to_dec(*eui64_rev)) == 0xFF) {
            if (--eui64_rev < eui64_str) {
                return;
            }
        }

        eui64->uint8[i] = digit;
        eui64_rev--;

        while ((digit = net_if_hex_to_dec(*eui64_rev)) == 0xFF) {
            if (--eui64_rev < eui64_str) {
                return;
            }
        }

        eui64->uint8[i] |= digit << 4;
        eui64_rev--;
    }
}

void net_if_init(void)
{
    memset(&interfaces, 0, sizeof(net_if_t) * NET_IF_MAX);
}

int net_if_init_interface(uint8_t protocols, transceiver_type_t transceivers)
{
    int i;

    if (transceivers == 0) {
        DEBUG("Interface initialization: Precondition not met.\n");
        return -1;
    }

    for (i = 0; i < NET_IF_MAX; i++) {
        if (!interfaces[i].initialized) {
            interfaces[i].initialized = 1;
            interfaces[i].protocols = protocols;
            mutex_init(&interfaces[i].address_buffer_mutex);
            interfaces[i].transceivers = transceivers;
            DEBUG("Initialized interface %d for protocols %d on transceivers 0x%x\n",
                  i, protocols, transceivers);
            return i;
        }
    }

    DEBUG("Interface buffer full.\n");
    return -1;
}

int net_if_iter_interfaces(int start)
{
    if (start == NET_IF_MAX - 1) {
        return -1;
    }

    start++;

    while (start < NET_IF_MAX && !interfaces[start].initialized) {
        start++;
    }

    return start;
}

int net_if_add_address(int if_id, net_if_addr_t *addr)
{
    if (!addr || !addr->addr_data) {
        DEBUG("Address addition: Precondition not met.\n");
        return 0;
    }

    if (if_id < 0 || if_id >= NET_IF_MAX || !interfaces[if_id].initialized) {
        DEBUG("Address addition: No interface initialized with ID %d.\n", if_id);
        return 0;
    }

    mutex_lock(&interfaces[if_id].address_buffer_mutex);

    interfaces[if_id].protocols |= addr->addr_protocol;

    clist_add((clist_node_t **)&interfaces[if_id].addresses,
              (clist_node_t *)addr);

    mutex_unlock(&interfaces[if_id].address_buffer_mutex);

    return 1;
}

int net_if_del_address(int if_id, net_if_addr_t *addr)
{
    if (!addr || !addr->addr_data) {
        DEBUG("Address deletion: Precondition not met.\n");
        return 0;
    }

    if (if_id < 0 || if_id >= NET_IF_MAX || !interfaces[if_id].initialized) {
        DEBUG("Address deletion: No interface initialized with ID %d.\n", if_id);
        return 0;
    }

    mutex_lock(&interfaces[if_id].address_buffer_mutex);

    clist_remove((clist_node_t **)&interfaces[if_id].addresses,
                 (clist_node_t *)addr);

    mutex_unlock(&interfaces[if_id].address_buffer_mutex);

    return 1;
}

net_if_addr_t *net_if_iter_addresses(int if_id, net_if_addr_t **addr)
{
    if (if_id < 0 || if_id >= NET_IF_MAX || !interfaces[if_id].initialized) {
        DEBUG("Address iteration: No interface initialized with ID %d.\n", if_id);
        return NULL;
    }

    if (*addr == NULL) {
        *addr = interfaces[if_id].addresses;
        return *addr;
    }

    clist_advance((clist_node_t **)addr);

    if (*addr == interfaces[if_id].addresses) {
        *addr = NULL;
    }

    return *addr;
}

net_if_l3p_t net_if_get_l3p_types(int if_id)
{
    net_if_l3p_t protocols;

    if (if_id < 0 || if_id >= NET_IF_MAX || !interfaces[if_id].initialized) {
        DEBUG("Get L3 protocols: No interface initialized with ID %d.\n", if_id);
        return NET_IF_L3P_FAILURE;
    }

    mutex_lock(&interfaces[if_id].address_buffer_mutex);

    protocols = interfaces[if_id].protocols;

    mutex_unlock(&interfaces[if_id].address_buffer_mutex);

    return protocols;
}

int net_if_add_l3p_types(int if_id, net_if_l3p_t protocols)
{
    if (if_id < 0 || if_id >= NET_IF_MAX || !interfaces[if_id].initialized) {
        DEBUG("Add L3 protocols: No interface initialized with ID %d.\n", if_id);
        return 0;
    }

    interfaces[if_id].protocols |= protocols;

    return 1;
}

int net_if_del_l3p_types(int if_id, net_if_l3p_t protocols)
{
    net_if_addr_t *addr_ptr = NULL;

    if (if_id < 0 || if_id >= NET_IF_MAX || !interfaces[if_id].initialized) {
        DEBUG("Remove L3 protocols: No interface initialized with ID %d.\n", if_id);
        return 0;
    }

    while (net_if_iter_addresses(if_id, &addr_ptr)) {
        if (addr_ptr->addr_protocol & protocols) {
            net_if_del_address(if_id, addr_ptr);
            addr_ptr = NULL;
        }
    }

    interfaces[if_id].protocols &= ~protocols;

    return 1;
}

uint32_t net_if_transceiver_get_set_handler(int if_id, uint16_t op_type,
        void *data)
{
    DEBUG("net_if_transceiver_get_set_handler: if_id = %d, op_type = %d, data = %p\n",
          if_id, op_type, data);
    msg_t msg;
    transceiver_command_t tcmd;

    tcmd.transceivers = interfaces[if_id].transceivers;
    tcmd.data = (char *)data;
    msg.content.ptr = (char *)&tcmd;
    msg.type = op_type;
    msg_send_receive(&msg, &msg, transceiver_pid);

    return msg.content.value;
}

int net_if_send_packet_broadcast(net_if_trans_addr_m_t preferred_dest_mode,
                                 const void *payload, size_t payload_len)
{
    int if_id = -1;
    int res = 0, res_prev = 0;

    while ((if_id = net_if_iter_interfaces(if_id)) >= 0) {
        if (interfaces[if_id].transceivers & (TRANSCEIVER_CC1100 | TRANSCEIVER_NATIVE)) {
            res = net_if_send_packet(if_id, 0,
                                     payload, payload_len);
        }
        else if (preferred_dest_mode == NET_IF_TRANS_ADDR_M_SHORT) {
            res = net_if_send_packet(if_id, IEEE_802154_SHORT_MCAST_ADDR,
                                     payload, payload_len);
        }
        else {
            net_if_eui64_t mcast_addr = IEEE_802154_LONG_MCAST_ADDR;
            res = net_if_send_packet_long(if_id, &mcast_addr, payload,
                                          payload_len);
        }

        if (res_prev != 0) {
            if (res != res_prev) {
                return -1;
            }
        }
        else {
            if (res == 0) {
                break;
            }
        }

        res_prev = res;
    }

    return res;
}

int net_if_send_packet(int if_id, uint16_t target, const void *payload,
                       size_t payload_len)
{
    DEBUG("net_if_send_packet: if_id = %d, target = %d, payload = %p, "
          "payload_len = %d\n", if_id, target, payload, payload_len);
    uint32_t response;

    if (if_id < 0 || if_id >= NET_IF_MAX || !interfaces[if_id].initialized) {
        DEBUG("Send packet: No interface initialized with ID %d.\n", if_id);
        return -1;
    }

    if (interfaces[if_id].transceivers & (TRANSCEIVER_CC2420 | TRANSCEIVER_AT86RF231 | TRANSCEIVER_MC1322X | TRANSCEIVER_KW2XRF)) {
        ieee802154_packet_t p;

        memset(&p, 0, sizeof(ieee802154_packet_t));

        p.frame.payload = (uint8_t *)payload;
        p.frame.payload_len = (uint8_t)payload_len;
        p.frame.fcf.src_addr_m = (uint8_t)interfaces[if_id].trans_src_addr_m;
        p.frame.fcf.dest_addr_m = IEEE_802154_SHORT_ADDR_M;
        p.frame.fcf.ack_req = 0;
        p.frame.fcf.sec_enb = 0;
        p.frame.fcf.frame_type = IEEE_802154_DATA_FRAME;
        p.frame.fcf.frame_pend = 0;

        p.frame.dest_pan_id = net_if_get_pan_id(if_id);
        uint16_t target_h = NTOHS(target);
        memcpy(p.frame.dest_addr, &target_h, 2);
        response = net_if_transceiver_get_set_handler(if_id, SND_PKT, (void *)&p);
    }
    else {
        radio_packet_t p;
        memset(&p, 0, sizeof(radio_packet_t));
        p.data = (uint8_t *) payload;
        p.length = payload_len;
        p.dst = target;
        response = net_if_transceiver_get_set_handler(if_id, SND_PKT, (void *)&p);
    }


    return (response > payload_len) ? (int)payload_len : (int)response;
}

int net_if_send_packet_long(int if_id, net_if_eui64_t *target,
                            const void *payload, size_t payload_len)
{
    DEBUG("net_if_send_packet: if_id = %d, target = %016" PRIx64 ", "
          "payload = %p, payload_len = %d\n", if_id, NTOHLL(target->uint64), payload,
          payload_len);
    uint32_t response;

    if (if_id < 0 || if_id >= NET_IF_MAX || !interfaces[if_id].initialized) {
        DEBUG("Send packet: No interface initialized with ID %d.\n", if_id);
        return -1;
    }

    if (interfaces[if_id].transceivers & (TRANSCEIVER_CC2420 |
                                          TRANSCEIVER_AT86RF231 |
                                          TRANSCEIVER_MC1322X |
                                          TRANSCEIVER_KW2XRF)) {
        ieee802154_packet_t p;
        memset(&p, 0, sizeof(ieee802154_packet_t));
        p.frame.payload = (uint8_t *)payload;
        p.frame.payload_len = (uint8_t)payload_len;
        p.frame.fcf.src_addr_m = (uint8_t)interfaces[if_id].trans_src_addr_m;
        p.frame.fcf.dest_addr_m = IEEE_802154_LONG_ADDR_M;
        p.frame.fcf.ack_req = 0;
        p.frame.fcf.sec_enb = 0;
        p.frame.fcf.frame_type = IEEE_802154_DATA_FRAME;
        p.frame.fcf.frame_pend = 0;
        p.frame.dest_pan_id = net_if_get_pan_id(if_id);
        uint64_t target_h = NTOHLL(target->uint64);
        memcpy(p.frame.dest_addr, &target_h, 8);
        response = net_if_transceiver_get_set_handler(if_id, SND_PKT, (void *)&p);
    }
    else {
        radio_packet_t p;
        memset(&p, 0, sizeof(radio_packet_t));
        p.data = (uint8_t *) payload;
        p.length = payload_len;
        p.dst = NTOHS(target->uint16[3]);
        response = net_if_transceiver_get_set_handler(if_id, SND_PKT, (void *)&p);
    }


    return (response > payload_len) ? (int)payload_len : (int)response;
}

int net_if_register(int if_id, kernel_pid_t pid)
{
    if (if_id < 0 || if_id >= NET_IF_MAX || !interfaces[if_id].initialized) {
        DEBUG("Register thread: No interface initialized with ID %d.\n", if_id);
        return 0;
    }

    return (int)transceiver_register(interfaces[if_id].transceivers, pid);
}

int net_if_get_eui64(net_if_eui64_t *eui64, int if_id, int force_generation)
{
    uint64_t tmp;

    if (if_id < 0 || if_id >= NET_IF_MAX || !interfaces[if_id].initialized) {
        DEBUG("Get EUI-64: No interface initialized with ID %d.\n", if_id);
        return 0;
    }

    if (eui64 == NULL) {
        DEBUG("Get EUI-64: parameter eui64 is a NULL pointer.\n");
        return 0;
    }

    net_if_transceiver_get_set_handler(if_id, GET_LONG_ADDR, &tmp);

    eui64->uint64 = HTONLL(tmp);

    if (eui64->uint64 == 0 || force_generation) {
        uint16_t hwaddr = net_if_get_hardware_address(if_id);

        if (hwaddr == 0) {
            return 0;
        }

        /* RFC 6282 Section 3.2.2 / RFC 2464 Section 4 */
        eui64->uint32[0] = HTONL(0x000000ff);
        eui64->uint16[2] = HTONS(0xfe00);

        if (sizeof(hwaddr) == 2) {
            eui64->uint16[3] = HTONS(hwaddr);
        }
        else if (sizeof(hwaddr) == 1) {
            eui64->uint8[6] = 0;
            eui64->uint8[7] = (uint8_t)hwaddr;
        }
        else {
            DEBUG("Error on EUI-64 generation: do not know what to do with "
                  "hardware address of length %d\n", sizeof(hwaddr));
            return 0;
        }

    }

    return 1;
}

int net_if_set_eui64(int if_id, net_if_eui64_t *eui64)
{
    if (eui64 == NULL) {
        return 0;
    }

    uint64_t tmp = NTOHLL(eui64->uint64);

    if (if_id < 0 || if_id >= NET_IF_MAX || !interfaces[if_id].initialized) {
        DEBUG("Set EUI-64: No interface initialized with ID %d.\n", if_id);
        return 0;
    }

    net_if_transceiver_get_set_handler(if_id, SET_LONG_ADDR, (void *) &tmp);

    return eui64->uint64 != 0;
}

uint16_t net_if_get_hardware_address(int if_id)
{
    uint16_t addr;

    if (if_id < 0 || if_id >= NET_IF_MAX || !interfaces[if_id].initialized) {
        DEBUG("Get hardware address: No interface initialized with ID %d.\n", if_id);
        return 0;
    }

    net_if_transceiver_get_set_handler(if_id, GET_ADDRESS, &addr);
    return addr;
}

uint16_t net_if_set_hardware_address(int if_id, uint16_t addr)
{
    if (if_id < 0 || if_id >= NET_IF_MAX || !interfaces[if_id].initialized) {
        DEBUG("Set hardware address: No interface initialized with ID %d.\n", if_id);
        return 0;
    }

    net_if_transceiver_get_set_handler(if_id, SET_ADDRESS, &addr);
    return addr;
}

int32_t net_if_get_channel(int if_id)
{
    int32_t channel;

    if (if_id < 0 || if_id >= NET_IF_MAX || !interfaces[if_id].initialized) {
        DEBUG("Get channel: No interface initialized with ID %d.\n", if_id);
        return -1;
    }

    net_if_transceiver_get_set_handler(if_id, GET_CHANNEL, &channel);
    return channel;
}

int32_t net_if_set_channel(int if_id, uint16_t channel)
{
    if (if_id < 0 || if_id >= NET_IF_MAX || !interfaces[if_id].initialized) {
        DEBUG("Set channel: No interface initialized with ID %d.\n", if_id);
        return -1;
    }

    net_if_transceiver_get_set_handler(if_id, SET_CHANNEL, &channel);
    return channel;
}

int32_t net_if_get_pan_id(int if_id)
{
    int32_t pan_id;

    if (if_id < 0 || if_id >= NET_IF_MAX || !interfaces[if_id].initialized) {
        DEBUG("Get PAN ID: No interface initialized with ID %d.\n", if_id);
        return -1;
    }

    net_if_transceiver_get_set_handler(if_id, GET_PAN, &pan_id);
    if (pan_id < 0) {
        return 0;
    }
    else {
        return pan_id;
    }

}

int32_t net_if_set_pan_id(int if_id, uint16_t pan_id)
{
    if (if_id < 0 || if_id >= NET_IF_MAX || !interfaces[if_id].initialized) {
        DEBUG("Set PAN ID: No interface initialized with ID %d.\n", if_id);
        return -1;
    }

    net_if_transceiver_get_set_handler(if_id, SET_PAN, &pan_id);
    return pan_id;
}

/**
 * @}
 */
