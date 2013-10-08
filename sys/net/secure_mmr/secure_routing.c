/*
 * secure_routing.c - Controller for the secure routing functionality.
 *
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * This source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 */


/*
 * @file        secure_routing.c
 * @brief       Main controller for Secure Routing functionality.
 *              The secure routing class functions as the main controller
 *              class for the entire secure routing layer. It receives send
 *              requests from applications on the higher classes, secures the
 *              packets and passes them on to the radio interface.
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Mark Essien <markessien@gmail.com>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * @date        15.09.2013 17:41:11
 */

/*---------------------------------------------------------------------------*/
//                                      Included Files
/*---------------------------------------------------------------------------*/
/*  Radio includes */
#include "drivers/cc110x/cc1100.h"
#include "radio/radio.h"
#include "cc1100_phy.h"

// Board & Sensor includes
#include <board.h>
#include "hwtimer.h"
#include <sht11.h>
#include "gpioint.h"

// Shell includes
#include <posix_io.h>
#include <shell.h>
#include <board_uart0.h>

/* Real-time clock */
#include <rtc.h>
#include <lpc2387-rtc.h>

/* Timer */
#include "hwtimer.h"
#include "timex.h"
#include "vtimer.h"

/* Sys includes */
#include "msg.h"
#include "kernel.h"
#include "thread.h"
#include "protocol-multiplex.h"
#include <mutex.h>

/* real time clock */
#include <rtc.h>
#include <lpc2387-rtc.h>

/* Project Include */
#include "secure_routing.h"
#include "microeval/microeval.h"
#include "network_nodes.h"

/* Packet Security */
#include "packet_security.h"
#include "packet_encryption.h"
#include "packet_mac.h"

/* Network Security */
#include "network_security.h"
#include "keyexchange.h"
#include "security_update.h"
#include "keepalive.h"
#include "micromesh_routing.h"
#include "replay_protection.h"

/*---------------------------------------------------------------------------*/
//                                      Structures
/*---------------------------------------------------------------------------*/

/**
 * @brief It's like a secure_packet, but includes the information provided by
 *        the radio layer
 */
typedef struct radio_process_packet {
    int valid;
    int to_be_sent;
    int writing; ///< Writing to the structure
    radio_address_t gateway;
    uint8_t protocol;
    uint8_t priority;
    uint16_t packet_len;
    int is_secure_packet;
    int send_attempts;
    int send_rerr_if_failed;
    packet_info_t packet_info;
    uint8_t packet[MAX_DATA_LENGTH];
} radio_process_packet_t;

/*---------------------------------------------------------------------------*/
//                                      Local Variables
/*---------------------------------------------------------------------------*/
// The table for storing the protocol handlers
static pm_table_t protocol_handler_table;
// The actual function pointers for the protocol handlers
static handler_entry_t protocol_handlers[MAX_SECURE_PROTOCOL_HANDLERS];
// When set, this node will stop processing packets
static uint8_t ignore_all = 0;
// Address of the basestation
uint8_t basestation_addr = 1;
// Indicates if this node is basestation
uint8_t i_am_basestation = 0;
// Variable to indicate which nodes should be ignored
static uint8_t ignore_except = 0;
// Global Encryption Key
uint8_t global_key[SECURE_ROUTING_KEYSIZE];
// Hardcoded Initial Key that never changes
uint8_t initial_key[SECURE_ROUTING_KEYSIZE];
// The prior global key used just before this one
uint8_t last_global_key[SECURE_ROUTING_KEYSIZE];
// The current security level pre-set
uint8_t security_preset = SecurityPresetOptimal;
// Number of messages sent by this node
uint32_t messages_sent;
// Time when a ping was sent out
uint32_t ping_start_time;
// Indicates if the layer is initialized and ready
static int initialized = 0;
// The encryption/MAC level of packets being set
static enum SecurityMode security_level = SecurityMode_EncryptAndMAC;

/* Radio Receive/Send Thread */
// The PID of the radio reception thread
static int radio_process_pid = -1;
// Buffer for radio reception thread
static char radio_send_stack_buffer[4500];
// Queue of messages intended for timer
static msg_t radio_msg_queue[16];
// Queue for messages to be sent and received
static radio_process_packet_t radio_process_queue[RADIO_PROCESS_QUEUE_SIZE];
// Prevents multiple threads from calling send
static mutex_t send_mutex;
// Stores last received packet (for debug purposes)
static secure_packet_t last_packet;

/* Externals. Can be refactored to use access functions */
extern int enable_encryption;
extern int enable_mac;
extern int enable_net_sec;

/* Watchdog */
// The PID of the watchdog thread
static int watchdog_process_pid = -1;
// Buffer for the watchdog thread
static char watchdog_stack_buffer[4500];

/*---------------------------------------------------------------------------*/
//                                      Function Declarations
/*---------------------------------------------------------------------------*/
static packet_handler_t packet_handler(void *datav, int data_length,
                                       packet_info_t *info);
static int send_packet(secure_packet_t *packet);
static void radio_process_thread(void);
static void watchdog_thread(void);
static int pump_radio_process_queue(void);

/**
 * @brief       Initialises the secure-routing layer
 */
