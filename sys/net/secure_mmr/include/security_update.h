/*
 * security_update.h - Handling of the security status.
 *
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * This source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 */

/*
 * @file        security_update.h
 * @brief       Handles the updates about security status.
 *              It is a Network Security component that manages the messages
 *              that are sent in the network to keep the network secure
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Mark Essien <markessien@gmail.com>
 * @author      Nicolai Schmittberger <nicolai.schmittberger@fu-berlin.de>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * @date        16.09.2013 14:59:33
 */

#ifndef __SECURITY_UPDATE_H
#define __SECURITY_UPDATE_H

// defines the number of security stauts structs that one packet can contain
#define MAX_SECURE_ITEMS (((MAX_PAYLOAD_LEN-2)/ \
                sizeof(security_status_item_t)) - 1)


/**
 * @brief Packet Structure for the Security-Refresh message
 */
typedef struct security_refresh_msg {
    // the type of the packet = security refresh
    uint8_t type;
    // the transmission mode of the global key to use from now on
    uint8_t gkey_transfer_mode;
    // the sequence number of the current global key
    uint16_t gkey_seqnmbr;
    // indicates if the security status packets are to be encrypted
    uint8_t encrypt_security_status;
} __attribute__((packed)) security_refresh_msg_t;

/**
 * @brief Structure for each status item as sent by a node
 */
typedef struct security_status_item {
    // ID of the reporting node
    uint8_t node_id;
    // currently used global key sequence number
    uint16_t gkey_seqnmbr;
    // number of wrong macs it has received so far
    uint8_t wrong_macs;
    // The interval over which the wrong macs were observed (minutes)
    uint8_t wrong_mac_interval;
    // Indicates that sequence ID will soon overflow
    uint8_t seqnmbr_overflow_warning;
} __attribute__((packed)) security_status_item_t;

/**
 * @brief Structure for the security status message that will be sent
 */
typedef struct security_status_msg {
    // the type of the packet = security status
    uint8_t type;
    // the number of status_items valid in the message
    uint8_t item_count;
    // Array where the status_items are stored
    security_status_item_t items[MAX_SECURE_ITEMS];
} __attribute__((packed)) security_status_msg_t;


/**
 * Gives a pointer to this component that can be used to call it from the
 * controller
 *
 * @return Pointer to the component
 */
network_security_component_t *security_update_get_component(void);

/**
 * Initializes the component and sets up everything needed to make sure
 * it runs. Has to be called before the component can be used
 *
 * @return NetworkSecurity_Result saying if the operation succeeded or failed
 */
int security_update_init(void);

/**
 * Called when this component should start operation. Will set a time that wait
 * an interval and then sends out the security status messages to the base
 * station
 *
 * @return NetworkSecurity_Result saying if the operation succeeded or failed
 */
int security_update_start(void);

/**
 * Turns off the timer that sends out the regular status updates
 *
 * @return NetworkSecurity_Result saying if the operation succeeded or failed
 */
int security_update_stop(void);

/**
 * Sends a refresh message directly (as a global broadcast). This will usually
 * be sent by a basestation to all nodes to alert them about the current state
 * they should be in
 *
 * @param destination             The destination for the message. Should be
 *                                the base station
 * @param gkey_transfer_mode      The new global key transfer mode for the
 *                                nodes
 * @param encrypt_security_status Says if the security status messages should
 *                                be encrypted
 *
 * @return NetworkSecurity_Result saying if the operation succeeded or failed
 */
int send_security_refresh(uint16_t destination, uint8_t gkey_transfer_mode,
                          uint8_t encrypt_security_status);

/**
 * Will send a security status between this node and the basestation.
 *
 * @param destination   The destination for the message. Should be the base
 *                      station
 *
 * @return NetworkSecurity_Result saying if the operation succeeded or failed
 */
int send_security_status(uint16_t destination);


/**
 * Sets the parameter that tells if the security status messages should be
 * encrypted or not
 *
 * @param encrypt 1 to encrypt all messages and 0 to not encrypt
 *
 */
void security_update_set_encrypt_security_status(uint8_t encrypt);

/**
 * Sets the parameter that tells if the security refresh messages should be
 * encrypted or not
 *
 * @param encrypt 1 to encrypt all messages and 0 to not encrypt
 *
 */
void security_update_set_encrypt_security_refresh(uint8_t encrypt);

/**
 * Gets the parameter that tells if a security message should be encrypted
 * or not
 *
 * @return The currently set value
 *
 */
int security_update_get_encrypt_security_status(void);

/**
 * Returns the parameter that tells if a security message should be encrypted
 * or not.
 *
 * @return The currently set value
 */
int security_update_get_encrypt_security_refresh(void);

/**
 * Prints out information about its internal state
 *
 */
void security_update_print_info(void);

#endif
