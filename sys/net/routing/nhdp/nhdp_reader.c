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
 * @brief       Reader implementation for message processing in NHDP
 *
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 *
 * @}
 */

#include "mutex.h"
#include "utlist.h"

#include "rfc5444/rfc5444.h"
#include "rfc5444/rfc5444_iana.h"
#include "rfc5444/rfc5444_reader.h"

#include "lib_table.h"
#include "nib_table.h"
#include "iib_table.h"
#include "nhdp.h"
#include "nhdp_address.h"
#include "nhdp_reader.h"

/* Internal variables */
struct rfc5444_reader reader;
static mutex_t mtx_packet_handler = MUTEX_INIT;

static kernel_pid_t if_pid;
static uint64_t val_time;
static uint64_t int_time;
static uint8_t sym = 0;
static uint8_t lost = 0;

/* Internal function prototypes */
static enum rfc5444_result _nhdp_blocktlv_msg_cb(struct rfc5444_reader_tlvblock_context *cont);
static enum rfc5444_result _nhdp_blocktlv_address_cb(struct rfc5444_reader_tlvblock_context *cont);
static enum rfc5444_result _nhdp_msg_end_cb(struct rfc5444_reader_tlvblock_context *cont,
        bool dropped);
static enum rfc5444_result check_msg_validity(struct rfc5444_reader_tlvblock_context *cont);
static enum rfc5444_result check_addr_validity(nhdp_addr_t *addr);
static nhdp_addr_t *get_nhdp_db_addr(uint8_t *addr, uint8_t prefix);
static void process_temp_tables(void);

/* Array containing the processable message TLVs for HELLO messages */
static struct rfc5444_reader_tlvblock_consumer_entry _nhdp_msg_tlvs[] = {
    [RFC5444_MSGTLV_INTERVAL_TIME] = { .type = RFC5444_MSGTLV_INTERVAL_TIME, .mandatory = false },
    [RFC5444_MSGTLV_VALIDITY_TIME] = { .type = RFC5444_MSGTLV_VALIDITY_TIME, .mandatory = true },
};

/* Array containing the processable address TLVs for HELLO message address blocks */
static struct rfc5444_reader_tlvblock_consumer_entry _nhdp_addr_tlvs[] = {
    [RFC5444_ADDRTLV_LOCAL_IF] = { .type = RFC5444_ADDRTLV_LOCAL_IF },
    [RFC5444_ADDRTLV_LINK_STATUS] = { .type = RFC5444_ADDRTLV_LINK_STATUS },
    [RFC5444_ADDRTLV_OTHER_NEIGHB] = { .type = RFC5444_ADDRTLV_OTHER_NEIGHB },
};

/* oonf_api message consumer used for HELLO message consumption */
static struct rfc5444_reader_tlvblock_consumer _nhdp_msg_consumer = {
    .msg_id = RFC5444_MSGTYPE_HELLO,
    .block_callback = _nhdp_blocktlv_msg_cb,
    .end_callback = _nhdp_msg_end_cb,
};

/* oonf_api message consumer user for HELLO message address block consumption */
static struct rfc5444_reader_tlvblock_consumer _nhdp_address_consumer = {
    .msg_id = RFC5444_MSGTYPE_HELLO,
    .addrblock_consumer = true,
    .block_callback = _nhdp_blocktlv_address_cb,
};


/*---------------------------------------------------------------------------*
 *                             NHDP Reader API                               *
 *---------------------------------------------------------------------------*/

void nhdp_reader_init(void)
{
    /* Initialize reader */
    rfc5444_reader_init(&reader);

    /* Register HELLO message consumer */
    rfc5444_reader_add_message_consumer(&reader, &_nhdp_msg_consumer,
                                        _nhdp_msg_tlvs, ARRAYSIZE(_nhdp_msg_tlvs));
    rfc5444_reader_add_message_consumer(&reader, &_nhdp_address_consumer,
                                        _nhdp_addr_tlvs, ARRAYSIZE(_nhdp_addr_tlvs));
}

