/*
 * packet_mac.c - MAC for single packets.
 *
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * This source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 */


/*
 * @file        packet_mac.c
 * @brief       Used to mac single packets
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Mark Essien <markessien@gmail.com>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * @date        13.09.2013 17:48:55
 */

/*---------------------------------------------------------------------------*/
//                                      Included Files
/*---------------------------------------------------------------------------*/

/*  Radio includes */
#include "cc1100.h"
#include "radio.h"
#include "cc1100_phy.h"

/* Standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

/* Timer */
#include "hwtimer.h"
#include "timex.h"
#include "vtimer.h"

/* Project Includes */
#include "secure_routing_globals.h"
#include "packet_security.h"
#include "packet_mac.h"
#include "microeval/microeval.h"
#include "network_nodes.h"
#include "network_security.h"
#include "SkipJack.h"
#include "RC5.h"

/*---------------------------------------------------------------------------*/
//                                      Function Declarations
/*---------------------------------------------------------------------------*/
int mac_init(MACContext *context, uint8_t keySize, uint8_t *key);
int mac_init_incremental_MAC(MACContext *context, uint16_t length);
int mac_incremental_MAC(MACContext *context, uint8_t *msg, uint16_t msgLen);
int mac_get_incremental_MAC(MACContext *context, uint8_t *res,
                              uint8_t macSize);
int mac_MAC(MACContext *context, uint8_t *msg, uint16_t length, uint8_t *res,
            uint8_t mac_size);
static uint8_t block_cipher_info_get_preferred_block_size(uint8_t
        cipher_index);

/*---------------------------------------------------------------------------*/
//                                      Structures and Enumerations
/*---------------------------------------------------------------------------*/

/* Component public interface */
packet_security_component_t mac_component = {
    "MAC",
    packet_mac_init_component,
    packet_mac_secure_packet,
    packet_mac_verify_packet,
    0
};

enum {
    // we allocate some static buffers on the stack; they have to be less
    // than this size
    CBCMAC_BLOCK_SIZE     = 8,
    CBCMAC_BLOCK_SIZE_AES = 16
};

/**
 * @brief Context for the calculation of a MAC with the initialized password
 */
typedef struct CBCMACContext {
#if defined(AES) || defined (TWOFISH)
    /* the result of our partial computation. we xor this with new data and
     * then encrypt it when full.
     */
    uint8_t  partial[CBCMAC_BLOCK_SIZE_AES];
#else
    /* the result of our partial computation. we xor this with new data and
     * then encrypt it when full.
     */
    uint8_t  partial[CBCMAC_BLOCK_SIZE];
#endif
    // the total number of bytes left for the MAC to process.
    uint16_t length;
    // the current offset into the partial array.
    uint8_t  blockPos;
} /*__attribute__ ((packed))*/ CBCMACContext;



/*---------------------------------------------------------------------------*/
//                                      Variables
/*---------------------------------------------------------------------------*/
// Interface to skipjack encryption
extern block_cipher_interface_t skipJackInterface;
// Interface to rc5 encryption
extern block_cipher_interface_t RC5Interface;
// Interface to aes encryption
extern block_cipher_interface_t AESInterface;
// Interface to twofish encryption
extern block_cipher_interface_t TWOFISHInterface;
// Interface to 3des encryption
extern block_cipher_interface_t ThreeDESInterface;
// Context for encryption
static MACContext macContext;
// Currently counting wrong macs
static int cur_wrong_MACs;
/* Number of wrong macs in the last *completed* interval (for example,
 * last 10-mins)
 */
static int last_interval_wrong_MACS;
/* Indicates that the last interval was reset back (we need this because 0 is
 * ambiguous)
 */
static int last_interval_reset;
// When the currently counting interval for wrong-macs started
static timex_t last_interval_started;
// Variable to force mac errors on every second packet. For debug  purposes.
static int force_mac_errors;
// Used to disable this component
int enable_mac;
// The archive object for the ciphers
BlockCipher_Archive_t mac_archive;
// Small Blocks = 8 Byte, big Blocks are 16-Byte
bool bigBlocks = 0;

