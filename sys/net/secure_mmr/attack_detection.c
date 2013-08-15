/*
 * attack_detection.c - Looks at incoming packets to discover an attack
 *
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * This source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 */

/*
 * @file        attack_detection.c
 * @brief       Looks at incoming packets to discover if an attack has taken place
 *
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Mark Essien <markessien@gmail.com>
 * @author      Nicolai Schmittberger <nicolai.schmittberger@fu-berlin.de>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * @date        08.09.2013 20:08:03
 */

/*---------------------------------------------------------------------------*/
//                                      Included Files
/*---------------------------------------------------------------------------*/

/*  Radio includes */
#include "drivers/cc110x/cc1100.h"
#include "radio/radio.h"
#include "cc1100_phy.h"

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

/* Project includes */
#include "secure_routing_globals.h"
#include "network_security.h"
#include "security_update.h"
#include "keyexchange.h"
#include "microeval.h"
#include "network_nodes.h"

// Packet Security
#include "packet_security.h"
#include "replay_protection.h"



/*---------------------------------------------------------------------------*/
//                                      Functions
/*---------------------------------------------------------------------------*/

/* @brief   Check the security limits to know if we should start encrypting
 *          messages and adapting security
 */
int check_security_limits(void)
{
    int nodes_with_errors = network_nodes_count_mac_errors(network_nodes_get_root(),
                            WRONG_MAC_COUNT_LIMIT);

    if (nodes_with_errors > 0) {
        ulog("> %d nodes with %d errors detected.", nodes_with_errors,
             WRONG_MAC_COUNT_LIMIT);

        if (nodes_with_errors >= NODES_WITH_ERRORS_LIMIT_HARD) {
            ulog("!> Hard limit exceeded on errors");

            security_update_set_encrypt_security_refresh(1);
            security_update_set_encrypt_security_status(1);
            keyexchange_set_encryption_mode(
                SecurityMode_EncryptAndMACPairwiseBroadcast);

            // Generate and send new global key. Will reset error count
            keyexchange_generate_new_key();
            keyexchange_send_keyrefresh();
            replay_protection_reset_sequence_number();

            // increment key refresh interval
            uint32_t key_interval = keyexchange_get_key_refresh_interval();

            if (key_interval == KeyExchange_QUARTERLY) {
                keyexchange_set_key_refresh_interval(KeyExchange_MONTHLY);
            }
            else if (key_interval == KeyExchange_MONTHLY) {
                keyexchange_set_key_refresh_interval(KeyExchange_WEEKLY);
            }
            else if (key_interval == KeyExchange_WEEKLY) {
                keyexchange_set_key_refresh_interval(KeyExchange_DAILY);
            }
            else if (key_interval == KeyExchange_DAILY) {
                keyexchange_set_key_refresh_interval(KeyExchange_HALFDAY);
            }
            else if (key_interval == KeyExchange_HALFDAY) {
                keyexchange_set_key_refresh_interval(KeyExchange_HOURLY);
            }

            send_security_refresh(GLOBAL_BROADCAST,
                                  SecurityMode_EncryptAndMACPairwise, 1);
        }
        else if (nodes_with_errors >= NODES_WITH_ERRORS_LIMIT_SOFT) {
            ulog("!> Soft limit exceeded on errors");

            // This is equivalent to have SECURITY_ERROR_LIMIT_SOFT items on
            // the exclusion list of the sending node in parsec
            security_update_set_encrypt_security_status(1);
            security_update_set_encrypt_security_refresh(1);
            keyexchange_set_encryption_mode(
                SecurityMode_EncryptAndMACPairwiseBroadcast);

            // Generate and send new global key. Will reset error count
            keyexchange_generate_new_key();
            keyexchange_send_keyrefresh();
            replay_protection_reset_sequence_number();

            send_security_refresh(GLOBAL_BROADCAST,
                                  SecurityMode_EncryptAndMACPairwise, 1);
        }
        else {
            ulog("> Less than %d nodes found with errors, no security \
            		adaptation", NODES_WITH_ERRORS_LIMIT_SOFT);
        }
    }
    else {
        ulog("> No nodes with MAC errors found");
    }

    return NetworkSecurity_OK;
}

