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
 * @brief       Interface Information Base interface for NHDP
 *
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 */

#ifndef IIB_TABLE_H
#define IIB_TABLE_H

#include "timex.h"
#include "kernel_types.h"

#include "nib_table.h"
#include "nhdp_address.h"
#include "nhdp_metric.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Possible L_STATUS values of a link tuple
 */
typedef enum {
    IIB_LT_STATUS_PENDING,
    IIB_LT_STATUS_LOST,
    IIB_LT_STATUS_HEARD,
    IIB_LT_STATUS_SYM,
    IIB_LT_STATUS_UNKNOWN
} iib_link_tuple_status_t;

/**
 * @brief   Link Set entry (link tuple)
 */
typedef struct iib_link_set_entry {
    nhdp_addr_entry_t *address_list_head;       /**< Pointer to head of this tuple's addresses */
    timex_t heard_time;                         /**< Time at which entry leaves heard status */
    timex_t sym_time;                           /**< Time at which entry leaves symmetry status */
    uint8_t pending;                            /**< Flag whether link is pending */
    uint8_t lost;                               /**< Flag whether link is lost */
    timex_t exp_time;                           /**< Time at which entry expires */
    nib_entry_t *nb_elt;                        /**< Pointer to corresponding nb tuple */
    iib_link_tuple_status_t last_status;        /**< Last processed status of link tuple */
    uint32_t metric_in;                         /**< Metric value for incoming link */
    uint32_t metric_out;                        /**< Metric value for outgoing link */
#if (NHDP_METRIC == NHDP_LMT_DAT)
    uint8_t dat_received[NHDP_Q_MEM_LENGTH];    /**< Queue for containing sums of rcvd packets */
    uint8_t dat_total[NHDP_Q_MEM_LENGTH];       /**< Queue for containing sums of xpctd packets */
    timex_t dat_time;                           /**< Time next HELLO is expected */
    uint8_t hello_interval;                     /**< Encoded HELLO interval value */
    uint8_t lost_hellos;                        /**< Lost HELLO count after last received HELLO */
    uint32_t rx_bitrate;                        /**< Incoming Bitrate for this link in Bit/s */
    uint16_t last_seq_no;                       /**< The last received packet sequence number */
#endif
    struct iib_link_set_entry *next;            /**< Pointer to next list entry */
} iib_link_set_entry_t;

/**
 * @brief   2-Hop Set entry (2-Hop tuple)
 */
typedef struct iib_two_hop_set_entry {
    iib_link_set_entry_t *ls_elt;               /**< Pointer to corresponding link tuple */
    nhdp_addr_t *th_nb_addr;                    /**< Address of symmetric 2-hop neighbor */
    timex_t exp_time;                           /**< Time at which entry expires */
    uint32_t metric_in;                         /**< Metric value for incoming link */
    uint32_t metric_out;                        /**< Metric value for outgoing link */
    struct iib_two_hop_set_entry *next;         /**< Pointer to next list entry */
} iib_two_hop_set_entry_t;

/**
 * @brief   Link set for a registered interface
 */
typedef struct iib_base_entry {
    kernel_pid_t if_pid;                                /**< PID of the interface */
    iib_link_set_entry_t *link_set_head;                /**< Pointer to this if's link tuples */
    iib_two_hop_set_entry_t *two_hop_set_head;          /**< Pointer to this if's 2-hop tuples */
    struct iib_base_entry *next;                        /**< Pointer to next list entry */
} iib_base_entry_t;

/**
 * @brief                   Register a new interface in the IIB
 *
 * This function creates a new empty Link Set and a new empty 2-Hop Set for the
 * given interface.
 *
 * @param[in] pid           PID of the interface
 *
 * @return                  0 on success
 * @return                  -1 on error
 */
int iib_register_if(kernel_pid_t pid);

/**
 * @brief                   Process a received HELLO message in the IIB
 *
 * @note
 * Must not be called from outside the NHDP reader's message processing.
 *
 * @param[in] if_pid        PID of the interface the message was received on
 * @param[in] nb_elt        Pointer to the Neighbor Tuple for the message originator
 * @param[in] validity_time Validity time in milliseconds for the originator's information
 * @param[in] is_sym_nb     Flag whether the link to the originator is symmetric
 * @param[in] is_lost       Flag whether the originator marked this link as lost
 *
 * @return                  Pointer to the new or updated Link Tuple
 * @return                  NULL on error
 */
iib_link_set_entry_t *iib_process_hello(kernel_pid_t if_pid, nib_entry_t *nb_elt,
                                        uint64_t validity_time, uint8_t is_sym_nb,
                                        uint8_t is_lost);

/**
 * @brief                   Add addresses to the currently constructed HELLO message
 *
 * @note
 * Must not be called from outside the NHDP writer's message creation process.
 *
 * @param[in] if_pid        PID of the interface the message is constructed for
 * @param[in] wr            The NHDP writer used for message construction
 */
void iib_fill_wr_addresses(kernel_pid_t if_pid, struct rfc5444_writer *wr);

/**
 * @brief                   Update L_STATUS of all existing Link Tuples
 *
 * @note
 * If a status change appears the steps described in section 13 of RFC 6130 are executed.
 *
 * @param[in] now           Pointer to current time timex representation
 */
void iib_update_lt_status(timex_t *now);

/**
 * @brief                   Exchange the corresponding Neighbor Tuple of existing Link Tuples
 *
 * This function exchanges the corresponding Neighbor Tuple of every Link Tuple that
 * was assigned to old_entry. Primarily used on Neighbor Tuple deletion of old_entry.
 *
 * @param[in] old_entry     Pointer to the old corresponding Neighbor Tuple
 * @param[in] new_entry     Pointer to the new corresponding Neighbor Tuple
 */
void iib_propagate_nb_entry_change(nib_entry_t *old_entry, nib_entry_t *new_entry);

/**
 * @brief                   Process steps for the chosen NHDP metric for a message
 *
 * @note
 * Must not be called from outside the NHDP reader's message processing.
 *
 * @param[in] ls_entry      Pointer to the Link Tuple that needs to be updated
 * @param[in] int_time      Interval time in milliseconds for the originator's HELLO
 */
void iib_process_metric_msg(iib_link_set_entry_t *ls_entry, uint64_t int_time);

/**
 * @brief                   Process steps for the chosen NHDP metric for a packet
 *
 * @note
 * Must not be called from outside the NHDP reader's packet processing.
 *
 * @param[in] ls_entry      Pointer to the Link Tuple that needs to be updated
 * @param[in] metric_out    Metric value for outgoing link direction
 * @param[in] seq_no        The sequence number from the received packet
 */
void iib_process_metric_pckt(iib_link_set_entry_t *ls_entry, uint32_t metric_out, uint16_t seq_no);

/**
 * @brief                   Update metric values for the chosen NHDP metric for all Link Tuples
 */
void iib_process_metric_refresh(void);

#ifdef __cplusplus
}
#endif

#endif /* IIB_TABLE_H */
/** @} */