/**
 * @brief Returns the message-authentication component
 *
 */
packet_security_component_t *packet_mac_get_component()
{
    return &mac_component;
}

/**
 * @brief Initializes the message-authentication component
 *
 * @return Either OK or Error as a PacketSecurity_Result
 */
int packet_mac_init_component()
{
    vtimer_now(&last_interval_started);
    cur_wrong_MACs = 0;
    last_interval_wrong_MACS = 0;
    // Default at 1, so does not send till at least first 10 mins
    last_interval_reset = 1;
    force_mac_errors = 0;

    mac_archive.NoCiphers = 0;
#ifdef SKIPJACK
    ulog_info("Inited MAC skipjack encryption");
    mac_archive.ciphers[mac_archive.NoCiphers] = skipJackInterface;
    mac_archive.NoCiphers += 1;
#endif
#ifdef RC5
    ulog_info("Inited MAC RC5 encryption");
    mac_archive.ciphers[mac_archive.NoCiphers] = RC5Interface;
    mac_archive.NoCiphers += 1;
#endif
#ifdef AES
    ulog_info("Inited MAC AES encryption");
    mac_archive.ciphers[mac_archive.NoCiphers] = AESInterface;
    mac_archive.NoCiphers += 1;
#endif
#ifdef TWOFISH
    ulog_info("Inited MAC twofish encryption");
    mac_archive.ciphers[mac_archive.NoCiphers] = TWOFISHInterface;
    mac_archive.NoCiphers += 1;
#endif
#ifdef THREEDES
    ulog_info("Inited MAC 3des encryption");
    mac_archive.ciphers[mac_archive.NoCiphers] = ThreeDESInterface;
    mac_archive.NoCiphers += 1;
#endif

    ulog_info("Initing MAC...");
    int result = mac_init(&macContext, SECURE_ROUTING_KEYSIZE, global_key);

    if (result <= 0) {
        ulog_error("Mac Init failed. Result=%d", result);
        return result;
    }

    char sz[21] = {'\0'};
    memcpy(sz, global_key, 20);

    for (int i = 0; i < sizeof(global_key); i++) {
        if (sz[i] < 48 || sz[i] > 122) {
            sz[i] = '?';
        }

        // printf("Out: %i", sz[i]);
    }

    ulog("> MAC-security inited");
    return result;
}

/**
 * @brief Updates the variables storing mac errors. Can be called at any time
 */
void packet_mac_update_mac_errors(void)
{
    // The wrong macs are calculated over a time-interval (set in config). This
    // value of the last chunk is what is used to send security errors messages
    timex_t time_now ;
    vtimer_now(&time_now);

    uint32_t time_diff = time_now.microseconds -
                         last_interval_started.microseconds;
    uint32_t time_diff_secs = (uint32_t)((float)time_diff / 1000000.0);

    ulog_info("> Time-difference since wrong mac interval=%lu",
              time_diff_secs);

    if ((time_diff_secs / 60.0) > WRONG_MAC_TIME_INTERVAL) {

        vtimer_now(&last_interval_started);
        last_interval_wrong_MACS = cur_wrong_MACs;
        cur_wrong_MACs = 0;
        last_interval_reset = 0;

        ulog_info("> Set last interval wrong macs to %d",
                  last_interval_wrong_MACS);
    }
}

/**
 * @brief Writes a message-authentication code. See header file for
 *        extended description
 */
