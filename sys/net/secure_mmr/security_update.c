/*
 * security_update.c - Handling of the security status.
 *
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * This source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 */

/*
 * @file        security_update.c
 * @brief       Handles the updates about security status.
 *              It is a Network Security component that manages the messages
 *              that are sent in the network to keep the network secure
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Mark Essien <markessien@gmail.com>
 * @author      Nicolai Schmittberger <nicolai.schmittberger@fu-berlin.de>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * @date        15.09.2013 18:55:13
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
#include <math.h>

/* Timer */
#include "timex.h"
#include "vtimer.h"

/* Encryption Modules */
#include "CBCMode.h"
#include "SkipJack.h"
#include "RC5.h"
#include "AES.h"
#include "TwoFish.h"
#include "3DES.h"

/* Project Includes */
#include "secure_routing_globals.h"
#include "network_security.h"
#include "security_update.h"
#include "microeval/microeval.h"
#include "network_nodes.h"
#include "keyexchange.h"
#include "packet_security.h"
#include "packet_mac.h"
#include "attack_detection.h"
#include "replay_protection.h"

/*---------------------------------------------------------------------------*/
//                                      Local Variables
/*---------------------------------------------------------------------------*/
// Indicates if security status messages will be encrypted
static uint8_t encrypt_security_status   = 0;
// Indicates if security refresh messages will be encrypted
static uint8_t encrypt_security_refresh  = 0;
// The unique ID of the security update timer
static int security_update_timer         = -1;

/*---------------------------------------------------------------------------*/
//                                      Function Declarations
/*---------------------------------------------------------------------------*/
static int packet_arrived(secure_packet_t *packet);
static int security_update_timer_ticked(int user_param);
static int security_status_arrived(secure_packet_t *packet);
static int security_refresh_arrived(secure_packet_t *packet);
static int security_update_event(enum NetworkSecurity_Event e,
                                 uint32_t param1);
static int start_security_timer(void);

/**
 * Interface over which the controllers can communicate with
 * the security update component
 */
network_security_component_t security_update_component = {
    "SecurityUpdate",
    security_update_init,
    security_update_start,
    security_update_stop,
    packet_arrived,
    security_update_event
};

/**
 * Gives a pointer to this component that can be used to call it from the
 * controller
 *
 * @return Pointer to the component
 */
network_security_component_t *security_update_get_component()
{
    return &security_update_component;
}

/**
 * @brief Initializes the component and sets up everything. See extended
 *        description in header.
 *
 */
int security_update_init()
{
    ulog("> Inited security update component");

    return NetworkSecurity_OK;
}

/**
 * @brief Starts the functionality of the component. See extended
 *        description in header.
 *
 */
int security_update_start()
{
    ulog("> Starting security update component");

    start_security_timer();

    return NetworkSecurity_OK;
}

/**
 * @brief Starts the timers of the component.
 *
 */
int start_security_timer()
{
    uint32_t timer_interval = SECUPDATE_INTERVAL + (rand() % 60);

    ulog("> Starting security update timer. Interval=%lu", timer_interval);

    /* Set the timer to tick at regular intervals, that the security update
     * gets sent
     **/
    if (security_update_timer != -1) {
        network_security_restart_timer(security_update_timer, timer_interval);
    }
    else {
        security_update_timer = network_security_register_timer(
                                    timer_interval,
                                    0,
                                    security_update_timer_ticked);
    }

    return NetworkSecurity_OK;
}

/**
 * Called by the timer when the registered interval has arrived. Will prepare
 * message and send it out to the base-station (if this is a node)
 *
 * @param user_param  The parameter specified to the timer. Not used
 *
 * @return NetworkSecurity_Continue if the timer is to continue ticking
 */
int security_update_timer_ticked(int user_param)
{
    ulog("> Security update timer ticked");

    // When it ticks, we should send out a security status to the base station
    send_security_status(basestation_addr);

    ulog("> Security update timer complete");

    return NetworkSecurity_Continue;
}

/**
 * @brief Turns off the timer that sends out the regular status updates.
 *        Extended description in header
 *
 */
int security_update_stop()
{
    network_security_stop_timer(security_update_timer);

    return NetworkSecurity_OK;
}

/**
 * Called when any packet meant for internal consumption arrives. The packet is
 * decrypted and verified at this point. The component examines if this is one
 * of the packets that can be handled here, and if so, handles it
 *
 * @param packet The pointer to the arrived packet
 *
 * @return NetworkSecurity_OK to indicate operation successful
 */
int packet_arrived(secure_packet_t *packet)
{
    if (packet->data[0] == NetworkSecurity_SecurityStatus) {
        security_status_arrived(packet);
        return NetworkSecurity_PacketHandled;;
    }
    else if (packet->data[0] == NetworkSecurity_SecurityRefresh) {
        security_refresh_arrived(packet);
        return NetworkSecurity_PacketHandled;
    }

    return NetworkSecurity_OK;
}

/**
 * Called to handle a security refresh packet. This packet is sent from the
 * base-station
 *
 * @param packet The pointer to the arrived packet
 *
 * @return NetworkSecurity_OK to indicate operation successful
 */