int secure_routing_init()
{
    ulog("Start secure routing layer");

    memset(global_key, 0, SECURE_ROUTING_KEYSIZE);
    memcpy(global_key, "ENCKeyNum1ENCKeyNum2", strlen("ENCKeyNum1ENCKeyNum2"));
    memcpy(last_global_key, global_key, SECURE_ROUTING_KEYSIZE);
    memcpy(initial_key, global_key, SECURE_ROUTING_KEYSIZE);
    memset(radio_process_queue,
           0,
           sizeof(radio_process_packet_t) * RADIO_PROCESS_QUEUE_SIZE);

    mutex_init(&send_mutex);
    //  mutex_init(&p_mutex);

    messages_sent = 0;
    initialized = 1;
    ignore_except = 0;
    ignore_all = 0;

    if (get_this_node_address() == basestation_addr) {
        ulog("!This is the BASE-STATION");
        i_am_basestation = 1;
    }

    if (radio_process_pid < 0) {
        // PRIORITY_MAIN-2, CREATE_STACKTEST
        radio_process_pid = thread_create(radio_send_stack_buffer,
                                          4500, PRIORITY_MAIN + 2,
                                          CREATE_STACKTEST,
                                          radio_process_thread,
                                          "radio-process-thread");
        ulog("> Radio process thread id=%d", radio_process_pid);
    }
    else {
        ulog("> Radio thread already exists. Pid=%d", radio_process_pid);
    }

    if (watchdog_process_pid < 0) {
        // PRIORITY_MAIN-2, CREATE_STACKTEST
        watchdog_process_pid = thread_create(watchdog_stack_buffer,
                                             4500, PRIORITY_MAIN + 2,
                                             CREATE_STACKTEST,
                                             watchdog_thread,
                                             "watchdog-thread");
        ulog("> Watchdog thread id=%d", watchdog_process_pid);
    }
    else {
        ulog("> Watchdog thread already exists. Pid=%d", watchdog_process_pid);
    }

    network_nodes_init();

    micromesh_init();

    // Register our secondary protocol handler table
    pm_init_table(&protocol_handler_table,
                  MAX_SECURE_PROTOCOL_HANDLERS,
                  protocol_handlers);

    // Add the components that will be used for packet security
    // The insertion order is important
    ulog("> Added replay component");
    packet_security_add_component(replay_protection_get_component());

    ulog("> Added encryption component");
    packet_security_add_component(packet_encryption_get_component());

    ulog("> Added mac component");
    packet_security_add_component(packet_mac_get_component());

    // Setup the packet security, that will encrypt and mac the packets
    ulog("Initializing packet security...");
    int res = packet_security_init();

    if (res == PacketSecurity_OK) {
        ulog("Packet security successfully initialized");
    }


    /* Add the network components. Uncomment here to disable particular
     * functionality
     */
    network_security_add_component(keyexchange_get_component());
    network_security_add_component(keep_alive_get_component());
    network_security_add_component(security_update_get_component());

    // Initialize the components needed for network security
    ulog("Initializing network security...");

    if (network_security_init() == NetworkSecurity_OK) {
        ulog("Network security successfully initialized");
    }

    network_security_set_send_function(send_packet);

    // Set the security pre-set needed. Will start network security also.
    // This will put on the timers that will regularly tick to do
    // network security tasks
    set_security_level_preset(SecurityPresetOptimal);

    return SecureRouting_OK;
}

/**
 * @brief       Prints out information about this node
 */
void secure_routing_print_info(void)
{
    // uses printf here so it works in eval mode too
    printf("\nAm I basestation? %d\n", i_am_basestation);
    printf("Basestation address=%d\n", basestation_addr);
    printf("My address=%d\n", get_this_node_address());

    printf("Node Security Information:\n");

    if (security_preset == SecurityPresetNone) {
        printf("     --- Security Preset selected is DISABLED\n");
    }

    if (security_preset == SecurityPresetLow) {
        printf("     --- Security Preset selected is LOW\n");
    }

    if (security_preset == SecurityPresetOptimal) {
        printf("     --- Security Preset selected is Optimal\n");
    }

    if (security_preset == SecurityPresetExtreme) {
        printf("     --- Security Preset selected is Extreme\n");
    }

    if (security_preset == SecurityPresetUserDef) {
        printf("     --- Security Preset selected is User-Defined\n");
    }

    if (security_level == SecurityMode_None) {
        printf("     --- Security Level for packets is NONE\n");
    }

    if (security_level == SecurityMode_MAC) {
        printf("     --- Security Level for packets is AUTH ONLY\n");
    }

    if (security_level == SecurityMode_MACWithInitialKey) {
        printf("     --- Security Level for packets is AUTH with \
                             Initial Key\n");
    }

    if (security_level == SecurityMode_Encrypt) {
        printf("     --- Security Level for packets is ENCRYPT ONLY\n");
    }

    if (security_level == SecurityMode_EncryptAndMAC) {
        printf("     --- Security Level for packets is AUTH and ENCRYPT\n");
    }

    if (security_level == SecurityMode_EncryptAndMACPriorKey) {
        printf("     --- Security Level for packets is AUTH+ENCRYPT prior \
                             Key\n");
    }

    if (security_level == SecurityMode_EncryptAndMACPairwise) {
        printf("     --- Security Level for packets is AUTH+ENCRYPT \
                             Pairwise\n");
    }

    if (security_level == SecurityMode_EncryptAndMACPairwiseBroadcast) {
        printf("     --- Security Level for packets is AUTH+ENC Pairwise \
                             Broadcast\n");
    }

    if (enable_net_sec) {
        printf("     --- Network security is ENABLED\n");
    }

    if (enable_mac) {
        printf("     --- Packet Authentication component is ENABLED\n");
    }

    if (enable_encryption) {
        printf("     --- Packet Encryption component is ENABLED\n");
    }

    security_update_print_info();
    keyexchange_print_key_info();
    replay_protection_print_info();
    keepalive_print_info();

    // Print radio queue
    printf("Printing current radio queue (top 5 items)\n");

    for (int i = 0; i < 5; i++) {
        radio_process_packet_t *r = &radio_process_queue[i];

        if (r->valid) {
            printf("> %d. (HAS PACKET) Msg to: %d\n", i, r->gateway);
        }
        else {
            printf("> %d. Empty\n", i);
        }
    }

    printf("Last Packet Received: Sender:%d. Dest=%d\n",
           last_packet.source,
           last_packet.destination);
    printf("Done\n");
}

/**
 * @brief       Set the security level
 *
 * @param       sec_level New security level. One of the SecurityPreset enums
 */