int nhdp_reader_handle_packet(kernel_pid_t rcvg_if_pid, void *buffer, size_t length)
{
    int result;

    mutex_lock(&mtx_packet_handler);

    /* Store PID of interface this packet was received on */
    if_pid = rcvg_if_pid;
    /* Parse packet with reader */
    result = rfc5444_reader_handle_packet(&reader, buffer, length);

    mutex_unlock(&mtx_packet_handler);

    return result;
}

void nhdp_reader_cleanup(void)
{
    rfc5444_reader_cleanup(&reader);
}


/*------------------------------------------------------------------------------------*/
/*                                Internal functions                                  */
/*------------------------------------------------------------------------------------*/

/**
 * Handle one address and its corresponding TLVs
 * Called by oonf_api for every included address to allow parsing
 */
static enum rfc5444_result
_nhdp_blocktlv_address_cb(struct rfc5444_reader_tlvblock_context *cont)
{
    uint8_t tmp_result;

    /* Get NHDP address for the current netaddr */
    nhdp_addr_t *current_addr = get_nhdp_db_addr(&cont->addr._addr[0], cont->addr._prefix_len);

    if (!current_addr) {
        /* Insufficient memory */
        return RFC5444_DROP_MESSAGE;
    }

    /* Check validity of address tlvs */
    if (check_addr_validity(current_addr) != RFC5444_OKAY) {
        nhdp_decrement_addr_usage(current_addr);
        return RFC5444_DROP_MESSAGE;
    }

    /* Handle address and add it to proper temporary list */
    if (_nhdp_addr_tlvs[RFC5444_ADDRTLV_LOCAL_IF].tlv) {
        switch (*_nhdp_addr_tlvs[RFC5444_ADDRTLV_LOCAL_IF].tlv->single_value) {
            case RFC5444_LOCALIF_THIS_IF:
                current_addr->in_tmp_table = NHDP_ADDR_TMP_SEND_LIST;
                break;

            case RFC5444_LOCALIF_OTHER_IF:
                current_addr->in_tmp_table = NHDP_ADDR_TMP_NB_LIST;
                break;

            default:
                /* Wrong value, drop message */
                nhdp_decrement_addr_usage(current_addr);
                return RFC5444_DROP_MESSAGE;
        }
    }
    else if ((tmp_result = lib_is_reg_addr(if_pid, current_addr))) {
        /* The address is one of our local addresses (do not add it for processing) */
        if ((!sym) && (tmp_result == 1) && _nhdp_addr_tlvs[RFC5444_ADDRTLV_LINK_STATUS].tlv) {
            /* If address is a local address of the receiving interface, check */
            /* whether we can derive a status for this link (symmetry or lost) */
            switch (*_nhdp_addr_tlvs[RFC5444_ADDRTLV_LINK_STATUS].tlv->single_value) {
                case RFC5444_LINKSTATUS_SYMMETRIC:
                    /* Fall - through */

                case RFC5444_LINKSTATUS_HEARD:
                    sym = 1;
                    break;

                case RFC5444_LINKSTATUS_LOST:
                    lost = 1;
                    break;

                default:
                    /* Wrong value, drop message */
                    nhdp_decrement_addr_usage(current_addr);
                    return RFC5444_DROP_MESSAGE;
            }
        }

        /* Address is one of our own addresses, ignore it */
        nhdp_decrement_addr_usage(current_addr);
    }
    else if (_nhdp_addr_tlvs[RFC5444_ADDRTLV_LINK_STATUS].tlv) {
        switch (*_nhdp_addr_tlvs[RFC5444_ADDRTLV_LINK_STATUS].tlv->single_value) {
            case RFC5444_LINKSTATUS_SYMMETRIC:
                current_addr->in_tmp_table = NHDP_ADDR_TMP_TH_SYM_LIST;
                break;

            case RFC5444_LINKSTATUS_HEARD:
                /* Fall-through */

            case RFC5444_LINKSTATUS_LOST:
                if (_nhdp_addr_tlvs[RFC5444_ADDRTLV_OTHER_NEIGHB].tlv
                    && *_nhdp_addr_tlvs[RFC5444_ADDRTLV_OTHER_NEIGHB].tlv->single_value
                    == RFC5444_OTHERNEIGHB_SYMMETRIC) {
                    /* Symmetric has higher priority */
                    current_addr->in_tmp_table = NHDP_ADDR_TMP_TH_SYM_LIST;
                }
                else {
                    current_addr->in_tmp_table = NHDP_ADDR_TMP_TH_REM_LIST;
                }

                break;

            default:
                /* Wrong value, drop message */
                nhdp_decrement_addr_usage(current_addr);
                return RFC5444_DROP_MESSAGE;
        }
    }
    else if (_nhdp_addr_tlvs[RFC5444_ADDRTLV_OTHER_NEIGHB].tlv) {
        switch (*_nhdp_addr_tlvs[RFC5444_ADDRTLV_OTHER_NEIGHB].tlv->single_value) {
            case RFC5444_OTHERNEIGHB_SYMMETRIC:
                current_addr->in_tmp_table = NHDP_ADDR_TMP_TH_SYM_LIST;
                break;

            case RFC5444_OTHERNEIGHB_LOST:
                current_addr->in_tmp_table = NHDP_ADDR_TMP_TH_REM_LIST;
                break;

            default:
                /* Wrong value, drop message */
                nhdp_decrement_addr_usage(current_addr);
                return RFC5444_DROP_MESSAGE;
        }
    }
    else {
        /* Addresses without expected TLV are ignored */
        nhdp_decrement_addr_usage(current_addr);
        return RFC5444_DROP_ADDRESS;
    }

    return RFC5444_OKAY;
}