int security_refresh_arrived(secure_packet_t *packet)
{
    security_refresh_msg_t *msg = (security_refresh_msg_t *)packet->data;
    ulog("> Received sec-refresh msg. global key transfer mode=%d, gk-seq=%d, \
	      enc-sec=%d", msg->gkey_transfer_mode, msg->gkey_seqnmbr,
         msg->encrypt_security_status);

    security_update_set_encrypt_security_status(msg->encrypt_security_status);
    keyexchange_set_encryption_mode(msg->gkey_transfer_mode);

    if (msg->gkey_seqnmbr > keyexchange_get_key_seq_nmbr()) {
        keyexchange_send_keyrequest(basestation_addr);
    }

    return NetworkSecurity_OK;
}

/**
 * Called to handle a secure packet. Depending on if this is a basestation
 * or a sensor node, the replies will be different. See inline comments.
 *
 * @param packet The pointer to the arrived packet
 *
 * @return NetworkSecurity_OK to indicate operation successful
 */
int security_status_arrived(secure_packet_t *packet)
{
    // This means a node received from base station new info on security
    // change the encryption settings on packets (ex. global-keep-alive)

    security_status_msg_t *msg = (security_status_msg_t *)packet->data;
    ulog("> Received sec-status msg. Count=%d", msg->item_count);

    if (i_am_basestation) {
        // resend key to any node with wrong key version
        for (int i = 0; i < msg->item_count; i++) {
            security_status_item_t *item = &(msg->items[i]);

            ulog("> Processed sec-item %d. NodeID=%d, KeySeq=%d. WrongMacs=%d.\
		          WrongMacsInterval=%d, Overflow=%d", i, item->node_id,
                 item->gkey_seqnmbr, item->wrong_macs,
                 item->wrong_mac_interval, item->seqnmbr_overflow_warning);

            if (item->gkey_seqnmbr != keyexchange_get_key_seq_nmbr()) {
                // Start key transfer to this node
                keyexchange_send_keytransfer(
                    item->node_id,
                    SecurityMode_EncryptAndMACPairwise);
            }
        }

        ulog("> Sending message to attack detection");
        attack_detection_security_status_arrived(packet->source, msg);
    }
    else {
        if (packet->destination == get_this_node_address()) {
            ulog_error("Sec-Status sent directly to node. Send it to the \
			            base-station. Not processing");
            return NetworkSecurity_PacketHandled;
        }

        // Check if this node is on the list. If so, just forward packet
        // Otherwise, append our own security update message, then forward

        ulog("> Routed security-status message received");

        int found = 0;
        int i = 0;

        for (i = 0; i < msg->item_count; i++) {
            security_status_item_t *item = &(msg->items[i]);

            if (item->node_id == get_this_node_address()) {
                found  = 1;
                // This node is already on the list
                ulog("> Node already on list");
                break;
            }
        }

        if (found == 0 && i < MAX_SECURE_ITEMS - 1) {
            // means we were not in the list. Append
            int was_reset = 0;
            msg->items[i].node_id = get_this_node_address();
            msg->items[i].gkey_seqnmbr = keyexchange_get_key_seq_nmbr();
            msg->items[i].wrong_macs =
                packet_mac_get_wrong_mac_count(&was_reset);
            msg->items[i].seqnmbr_overflow_warning =
                replay_protection_get_overflow();

            if (was_reset == 1) {
                // Value of 0 here indicates this has already been sent
                msg->items[i].wrong_mac_interval = 0;
            }
            else {
                msg->items[i].wrong_mac_interval = WRONG_MAC_TIME_INTERVAL;
            }

            msg->item_count++;

            // Reset mac errors so it does not get resent
            packet_mac_reset_mac_errors();

            ulog("> Added this node to sec-status at pos %d. KeySeq=%d. \
		          Node_id=%d, wrong_macs=%lu. wrong_macs_interval=%d, \
		          Overflow=%d", i,
                 msg->items[i].gkey_seqnmbr, msg->items[i].node_id,
                 msg->items[i].wrong_macs, msg->items[i].wrong_mac_interval,
                 msg->items[i].seqnmbr_overflow_warning);
        }

        // Make sure if we route such an item, we reset the timer
        // network_security_restart_timer(security_update_timer);
    }

    return NetworkSecurity_PacketHandled;
}

/**
 * @brief Sends a refresh message directly (as a global broadcast). See
 *                header file for extended description
 */