void set_security_level_preset(uint8_t sec_level)
{
    ulog("Setting security level preset to %d", sec_level);
    security_preset = sec_level;

    if (sec_level == SecurityPresetNone) {
        ulog("> Security level is set to NONE");
        packet_mac_enable(0);
        packet_encryption_enable(0);

        security_level = SecurityMode_None;

        enable_network_security(0);
    }
    else if (sec_level == SecurityPresetLow) {

        ulog("> Security level is set to Low");
        packet_mac_enable(1);
        packet_encryption_enable(0);

        keyexchange_set_key_refresh_interval(KeyExchange_MONTHLY);
        keyexchange_set_encryption_mode(SecurityMode_EncryptAndMAC);
        security_update_set_encrypt_security_status(0);
        security_level = SecurityMode_MAC;

        // Calls start
        enable_network_security(1);
    }
    else if (sec_level == SecurityPresetOptimal) {
        ulog("> Security level is set to optimal");

        packet_mac_enable(1);
        packet_encryption_enable(1);

        keyexchange_set_key_refresh_interval(KeyExchange_HOURLY);
        keyexchange_set_encryption_mode(SecurityMode_EncryptAndMAC);
        security_update_set_encrypt_security_status(0);
        security_level = SecurityMode_EncryptAndMAC;

        // Calls start
        enable_network_security(1);
    }
    else if (sec_level == SecurityPresetExtreme) {

        ulog("> Security level is set to extreme");

        packet_mac_enable(1);
        packet_encryption_enable(1);

        ulog("> 5x encryption has to be enabled via define and recompile");

        keyexchange_set_key_refresh_interval(KeyExchange_HOURLY);
        keyexchange_set_encryption_mode(SecurityMode_EncryptAndMACPairwise);
        security_update_set_encrypt_security_status(1);
        security_level = SecurityMode_EncryptAndMAC;

        // Calls start
        enable_network_security(1);
    }
    else if (sec_level == SecurityPresetUserDef) {

        // User can change pre-defined settings here.
        // they will not be automatically adapted in attack
        // detection in that case
        ulog("> Security level is set to user-def");

        packet_mac_enable(USER_DEF_SECURITY_MAC_ENABLE);
        packet_encryption_enable(USER_DEF_SECURITY_ENC_ENABLE);

        keyexchange_set_key_refresh_interval(USER_DEF_SECURITY_KEY_INTERVAL);
        keyexchange_set_encryption_mode(USER_DEF_SECURITY_ENC_MODE);
        security_update_set_encrypt_security_status(USER_DEF_SECURITY_ENC_MODE
                                                            );
        security_level = USER_DEF_SECURITY_SEC_LEVEL;

        // Calls start
        enable_network_security(1);

    }
}

/**
 * @brief       Executes a textual command passed to it. See extended description
 *          in header file
 *
 * @param       cmd The command to be executed
 *
 * @return      Error code indicating the result of the command. Type dependent on
 *          the command executed
 */
int secure_routing_exec(char *cmd)
{
    char *second = 0;
    char *first = strchr(cmd, ' ');

    if (first != 0) {
        first++;
        second = strchr(first, ' ');

        if (second != 0) {
            second[0] = '\0';
            second++;
        }
    }

    if (STARTS_WITH(cmd, "send")) {
        if (STARTS_WITH(first, "sec_status")) {

            send_security_status(atoi(second));
        }
        else if (STARTS_WITH(first, "keytransferpw")) {
            keyexchange_send_keytransfer(atoi(second),
                                         SecurityMode_EncryptAndMACPairwise);
        }
        else if (STARTS_WITH(first, "keytransfer")) {
            keyexchange_send_keytransfer(atoi(second),
                                         SecurityMode_EncryptAndMAC);
        }
        else if (STARTS_WITH(first, "keyrequest")) {
            keyexchange_send_keyrequest(atoi(second));
        }
        else {

            if (strlen(second) == 4 && strcmp(second, "PING") == 0) {
                ping_start_time = hwtimer_now();
            }

            if (second == 0 || strlen(second) < 1) {
                ulog_error("Data too short. Send (addr) (data)");
            }
            else {
                ulog("Sending data to node %d", atoi(first));
                return secure_send(atoi(first), 3, 0, DEFAULT_NET_TTL, second,
                                   strlen(second));
            }
        }
    }
    else if (STARTS_WITH(cmd, "ping")) {
        if (second && strcmp(second, "pairwise") == 0) {
            security_level = SecurityMode_EncryptAndMACPairwise;
        }

        if (atoi(first) == 10) {
            ulog_error("> Crosscheck ping syntax. Do not add net, only host");
        }

        ping_start_time = hwtimer_now();
        ulog("> Sending ping. Start Timestamp=%lu", ping_start_time);
        return secure_send(atoi(first), 3, 0, DEFAULT_NET_TTL, "PING",
                           strlen("PING"));
    }
    else if (STARTS_WITH(cmd, "broadcast")) {
        if (STARTS_WITH(first, "keyrefreshpw")) {
            keyexchange_set_encryption_mode(
                SecurityMode_EncryptAndMACPairwiseBroadcast);
            keyexchange_send_keyrefresh();
        }
        else if (STARTS_WITH(first, "keyrefresh")) {
            keyexchange_set_encryption_mode(
                SecurityMode_EncryptAndMACPriorKey);
            keyexchange_send_keyrefresh();
        }
        else if (STARTS_WITH(first, "sec_refresh")) {
            send_security_refresh(GLOBAL_BROADCAST,
                                  keyexchange_get_encryption_mode(),
                                  security_update_get_encrypt_security_status()
                                 );
        }
        else if (STARTS_WITH(first, "keepalive")) {
            broadcast_keepalive();
        }
        else if (STARTS_WITH(first, "hello")) {
            keyexchange_sendhello(LOCAL_BROADCAST);
        }
    }
    else if (STARTS_WITH(cmd, "print")) {
        if (first == 0 || strcmp(first, "info") == 0) {
            secure_routing_print_info();
        }
        else if (strcmp(first, "neighbours") == 0) {
            network_nodes_print_nodes(network_nodes_get_root(), 1);
        }
        else if (strcmp(first, "neighbors") == 0) {
            network_nodes_print_nodes(network_nodes_get_root(), 1);
        }
        else if (strcmp(first, "nodes") == 0) {
            network_nodes_print_nodes(network_nodes_get_root(), 0);
        }
        else if (strcmp(first, "routes") == 0) {
            network_nodes_print_nodes(network_nodes_get_root(), 0);
        }
        else if (strcmp(first, "mmr_stats") == 0) {
            ulog("Printing Micro-Mesh Stats");
            micromesh_print_stats();
        }
        else if (strcmp(first, "keys") == 0) {
            keyexchange_print_key_info();
        }
        else if (strcmp(first, "seq_nmbr") == 0) {
            replay_protection_print_info();
        }
        else if (strcmp(first, "sec_lvl") == 0) {
            ulog("Current Security Level=%d", security_preset);
        }
    }
    else if (STARTS_WITH(cmd, "setkey")) {
        if (strcmp(first, "key") == 0) {
            keyexchange_set_new_key((uint8_t *) second, strlen(second),
                                    keyexchange_get_key_seq_nmbr() + 1);
        }
        else if (strcmp(first, "new") == 0) {
            keyexchange_generate_new_key();
        }
        else if (strcmp(first, "transfer_mode") == 0) {
            keyexchange_set_encryption_mode((atoi(second)));
        }
        else if (strcmp(first, "refresh_interval") == 0) {
            keyexchange_set_key_refresh_interval(atoi(second));
        }
        else if (strcmp(first, "seq_nmbr") == 0) {
            keyexchange_set_key_sequence_number(atoi(second));
        }
        else {
            ulog("Setkey command not found");
        }
    }
    else if (STARTS_WITH(cmd, "set")) {
        if (strcmp(first, "sec_lvl") == 0) {
            set_security_level_preset(atoi(second));
        }
        else if (strcmp(first, "seq_nmbr") == 0) {
            replay_protection_set_current_sequence_number(atoi(second));
        }
        else if (strcmp(first, "wrongmacs") == 0) {
            packet_mac_set_wrong_mac_count(atoi(second));

        }
        else if (strcmp(first, "forcemacerrors") == 0) {
            packet_mac_set_force_mac_errors(1);
        }
        else if (strcmp(first, "enc_secstatus") == 0) {
            security_update_set_encrypt_security_status(atoi(second));
        }
        else if (strcmp(first, "enc_secrefresh") == 0) {
            security_update_set_encrypt_security_refresh(atoi(second));
        }
        else if (strcmp(first, "ciphers") == 0) {
            ulog("The ciphers command is not supported \
                 (multiple encryption not enabled)");
        }
        else if (strcmp(first, "overflow") == 0) {
            ulog("Setting node soft overflow flag to %d", atoi(second));
            replay_protection_set_overflow(atoi(second));
        }

        else {
            ulog("Set command not found");
        }
    }
    else if (STARTS_WITH(cmd, "delete")) {
        if (strcmp(first, "nodes") == 0) {
            ulog("> Deleting node table. DO NOT USE FUNCTON WHILE TREE ACTIVE \
                 (not threadsafe)");
            network_nodes_delete_all(network_nodes_get_root());
        }
    }
    else if (STARTS_WITH(cmd, "start")) {
        if (strcmp(first, "network_security") == 0) {
            network_security_start();
        }
    }
    else if (STARTS_WITH(cmd, "ignore")) {
        char *b = cmd + strlen("ignore ");

        if (strncmp(b, "b", 1) == 0) {
            char *addr = strchr(cmd, '!') + 1;

            if (strlen(addr) > 0) {
                ulog_force("> Ignoring broadcasts except %d", atoi(addr));
                ignore_except = atoi(addr);
            }
            else {
                ulog_error("Could not parse address");
            }

            return SecureRouting_OK;
        }
        else if (strncmp(b, "all", 3) == 0) {
            ignore_all = !ignore_all;
            ulog("Ignore_all = %d", ignore_all);

            return SecureRouting_OK;
        }
        else {
            ulog("Ignore command not supported.");
        }

    }
    else {
        // ulog_info("Unknown command: %s", cmd);
    }

    return SecureRouting_OK;
}

