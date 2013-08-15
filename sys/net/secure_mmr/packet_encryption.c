/*
 * packet_encryption.c - Encryption of single packets
 *
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * This source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 */

/*
 * @file        packet_encryption.c
 * @brief       Used to encrypt single packets.
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Mark Essien <markessien@gmail.com>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * @date        13.09.2013 13:31:49
 */

/*---------------------------------------------------------------------------*/
//                                      Included Files
/*---------------------------------------------------------------------------*/

/*  Radio includes */
#include "drivers/cc110x/cc1100.h"
#include "radio/radio.h"
#include "cc1100_phy.h"

/* Package includes */
#include "secure_routing_globals.h"
#include "packet_security.h"
#include "packet_encryption.h"
#include "microeval/microeval.h"
#include "network_nodes.h"

/* Encryption Modules */
#include "Ciphers.h"
#include "CBCMode.h"
#include "SkipJack.h"
#include "RC5.h"
#include "AES.h"
#include "TwoFish.h"
#include "3DES.h"

/*---------------------------------------------------------------------------*/
//                                      Variables
/*---------------------------------------------------------------------------*/
// Archive of all available cipher-algorithms
BlockCipher_Archive_t archive;
// The context for encrypt/decrypt ops
static CipherModeContext cipherModeContext;
// Interface for skipjack
extern block_cipher_interface_t skipJackInterface;
// Interface for RC5
extern block_cipher_interface_t RC5Interface;
// Interface for AES
extern block_cipher_interface_t AESInterface;
// Interface for Two Fish
extern block_cipher_interface_t TWOFISHInterface;
// Interface for 3DES
extern block_cipher_interface_t ThreeDESInterface;
// Tells if this node is enabled
int enable_encryption   = 1;

/**
 * @brief the packet-encryption component interface
 */
packet_security_component_t encryption_component = {
    "Encryption",
    packet_encryption_init_component,
    packet_encryption_secure_packet,
    0,
    packet_encryption_desecure_packet
};

/**
 * @brief Gets the packet-encryption component. See header file
 *        for extended description
 *
 */
packet_security_component_t *packet_encryption_get_component()
{
    return &encryption_component;
}

/**
 * @brief Init the packet-encryption component. See header file
 *        for extended description
 */
int packet_encryption_init_component()
{
    ulog("> Initing the PacketEncryption component");

    archive.NoCiphers = 0;

#ifdef SKIPJACK
    ulog("!> Inited skipjack encryption");
    archive.ciphers[archive.NoCiphers] = skipJackInterface;
    archive.NoCiphers += 1;
#endif
#ifdef RC5
    ulog("!> Inited RC5 encryption");
    archive.ciphers[archive.NoCiphers] = RC5Interface;
    archive.NoCiphers += 1;
#endif
#ifdef AES
    ulog("!> Inited AES encryption");
    archive.ciphers[archive.NoCiphers] = AESInterface;
    archive.NoCiphers += 1;
#endif
#ifdef TWOFISH
    ulog("!> Inited twofish encryption");
    archive.ciphers[archive.NoCiphers] = TWOFISHInterface;
    archive.NoCiphers += 1;
#endif
#ifdef THREEDES
    ulog("!> Inited 3des encryption");
    archive.ciphers[archive.NoCiphers] = ThreeDESInterface;
    archive.NoCiphers += 1;
#endif

    //zeroize the standard cipher-mode-context
    ulog_info("Zeroing the ciphermodecontext");

    memset(&cipherModeContext, 0, sizeof(cipherModeContext));

    //init the standard cipher-mode-context with the standard encryption key
    int res = block_cipher_mode_init(&cipherModeContext,
                                   SECURE_ROUTING_KEYSIZE,
                                   global_key);

    if (res <= 0) {
        ulog_error("BlockCipherMode_init failed with result %d", res);
        return res;
    }

    char sz[21] = {'\0'};
    memcpy(sz, global_key, 20);

    for (int i = 0; i < sizeof(global_key); i++) if (sz[i] < 48 ||
                sz[i] > 122) {
            sz[i] = '?';
        }

    ulog("> Packet Encryption inited. ");

    return PacketSecurity_OK;
}

/**
 * @brief Creates the initialisation vector for encrypting or decrypting a
 *        secure packet
 *
 *      @param packet The secure packet to be encrypted or decrypted
 *      @param buffer The buffer for the initialisation vector
 *      @param rand_bits Random bits used to initialise
 *
 */
void make_initvector(secure_packet_t *packet, uint8_t *buffer,
                     uint8_t rand_bits)
{

    ulog_info("> Initing vector. Rand=%d. Flags=%d", rand_bits, packet->flags);

    uint8_t sequence_nmbrA = (uint8_t)packet->seq_nmbr;
    uint8_t sequence_nmbrB = (uint8_t)(packet->seq_nmbr >> 8);
    uint8_t sequence_nmbrC = (uint8_t)(packet->seq_nmbr >> 16);

    buffer[0] = sequence_nmbrA ^ rand_bits;
    buffer[1] = sequence_nmbrB ^ sequence_nmbrC ^ rand_bits;
    buffer[2] = packet->payload_len;
    buffer[3] = packet->destination;
    buffer[4] = packet->source;
    buffer[5] = sequence_nmbrC;
    buffer[6] = sequence_nmbrB;
    buffer[7] = sequence_nmbrA;

}