int packet_mac_secure_packet(secure_packet_t *packet, uint16_t phy_src)
{
    if (enable_mac == 0) {
        return PacketSecurity_Disabled;
    }

    ulog("> Applying Message-Authentication...");

    int payload_len = packet->payload_len;

    if (payload_len < 16) {
        payload_len = 16;
    }

    uint8_t *key = global_key;

    uint8_t original_destination = packet->destination;

    if (GET_SECURITY_MODE(packet->flags) == SecurityMode_MACWithInitialKey) {
        ulog("> In initial key mac mode");
        key = initial_key;
    }
    else if (GET_SECURITY_MODE(packet->flags) ==
             SecurityMode_EncryptAndMACPriorKey) {
        ulog("> Using previous global key to MAC packet");
        key = last_global_key;
    }
    else if (GET_SECURITY_MODE(packet->flags) ==
             SecurityMode_EncryptAndMACPairwise ||
             GET_SECURITY_MODE(packet->flags) ==
             SecurityMode_EncryptAndMACPairwiseBroadcast) {
        ulog("> In pairwise mac mode");

        network_node_t *node = network_nodes_find(packet->destination);

        if (node == 0) {
            ulog_error("Node not found. Pairwise decryption to non-neighbours \
                        not supported");
            return PacketSecurity_NotAvailable;
        }

        if (node->is_neighbour == 0) {
            node = network_nodes_find(node->gateway);

            if (node == 0 || node->is_neighbour == 0) {
                ulog_error("Pairwise encryption not possible. Gateway not \
                            found");
                return PacketSecurity_NotAvailable;
            }

            ulog("> Found gateway neighbour for pw encryption");
        }

        // at this point, it's a neighbour and we have the node
        key = node->pairwise_key;

        /* In pairwise broadcast mode, the destination in the packet is 0, but
         * the actual radio destination is the original destination
         */
        if (GET_SECURITY_MODE(packet->flags) ==
            SecurityMode_EncryptAndMACPairwiseBroadcast) {
            packet->destination = 0;
        }
    }

    int res = mac_init(&macContext, SECURE_ROUTING_KEYSIZE, key);

    if (res <= 0) {
        ulog_error("Mac Init failed. Result=%d", res);
        return PacketSecurity_Error;
    }

    // 2 bytes for physical source
    uint8_t mac_buffer[sizeof(secure_packet_t) + 2];
    memcpy(mac_buffer, &phy_src, 2);
    memcpy(&mac_buffer[2], (uint8_t *)packet, sizeof(secure_packet_t));

    res = mac_MAC(&macContext, mac_buffer,
                  2 + SECURE_HEADER_LEN + payload_len,
                  packet->mac,
                  4);

    if (res < 0) {
        ulog_error("MAC MAC failed. Result=%d", res);
        return PacketSecurity_Error;
    }

    // In case it was changed for pair-wise broadcast mode, set it back
    packet->destination = original_destination;

    return PacketSecurity_OK;
}

/**
 * @brief Verifies that the message-authentication code is valid. See
 *        header for extended description.
 */