/**
 * Handle message TLVs of received HELLO
 * Called by oonf_api to allow message TLV parsing
 */
static enum rfc5444_result
_nhdp_blocktlv_msg_cb(struct rfc5444_reader_tlvblock_context *cont)
{
    /* Check whether specified message TLVs are correctly included */
    if (check_msg_validity(cont) != RFC5444_OKAY) {
        return RFC5444_DROP_MESSAGE;
    }

    /* Validity time must be included as message tlv */
    val_time = rfc5444_timetlv_decode(
                   *_nhdp_msg_tlvs[RFC5444_MSGTLV_VALIDITY_TIME].tlv->single_value);

    /* Interval time is not mandatory as message tlv */
    if (_nhdp_msg_tlvs[RFC5444_MSGTLV_INTERVAL_TIME].tlv) {
        int_time = rfc5444_timetlv_decode(
                       *_nhdp_msg_tlvs[RFC5444_MSGTLV_INTERVAL_TIME].tlv->single_value);
    }

    return RFC5444_OKAY;
}

/**
 * Process received addresses and clean up temporary stuff
 * Called by oonf_api after message was parsed
 */
static enum rfc5444_result
_nhdp_msg_end_cb(struct rfc5444_reader_tlvblock_context *cont __attribute__((unused)),
                 bool dropped)
{
    if (!dropped) {
        /* Only process the received addresses if message was valid */
        process_temp_tables();
    }

    /* Clean all temporary stuff */
    val_time = 0ULL;
    int_time = 0ULL;
    sym = 0;
    lost = 0;
    nhdp_reset_addresses_tmp_usg(1);

    if (dropped) {
        return RFC5444_DROP_MESSAGE;
    }

    return RFC5444_OKAY;
}

/**
 * Check validity of HELLO message header and message TLVs
 */
static enum rfc5444_result check_msg_validity(struct rfc5444_reader_tlvblock_context *cont)
{
    if (cont->has_hoplimit && cont->hoplimit != 1) {
        /* Hop Limit other than 1 */
        return RFC5444_DROP_MESSAGE;
    }

