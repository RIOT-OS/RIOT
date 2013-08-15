/*
 * secure_routing_config.h - settings for the secure routing.
 *
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * This source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 */

#ifndef __SECURE_CONFIG_H
#define __SECURE_CONFIG_H

#include "cc1100_phy.h"

/*
 * @file        secure_routing_config.h
 * @brief       Contains all the settings for the secure routing.
 *
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Mark Essien <markessien@gmail.com>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * @date        16.09.2013 13:20:17
 */


/**
 * Defines that control the behaviour and the parameters of the secure-routing
 * layer. Can be modified here to change the behaviour
 */

// The protocol number
#define SECURE_ROUTING_PROTOCOL         4
// Length of the secure header
#define SECURE_HEADER_LEN               8
// Length in bytes of the message-auth
#define SECURE_MAC_LEN                  4
// Default time-to-live for packets we send
#define DEFAULT_NET_TTL                 10
// Number of protocols that can be used via secure routing
#define MAX_SECURE_PROTOCOL_HANDLERS    32
// Interval in seconds between keep-alive messages. Default = 295
#define KEEPALIVE_INTERVAL              395
// Interval in seconds between security update packets. Default = 1200
#define SECUPDATE_INTERVAL              1200
// Size of the secure-key
#define SECURE_ROUTING_KEYSIZE          20
// largest allowed payload
#define MAX_PAYLOAD_LEN                 MAX_DATA_LENGTH- \
                                        (SECURE_HEADER_LEN + SECURE_MAC_LEN)
/* The limit where the attack detection starts encrypting. E.g 1 mac error
 * in 10 minutes
 */
#define WRONG_MAC_RATIO_LIMIT_SOFT      0.1f
/* The limit where the attack detection starts encrypting+pairwise.
 * E.g 5 mac errors in 10 minutes
 */
#define WRONG_MAC_RATIO_LIMIT_HARD      0.5f
// If 5 nodes have errors within *interval*, then activate soft security mode
#define NODES_WITH_ERRORS_LIMIT_SOFT    5
// If 15 nodes have errors within *interval*, then activate hard security mode
#define NODES_WITH_ERRORS_LIMIT_HARD    15
/* When counting nodes with errors, this is number of wrong macs node should
 * at least have (5 is default)
 */
#define WRONG_MAC_COUNT_LIMIT           5
// Time-chunk for mac errors in MINUTES (Default is 10 mins)
#define WRONG_MAC_TIME_INTERVAL         10
/* Max = 16777216. So when 216 packets left, send warnings with every
 * sec-status message
 */
#define REPLAY_OVERFLOW_WARNING         16777000
// If sequence number hits this, packets will all fail replay protection
#define REPLAY_OVERFLOW_HARD_LIMIT      16777215
// Size of the nonce
#define NONCE_SIZE                      8
// Interval between hello messages
#define HELLO_MSG_INTERVAL              3
// Packets to store for send via radio
#define RADIO_PROCESS_QUEUE_SIZE        20

/*** Settings for Jitter ***/

/* Number of radio send intervals for ack to wait before replying.
 * Default = 10. Set to 0 to disable
 */
#define ACK_WAIT_SLOTS                  10
// Unicasts will be tried 3 times
#define RADIO_SEND_RETRY                3

/*** Settings for network nodes **/

// Allow only these number of nodes in total
#define MAX_NUMBER_NODES                150
// The space reserved for neighbour nodes. Will only be enforced when no space
#define NEIGHBOURS_NODES_ALLOCATION     40
// The space reserved for routing nodes. Will only be enforced when no space
#define ROUTE_NODES_ALLOCATION          110


/*** Intervals for KeyRefresh in seconds **/
// Every 5 minutes, key exchange. Only for debug
#define KeyExchange_DEBUG               5*60
// extremely often
#define KeyExchange_HOURLY              3600
// very very often
#define KeyExchange_HALFDAY             43200
// very often (maximum recommendation)
#define KeyExchange_DAILY               86400
// frequently (recommendation)
#define KeyExchange_WEEKLY              604800
// appropriate (minimum recommendation)
#define KeyExchange_MONTHLY             2635200
// seldom (only for low security and communication)
#define KeyExchange_QUARTERLY           7905600

/**
 * User Defined Security
 */
// To change the user-defined settings for encryption, open secure_routing.c
// and go to the bottom of the function set_security_level_preset
#define USER_DEF_SECURITY_MAC_ENABLE          1
#define USER_DEF_SECURITY_ENC_ENABLE          1
#define USER_DEF_SECURITY_KEY_INTERVAL        KeyExchange_WEEKLY
#define USER_DEF_SECURITY_ENC_MODE            SecurityMode_EncryptAndMAC
#define USER_DEF_SECURITY_ENCRYPT_SEC_STATUS  1
#define USER_DEF_SECURITY_SEC_LEVEL           SecurityMode_EncryptAndMAC

#endif