/**
 * @brief Secures the packet by encrypting it. Examines the flag
 *        to see how exactly it should be encrypted. See header
 *        file for extended description
 *
 */
int packet_encryption_secure_packet(secure_packet_t *packet, uint16_t phy_src)
{
    if (enable_encryption == 0) {
        return PacketSecurity_Disabled;
    }

    if (GET_SECURITY_MODE(packet->flags) !=
        SecurityMode_Encrypt &&
        GET_SECURITY_MODE(packet->flags) !=
        SecurityMode_EncryptAndMAC &&
        GET_SECURITY_MODE(packet->flags) !=
        SecurityMode_EncryptAndMACPriorKey &&
        GET_SECURITY_MODE(packet->flags) !=
        SecurityMode_EncryptAndMACPairwise &&
        GET_SECURITY_MODE(packet->flags) !=
        SecurityMode_EncryptAndMACPairwiseBroadcast) {
        ulog("> Packet-Encryption not required");
        return PacketSecurity_OK; // Do not need to decrypt
    }

    ulog("> Encrypting packet with AES...");

    //zeroize the standard cipher-mode-context
    ulog_info("Zeroing the ciphermodecontext");

    uint8_t original_destination = packet->destination;
    uint8_t *key = global_key;

    if (GET_SECURITY_MODE(packet->flags) ==
        SecurityMode_EncryptAndMACPriorKey) {
        ulog("> Using previous global key to encrypt packet");
        key = last_global_key;
    }
    else if (GET_SECURITY_MODE(packet->flags) ==
             SecurityMode_EncryptAndMACPairwise ||
             GET_SECURITY_MODE(packet->flags) ==
             SecurityMode_EncryptAndMACPairwiseBroadcast) {
        ulog("> In pairwise encryption mode. Destination=%d",
             packet->destination);
        network_node_t *node = network_nodes_find(packet->destination);

        if (node == 0) {
            ulog_error("Node not found. Pairwise encryption to non-neighbours \
            		   not yet supported");
            return PacketSecurity_NotAvailable;
        }

        ulog("> Found pairwise neighbour");

        if (node->is_neighbour == 0) {
            node = network_nodes_find(node->gateway);

            if (node == 0 || node->is_neighbour == 0) {
                ulog_error("Pairwise encryption not possible. Gateway not \
                		    found");
                return PacketSecurity_NotAvailable;
            }

            ulog("Found gateway neighbour for pw encryption");
        }


        // at this point, it's a neighbour and we have the node
        key = node->pairwise_key;

        if (GET_SECURITY_MODE(packet->flags) ==
            SecurityMode_EncryptAndMACPairwiseBroadcast) {
            packet->destination = 0;
        }
    }

    memset(&cipherModeContext, 0, sizeof(cipherModeContext));


    //init the standard cipher-mode-context with the standard encryption key
    int res = block_cipher_mode_init(&cipherModeContext,
                                   SECURE_ROUTING_KEYSIZE,
                                   key);

    if (res <= 0) {
        ulog_error("BlockCipherMode_init failed with result %d", res);
        return PacketSecurity_EncryptionError;
    }

    // Consider changing the parameter of the function here to be the packet
    ulog_info("Creating initVector");

    uint8_t random = rand() % 15;
    random = random & 15; // Zero the top 4 bits

    ulog_info("Encrypting with random %d", random);

    uint8_t init_vector[8];
    make_initvector(packet, init_vector, random);

    uint8_t flags = packet->flags & 15; // Should clear the top 4 bits
    packet->flags = random << 4 | flags;

    uint8_t packetcopy[MAX_PAYLOAD_LEN];
    memcpy(packetcopy, packet->data, MAX_PAYLOAD_LEN);

    int payload_len = packet->payload_len;

    if (payload_len < 16) {
        payload_len = 16;
    }

    //encrypt payload
    ulog_info("Encrypting payload. Payload Length is %d. Packet Length is %d",
              packet->payload_len, sizeof(secure_packet_t));
    res = block_cipher_mode_encrypt(&cipherModeContext,
                                  packetcopy,
                                  packet->data,
                                  payload_len,
                                  init_vector);

    if (res <= 0) {     //if error occurred
        ulog_error("Encryption failed code = %i", res);
        return PacketSecurity_EncryptionError;
    }

    // In case it was changed for pair-wise broadcast mode, set it back
    packet->destination = original_destination;

    return PacketSecurity_OK;
}

/**
 * @brief Decrypts the passed in packet. See header file
 *                      for extended description
 */
