/*
 * keyexchange.c - Exchanges the keys with other nodes in the network
 *
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * This source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 */

/*
 * @file        keyexchange.c
 * @brief       Exchanges the keys with other nodes in the network, and
 *              transfers key between nodes
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics.
 * @author      Nicolai Schmittberger <nicolai.schmittberger@fu-berlin.de>
 * @author      Mark Essien <markessien@gmail.com>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 * @date        12.09.2013 16:25:17
 *
 **/

/*---------------------------------------------------------------------------*/
//                                      Included Files
/*---------------------------------------------------------------------------*/

/*  Radio includes */
#include "drivers/cc110x/cc1100.h"
#include "radio/radio.h"
#include "cc1100_phy.h"

/* Timer Includes */
#include "vtimer.h"

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

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
#include "keyexchange.h"
#include "microeval/microeval.h"
#include "network_nodes.h"

#include "packet_security.h"
#include "replay_protection.h"

/*---------------------------------------------------------------------------*/
//                                      Variables
/*---------------------------------------------------------------------------*/
static uint8_t node_nonce[NONCE_SIZE];
static CipherModeContext key_ciphermode_context;
// Starts at 100 so we are sure it's not an init error
static uint16_t globalkey_seq_nmbr = 100;
static int hello_timer_id = -1;
static int keyrefresh_timer_id = -1;
static enum SecurityMode key_exchange_encryption =
    SecurityMode_EncryptAndMACPriorKey;
static uint32_t key_refresh_interval = KeyExchange_WEEKLY;
static uint8_t sequence_nmbrA;
static uint8_t sequence_nmbrB;
static uint8_t sequence_nmbrC;
static uint32_t ack_messages_sent = 0;
static uint32_t hello_messages_sent = 0;
// To prevent keyrefresh messages being eternally passed round, store ID
static uint8_t last_seen_keyrefresh_node = 0;
// To prevent keyrefresh messages being eternally passed round, store ID
static uint32_t last_seen_keyrefresh_seq = 0;
/* Specifies that unpaired neighbours seen, when hello timer ticks, it will
 * send out hello
 */
static int unpaired_neighbours_seen = 0;
// The address of last seen unpaired neighbour
static uint16_t unpaired_neighbour_addy = 0;

/*---------------------------------------------------------------------------*/
//                                      Functions
/*---------------------------------------------------------------------------*/
static int create_new_nonce(uint8_t *buffer);
static int keyexchange_hello_arrived(secure_packet_t *packet);
static int keyexchange_ack_arrived(secure_packet_t *packet);
static int hello_timer_ticked(int user_param);
static int keyexchange_keyrequest_arrived(secure_packet_t *packet);
static int keyexchange_keytransfer_arrived(secure_packet_t *packet);
static int keyrefresh_timer_ticked(int user_param);
static int packet_arrived(secure_packet_t *packet);
static int register_key_refresh_timer(void);
static int start_hello_timer(void);

/* Component public interface */
network_security_component_t key_exchange_component = {
    "KeyExchange",
    keyexchange_init,
    keyexchange_start,
    keyexchange_stop,
    packet_arrived,
    keyexchange_event
};

/**
 * @brief Gets the Key Exchange components
 *
 */
network_security_component_t *keyexchange_get_component()
{
    return &key_exchange_component;
}

/**
 * @brief Initializes the Key Exchange components
 *
 */
int keyexchange_init()
{
    // Calculate our nonce
    create_new_nonce(0);

    ulog_info("Initializing the encryption for the key...");
    int result = block_cipher_mode_init(&key_ciphermode_context,
                                      SECURE_ROUTING_KEYSIZE, initial_key);

    if (result <= 0) {
        ulog_error("Could not init encryption");
        return NetworkSecurity_Error;
    }

    char sz[21] = {'\0'};
    memcpy(sz, initial_key, 20);

    for (int i = 0; i < sizeof(global_key); i++) {
        if (sz[i] < 48 || sz[i] > 122) {
            sz[i] = '?';
        }

        // printf("%c", sz[i]);
    }

    ulog("> Key-Exchange initialized.");
    return NetworkSecurity_OK;
}