int packet_mac_verify_packet(secure_packet_t *packet, uint16_t phy_src)
{
    if (enable_mac == 0) {
        ulog("> Packet MAC verify disabled");
        return PacketSecurity_Disabled;
    }

    int payload_len = packet->payload_len;

    if (payload_len < 16) {
        payload_len = 16;
    }

    uint8_t *key = global_key;

    if (GET_SECURITY_MODE(packet->flags) == SecurityMode_None ||
        GET_SECURITY_MODE(packet->flags) == SecurityMode_Encrypt) {
        ulog("> Packet has no authentication");
        return PacketSecurity_OK;
    }
    else if (GET_SECURITY_MODE(packet->flags) ==
             SecurityMode_MACWithInitialKey) {
        ulog("> In initial key mac mode");
        key = initial_key;
    }
    else if (GET_SECURITY_MODE(packet->flags) ==
             SecurityMode_EncryptAndMACPairwise ||
             GET_SECURITY_MODE(packet->flags) ==
             SecurityMode_EncryptAndMACPairwiseBroadcast) {
        ulog("> In pairwise mac mode");

        network_node_t *node = network_nodes_find(phy_src);

        if (node == 0) {
            ulog_error("Node not found. Pairwise decryption to non-neighbours \
                        not supported");
            return PacketSecurity_NotAvailable;
        }

        if (node->is_neighbour == 0) {
            ulog_error("Pairwise key verification can only be done for \
                        neighbours. %d not neighbour.", node->node_id);
            return PacketSecurity_NotAvailable;
        }

        // at this point, it's a neighbour and we have the node
        key = node->pairwise_key;
    }

    int res = mac_init(&macContext, SECURE_ROUTING_KEYSIZE, key);

    if (res <= 0) {
        ulog_error("Mac Init failed. Result=%d", res);
        return PacketSecurity_Error;
    }

    // 2 bytes for physical source
    uint8_t mac_buffer[sizeof(secure_packet_t) + 2];
    memcpy(mac_buffer, &phy_src, 2);
    memcpy(&mac_buffer[2], (uint8_t *)packet, sizeof(secure_packet_t));

    uint8_t this_MAC[4] = {0};
    res = mac_MAC(&macContext, mac_buffer,
                  2 + SECURE_HEADER_LEN + payload_len,
                  this_MAC,
                  4);

    if (res < 0) {
        ulog_error("Macking the packet for verification failed");
        return PacketSecurity_Error;
    }

    //check if the calculated mac equals the passed one
    int mac_error = PacketSecurity_OK;

    for (int i = 0; i < 4; i++) {
        int force_error = 0;
        int x = rand() % 2;

        if (x == 1 && force_mac_errors) {
            force_error = 1;
        }

        if (packet->mac[i] != this_MAC[i] || force_error == 1) {

            ulog_error("MAC Verification failed. Received MAC= [%x][%x][%x]\
                        [%x] and calculated MAC= [%x][%x][%x][%x].",
                       packet->mac[0],
                       packet->mac[1],
                       packet->mac[2],
                       packet->mac[3],
                       this_MAC[0],
                       this_MAC[1],
                       this_MAC[2],
                       this_MAC[3]);

            network_node_t *node = network_nodes_find(phy_src);

            if (node != 0) {
                node->wrong_MACs++;
            }

            cur_wrong_MACs++;

            mac_error = PacketSecurity_Error;
            break;
        }
    } //end of for-loop - MAC OK

    packet_mac_update_mac_errors();

    if (mac_error != PacketSecurity_Error) {
        ulog_info("> MAC Succeeded. Received MAC= [%x][%x][%x][%x] and \
                          calculated MAC= [%x][%x][%x][%x].",
                  packet->mac[0],
                  packet->mac[1],
                  packet->mac[2],
                  packet->mac[3],
                  this_MAC[0],
                  this_MAC[1],
                  this_MAC[2],
                  this_MAC[3]);
    }

    ulog("> Packet Authentication succeeded");

    return mac_error;
}

/**
 * @brief Disables or enables this component
 *
 */
void packet_mac_enable(int val)
{
    enable_mac = val;

    if (val == 0) {
        ulog("> Packet MAC disabled");
    }
}

/**
 * @brief Gets the number of wrong macs that have been recorded by
 *        the component. See header file for extended description
 */
int packet_mac_get_wrong_mac_count(int *was_reset)
{
    packet_mac_update_mac_errors();

    *was_reset = last_interval_reset;
    return last_interval_wrong_MACS;
}

/**
 * @brief Forces the component to have a mac error every 2nd packet
 *        for debugging purposes
 */
void packet_mac_set_force_mac_errors(int val)
{
    force_mac_errors = 1;
}

/**
 * @brief Sets the number of wrong macs that have been recorded by
 *        the component. See header file for extended description
 */
void packet_mac_set_wrong_mac_count(int new_wrong_macs)
{
    ulog("Setting last-interval wrong macs to %d", new_wrong_macs);
    last_interval_wrong_MACS = new_wrong_macs;
    last_interval_reset = 0;
    vtimer_now(&last_interval_started);
}

/**
 * Resets the recorded message-authentication errors back
 * to 0. See header for extended description
 */
void packet_mac_reset_mac_errors()
{
    last_interval_wrong_MACS = 0;
    last_interval_reset = 1;
}