/**
 * Secures a byte buffer and sends it to another node in the network. See
 * extended description in the header file.
 */
int secure_send(radio_address_t destination_address, protocol_t protocol,
                int priority, int ttl, char *payload, int payload_len)
{
    if (initialized == 0) {
        ulog("Node not initialized");
        return SecureRouting_NotInited;
    }

    ulog("Secure send called. Dest=%d. Protocol=%d",
         (uint8_t)destination_address, protocol);

    if (payload_len > MAX_PAYLOAD_LEN) {
        ulog_error("The packet being sent was too long. Length=%d bytes. \
                    MaxLength=%d bytes", payload_len, MAX_PAYLOAD_LEN);
        return SecureRouting_TooLong;
    }

    // mutex_lock(&processing_mutex);

    // Create the packet that will be used to wrap the data. The details of
    // the packet will be set in subsequent components like the routing or
    // packet security component
    secure_packet_t packet;
    memset(&packet, 0, sizeof(secure_packet));
    packet.protocol = protocol;
    packet.payload_len = payload_len;
    packet.destination = (uint8_t)destination_address;
    packet.source = get_this_node_address();
    packet.ttl = ttl;
    packet.flags = SECURE_FLAG(PeekHint_DontPeek, security_level);

    set_physical_source_p(packet);

    // Copy the data into the custom header data area
    memcpy(packet.data, payload, payload_len);

    send_packet(&packet);

    // mutex_unlock(&processing_mutex, MUTEX_YIELD);

    return SecureRouting_OK;
}

/**
 * Takes a single packet and then sends it via the radio after encryption.
 * A pointer to this function is provided to the network security classes
 * so they can access the radio
 *
 * @param packet A pointer to the secure packet to be sent
 *
 * @return If the send was successful or not
 */
int send_packet(secure_packet_t *packet)
{
    if (initialized == 0) {
        ulog("Node not initialized");
        return SecureRouting_NotInited;
    }

    // Change the physical source of it
    set_physical_source(packet);

    // Send the packet out via the routing layer
    // the secure function will be called from there
    ulog_info("Sending packet to radio");

    if (GET_SECURITY_MODE(packet->flags) ==
        SecurityMode_EncryptAndMACPairwiseBroadcast &&
        packet->destination == GLOBAL_BROADCAST) {
        /* This is a special case scenario. When the packet is in pairwise
         * encryption and a broadcast is requested, then the packet has to
         * sent to each neighbour individually and encrypted separately
         */
        ulog("> In pairwise broadcast mode. Calling send for each neighbour");

        /* The network nodes module will re-call this function for each
         * neighbour after making sure that the address is not set to global
         * broadcast. That will queue up the packet in micro-mesh
         *
         */
        secure_packet_t packets[10];
        int count = network_nodes_packets_to_neighbors(
                        network_nodes_get_root(),
                        packet, packets,
                        10,
                        0);

        for (int i = 0; i < count; i++) {
            ulog("> Sending packet to %d", packets[i].destination);
            micromesh_send(&packets[i]);
        }
    }
    else {
        if (!micromesh_send(packet)) {
            return SecureRouting_Error;
        }
    }

    return SecureRouting_OK;
}