/**
 * @brief Creates a new nonce and writes it to a buffer
 *
 * @param buffer The buffer that the nonce will be written to
 *
 * @return NetworkSecurity_OK if all was okay
 */
int create_new_nonce(uint8_t *buffer)
{
    // buffer is an optional variable where the new
    // nonce will be copied into

    uint8_t i;

    for (i = 0; i < NONCE_SIZE; i++) { //taking random values for now...
        node_nonce[i] = (uint8_t)(rand() % 256);
    }

    if (buffer != 0) {
        memcpy(buffer, node_nonce, NONCE_SIZE);
    }

    ulog_info("Creating new nonce");

    return NetworkSecurity_OK;
}

/**
 * @brief Starts the key exchange
 */
int keyexchange_start()
{
    ulog("> Starting key exchange component");

    // Comment this to prevent neighbours being built
    start_hello_timer();

    // Timer that sends a periodic key refresh depending on interval set
    register_key_refresh_timer();

    return NetworkSecurity_OK;
}

/**
 * @brief Starts the hello timer
 */
int start_hello_timer()
{
    uint32_t timer_interval = HELLO_MSG_INTERVAL + (rand() % 4);

    ulog("> Registering hello timer. Interval=%lu", timer_interval);

    // Use this variable to make sure at least 1 hello message is sent out
    unpaired_neighbours_seen = 1;

    if (hello_timer_id != -1) {
        network_security_restart_timer(hello_timer_id, timer_interval);
    }
    else {
        hello_timer_id = network_security_register_timer(timer_interval,
                         0,
                         hello_timer_ticked);
    }

    return NetworkSecurity_OK;
}

/**
 * @brief Called when an event occurs. See header file for extended description
 */
int keyexchange_event(enum NetworkSecurity_Event e, uint32_t param1)
{

    if (e == NetworkSecurity_NoNeighbours) {
        ulog("No neighbours message received");
        start_hello_timer();
    }

    if (e == NetworkSecurity_NeighbourDetected) {
        ulog("> Unpaired neighbour detected. Sending hello");
        unpaired_neighbours_seen = 1;

        if (unpaired_neighbour_addy != 0) {
            // means there is already such a request pending
            unpaired_neighbour_addy = 0; // set to 0 so it is a broadcast
        }
        else {
            unpaired_neighbour_addy = param1;
        }

        start_hello_timer();
    }

    return NetworkSecurity_OK;
}

/**
 * @brief Stops the key exchange process
 */
int keyexchange_stop()
{
    network_security_stop_timer(hello_timer_id);
    hello_timer_id = -1;

    // if (i_am_basestation)
    {
        network_security_stop_timer(keyrefresh_timer_id);
        keyrefresh_timer_id = -1;
    }

    return NetworkSecurity_OK;
}

/**
 * @brief Called when a new secure packet arrives. It is decrypted already
 *                      at this point
 *
 * @param packet The encrypted packet
 *
 * @return NetworkSecurity_Result with the values
 */
int packet_arrived(secure_packet_t *packet)
{
    // Every packet that arrives is sent here after decryption.
    // The component has to decide if this packet is meant for it

    if (packet->data[0] == NetworkSecurity_HELLO) {
        return keyexchange_hello_arrived(packet);
    }
    else if (packet->data[0] == NetworkSecurity_ACK) {
        return keyexchange_ack_arrived(packet);
    }
    else if (packet->data[0] == NetworkSecurity_KeyRequest) {
        return keyexchange_keyrequest_arrived(packet);
    }
    else if (packet->data[0] == NetworkSecurity_KeyTransfer) {
        return keyexchange_keytransfer_arrived(packet);
    }


    return NetworkSecurity_OK;
}

/**
 * @brief Called when the timer ticks
 *
 * @return NetworkSecurity_Continue if the timer is to continue ticking
 */
