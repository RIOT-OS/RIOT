/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     nhdp
 * @{
 *
 * @file
 * @brief       Writer implementation for message generation in NHDP
 *
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 *
 * @}
 */

#include <string.h>

#include "timex.h"
#include "mutex.h"

#include "rfc5444/rfc5444.h"
#include "rfc5444/rfc5444_iana.h"
#include "rfc5444/rfc5444_writer.h"

#include "nhdp.h"
#include "nhdp_address.h"
#include "nhdp_writer.h"
#include "lib_table.h"
#include "nib_table.h"
#include "iib_table.h"

/* Internal variables */
static mutex_t mtx_packet_write = MUTEX_INIT;
static struct rfc5444_writer nhdp_writer;
static nhdp_if_entry_t *nhdp_wr_curr_if_entry;
static uint8_t msg_buffer[NHDP_WR_MSG_BUF_SIZE];
static uint8_t msg_addrtlvs[NHDP_WR_TLV_BUF_SIZE];

/* Internal function prototypes */
static void _nhdp_add_hello_msg_header_cb(struct rfc5444_writer *wr,
        struct rfc5444_writer_message *msg);
static void _nhdp_add_message_tlvs_cb(struct rfc5444_writer *wr);
static void _nhdp_add_addresses_cb(struct rfc5444_writer *wr);
static void _nhdp_add_packet_header_cb(struct rfc5444_writer *writer,
                                       struct rfc5444_writer_target *rfc5444_target);
static void netaddr_from_nhdp_address(struct netaddr *target, nhdp_addr_t *n_addr);

/* Array containing the known Address TLVs */
static struct rfc5444_writer_tlvtype _nhdp_addrtlvs[] = {
    [RFC5444_ADDRTLV_LOCAL_IF] = { .type = RFC5444_ADDRTLV_LOCAL_IF },
    [RFC5444_ADDRTLV_LINK_STATUS] = { .type = RFC5444_ADDRTLV_LINK_STATUS },
    [RFC5444_ADDRTLV_OTHER_NEIGHB] = { .type = RFC5444_ADDRTLV_OTHER_NEIGHB },
};

/* Writer content provider for HELLO messages */
static struct rfc5444_writer_content_provider _nhdp_message_content_provider = {
    .msg_type = RFC5444_MSGTYPE_HELLO,
    .addMessageTLVs = _nhdp_add_message_tlvs_cb,
    .addAddresses = _nhdp_add_addresses_cb,
};


/*---------------------------------------------------------------------------*
 *                            NHDP Writer API                                *
 *---------------------------------------------------------------------------*/

void nhdp_writer_init(void)
{
    struct rfc5444_writer_message *_hello_msg;

    mutex_lock(&mtx_packet_write);

    /* Reset current interface */
    nhdp_wr_curr_if_entry = NULL;

    /* Configure NHDP writer */
    nhdp_writer.msg_buffer = msg_buffer;
    nhdp_writer.msg_size = sizeof(msg_buffer);
    nhdp_writer.addrtlv_buffer = msg_addrtlvs;
    nhdp_writer.addrtlv_size  = sizeof(msg_addrtlvs);

    /* Initialize writer */
    rfc5444_writer_init(&nhdp_writer);

    /* Register HELLO msg with 16 byte addresses and content provider */
    rfc5444_writer_register_msgcontentprovider(&nhdp_writer,
            &_nhdp_message_content_provider, _nhdp_addrtlvs, ARRAYSIZE(_nhdp_addrtlvs));
    _hello_msg = rfc5444_writer_register_message(&nhdp_writer, RFC5444_MSGTYPE_HELLO, false, 16);
    _hello_msg->addMessageHeader = _nhdp_add_hello_msg_header_cb;

    mutex_unlock(&mtx_packet_write);
}

void nhdp_writer_cleanup(void)
{
    mutex_lock(&mtx_packet_write);

    nhdp_wr_curr_if_entry = NULL;
    rfc5444_writer_cleanup(&nhdp_writer);

    mutex_unlock(&mtx_packet_write);
}

void nhdp_writer_register_if(struct rfc5444_writer_target *new_if)
{
    mutex_lock(&mtx_packet_write);

    /* Add packet header callback to writer target of the interface */
    new_if->addPacketHeader = _nhdp_add_packet_header_cb;
    /* Register target interface in writer */
    rfc5444_writer_register_target(&nhdp_writer, new_if);

    mutex_unlock(&mtx_packet_write);
}

void nhdp_writer_send_hello(nhdp_if_entry_t *if_entry)
{
    mutex_lock(&mtx_packet_write);

    /* Register interface as current sending interface */
    nhdp_wr_curr_if_entry = if_entry;

    /* Create HELLO message and send it using the given interface */
    rfc5444_writer_create_message(&nhdp_writer, RFC5444_MSGTYPE_HELLO,
                                  rfc5444_writer_singletarget_selector, &if_entry->wr_target);
    rfc5444_writer_flush(&nhdp_writer, &if_entry->wr_target, false);

    mutex_unlock(&mtx_packet_write);
}