/**
 * @brief       Places specified buffer in queue for later sending
 */
int enqueue_for_send(radio_address_t gateway, uint8_t protocol,
                     uint8_t priority, uint8_t *packet, uint16_t packet_len,
                     int is_secure_packet, int send_attempts,
                     int send_rerr_if_failed)
{
    if (initialized == 0) {
        ulog("Node not initialized");
        return SecureRouting_NotInited;
    }

    if (send_attempts == 0) {
        ulog_error("Calling radio send with attempts = 0. Probably not what \
                    you meant");
    }

    /* Special handling for pairwise broadcast mode. The packet was secured
     * with dest = 0, but this was set back to destination so it could be sent,
     * but we set it back here
     */
    if (is_secure_packet) {
        secure_packet_t *p = (secure_packet_t *)packet;

        if (GET_SECURITY_MODE(p->flags) ==
            SecurityMode_EncryptAndMACPairwiseBroadcast) {
            p->destination = 0;
        }
    }

    messages_sent++;

    ulog_info("> EXPERIMENT(MessagesSent, Sent=%lu)", messages_sent);
    ulog("> Enqueue function called. Gateway=%d, protocol=%d, packet_len=%d",
         gateway, protocol, packet_len);


    // find an empty spot in queue
    int store_index = -1;

    for (int i = 0; i < RADIO_PROCESS_QUEUE_SIZE; i++) {
        if (radio_process_queue[i].valid == 0 &&
            radio_process_queue[i].writing == 0) {
            store_index = i;

            // Copy packet in
            radio_process_queue[i].writing = 1;
            radio_process_queue[i].to_be_sent = 1;
            radio_process_queue[i].gateway = gateway;
            radio_process_queue[i].protocol = protocol;
            radio_process_queue[i].priority = priority;
            radio_process_queue[i].packet_len = packet_len;
            radio_process_queue[i].is_secure_packet = is_secure_packet;
            radio_process_queue[i].send_attempts = send_attempts;
            radio_process_queue[i].send_rerr_if_failed = send_rerr_if_failed;
            memcpy(radio_process_queue[i].packet, packet, packet_len);
            radio_process_queue[i].valid = 1;
            radio_process_queue[i].writing = 0;
            break;
        }
    }

    if (store_index < 0) {
        ulog_error("Radio queue filled up!Packet dropped");
        return SecureRouting_Error;
    }
    else {
        ulog_info("> Stored packet at location: %d", store_index);
    }

    if (radio_process_pid != thread_getpid()) {
        // Send a message to processing thread.
        ulog_info("> Sending message to radio pump thread");

        // Make sure we are not already in the thread
        msg_t m;
        memset(&m, 0, sizeof(msg_t));

        if (msg_send(&m, radio_process_pid, 1) == 1) {
            ulog_info("> Radio pump thread msg send successful");

        }
        else {
            // If this fails, find out why. It should never fail!
            ulog_error("MAJOR ERROR: Could not send message to radio_receive \
                        thread");
            return SecureRouting_Error;
        }
    }

    // else
    // Remove this
    // pump_radio_process_queue();

    return SecureRouting_OK;
}

/**
 * @brief       Takes a byte buffer and sends it directly via the radio interface
 *          without further manipulation. See header file for further details.
 */
int radio_send_private(radio_address_t gateway, uint8_t protocol,
                       uint8_t priority, uint8_t *packet, uint16_t packet_len,
                       int is_secure_packet, int send_attempts,
                       int send_rerr_if_failed)
{

    int retries = send_attempts;
    int attempts = 0;
    int res = -10;

    while (retries > 0) {
        ulog("> About to radio send to %d. PacketLen=%d, RetryCountdown=%d",
             gateway, packet_len, retries);

        attempts++;
        res = cc1100_send_csmaca(gateway, protocol, priority, (char *)packet,
                                 packet_len);

        if (res < 0) {

            ulog("> Send failed=%d", retries);

            static uint32_t radio_failed = 0;
            static uint32_t radio_busy = 0;
#if ULOG_ENABLED
            char *error_name = "Error";
#endif
            if (res == RADIO_OP_FAILED) {
                radio_failed++;
#if ULOG_ENABLED
                error_name = "RADIO_OP_FAILED";
                ulog_info("> EXPERIMENT(SendError, %s, RadioFail=%lu, \
                           RadioBusy=%lu)", error_name, radio_failed,
                          radio_busy);
#endif
            }
            else if (res == RADIO_CS_TIMEOUT) {
                radio_busy++;
#if ULOG_ENABLED
                // Carrier Sense timeout: air was never free
                error_name = "RADIO_CS_TIMEOUT";
                ulog_info("> EXPERIMENT(SendError, RadioFail=%lu, \
                           RadioBusy=%lu)", radio_failed, radio_busy);
#endif
            }

            ulog_error("Radio send failed code %i. Err=%s Gateway=%d, \
                        protocol=%d, priority=%d, packet_len=%d", res,
                       error_name, gateway, protocol, priority, packet_len);

            if (packet_len > MAX_DATA_LENGTH) {
                ulog_error("Packet was too long to be sent");
            }
            retries--;
        }
        else {
            retries = 0;
            // ulog("> Send worked Retry=%d", retries);
        }
    }

    if (res >= 0) {
        ulog("> Send was successful. Return code %i. Send Attempts=%d", res,
             attempts);
    }
    else {
        ulog("> Send failed finally. Return code %i. Send Attempts=%d", res,
             attempts);

        if (is_secure_packet && send_rerr_if_failed) {
            secure_packet_t *sec_packet = (secure_packet_t *)packet;
            network_node_t *source_node = network_nodes_find(
                                              sec_packet->source);
            network_node_t *destination_node = network_nodes_find(
                                                   sec_packet->destination);

            if (source_node && source_node->gateway > 0 &&
                destination_node && destination_node->gateway > 0) {
                ulog("> -- Broken-Route on packet. Sending back to = %d. \
                      Packet Destination was =%d", source_node->node_id,
                     sec_packet->destination);
                micromesh_packet_dropped(sec_packet, destination_node->gateway,
                                         ROUTE_ERROR_BROKEN_ROUTE);
            }
            else {
                ulog_error("> Could not send route-error because no route");
            }
        }

        return SecureRouting_Error;
    }

    return res;
}