int hello_timer_ticked(int user_param)
{

    static int tick_count = 0;
    tick_count++;

    ulog("!Hello timer ticked. Time=%d", tick_count);

    // check if I have any neighbour. If so, stop this
    if (network_nodes_get_any_neighbour(network_nodes_get_root()) == 0 ||
        unpaired_neighbours_seen == 1) {
        ulog("> No neighbours. Sending hello message");

        if (unpaired_neighbour_addy != 0) {
            // Means not set or broadcast
            keyexchange_sendhello(unpaired_neighbour_addy);
            unpaired_neighbour_addy = 0;
        }
        else {
            keyexchange_sendhello(LOCAL_BROADCAST);
        }

        // Keep the timer ticking
        ulog("> Stopped hello timer. No further ticks");
        return NetworkSecurity_Stop;
    }

    ulog("!Stopping hello timer. Neighbours found");

    // This will stop the timer from ticking again cos we
    // have a neighbour. It can be enabled with network_security_continue_timer
    // function
    return NetworkSecurity_Stop;
}

/**
 * @brief Computes a new pairwise key with a particular node
 *
 * @param nonce_a The nonce of this node
 * @param nonce_b The none of the connected node
 * @param init_vector The inialisation vector
 * @param buffer The buffer where the key will be stored
 *
 * @return NetworkSecurity_OK if the key was computed properly
 */
int compute_pairwise_key(uint8_t *nonce_a, uint8_t *nonce_b,
                         uint8_t *init_vector, uint8_t *buffer, int buffer_len)
{

    int result = block_cipher_mode_init(&key_ciphermode_context,
                                      SECURE_ROUTING_KEYSIZE, initial_key);

    if (result <= 0) {
        ulog_error("Could not init encryption");
        return NetworkSecurity_Error;
    }

    // Copy the both nonces behind each other on the plaintext variable
    uint8_t plaintext[NONCE_SIZE * 2];
    memcpy(plaintext, nonce_a, NONCE_SIZE);
    memcpy(&plaintext[8], nonce_b, NONCE_SIZE);

    uint8_t ciphertext[NONCE_SIZE * 2];
    memcpy(ciphertext, plaintext, NONCE_SIZE * 2);
    ulog_info("> Encrypting the pairwise key. Len=%d. sizeof=%d",
              NONCE_SIZE * 2, sizeof(plaintext));

    // Encrypt this variable
    int res;
    res = block_cipher_mode_encrypt(&key_ciphermode_context, plaintext,
                                  ciphertext, NONCE_SIZE * 2, init_vector);

    if (res <= 0) {
        ulog("> Could not encrypt the nonces");
        return NetworkSecurity_Error;
    }

    ulog_info("> Expanding result to fit buffer");

    // Copy the encrypted key to the target buffer. If keysize
    // is longer than encryption result, then repeat
    for (int i = 0; i < buffer_len; i++) {
        buffer[i] = plaintext[i % NONCE_SIZE * 2];
    }

    return NetworkSecurity_OK;
}

/**
 * @brief Sends a hello message out via broadcast. See header for further info
 *
 */
int keyexchange_sendhello(uint16_t destination)
{
    ulog("> Starting keyexchange hello process");

    unpaired_neighbours_seen = 0;

    hello_messages_sent++;
    ulog_info("> EXPERIMENT(KeyExchangeCount, HelloCount=%lu)",
              hello_messages_sent);

    hello_msg_t msg;
    msg.type = NetworkSecurity_HELLO;
    msg.is_basestation = i_am_basestation;

    memcpy(msg.nonce, node_nonce, NONCE_SIZE);

    ulog("> Sending hello packet. Msg.type=%d. Msg.base=%d. ", msg.type,
         msg.is_basestation);

    network_security_send_packet((uint8_t *)&msg, sizeof(hello_msg_t),
                                 destination,
                                 1,
                                 SecurityMode_MACWithInitialKey,
                                 PeekHint_DontPeek);

    // Store the sequence number for later use to verify ack
    uint32_t sequence_nmbr = replay_protection_get_current_sequence_number();
    sequence_nmbrA = sequence_nmbr & 0xff;
    sequence_nmbrB = (sequence_nmbr >> 8) & 0xff;
    sequence_nmbrC = (sequence_nmbr >> 16) & 0xff;

    return NetworkSecurity_OK;
}

/**
 * @brief Sends an ack message out. See header for further info
 *
 */
