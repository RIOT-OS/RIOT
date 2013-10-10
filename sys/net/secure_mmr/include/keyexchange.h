/*
 * keyexchange.c - Exchanges the keys with other nodes in the network
 *
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * This source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 */

/*
 * @file        keyexchange.h
 * @brief       Exchanges the keys with other nodes in the network.
 *              Transfers key between nodes
 *
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics,
 * @author      Nicolai Schmittberger <nicolai.schmittberger@fu-berlin.de>
 * @author      Mark Essien <markessien@gmail.com>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * @date        16.09.2013 11:46:06
 */

#ifndef __KEY_EXCHANGE_H
#define __KEY_EXCHANGE_H


/**
 * @brief Hello-Message
 *
 * This represents the content of an ACK-Message
 *
 */
typedef struct hello_msg {
    // the type of the Message - here HELLO
    uint8_t type;
    // true/false - is this node a base-station
    uint8_t is_basestation;
    // Nonce of the Sender -> NonceA
    uint8_t nonce[NONCE_SIZE];
} __attribute__((packed)) hello_msg_t;

/**
 * @brief ACK-Message
 *
 * This represents the content of an ACK-Message
 *
 */
typedef struct ack_msg {
    // the type of the Message - here ACK
    uint8_t  type;
    // replay protection counter of the sender (for freshness)
    uint32_t sender_sequence_nmbr;
    // Global key sequence number of sender
    uint16_t sender_key_nmbr;
    // own Nonce -> NonceB
    uint8_t  nonce[NONCE_SIZE];
} __attribute__((packed)) ack_msg_t;

/**
 * @brief Key-Request-Message
 *
 * This represents the content of an Key-Request-Message
 *
 */
typedef struct key_request {
    // the type of the Message - here Key-Request
    uint8_t type;
    // Sequence number of the global key
    uint16_t gkey_seq_nmbr;
} __attribute__((packed)) key_request_t;

/**
 * @brief Key-Transfer-Message
 *
 * This represents the content of an Key-Transer-Message
 *
 */
typedef struct key_transfer {
    // the type of the Message - here Key-Transfer
    uint8_t type;
    // Sequence number of the global key
    uint16_t gkey_seq_nmbr;
    // Global Key
    uint8_t  gkey[SECURE_ROUTING_KEYSIZE];
} __attribute__((packed)) key_transfer_t;


/**
 * Returns the key exchange component
 *
 * @return Key exchange component
 */
network_security_component_t *keyexchange_get_component(void);

/**
 * Initializes the Key Exchange components
 *
 * @return Whether initialization was successful.
 */
int keyexchange_init(void);

/**
 * Starts the process of key exchange
 *
 * @return Whether initialization was successful.
 */
int keyexchange_start(void);


/**
 * Stop the process of key exchange
 *
 * @return Whether stop was successful.
 */
int keyexchange_stop(void);

/**
 * Sends a broadcast hello-message out if destination = 0
 *
 * @param destination The target address or 0 for broadcast
 *
 * @return Whether message sending was successful.
 */
int keyexchange_sendhello(uint16_t destination);

/**
 * Sends a key-request message to the base-station
 *
 * @param destination  The address of the node to request key from
 *
 * @return Whether message sending was successful.
 */
int keyexchange_send_keyrequest(uint8_t destination);

/**
 * Used to inform the key-exchange component that an event
 * has happened, and that it should potentially react to this
 * event. For example, when the no-neighbours event arrives here
 * the component will renenable its timer to periodically send
 * out hello messages
 *
 * @param e         The event ID
 * @param param1    Optional user defined parameter
 *
 * @return A NetworkSecurity_Result value indicating if it succeeded or failed
 */
int keyexchange_event(enum NetworkSecurity_Event e, uint32_t param1);

/**
 * Returns the sequence number of the current global key
 *
 * @return The current sequence number of the global key
 */
uint16_t keyexchange_get_key_seq_nmbr(void);

/**
 * Starts a global key update. This can only be called from the
 * base-station. In pairwise mode, the key is sent to all neighbours
 * individually, and in global mode (!pairwise_mode), the message
 * is encrypted with the previous global key and sent out
 *
 *
 * @return The success or failure of the operation
 */
int keyexchange_send_keyrefresh(void);

/**
 * Starts a keytransfer to the specified destination. This will
 * send the current global key to that node. The node could be a
 * direct neighbour or could be routed
 *
 * @param destination  The address of the node the message is to be sent to or
 *                     global broadcast
 * @param sec_mode     The security mode of the key-transfer
 *
 * @return The current sequence number of the global key
 */
int keyexchange_send_keytransfer(uint16_t destination,
                                 enum SecurityMode sec_mode);

/**
 * Sets the encryption mode of the global key
 *
 * @param m  The encryption mode as an enum SecurityMode
 */
void keyexchange_set_encryption_mode(enum SecurityMode m);

/**
 * Gets encryption mode
 *
 * @return The currently set global key transfer encryption mode
 *
 */
enum SecurityMode keyexchange_get_encryption_mode(void);

/**
 * Gets the currently set key refresh interval
 *
 * @return key refresh interval
 */
uint32_t keyexchange_get_key_refresh_interval(void);

/**
 * Sets the currently set key refresh interval
 *
 * @param new_interval The interval of the key refresh
 */
void keyexchange_set_key_refresh_interval(uint32_t new_interval);

/**
 * Generates and saves a new global key
 */
void keyexchange_generate_new_key(void);

/**
 * Prints out information about the current keys
 */
void keyexchange_print_key_info(void);

/**
 * @brief Sets a new global key
 *
 * @param new_global_key   The new global key to set
 * @param buffer_len       The length of the sent in buffer.
 * @param key_seq_nmbr     The new sequence number of the key
 *
 */
void keyexchange_set_new_key(uint8_t *new_global_key, int buffer_len,
                             uint16_t key_seq_nmbr);

/**
 * @brief Sets the global key sequence number
 *
 * @param seq_nmbr   The new global key sequence nmbr to set
 *
 */
void keyexchange_set_key_sequence_number(uint16_t seq_nmbr);

#endif