/* @brief  Check the MAC limits to know if we should start encrypting messages
 */
void check_mac_limits(uint16_t sender_id, security_status_item_t *item)
{
    // check if packet has this val set at all
    if (item->wrong_macs > 0 && item->wrong_mac_interval > 0) {
        ulog("> Bad MACs detected. Examining for limits");
        float wrong_mac_ratio = (float)item->wrong_macs /
                                (float)item->wrong_mac_interval;
        ulog("> Wrong mac ratio detected as %.2f", wrong_mac_ratio);

        if (wrong_mac_ratio > WRONG_MAC_RATIO_LIMIT_HARD) {
            security_update_set_encrypt_security_status(1);
            security_update_set_encrypt_security_refresh(1);
            keyexchange_set_encryption_mode(
                SecurityMode_EncryptAndMACPairwise);
        }
        else if (wrong_mac_ratio > WRONG_MAC_RATIO_LIMIT_SOFT) {
            security_update_set_encrypt_security_status(1);
            security_update_set_encrypt_security_refresh(1);
        }



        // Update the node info on this item. If it does not exist, add it
        network_node_t *node = network_nodes_find_or_add(
                                   network_nodes_get_root(),
                                   item->node_id, 0);
        node->wrong_MACs = item->wrong_macs;
    }
    else {
        ulog("> No MAC errors on this node %d", sender_id);

        // If the node previously had mac errors, then it is in the list of
        // nodes. So this is reset. If it never had mac errors, then it's not
        // in the list at all and no need to reset

        network_node_t *node = network_nodes_find(item->node_id);

        if (node) {
            node->wrong_MACs = 0;
        }
    }
}

/* @brief   Check if the packets with mac problems exceeds the soft and hard
 *          limits and send message to network security. Network security in
 *          that case will adapt the security level to match
 *          (if in optimal mode).
 */
int attack_detection_security_status_arrived(uint16_t sender_id,
        security_status_msg_t *msg)
{
    // Check if this is the basestation
    if (i_am_basestation == 0) {
        ulog_error("Attack detection called on non-basestation");
        return NetworkSecurity_NotBaseStation;
    }

    ulog("> Entered Sec-Status Arrived");


    // Check if optimal securtity is enabled
    if (security_preset != SecurityPresetOptimal) {
        ulog("!> Not adapting security because not set to optimal");
        return NetworkSecurity_NoAutoSecurity;
    }

    int seq_overflow_warning = 0;

    // Go through all nodes in message and check if any node has exceeded the
    // MAC error limit
    for (int i = 0; i < msg->item_count; i++) {
        security_status_item_t *item = &(msg->items[i]);

        ulog("> Checking %d for attack. NodeID=%d, KeySeq=%d. WrongMacs=%d. \
        		WrongMacsInterval=%d. SeqOverlow=%d", i, item->node_id,
             item->gkey_seqnmbr, item->wrong_macs, item->wrong_mac_interval,
             item->seqnmbr_overflow_warning);

        check_mac_limits(sender_id, item);

        if (item->seqnmbr_overflow_warning > 0) {
            ulog_error("Received warning about sequence number overflow from \
            		    %lu", sender_id);
            seq_overflow_warning = 1;
        }
    }

    // Go through the nodes known here and check if many have errors and we
    // need to react to them
    check_security_limits();

    if (seq_overflow_warning == 1) {
        ulog("> Sending new key because of sequence-overflow");

        keyexchange_generate_new_key();
        keyexchange_send_keyrefresh();
        replay_protection_reset_sequence_number();
    }

    return NetworkSecurity_OK;
}