int keyexchange_sendack(uint8_t destination, uint32_t sender_sequence_nmbr)
{
    ulog("> Sending ACK");

    ack_messages_sent++;
    ulog_info("> EXPERIMENT(KeyExchangeCount, AckCount=%lu)",
              ack_messages_sent);
    ack_msg_t msg;
    msg.type = NetworkSecurity_ACK;
    msg.sender_sequence_nmbr = sender_sequence_nmbr;
    msg.sender_key_nmbr = keyexchange_get_key_seq_nmbr();
    memcpy(msg.nonce, node_nonce, NONCE_SIZE);

    network_security_send_packet((uint8_t *)&msg,
                                 sizeof(ack_msg_t),
                                 destination,
                                 1,
                                 SecurityMode_MACWithInitialKey,
                                 PeekHint_DontPeek);
    return NetworkSecurity_OK;
}

/**
 * @brief Sends a key-request out. See header for further info
 *
 */
int keyexchange_send_keyrequest(uint8_t destination)
{
    if (i_am_basestation) {
        ulog("> Not sending key-request because this is basestation");
        return NetworkSecurity_OK;
    }

    key_request_t msg;
    msg.type = NetworkSecurity_KeyRequest;
    msg.gkey_seq_nmbr = globalkey_seq_nmbr;

    ulog("> Sending key request to node: %d. My Key Seq=%d", destination,
         globalkey_seq_nmbr);

    return network_security_send_packet((uint8_t *)&msg, sizeof(key_request_t),
                                        destination, 10,
                                        SecurityMode_EncryptAndMACPairwise,
                                        PeekHint_DontPeek);
}

/**
 * @brief Called when a key-request packet arrives at this node
 *
 * @param packet The secure packet to be sent
 *
 * @return The result of the keytransfer as NetworkSecurity_Result
 */
int keyexchange_keyrequest_arrived(secure_packet_t *packet)
{
    ulog("> Key request arrived. Source=%d", (uint8_t)packet->source);
    return keyexchange_send_keytransfer(packet->source,
                                        SecurityMode_EncryptAndMACPairwise);
}

/**
 * @brief Starts a global key update. Only allowed from base-station. See
 *        header file for more details
 *
 */
int keyexchange_send_keyrefresh()
{
    if (!i_am_basestation) {
        ulog("Key refresh only allowed from base-station");
        return NetworkSecurity_NotBaseStation;
    }

    return keyexchange_send_keytransfer(0, key_exchange_encryption);
}

/**
 * @brief Sends a key transfer to a specific destination. See header file
 *        for more details
 *
 */
int keyexchange_send_keytransfer(uint16_t destination,
                                 enum SecurityMode sec_mode)
{
    char sz[21] = {'\0'};
    memcpy(sz, global_key, 20);

    for (int i = 0; i < sizeof(global_key); i++) {
        if (sz[i] < 48 || sz[i] > 122) {
            sz[i] = '?';
        }

        // printf("Out: %i", sz[i]);
    }

    ulog("> Sending key transfer to %d. ", (uint8_t)destination);

    key_transfer_t msg;
    msg.type = NetworkSecurity_KeyTransfer;
    msg.gkey_seq_nmbr = globalkey_seq_nmbr;

    memcpy(msg.gkey, global_key, SECURE_ROUTING_KEYSIZE);

    if (network_security_send_packet((uint8_t *)&msg, sizeof(key_transfer_t),
                                     destination, DEFAULT_NET_TTL, sec_mode,
                                     PeekHint_Peek) < NetworkSecurity_OK) {
        ulog_error("Key Transfer send failed");
    }

    return NetworkSecurity_PacketHandled;
}

/**
 * @brief Called when a keytransfer arrives
 *
 * @param packet The secure packet which contained it
 *
 * @return The result of the keytransfer as NetworkSecurity_Result
 */