    if (cont->has_hopcount && cont->hopcount != 0) {
        /* Hop Count other than zero */
        return RFC5444_DROP_MESSAGE;
    }

    if (!(_nhdp_msg_tlvs[RFC5444_MSGTLV_VALIDITY_TIME].tlv)) {
        /* No validity time tlv */
        return RFC5444_DROP_MESSAGE;
    }
    else if (_nhdp_msg_tlvs[RFC5444_MSGTLV_VALIDITY_TIME].tlv->next_entry) {
        /* Multiple validity time tlvs */
        return RFC5444_DROP_MESSAGE;
    }

    if (_nhdp_msg_tlvs[RFC5444_MSGTLV_INTERVAL_TIME].tlv
        && _nhdp_msg_tlvs[RFC5444_MSGTLV_INTERVAL_TIME].tlv->next_entry) {
        /* Multiple interval time tlvs */
        return RFC5444_DROP_MESSAGE;
    }

    return RFC5444_OKAY;
}

/**
 * Check validity of address block TLVs
 */
static enum rfc5444_result check_addr_validity(nhdp_addr_t *addr)
{
    if (_nhdp_addr_tlvs[RFC5444_ADDRTLV_LOCAL_IF].tlv) {
        if (_nhdp_addr_tlvs[RFC5444_ADDRTLV_LINK_STATUS].tlv
            || _nhdp_addr_tlvs[RFC5444_ADDRTLV_OTHER_NEIGHB].tlv) {
            /* Conflicting tlv types for the address */
            return RFC5444_DROP_MESSAGE;
        }
        else if (_nhdp_addr_tlvs[RFC5444_ADDRTLV_LOCAL_IF].tlv->next_entry) {
            /* Multiple tlvs of the same type are not allowed */
            return RFC5444_DROP_MESSAGE;
        }
        else if (lib_is_reg_addr(if_pid, addr)) {
            /* Address of one of neighbor's IFs equals one of ours */
            return RFC5444_DROP_MESSAGE;
        }
    }
    else if (_nhdp_addr_tlvs[RFC5444_ADDRTLV_LINK_STATUS].tlv
             && _nhdp_addr_tlvs[RFC5444_ADDRTLV_LINK_STATUS].tlv->next_entry) {
        /* Multiple tlvs of the same type are not allowed */
        return RFC5444_DROP_MESSAGE;
    }
    else if (_nhdp_addr_tlvs[RFC5444_ADDRTLV_OTHER_NEIGHB].tlv
             && _nhdp_addr_tlvs[RFC5444_ADDRTLV_OTHER_NEIGHB].tlv->next_entry) {
        /* Multiple tlvs of the same type are not allowed */
        return RFC5444_DROP_MESSAGE;
    }

    return RFC5444_OKAY;
}

/**
 * Get a new or existing NHDP address entry from the centralized address storage
 * for the given address data
 */
static nhdp_addr_t *get_nhdp_db_addr(uint8_t *addr, uint8_t prefix)
{
    switch (prefix) {
        case 8:
            return nhdp_addr_db_get_address(addr, 1, AF_CC110X);

        case 32:
            return nhdp_addr_db_get_address(addr, 4, AF_INET);

        default:
            if (prefix < 32) {
                return nhdp_addr_db_get_address(addr, 4, AF_INET);
            }
            else {
                return nhdp_addr_db_get_address(addr, 16, AF_INET6);
            }
    }
}

/**
 * Process address lists from the HELLO msg in the information bases
 */
static void process_temp_tables(void)
{
    nib_entry_t *nib_elt;
    timex_t now;

    vtimer_now(&now);
    iib_update_lt_status(&now);

    nib_elt = nib_process_hello();

    if (nib_elt) {
        iib_process_hello(if_pid, nib_elt, val_time, sym, lost);
    }
}