int packet_encryption_desecure_packet(secure_packet_t *packet,
                                      uint16_t phy_src)
{
    if (enable_encryption == 0) {
        return PacketSecurity_Disabled;
    }

    if (GET_SECURITY_MODE(packet->flags) !=
        SecurityMode_Encrypt &&
        GET_SECURITY_MODE(packet->flags) !=
        SecurityMode_EncryptAndMAC &&
        GET_SECURITY_MODE(packet->flags) !=
        SecurityMode_EncryptAndMACPriorKey &&
        GET_SECURITY_MODE(packet->flags) !=
        SecurityMode_EncryptAndMACPairwise &&
        GET_SECURITY_MODE(packet->flags) !=
        SecurityMode_EncryptAndMACPairwiseBroadcast) {
        ulog("> Packet that does not need to be decrypted arrived");
        return PacketSecurity_OK; // Do not need to decrypt
    }


    uint8_t *key = global_key;

    if (GET_SECURITY_MODE(packet->flags) ==
        SecurityMode_EncryptAndMACPairwise ||
        GET_SECURITY_MODE(packet->flags) ==
        SecurityMode_EncryptAndMACPairwiseBroadcast) {
        ulog("> In pairwise decryption mode");

        network_node_t *node = network_nodes_find(phy_src);

        if (node == 0) {
            ulog_error("Node %d not known. PW decryption to non-neighbours \
            		    not supported", phy_src);
            network_nodes_print_nodes(network_nodes_get_root(), 1);
            return PacketSecurity_NotAvailable;
        }

        if (node->is_neighbour == 0) {
            ulog_error("Pairwise key verification can only be done for \
            		    neighbours. %d not neighbour", node->node_id);
            return PacketSecurity_NotAvailable;
        }

        // at this point, it's a neighbour and we have the node
        key = node->pairwise_key;
    }

    memset(&cipherModeContext, 0, sizeof(cipherModeContext));

    // init the standard cipher-mode-context with the standard encryption key
    int res = block_cipher_mode_init(&cipherModeContext,
                                   SECURE_ROUTING_KEYSIZE,
                                   key);

    if (res <= 0) {
        ulog_error("BlockCipherMode_init failed with result %d", res);
        return PacketSecurity_Error;
    }

    uint8_t plaintext[MAX_PAYLOAD_LEN];
    memset(plaintext, 0, MAX_PAYLOAD_LEN);

    uint8_t random = packet->flags & 240; // Should clear the bottom 4 bits
    random = random >> 4; // move bits into the bottom 4

    ulog_info("> RandomBits = %d", random);

    /* Create initialisation vector using the random bits stores in the
     * seq clear id
     */
    uint8_t initVector[8];
    make_initvector(packet, initVector, random);

    int payload_len = packet->payload_len;

    if (payload_len < 16) {
        payload_len = 16;
    }

    res = block_cipher_mode_decrypt(&cipherModeContext,
                                  (uint8_t *)packet->data,
                                  plaintext,
                                  payload_len,
                                  (uint8_t *)initVector);

    if (!res) {
        ulog_error("Packet decryption failure!");
        return PacketSecurity_Error;
    }
    else {
        ulog_info("Decryption was successful");
    }

    memcpy(packet->data, plaintext, packet->payload_len);
    return PacketSecurity_OK;
}

/**
 * @brief Disables or enables this component
 *
 */
void packet_encryption_enable(int val)
{
    enable_encryption = val;

    if (val == 0) {
        ulog("> Packet encryption disabled");
    }
}

/**
 * @brief Initialised the specified block-cipher
 *
 * @param context The encryption context
 * @param blockSize The size each block for this encryption
 * @param keySize
 * @param key
 * @param cipher_index
 *
 * @return 1 if it succeeded, otherwise -1 (used by encryption modules)
 */
int block_cipher_init(CipherContext *context,
                     uint8_t blockSize,
                     uint8_t keySize,
                     uint8_t *key,
                     uint8_t cipher_index)
{
    ulog_info("BlockCipher_init called");
    int res;

    if (archive.NoCiphers <= 0) {
        ulog_error("No ciphers detected.");
        return -1; // NoCiphers
    }
    else {
        ulog_info("Calling init on the particular cipher");
        res = archive.ciphers[cipher_index].BlockCipher_init(context,
        		                                             blockSize,
        		                                             keySize,
        		                                             key);
    }

    if (res) {
        ulog_info("Particular cipher init succeeded");
        return 1; // TRUE
    }
    else {
        ulog_error("Particular cipher init failed");
        return res;
    }
}

/**
 * @brief Gets the preferred block size for the specified block-cipher
 *
 * @param cipher_index The index of the cipher in the archives structure
 *
 * @return The preferred block size
 */
uint8_t block_cipher_info_get_preferred_block_size(uint8_t cipher_index)
{
    uint8_t res;
    res = archive.ciphers[cipher_index].BlockCipherInfo_getPreferredBlockSize(
    		                                                                 );
    return res;
}