int keyexchange_keytransfer_arrived(secure_packet_t *packet)
{
    ulog("> Key Transfer message arrived. Source=%d. Dest=%d",
         (uint8_t)packet->source, packet->destination);
    key_transfer_t *msg = (key_transfer_t *)packet->data;

    // Basestation does not accept key transfers from nodes
    if (i_am_basestation) {
        ulog_error("Not accepting key-transfer because I AM BASESTATION");
        return NetworkSecurity_OK;
    }

    if (last_seen_keyrefresh_node == packet->source &&
        last_seen_keyrefresh_seq == packet->seq_nmbr) {
        // The changing of the global key resets the sequence number
        // counters of passed packets. As such a global key change needs
        // a secondary check to make sure that packets do not bounced around
        // in the network
        ulog("Have seen this keytransfer before. Do not re-broadcast");
        return NetworkSecurity_NoRebroadcast;
    }

    // Check if its a newer key
    if (msg->gkey_seq_nmbr < globalkey_seq_nmbr) {
        ulog_error("Older key sequence arrived");
        return NetworkSecurity_WrongVersion;
    }

    last_seen_keyrefresh_node = packet->source;
    last_seen_keyrefresh_seq = packet->seq_nmbr;

    // Set new global key
    ulog_info("Intended destination for key transfer was this node: %d",
              (uint8_t)packet->destination);
    ulog("> Received new global key. Broadcast: %d:%lu",
         last_seen_keyrefresh_node, last_seen_keyrefresh_seq);
    keyexchange_set_new_key(msg->gkey, SECURE_ROUTING_KEYSIZE,
    		                msg->gkey_seq_nmbr);

    if (packet->destination == GLOBAL_BROADCAST ||
        GET_SECURITY_MODE(packet->flags) ==
        SecurityMode_EncryptAndMACPairwiseBroadcast) {
        // We only reset our own sequence number if this was a global key
        // refresh. If it was a unicast key-transfer to us, we should not,
        // otherwise nodes that know this node will kick replay errors
        replay_protection_reset_sequence_number();
    }

    return NetworkSecurity_PacketHandled;
}


/**
 * @brief Called when a hello packet arrives
 *
 * @param packet The secure packet which contained it
 *
 * @return The result of the operation as NetworkSecurity_Result
 */
int keyexchange_hello_arrived(secure_packet_t *packet)
{
    ulog_info("Hello Packet Arrived");

    // Switch off this flag so a hello message does not trigger
    // another hello message, and to prevent re-ticks during
    // initial pairing process
    // unpaired_neighbours_seen = 0;

    hello_msg_t *msg = (hello_msg_t *)packet->data;

    uint8_t sequence_nmbrA = (uint8_t)packet->seq_nmbr;
    uint8_t sequence_nmbrB = (uint8_t)(packet->seq_nmbr >> 8);
    uint8_t sequence_nmbrC = (uint8_t)(packet->seq_nmbr >> 16);

    uint8_t init_vector[8];
    init_vector[0] = get_this_node_address();
    init_vector[1] = packet->source;
    init_vector[2] = NetworkSecurity_ACK;
    init_vector[3] = sizeof(hello_msg_t);
    init_vector[4] = sequence_nmbrA;
    init_vector[5] = sequence_nmbrB;
    init_vector[6] = sequence_nmbrC;
    init_vector[7] = 0;

    ulog("> Hello Packet arrived. Src=%d Dst=%d. Type=%d BaseStation=%d ",
         packet->source, packet->destination, msg->type, msg->is_basestation);

    // Compute pairwise key between this node and the node sending the hello
    // message
    uint8_t pairwise_key[SECURE_ROUTING_KEYSIZE];
    compute_pairwise_key(node_nonce, msg->nonce, init_vector, pairwise_key,
                         SECURE_ROUTING_KEYSIZE);

    int was_neighbour_already = 0;
    network_node_t *node = network_nodes_find(packet->source);

    if (node != 0 && node->is_neighbour == 1) {
        was_neighbour_already = 1;
    }

    // Add pairwise key/node
    ulog_info("Registering this node in our nodes table");
    network_nodes_set_neighbour(packet->source, pairwise_key);

    if (ACK_WAIT_SLOTS > 0) {
        // Add small jitter, then send an ack as reply
        // cc1100_get_avg_transmission_duration is in milliseconds.
        // We pick a random number between 0 and 10 transmission-durations
        /*
        uint32_t jitter = cc1100_get_avg_transmission_duration() * (rand() % ACK_WAIT_SLOTS);
        ulog("> Ack jitter was %lums", jitter);

        timex_t t = timex_set(0, jitter * 1000);
        vtimer_sleep(t);
        */
    }
    else {
        ulog("> No ack jitter");
    }

    uint32_t seq = packet->seq_nmbr;
    keyexchange_sendack(packet->source, seq);

    if (was_neighbour_already == 0) {
        // This implies we did not have this node as a neighbour, so
        // we send a hello out some time later
        // ulog("> Node was not a neighbour already. Sending confirmation
        //      hello");
        // network_security_raise_event(NetworkSecurity_NeighbourDetected, 0);
    }

    ulog("> Hello complete");
    return NetworkSecurity_PacketHandled;
}