/**
 * Returns the preferred block size for a particular cipher from the table
 *
 * @param cipher_index The index of the cipher
 *
 * @return The preferred block size for that cipher
 */
uint8_t block_cipher_info_get_preferred_block_size(uint8_t cipher_index)
{
    uint8_t res;
    res = mac_archive.ciphers[cipher_index].
          BlockCipherInfo_getPreferredBlockSize();
    return res;
}

/**
 * Initializes the MAC layer and stores any local state into the context
 * variable. The context variable should be used for future invocations
 * which share this key. It uses the preferred block size of the underlying
 * BlockCipher
 *
 * @param context       opaque data structure to hold the module specific state
 *                      associated with this key.
 * @param keySize       length of the key in bytes.
 * @param key           pointer to a buffer containing keySize bytes of shared
 *                      key data
 *
 * @return Whether initialization was successful. The command may be
 *         unsuccessful if the key size or blockSize are not valid for the
 *         given cipher implementation.
 */
int mac_init(MACContext *context, uint8_t keySize, uint8_t *key)
{
    //if AES or TWOFISH is the MAC-Algorithm, we have 16Byte BlockSize
    char aesString[] = "AES\0";
    char twofishString[] = "TWOFISH\0";
    bigBlocks = !(strcmp(mac_archive.ciphers[0].name, aesString));

    if (!bigBlocks) {
        bigBlocks = !(strcmp(mac_archive.ciphers[0].name, twofishString));
    }

    if (bigBlocks) {
        if (block_cipher_info_get_preferred_block_size(0) !=
            CBCMAC_BLOCK_SIZE_AES) {
            ulog("MAC Block size exceeds max size");
            // the block cipher exceeds our max size.
            return 0;
        }

        /* just init the underlying block cipher - with the first
         * cipher-algorithm (for now)
         */
        return mac_archive.ciphers[0].BlockCipher_init(&context->cc,
                CBCMAC_BLOCK_SIZE_AES,
                keySize,
                key);
    }
    else {  //SKIPJACK or RC5 or 3DES is the MAC-Algorithm so 8 Byte BlockSize
        if (block_cipher_info_get_preferred_block_size(0) != CBCMAC_BLOCK_SIZE) {
            ulog("MAC Block size exceeds max size");
            // the block cipher exceeds our max size.
            return 0;
        }

        /* just init the underlying block cipher - with the first
         * cipher-algorithm (for now)
         */
        return mac_archive.ciphers[0].BlockCipher_init(&context->cc,
                CBCMAC_BLOCK_SIZE,
                keySize,
                key);
    }
}


/**
 * Initializes an invocation of an incremental MAC computation. This is
 * provided for asynchronous operation so that the MAC may be incrementally
 * computed. Partial state is stored in the context.
 *
 * @param context   opaque data structure to hold the module specific state
 *                  associated with this invocation of the incremental
 *                  computation.
 * @param length    the total length of data that is forthcoming
 *
 * @return whether the incremental initialization was successful.
 *         This can fail if the underlying cipher operation fails.
 */