int send_security_refresh(uint16_t destination, uint8_t gkey_transfer_mode,
                          uint8_t encrypt_security_status)
{
    if (!i_am_basestation) {
        ulog_error("This is sensor node. Send security refresh from \
			        base-station");
        return NetworkSecurity_NotBaseStation;
    }

    ulog("Sending Sec Refresh. Dest=%d gkey_trans=%d enc_sec=%d",
         destination, gkey_transfer_mode, encrypt_security_status);

    // Prepare security refresh packet
    security_refresh_msg_t msg;
    msg.type = NetworkSecurity_SecurityRefresh;
    msg.gkey_transfer_mode = gkey_transfer_mode;
    msg.gkey_seqnmbr = keyexchange_get_key_seq_nmbr();
    msg.encrypt_security_status = encrypt_security_status;

    enum SecurityMode sec = SecurityMode_MAC;

    if (encrypt_security_refresh) {
        ulog("> Sending security refresh encrypted");
        sec = SecurityMode_EncryptAndMAC;
    }
    else {
        ulog("> Sending security refresh unencrypted");
    }

    // Send it out via global broadcast
    return network_security_send_packet((uint8_t *)&msg,
                                        sizeof(security_refresh_msg_t),
                                        GLOBAL_BROADCAST, DEFAULT_NET_TTL,
                                        sec,
                                        PeekHint_Peek);
}

/**
 * @brief Sends out a security status message to the base-station. See
 *        header file for extended description
 */
int send_security_status(uint16_t destination)
{
    // Send it out via routing to base station
    if (destination == 0) {
        destination = basestation_addr;
    }

    if (i_am_basestation) {
        ulog_error("This is base-station. Will not send security status \
			        messages");
        return NetworkSecurity_NotBaseStation;
    }

    ulog("> Sending security status message to %d", (uint8_t)destination);

    // Prepare security refresh item
    int was_reset = 0;
    security_status_item_t sec_item;
    sec_item.node_id = get_this_node_address();
    sec_item.gkey_seqnmbr = keyexchange_get_key_seq_nmbr();
    sec_item.wrong_macs = packet_mac_get_wrong_mac_count(&was_reset);
    sec_item.seqnmbr_overflow_warning = replay_protection_get_overflow();

    if (was_reset == 1) {
        // Value of 0 here indicates this has already been sent
        sec_item.wrong_mac_interval = 0;
    }
    else {
        sec_item.wrong_mac_interval = WRONG_MAC_TIME_INTERVAL;
    }

    /* Prepare the packet itself */
    security_status_msg_t msg;
    msg.type = NetworkSecurity_SecurityStatus;
    msg.item_count = 1;
    msg.items[0] = sec_item;

    enum NetworkSecurity_Result result = NetworkSecurity_OK;

    enum SecurityMode sec = SecurityMode_MAC;

    if (encrypt_security_status) {
        sec = SecurityMode_EncryptAndMAC;
    }

    ulog("> Sending SecStatus - wrong_macs=%d. Interval=%d gkey=%d. Sec=%d. \
	      Overflow=%d", sec_item.wrong_macs, sec_item.wrong_mac_interval,
         sec_item.gkey_seqnmbr, sec, sec_item.seqnmbr_overflow_warning);
    result = network_security_send_packet((uint8_t *)&msg,
                                          sizeof(security_status_msg_t),
                                          destination, DEFAULT_NET_TTL,
                                          sec,
                                          PeekHint_Peek);

    if (result == NetworkSecurity_OK) {
        ulog("> Send successful. Reseting mac errors");
        packet_mac_reset_mac_errors();
    }

    return result;
}


/**
 * @brief Called to indicate that an event has been observed that this
 *         component may need to react to
 *
 */
int security_update_event(enum NetworkSecurity_Event e, uint32_t param1)
{
    if (e == NetworkSecurity_OverflowWarning) {
        // Do nothing actively. Will be sent on next sec_status error
    }

    return NetworkSecurity_OK;
}

/**
 * @brief Sets the parameter that tells if a security message should be
 *        encrypted or not. See header file for further details
 *
 */
void security_update_set_encrypt_security_status(uint8_t encrypt)
{
    ulog("> Setting encrypt-sec-status to %d", encrypt != 0);
    encrypt_security_status = encrypt != 0;
}

/**
 * @brief Returns the parameter that tells if a security message should be
 *        encrypted or not. See header file for further details
 *
 */
int security_update_get_encrypt_security_refresh()
{
    return encrypt_security_refresh;
}

/**
 * @brief Sets the parameter that tells if a security message should be
 *        encrypted or not. See header file for further details
 *
 */
void security_update_set_encrypt_security_refresh(uint8_t encrypt)
{
    ulog("> Setting encrypt-sec-refresh to %d", encrypt != 0);
    encrypt_security_refresh = encrypt != 0;
}

/**
 * @brief Gets the parameter that tells if a security message should be
 *        encrypted or not. See header file for further details
 *
 */
int security_update_get_encrypt_security_status()
{
    return encrypt_security_status;
}

/**
 * @brief Prints out information about its internal state
 *
 */
void security_update_print_info()
{
    if (encrypt_security_status) {
        printf("Security status messages are ENCRYPTED\n");
    }
    else {
        printf("Security status messages are NOT ENCRYPTED\n");
    }

    if (encrypt_security_refresh) {
        printf("Security refresh messages are ENCRYPTED\n");
    }
    else {
        printf("Security refresh messages are NOT ENCRYPTED\n");
    }

    if (security_update_timer > -1) {
        printf("Security update timer ENABLED. Ticking every %ds\n",
               SECUPDATE_INTERVAL);
    }
    else {
        printf("Security update timer DISABLED\n");
    }
}