/**
 * @brief       Called by a higher layer to set the handler for a particular
 *          protocol
 *
 * @param       protocol   The protocol for which it should be set
 * @param       handler    The packet handler
 *
 * @return If the send was successful or not
 */
int secure_routing_sethandler(protocol_t protocol, packet_handler_t handler)
{
    if (initialized == 0) {
        ulog("Node not initialized");
        return SecureRouting_NotInited;
    }

    ulog("Setting routing handler for protocol %d", protocol);

    // Set the packet handler on radio level
    int res = cc1100_set_packet_handler(SECURE_ROUTING_PROTOCOL,
                                        (packet_handler_t)packet_handler);

    if (res < 0) {
        ulog_error("Could not register protocol %i with the cc1100 radio. \
                    cc1100_set_packet_handler returned %i", protocol, res);
        return res;
    }

    // Set the protocol handler in our own internal table, so we know where
    // to route the packets to when they arrive
    ulog_info("Setting protocol handler on the table");
    res = pm_set_handler(&protocol_handler_table, protocol, handler);

    if (res < 0) {
        ulog_error("Could not register protocol %i in the handler table. \
                    pm_set_handler returned %i", protocol, res);
        return res;
    }

    return SecureRouting_OK;
}

/**
 * Called when a packet arrives that is meant to be routed on
 *
 * @param packet  A pointer to the secure radio packet
 *
 * @return If the send was successful or not
 */
packet_handler_t route_packet(radio_process_packet_t *radio_packet)
{
    secure_packet_t *packet = (secure_packet_t *)radio_packet->packet;

    ulog("> Routing Packet. TTL=%d, Dest=%d", packet->ttl,
         packet->destination);

    int res = 0;

    if (packet->ttl > 0) {
        // It's important that this does not start a route-request
        // That's the responsibility of the guy who sent it


        // check the mac and the replay of the packet
        if ((res = verify_packet(packet,
                                 radio_packet->packet_info.phy_src)) <
            PacketSecurity_OK) {
            ulog_error("ERROR: Packet failed verification");
            return 0;
        }

        // Reduce the TTL
        packet->ttl--;

        int should_send_rerr = 1;

        // Look if we need to decrypt the packet and process it
        if (SHOULD_PEEK(packet->flags)) {
            ulog("> Peeking at packet");
            // If we are peeking, then it's intenal. No rerr
            should_send_rerr = 0;

            // We are just routing the packet, but there is a hint
            // that we need to examine the contents of the packet
            // So we decrypt it.

            desecure_packet(packet, radio_packet->packet_info.phy_src);

            // Send to micro-mesh, who may want to see if it can use it
            micromesh_peek(packet);

            // Perhaps the network security can use this packet
            if (network_security_packet_arrived(packet) ==
                NetworkSecurity_NoRebroadcast) {
                ulog("Packet rebroadcast stopped. Dropped.");
                return 0;
            }

            // Encrypt and mac it again
            set_physical_source(packet);

            if (secure_packet(packet, get_this_node_address()) <
                PacketSecurity_OK) {
                ulog_error("Peeked at packet but could not re-encrypt it. \
                                   Sorry. Drop.");
                return 0;
            }
        }
        else {
            // The packet has been modified, but was not decrypted
            if (GET_SECURITY_MODE(packet->flags) ==
                SecurityMode_EncryptAndMACPairwise) {
                /* In this mode, we need to re-encrypt the contents with our
                 * shared key
                 **/
                desecure_packet(packet, radio_packet->packet_info.phy_src);

                set_physical_source(packet);

                if (secure_packet(packet, get_this_node_address()) <
                    PacketSecurity_OK) {
                    ulog_error("Could not re-secure packet in pairwise mode.\
                                Drop.");
                    return 0;
                }
            }
            else {
                /* We are routing the packet, but we changed the TTL, so we
                 * need to re-mac it
                 **/
                set_physical_source(packet);
                packet_mac_secure_packet(packet, get_this_node_address());
            }
        }

        int sent_successfully = 0;
        network_node_t *node = network_nodes_find(packet->destination);

        // If next hop address found in routing table, forward message
        if (node != 0 && node->gateway != 0) {
            if (radio_packet->packet_info.phy_src == node->gateway) {
                ulog("Gateway to node = where it came from. Dropping packet");
                sent_successfully = 0;
            }
            else {

                ulog("> Routing via Gateway %d", node->gateway);

                if (enqueue_for_send(node->gateway,
                                     SECURE_ROUTING_PROTOCOL,
                                     PRIORITY_DATA, (uint8_t *)packet,
                                     sizeof(secure_packet_t),
                                     1,
                                     RADIO_SEND_RETRY,
                                     should_send_rerr) != SecureRouting_OK) {
                    sent_successfully = 0;
                }
                else {
                    sent_successfully = 1;
                }

            }

            if (sent_successfully != 1) {
                /* If this hits, then it means the packet could not be sent.
                 * We are not supposed to receive a packet we cannot handle, so
                 * a route-error message has to be sent back to the send
                 **/

                network_node_t *source_node = network_nodes_find(
                                                  packet->source);
                network_node_t *destination_node = network_nodes_find(
                                                       packet->destination);

                ulog("> Broken-Route on packet. Sending back to = %d. Packet \
                     Destination was =%d. Gateway=%d",
                     destination_node ? destination_node->node_id : 0,
                     packet->destination,
                     destination_node ? destination_node->gateway : 0);

                if (source_node && source_node->gateway > 0 &&
                    destination_node && destination_node->gateway > 0) {
                    micromesh_packet_dropped(packet,
                                             destination_node->gateway,
                                             destination_node == 0 ?
                                             ROUTE_ERROR_MISSING_ROUTE :
                                             ROUTE_ERROR_BROKEN_ROUTE);
                }
                else {
                    ulog_error("> Could not send route error because no \
                                route");
                }
            }
        }
    }
    else {
        ulog("Packet TTL <= 0. Dropping");
    }

    return 0;
}