/**
 * Prints out information about the current keys
 */
void keyexchange_print_key_info()
{
    char sz[21] = {'\0'};
    memcpy(sz, global_key, 20);

    for (int i = 0; i < SECURE_ROUTING_KEYSIZE; i++) {
        if (sz[i] < 48 || sz[i] > 122) {
            sz[i] = '?';
        }

        // printf("Out: %i", sz[i]);
    }

    // ntf("Global Key ASCII: %s\n\n", sz);

    printf("Current global key (HEX):");

    for (int i = 0; i < SECURE_ROUTING_KEYSIZE; i++) {
        printf("%x", global_key[i]);
    }

    printf("\n");

    printf("Current initial key:");

    for (int i = 0; i < SECURE_ROUTING_KEYSIZE; i++) {
        printf("%x", initial_key[i]);
    }

    printf("\n");

    if (keyexchange_get_encryption_mode() == SecurityMode_None) {
        printf("Key Transfer Mode=No Encryption\n");
    }

    if (keyexchange_get_encryption_mode() == SecurityMode_MAC) {
        printf("Key Transfer Mode=Only Authentication\n");
    }

    if (keyexchange_get_encryption_mode() == SecurityMode_MACWithInitialKey) {
        printf("Key Transfer Mode=Authenticated with Initial Key\n");
    }

    if (keyexchange_get_encryption_mode() == SecurityMode_Encrypt) {
        printf("Key Transfer Mode=Encrypted only\n");
    }

    if (keyexchange_get_encryption_mode() == SecurityMode_EncryptAndMAC) {
        printf("Key Transfer Mode=Encrypted and Authenticated\n");
    }

    if (keyexchange_get_encryption_mode() ==
        SecurityMode_EncryptAndMACPriorKey) {
        printf("Key Transfer Mode=Encrypted and Auth with Previous key\n");
    }

    if (keyexchange_get_encryption_mode() ==
        SecurityMode_EncryptAndMACPairwise) {
        printf("Key Transfer Mode=Encrypted and Authed with Pairwise Key\n");
    }

    if (keyexchange_get_encryption_mode() ==
        SecurityMode_EncryptAndMACPairwiseBroadcast) {
        printf("Key Transfer Mode=PairwiseBroadcast\n");
    }

    printf("My Key Seq=%d\n", globalkey_seq_nmbr);

    if (key_refresh_interval == KeyExchange_DEBUG) {
        printf("Key Refresh is DEBUG\n");
    }

    if (key_refresh_interval == KeyExchange_HOURLY) {
        printf("Key Refresh is HOURLY\n");
    }

    if (key_refresh_interval == KeyExchange_HALFDAY) {
        printf("Key Refresh is HALFDAY\n");
    }

    if (key_refresh_interval == KeyExchange_DAILY) {
        printf("Key Refresh is DAILY\n");
    }

    if (key_refresh_interval == KeyExchange_WEEKLY) {
        printf("Key Refresh is WEEKLY\n");
    }

    if (key_refresh_interval == KeyExchange_MONTHLY) {
        printf("Key Refresh is MONTHLY\n");
    }

    if (key_refresh_interval == KeyExchange_QUARTERLY) {
        printf("Key Refresh is QUARTERLY\n");
    }

    if (hello_timer_id > -1) {
        printf("Hello timer currently enabled. Interval=%ds\n",
               HELLO_MSG_INTERVAL);
    }
    else {
        printf("Hello timer currently turned off\n");
    }

    if (keyrefresh_timer_id > -1) {
        printf("Key Refresh timer currently enabled\n");
    }
    else {
        printf("Key Refresh timer currently turned off\n");
    }
}

/**
 * @brief Called when an ack packet arrives
 *
 * @param packet The secure packet which contained it
 *
 * @return The result of the operation as NetworkSecurity_Result
 */
