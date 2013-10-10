/*
 * network_security.h - Encryption of single packets
 *
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * This source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 */

/*
 * @file        packet_encryption.h
 * @brief       Used to encrypt single packets
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Mark Essien <markessien@gmail.com>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * @date        16.09.2013 13:35:21
 */

#ifndef __PACKET_ENCRYPTION_H
#define __PACKET_ENCRYPTION_H

#include "Ciphers.h"
#include "packet_security.h"



/**
 * Returns the packet-encryption component
 *
 * @return the packet-encryption component
 */

packet_security_component_t *packet_encryption_get_component(void);

/**
 * Inits the packet-encryption component
 *
 * @return The result of the process
 */
int packet_encryption_init_component(void);

/**
 * Encrypts the packet that is passed in
 *
 * @param packet  the packet to be encrypted
 *
 * @return The result of the process
 */
int packet_encryption_secure_packet(secure_packet_t *packet,
                                    uint16_t phy_src);

/**
 * Decrypts the packet that is passed in
 *
 * @param packet the packet to be decrypted
 *
 * @return The result of the process as a
 */
int packet_encryption_desecure_packet(secure_packet_t *packet,
                                      uint16_t phy_src);

/**
 * Disables or enables this component
 *
 * @param val If it should be enabled or not
 *
 * @return The result of the process as a
 */
void packet_encryption_enable(int val);

int block_cipher_init(CipherContext *context, uint8_t blockSize,
                      uint8_t keySize, uint8_t *key, uint8_t cipher_index);

uint8_t block_cipher_info_get_preferred_block_size(uint8_t cipher_index);

#endif