/**
 * Called when a packet arrives that is meant for this node
 * (broadcast or unicast)
 *
 * @param packet  A pointer to the secure radio packet
 * @param info    Pointer to the packet_info structure as provided by the radio
 *
 * @return If the send was successful or not
 */
packet_handler_t handle_local_packet(radio_process_packet_t *radio_packet)
{
    ulog_info("Packet verification succeeded");

    secure_packet_t *packet = (secure_packet_t *)radio_packet->packet;

    if (IS_LOCAL_ADDRESS(packet->destination)) {
        ulog("> Unicast Packet addressed to us arrived");
    }
    else if (packet->destination == GLOBAL_BROADCAST ||
             packet->destination == LOCAL_BROADCAST) {
        ulog("> Broadcast Packet to us arrived");

        // Special ignore flag for debugging routing. Ignores physical sources
        if (ignore_except != 0) {
            if ((uint8_t)radio_packet->packet_info.phy_src != ignore_except) {
                ulog("> IGNORED. Broadcast from %d. Ignore=%d",
                     radio_packet->packet_info.phy_src, ignore_except);
                return 0;
            }
            else {
                ulog("> ACCEPTED. Broadcast from %d. Ignore=%d",
                     radio_packet->packet_info.phy_src, ignore_except);
            }

        }
    }
    else {
        ulog_error("> Packet not for us arrived");
    }

    int res = 0;

    // check the mac and the replay of the packet
    if ((res = verify_packet(packet,
                             radio_packet->packet_info.phy_src)) <
        PacketSecurity_OK) {
        if (res != PacketSecurity_ReplayedBroadcast) {
            ulog_error("Packet failed verification");
        }
        else {
            ulog("> We've seen this broadcast before (node:seq=%d:%lu). Exit.",
                 packet->source, packet->seq_nmbr);
        }

        return 0;
    }

    // decrypt packet if it is encrypted
    if (desecure_packet(packet, radio_packet->packet_info.phy_src) <
        PacketSecurity_OK) {
        ulog_error("Could not desecure packet");
        return 0;
    }

    /* Check if the physical source is one of the known neighbours. We only
     * accept packets from neighbours
     **/
    if (packet->data[0] != NetworkSecurity_HELLO &&
        packet->data[0] != NetworkSecurity_ACK) {
        /*
        if (network_nodes_is_neighbour(radio_packet->packet_info.phy_src) == 0)
        {
                ulog("> EXPERIMENT(RREQCoverage, REJECTED=1)");
                ulog_error("Only packets from neighbours accepted.");
                return 0;
        }
        */
    }

    ulog_info("Packet desecuring succeeded");

    int net_res = network_security_packet_arrived(packet);

    if (net_res < NetworkSecurity_OK) {
        ulog_error("> The network security tried but failed to process \
                    packet");
        return 0;
    }

    if (net_res == NetworkSecurity_NoRebroadcast) {
        ulog("> Network security requested packet drop. Not rebroadcasting");
        return 0;
    }

    // If internal protocol is not secure routing, propagate up
    if (packet->protocol != SECURE_ROUTING_PROTOCOL) {
        ulog_info("> Packet meant for this node");

        // Set the parameters on the packet info structure
        radio_packet->packet_info.tos = packet->flags;
        radio_packet->packet_info.ttl = packet->ttl - 1;
        radio_packet->packet_info.source = packet->source;
        radio_packet->packet_info.destination = packet->destination;

        // Invoke higher layer handler with its payload
        ulog("> Invoking packet handler");
        pm_invoke(&protocol_handler_table, packet->protocol, packet->data,
                  packet->payload_len, &(radio_packet->packet_info));
    }

    /* pass it on to routing layer, in case it is a route-request,
     * route-reply etc
     * */
    int mm_res = micromesh_receive(packet, &(radio_packet->packet_info));

    if (mm_res == Routing_PacketHandledNoRebroadcast) {
        // That means we handled this packet no need to rebroadcast
        ulog("> Packet handled by micromesh. Not rebroadcasting");
        mm_res = NetworkSecurity_PacketHandled;
    }
    else if (packet->ttl > 0) {
        ulog_info("> Packet TTL is %d. Dest=%d. IsLocal=%d",
                  packet->ttl,
                  packet->destination,
                  IS_LOCAL_ADDRESS(packet->destination));
        packet->ttl--;

        if (packet->ttl > 0 && IS_LOCAL_ADDRESS(packet->destination) == 0) {
            ulog("> Packet Rebroadcasting");

            send_packet(packet);
        }
        else {
            if (packet->ttl == 0) {
                ulog("> Packet not re-broadcasted because ttl = 0");
            }
        }
    }

    if (packet->protocol == SECURE_ROUTING_PROTOCOL) {

        if (net_res == NetworkSecurity_PacketHandled) {
            ulog("!> Packet handled by network security");
        }

        if (mm_res == Routing_PacketHandledNoRebroadcast ||
            mm_res == Routing_PacketHandledRebroadcast) {
            ulog("!> Packet handled by micromesh");
            mm_res = NetworkSecurity_PacketHandled;
        }

        if (net_res != NetworkSecurity_PacketHandled &&
            mm_res != NetworkSecurity_PacketHandled) {
            ulog("> This packet had SecureRouting protocol, but could not be \
                 handled. Cross-check");
            ulog("!Unknown packet arrived. Check that handler component \
                 enabled");
        }

        ulog_info("Internally handled. Local=%d Prot=%d",
                  IS_LOCAL_ADDRESS(packet->destination),
                  packet->protocol);
    }

    return 0;
}


/**
 * Called when a packet arrives. Called by lower layers
 *
 * @param datav       A pointer to the data buffer
 * @param data_length The length of the data added
 * @param info        Pointer to the packet_info structure as provided by
 *                    the radio
 *
 * @return If the send was successful or not
 */