void nhdp_writer_add_addr(struct rfc5444_writer *wr, nhdp_addr_t *addr,
                          enum rfc5444_addrtlv_iana type, uint8_t value)
{
    struct rfc5444_writer_address *wr_addr;
    struct netaddr n_addr;

    netaddr_from_nhdp_address(&n_addr, addr);

    switch (type) {
        case RFC5444_ADDRTLV_LOCAL_IF:
            /* Address is mandatory for every sub-msg (if message is splitted) */
            wr_addr = rfc5444_writer_add_address(wr, _nhdp_message_content_provider.creator,
                                                 &n_addr, true);
            break;

        case RFC5444_ADDRTLV_LINK_STATUS:
            /* Fall through */

        case RFC5444_ADDRTLV_OTHER_NEIGHB:
            /* Address only has to be included in one sub-msg (if message is splitted) */
            wr_addr = rfc5444_writer_add_address(wr, _nhdp_message_content_provider.creator,
                                                 &n_addr, false);
            break;

        default:
            /* Unknown type, extend switch if other types are allowed */
            return;
    }

    rfc5444_writer_add_addrtlv(wr, wr_addr, &_nhdp_addrtlvs[type],
                               &value, sizeof(uint8_t), false);
}


/*------------------------------------------------------------------------------------*/
/*                                Internal functions                                  */
/*------------------------------------------------------------------------------------*/

/**
 * Set the header for the currently constructed HELLO message
 * Called by oonf_api during message creation
 */
static void
_nhdp_add_hello_msg_header_cb(struct rfc5444_writer *wr, struct rfc5444_writer_message *msg)
{
    /* No originator, no hopcount, no hoplimit, no sequence number */
    rfc5444_writer_set_msg_header(wr, msg, false, false, false, false);
}

/**
 * Add validity time and interval time message TLVs to current message
 * Called by oonf_api during message creation
 */
static void _nhdp_add_message_tlvs_cb(struct rfc5444_writer *wr)
{
    uint8_t validity_time, interval_time;
    /* Convert validity time and interval time to milliseconds */
    uint64_t val_tmp = (uint64_t) nhdp_wr_curr_if_entry->validity_time.seconds * SEC_IN_MS
                       + (nhdp_wr_curr_if_entry->validity_time.microseconds / 1000ULL);
    uint64_t int_tmp = (uint64_t) nhdp_wr_curr_if_entry->hello_interval.seconds * SEC_IN_MS
                       + (nhdp_wr_curr_if_entry->hello_interval.microseconds / 1000ULL);

    /* Add validity time (mandatory) and interval time to msg */
    validity_time = rfc5444_timetlv_encode(val_tmp);
    interval_time = rfc5444_timetlv_encode(int_tmp);
    rfc5444_writer_add_messagetlv(wr, RFC5444_MSGTLV_VALIDITY_TIME, 0, &validity_time,
                                  sizeof(validity_time));
    rfc5444_writer_add_messagetlv(wr, RFC5444_MSGTLV_INTERVAL_TIME, 0, &interval_time,
                                  sizeof(interval_time));
}

/**
 * Add addresses and corresponding TLVs to current message
 * Called by oonf_api during message creation
 */
static void _nhdp_add_addresses_cb(struct rfc5444_writer *wr)
{
    lib_fill_wr_addresses(nhdp_wr_curr_if_entry->if_pid, wr);
    iib_fill_wr_addresses(nhdp_wr_curr_if_entry->if_pid, wr);
    nib_fill_wr_addresses(wr);
    nhdp_reset_addresses_tmp_usg(0);
}

/**
 * Add packet header with sequence number to current packet
 * Called by oonf_api during packet creation
 */
static void _nhdp_add_packet_header_cb(struct rfc5444_writer *writer,
                                       struct rfc5444_writer_target *rfc5444_target)
{
    rfc5444_writer_set_pkt_header(writer, rfc5444_target, true);
    rfc5444_writer_set_pkt_seqno(writer, rfc5444_target, ++nhdp_wr_curr_if_entry->seq_no);
}

/**
 * Construct a netaddr from a given NHDP address
 */
static void netaddr_from_nhdp_address(struct netaddr *target, nhdp_addr_t *n_addr)
{
    memset(target->_addr, 0, NETADDR_MAX_LENGTH);
    memcpy(target->_addr, n_addr->addr, n_addr->addr_size);

    switch (n_addr->addr_type) {
        case AF_CC110X:
            target->_prefix_len = 8u;
            target->_type = AF_CC110X;
            break;

        case AF_INET:
            target->_prefix_len = 32u;
            target->_type = AF_INET;
            break;

        case AF_INET6:
            /* Fall-through */

        default:
            target->_prefix_len = 128u;
            target->_type = AF_INET6;
            break;
    }
}