int mac_init_incremental_MAC(MACContext *context, uint16_t length)
{
    // temp. plain text. to make CBC-Mac secure for variable length messages
    // we need to modify the normal CBC procedure: namely, we initialze the
    // mac by encrypting the 0th block as the length (in blocks) of the
    // message. This results in a secure MAC.
    //
    // see Mihir Bellare, Joe Kilian, Phillip Rogaway
    // The Security of the Cipher Block Chaining Message Authentication Code
    // 1995, p12-13

    // temp buffer to hold length buffer which we'll encrypt to the
    // real "partial" stored in the context.

    // the first algorithm in the archive is taken for MACing - so check which
    // one it is to decide on BlockSize

    if (bigBlocks) {    //AES and TWOFISH have 16-Byte Blocks
        uint8_t partial[CBCMAC_BLOCK_SIZE_AES];
        // length divided by 16 [ ie num blocks]
        uint8_t numBlocks = length >> 4;
        memset(partial, 0, 14);
        partial[14] = (numBlocks >> 8) & 0xff;
        partial[15] = (numBlocks & 0xff);

        ((CBCMACContext *) context->context)->length = length;
        ((CBCMACContext *) context->context)->blockPos = 0;
        return mac_archive.ciphers[0].BlockCipher_encrypt(&context->cc,
                partial,
                ((CBCMACContext *) context->context)->partial);
    }
    else {      //SKIPJACK, RC5 and 3DES have 8-Byte Blocks
        uint8_t partial[CBCMAC_BLOCK_SIZE];
        // length divided by 8 [ ie num blocks]
        uint8_t numBlocks = length >> 3;
        memset(partial, 0, 6);
        partial[6] = (numBlocks >> 8) & 0xff;
        partial[7] = (numBlocks & 0xff);

        ((CBCMACContext *) context->context)->length = length;
        ((CBCMACContext *) context->context)->blockPos = 0;
        return mac_archive.ciphers[0].BlockCipher_encrypt(&context->cc,
                partial,
                ((CBCMACContext *) context->context)->partial);
    }
}


/**
 * Computes an incremental MAC on msgLen bytes of the msg. This call is
 * tied to the initIncrementalMAC call, which must be made first. This call
 * can fail if the msgLen provided exceeds the amount specified earlier or
 * if a block cipher operation fails.
 *
 * @param context   opaque data structure to hold the module specific state
 *                  associated with this invocation of the incremental
 *                  computation.
 * @param msg       the message data to add to the incremental computation.
 * @param msgLen    number of bytes to add for the incremental computation.
 *
 * @return whether the incremental mac computation succeeded or not. It can
 *         fail if more data is provided than the initial initialization
 *         indicated or if the underlying block cipher fails.
 */
int mac_incremental_MAC(MACContext *context, uint8_t *msg, uint16_t msgLen)
{
    uint8_t i, pos = ((CBCMACContext *) context->context)->blockPos;
    uint8_t *partial = ((CBCMACContext *) context->context)->partial;

    // only proceed if the we're expecting less than msgLen of data.
    if (((CBCMACContext *) context->context)->length < msgLen) {
        return 0;
    }

    // simple here: just xor the msg with the partial and when we fill up
    // the partial, encrypt it.
    for (i = 0; i < msgLen; i++) {
        // unroll
        partial[pos++] ^= msg[i];

        if (bigBlocks) {
            if (pos == 15) {
                if (!mac_archive.ciphers[0].BlockCipher_encrypt(&context->cc,
                        partial,
                        partial)) {
                    return 0;
                }

                pos = 0;
            }
        }
        else {
            if (pos == 7) {
                if (!mac_archive.ciphers[0].BlockCipher_encrypt(&context->cc,
                        partial,
                        partial)) {
                    return 0;
                }

                pos = 0;
            }
        }
    }

    ((CBCMACContext *) context->context)->length -= msgLen;
    ((CBCMACContext *) context->context)->blockPos = pos;
    return 1;
}


/**
 * Returns the actual MAC code from an in-progress incremental MAC
 * computation. The initIncrementalMAC and length bytes of data must have
 * been computed using the provided context for this function to succeed.
 * This function may fail if the requested MAC size exceeds the underlying
 * cipher block size, or if the incremental MAC computation has not yet
 * finished.
 *
 * @param context   opaque data structure to hold the module specific state
 *                  associated with this invocation of the incremental
 *                  computation.
 * @param MAC       resulting buffer of at least macSize to hold the
 *                  generated MAC
 * @param macSize   the number of bytes of MAC to generate. This must be
 *                  less than or equal to the underlying blockCipher block
 *                  size.
 *
 * @return whether the command succeeded or not. It can fail if the
 *         underlying block cipher fails or if not all expected data was
 *         received from the initialization function
 */