int keyexchange_ack_arrived(secure_packet_t *packet)
{
    ulog("> Ack message arrived. From=%d. To=%d.", packet->source,
         packet->destination);

    ack_msg_t *msg = (ack_msg_t *)packet->data;

    // Switch off this flag so a hello message does not trigger
    // another hello message, and to prevent re-ticks during
    // initial pairing process
    // unpaired_neighbours_seen = 0;

    uint8_t packet_sequence_nmbrA = msg->sender_sequence_nmbr & 0xff;
    uint8_t packet_sequence_nmbrB = (msg->sender_sequence_nmbr >> 8) & 0xff;
    uint8_t packet_sequence_nmbrC = (msg->sender_sequence_nmbr >> 16) & 0xff;

    if (sequence_nmbrA != packet_sequence_nmbrA ||
        sequence_nmbrB != packet_sequence_nmbrB ||
        sequence_nmbrC != packet_sequence_nmbrC) {
        ulog_error("Sequence number does not match. Received=%d Store=%d",
                   packet_sequence_nmbrA, sequence_nmbrA);
        return NetworkSecurity_Error;
    }

    uint8_t init_vector[8];
    init_vector[0] = packet->source;
    init_vector[1] = get_this_node_address();
    init_vector[2] = NetworkSecurity_ACK;
    init_vector[3] = sizeof(hello_msg_t);
    init_vector[4] = sequence_nmbrA;
    init_vector[5] = sequence_nmbrB;
    init_vector[6] = sequence_nmbrC;
    init_vector[7] = 0;

    // Compute pairwise key between this node and the node sending the ack
    // message, Nonce order is swaped compared to hello msg
    uint8_t pairwise_key[SECURE_ROUTING_KEYSIZE];
    compute_pairwise_key(msg->nonce, node_nonce, init_vector, pairwise_key,
                         SECURE_ROUTING_KEYSIZE);

    // Add pairwise key/node
    network_nodes_set_neighbour(packet->source, pairwise_key);
    ulog("> Ack arrived. Neighbours found");

    if (i_am_basestation) {
        // Send the global key to him
        if (msg->sender_key_nmbr < keyexchange_get_key_seq_nmbr()) {
            keyexchange_send_keytransfer(packet->source,
                                         SecurityMode_EncryptAndMACPairwise);
        }
    }
    else {
        // Request or transfer global key if my key is different from his
        if (msg->sender_key_nmbr < keyexchange_get_key_seq_nmbr()) {
            keyexchange_send_keytransfer(packet->source,
                                         SecurityMode_EncryptAndMACPairwise);
        }
        else if (msg->sender_key_nmbr > keyexchange_get_key_seq_nmbr()) {
            keyexchange_send_keyrequest(packet->source);
        }
    }

    return NetworkSecurity_PacketHandled;;
}

/**
 * @brief Gets the sequence number
 *
 */
uint16_t keyexchange_get_key_seq_nmbr()
{
    return globalkey_seq_nmbr;
}

/**
 * @brief Sets the global key sequence number
 *
 */
void keyexchange_set_key_sequence_number(uint16_t seq_nmbr)
{
    ulog("> Setting key seq number to %lu", seq_nmbr);
    globalkey_seq_nmbr = seq_nmbr;
}

/**
 * @brief Sets encryption mode
 *
 */
void keyexchange_set_encryption_mode(enum SecurityMode m)
{
    if (m == SecurityMode_EncryptAndMACPairwise) {
        ulog("> Changing key exchange encryption mode to \
                 SecurityMode_EncryptAndMACPairwise");
    }
    else if (m == SecurityMode_EncryptAndMACPriorKey) {
        ulog("> Changing key exchange encryption mode to \
                 SecurityMode_EncryptAndMACPriorKey");
    }
    else if (m == SecurityMode_EncryptAndMAC) {
        m = SecurityMode_EncryptAndMACPriorKey;
        ulog("> Forcing mode SecurityMode_EncryptAndMACPriorKey. \
                 Attempt to change key exchange encryption mode to \
                 SecurityMode_EncryptAndMAC");
    }
    else {
        ulog("> Changing key exchange encryption mode to %d", m);
    }


    key_exchange_encryption = m;
}

/**
 * @brief Gets encryption mode
 *
 */