packet_handler_t packet_handler(void *datav, int data_length,
                                packet_info_t *info)
{
    if (initialized == 0) {
        ulog("Node not initialized");
        return 0;
    }

    // Called when packets arrive here
    secure_packet_t *packet = (secure_packet_t *)datav;

    // Do some preliminary sanity checks

    // Make sure we did not send out this packet ourselves
    if (IS_LOCAL_ADDRESS(packet->source)) {
        ulog_info("> Packet for local address arrived");
        return 0;
    }

    // If packet has expired, discard without further processing
    if (packet->ttl <= 0) {
        ulog_error("Packet with ttl <= 0 arrived. Source=%d. Dest=%d",
                   packet->source,
                   packet->destination);
        return 0;
    }

    if (packet->ttl > 15) {
        ulog_error("Packet with HIGH ttl arrived. Dropping.");
        return 0;
    }

    // If ignore flag is set, then packets won't be processed
    if (ignore_all == 1) {
        ulog("Packet arrived, but ignore_all is = 1");
        return 0; // Stop receiving packet
    }

#ifdef _PACKET_PHY_SRC

    if (packet->phy_source == 0 || info->phy_src != packet->phy_source) {
        ulog_error("Bad packet source. PacketPhySrc=%d, HwPhySrc=%d",
                   packet->phy_source, info->phy_src);
        return 0;
    }

#endif

    ulog("!Packet arrived from %d->%d (via %d). Len=%d. Type=%d",
         (uint8_t)packet->source, (uint8_t)packet->destination, info->phy_src,
         packet->payload_len, (uint8_t)(packet->data[0]));


    // Copy packet into radio processing thread
    int store_index = -1;

    for (int i = 0; i < RADIO_PROCESS_QUEUE_SIZE; i++) {
        if (radio_process_queue[i].valid == 0 &&
            radio_process_queue[i].writing == 0) {
            store_index = i;
            radio_process_queue[i].writing = 1;
            radio_process_queue[i].to_be_sent = 0;
            radio_process_queue[i].gateway = 0;
            radio_process_queue[i].protocol = 0;
            radio_process_queue[i].priority = 0;
            radio_process_queue[i].packet_len = 0;
            radio_process_queue[i].is_secure_packet = 0;
            radio_process_queue[i].send_attempts = 0;
            radio_process_queue[i].send_rerr_if_failed = 0;
            memcpy(radio_process_queue[i].packet, packet, data_length);
            memcpy(&(radio_process_queue[i].packet_info), info,
                   sizeof(packet_info_t));
            radio_process_queue[i].valid = 1;
            radio_process_queue[i].writing = 0;
            break;
        }
    }

    memcpy(&last_packet, packet, sizeof(secure_packet_t));

    if (store_index < 0) {
        ulog_error("Radio receive queue filled up!Packet dropped");
        return 0;
    }
    else {
        ulog_info("> Stored received packet at location: %d", store_index);
    }

    msg_t m;
    memset(&m, 0, sizeof(msg_t));

    if (msg_send(&m, radio_process_pid, 1) == 1) {
        ulog_info("> Sent message for packet processing");
    }

    return 0;
}

/**
 * @brief       Sends out everything in the send queue
 */
int pump_radio_process_queue()
{
    ulog_info("> Radio Process pump entered");

    for (int i = 0; i < RADIO_PROCESS_QUEUE_SIZE; i++) {
        if (radio_process_queue[i].valid == 1) {
            if (radio_process_queue[i].to_be_sent == 1) {
                ulog("> Sending packet: Gateway=%d, Protocol=%d, PacketLen=%d",
                     radio_process_queue[i].gateway,
                     radio_process_queue[i].protocol,
                     radio_process_queue[i].packet_len);

                radio_send_private(radio_process_queue[i].gateway,
                                   radio_process_queue[i].protocol,
                                   radio_process_queue[i].priority,
                                   radio_process_queue[i].packet,
                                   radio_process_queue[i].packet_len,
                                   radio_process_queue[i].is_secure_packet,
                                   radio_process_queue[i].send_attempts,
                                   radio_process_queue[i].send_rerr_if_failed);
            }
            else {
                secure_packet_t *packet = (secure_packet_t *)
                                          radio_process_queue[i].packet;

                if (packet->destination == GLOBAL_BROADCAST ||
                    packet->destination == LOCAL_BROADCAST ||
                    IS_LOCAL_ADDRESS(packet->destination)) {

                    handle_local_packet(&radio_process_queue[i]);
                }
                else {
                    route_packet(&radio_process_queue[i]);
                }
            }

            radio_process_queue[i].valid = 0;
        }
    }

    ulog_info("> Radio Process pump exit");

    return SecureRouting_OK;
}

/**
 * Runs the process for executing the radio receive
 *
 */
static void radio_process_thread(void)
{
    ulog("> Started Radio Receive Thread");

    int res = msg_init_queue(radio_msg_queue, 16);

    if (res < 0) {
        ulog_error("ERROR: Radio Message queue could not be inited");
    }

    msg_t m;

    do {
        // Get the message
        msg_receive(&m);

        ulog_info("> Message received by radio send thread");
        pump_radio_process_queue();

        // In case the above wrote a packet in the lower part
        // of the queue
        pump_radio_process_queue();

    }
    while (m.type != MSG_EXIT);

    ulog_error("> Radio send thread exit");
}

static void watchdog_thread(void)
{
    ulog("> Started Watchdog Thread");

    int pending_count = 0;

    do {
        timex_t t = timex_set(0, 100000);
        vtimer_sleep(t);

        if (thread_getstatus(radio_process_pid) == STATUS_SLEEPING) {
            pending_count = 0;
            ulog_error("> Radio Thread is sleeping!");
            thread_wakeup(radio_process_pid);

            ulog("> Asked radio thread to wake-up via watchdog");
        }
        else if (thread_getstatus(radio_process_pid) == STATUS_PENDING) {
            pending_count++;
            ulog_error("> Radio thread is in pending status");

            if (pending_count >= 3) {
                // thread has been pending more than 3 seconds
                ulog_error("Thread was pending for > 3 seconds. Workaround");

                tcb_t *me = (tcb_t *) sched_threads[radio_process_pid];
                ulog("Forcefully re-running thread %s. Fix this if you can.",
                     me->name);
                sched_set_status(me,  STATUS_SLEEPING);
            }
        }
        else {
            pending_count = 0;
        }
    } while (1);

    ulog_error("> Watchdog send thread exit");
}