int mac_get_incremental_MAC(MACContext *context,
                              uint8_t *res,
                              uint8_t macSize)
{
    uint8_t blockPos = ((CBCMACContext *) context->context)->blockPos;
    uint8_t *partial = ((CBCMACContext *) context->context)->partial;

    // make sure they're asking for a valid mac size and that we've received
    // all the data that we're expecting.
    if (bigBlocks) {
        if (!macSize || macSize > 16 ||
            ((CBCMACContext *) context->context)->length) {
            // dbg(DBG_CRYPTO,"MAC getIncrementalMAC failure:
            // length left %d.\n",((CBCMACContext*) context->context)->length);
            return 0;
        }
    }
    else {
        if (!macSize || macSize > 8 ||
            ((CBCMACContext *) context->context)->length) {
            // dbg(DBG_CRYPTO,"MAC getIncrementalMAC failure:
            // length left %d.\n",((CBCMACContext*) context->context)->length);
            return 0;
        }
    }

    // the last block may be a partial block [ie, may have some data that
    // has been xored but not yet encrypted]. if so, encrypt it.
    if (blockPos) {
        // one last encr: xor with 10000
        partial[++blockPos] ^= 1;

        if (!mac_archive.ciphers[0].BlockCipher_encrypt(&context->cc,
                partial,
                partial)) {
            return 0;
        }

        ((CBCMACContext *) context->context)->blockPos = 0;
    }

    memcpy(res, ((CBCMACContext *) context->context)->partial, macSize);
    return 1;
}

/**
 * Computes a non-incremental MAC calculation on the given message. The
 * key from the init() call will be used for the MAC calculation.
 *
 * @param context   opaque data structure to hold the module specific state
 *                  associated with this invocation of the incremental
 *                  computation.
 * @param msg       a buffer of length size on which the MAC will be calculated
 * @param length    the total length of the msg
 * @param res       buffer of at least macSize where the resulting MAC
 *                  calculation will be stored.
 * @param macSzie   the number of bytes of MAC to generate. This must be less
 *                  than or equal to the underlying blockCipher block size.
 *
 * @return   whether the command succeeded or not. It can fail if the
 *           underlying blockCipher fails.
 */
int mac_MAC(MACContext *context,
            uint8_t *msg,
            uint16_t length,
            uint8_t *res,
            uint8_t mac_size)
{

    // we'll just call the incremental primitives that we've built:

    //check if length is at least big-BlockSize
    if (bigBlocks && (length < CBCMAC_BLOCK_SIZE_AES)) {
        uint8_t *orig_msg;
        uint8_t *big_msg = malloc(CBCMAC_BLOCK_SIZE_AES);

        if (big_msg) {
            //copy original content
            memcpy(big_msg, msg, length);

            //pad remaining space with 0-bytes
            uint8_t i;

            for (i = length; i < CBCMAC_BLOCK_SIZE_AES; i++) {
                big_msg[i] = 0;
            }

            //save msg and point msg to bigMsg
            orig_msg = msg;
            msg = big_msg;
            length = CBCMAC_BLOCK_SIZE_AES;
        }
        else {
            printf("%-40s: [ERROR] Could NOT malloc array for padding!\r\n",
                   __FUNCTION__);
            return 0;
        }

        if (mac_init_incremental_MAC(context, length) != 1) {
            return 0;
        }

        if (mac_incremental_MAC(context, msg, length) != 1) {
            return 0;
        }

        uint8_t result;
        result = mac_get_incremental_MAC(context, res, mac_size);

        //restore msg-pointer and free bigMsg
        msg = orig_msg;
        free(big_msg);

        return result;
    }
    /* AES/TWOFISH is not MAC-algorithm and/or length is >=
     * CBCMAC_BLOCK_SIZE_AES
     */
    else {
        if (mac_init_incremental_MAC(context, length) != 1) {
            return 0;
        }

        if (mac_incremental_MAC(context, msg, length) != 1) {
            return 0;
        }

       return  mac_get_incremental_MAC(context, res, mac_size);
    }
}