enum SecurityMode keyexchange_get_encryption_mode()
{
    return key_exchange_encryption;
}

/**
 * @brief Gets key refresh interval
 *
 */
uint32_t keyexchange_get_key_refresh_interval()
{
    return key_refresh_interval;
}

/**
 * @brief Sets key refresh interval
 *
 */
void keyexchange_set_key_refresh_interval(uint32_t new_interval)
{
    if (new_interval < 10) {
        ulog("Setting key refresh interval to preset val %lu", new_interval);

        if (new_interval == 1) {
            new_interval = KeyExchange_HOURLY;
        }
        else if (new_interval == 2) {
            new_interval = KeyExchange_HALFDAY;
        }
        else if (new_interval == 3) {
            new_interval = KeyExchange_DAILY;
        }
        else if (new_interval == 4) {
            new_interval = KeyExchange_WEEKLY;
        }
        else if (new_interval == 5) {
            new_interval = KeyExchange_MONTHLY;
        }
        else if (new_interval == 6) {
            new_interval = KeyExchange_QUARTERLY;
        }

        ulog("Key Refresh Interval = %lus", new_interval);
    }
    else {
        ulog("Setting key refresh to %lu", new_interval);
    }

    key_refresh_interval = new_interval;

    register_key_refresh_timer();
}

/**
 * @brief Register the key-refresh timer to tick
 *
 */
int register_key_refresh_timer()
{
    if (!i_am_basestation) {
        return NetworkSecurity_NotBaseStation;
    }

    ulog("> Registering key-refresh timer");

    if (keyrefresh_timer_id != -1) {
        network_security_restart_timer(keyrefresh_timer_id,
                                       key_refresh_interval);
    }
    else {
        keyrefresh_timer_id = network_security_register_timer(
                                  key_refresh_interval,
                                  0,
                                  keyrefresh_timer_ticked);
    }

    return NetworkSecurity_OK;
}

/**
 * @brief Called when the key refresh timer is ticked
 *
 */
int keyrefresh_timer_ticked(int user_param)
{
    ulog("!Key-Refresh timer ticked");

    if (i_am_basestation) {
        keyexchange_generate_new_key();
        keyexchange_send_keyrefresh();
        replay_protection_reset_sequence_number();
    }

    return NetworkSecurity_Continue;
}

/**
 * @brief Generates new global key
 *
 */
void keyexchange_generate_new_key()
{
    uint8_t g[SECURE_ROUTING_KEYSIZE];

    // fill the char-array with random values representing the new key
    for (int i = 0; i < SECURE_ROUTING_KEYSIZE; i++) {
        g[i] = (uint8_t)(rand() % 256);
    }

    // globalkey_seq_nmbr++;
    ulog("> Generating new key automatically. New key seq=%lu",
         globalkey_seq_nmbr + 1);
    keyexchange_set_new_key(g, SECURE_ROUTING_KEYSIZE, globalkey_seq_nmbr + 1);
}

/**
 * @brief Sets a new global key
 *
 */
void keyexchange_set_new_key(uint8_t *new_global_key, int buffer_len,
                             uint16_t key_seq_nmbr)
{
    // Copy the last global key into backup (for global key updates)
    memcpy(last_global_key, global_key, SECURE_ROUTING_KEYSIZE);

    // Init new key to 0
    memset(global_key, 0, SECURE_ROUTING_KEYSIZE);

    // Copy the new key to the buffer. If the key is not long enough the
    // rest will be = 0
    for (int i = 0; i < buffer_len; i++) {
        global_key[i] = new_global_key[i];
    }

    // Let network nodes decide what to do when global key updated.
    // Will reset mac errors, sequence numbers
    network_nodes_set_globalkey(network_nodes_get_root(), global_key);

    // Print out a warning if sequence number not changes
    if (key_seq_nmbr <= globalkey_seq_nmbr) {
        ulog_error("Key changed but sequence number not changed");
    }

    // print new sequence number
    ulog("> Set New GlobalKey:SeqNmbr=%lu. OldSeqNmbr=%lu", key_seq_nmbr,
         globalkey_seq_nmbr);

    // Set new sequence number
    globalkey_seq_nmbr = key_seq_nmbr;
}
