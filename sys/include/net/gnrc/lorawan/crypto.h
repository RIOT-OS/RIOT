/*
 * Copyright (C) 2017 Fundación Inria Chile
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_lorawan   Definition of GNRC lorawan crypto functions.
 * @ingroup     net_gnrc
 * @brief       Provides definitions of lorawan crypto functions. Based on the original Semtech LoRaMAC implementation.
 * @see         https://github.com/Lora-net/LoRaMac-node
 *              @ref net_gnrc
 *
 * @{
 *
 * @file
 * @brief   GNRC Lorawan crypto specific function definitions
 *
 * @author  Francisco Molina <femolina@uc.cl>
 * @author  José Ignacio Alamos <jose.alamos@inria.cl>
 */

#ifndef __LORAMAC_CRYPTO_H__
#define __LORAMAC_CRYPTO_H__
#include <inttypes.h>

/*!
 * Computes the LoRaMAC frame MIC field
 *
 * \param [IN]  buffer          - Data buffer
 * \param [IN]  size            - Data buffer size
 * \param [IN]  key             - AES key to be used
 * \param [IN]  address         - Frame address
 * \param [IN]  dir             - Frame direction [0: uplink, 1: downlink]
 * \param [IN]  sequenceCounter - Frame sequence counter
 * \param [OUT] mic             - Computed MIC field
 */
uint32_t LoRaMacComputeMic( const uint8_t *buffer, uint16_t size, const uint8_t *key, uint32_t address, uint8_t dir, uint32_t sequenceCounter);

/*!
 * Computes the LoRaMAC payload encryption
 *
 * \param [IN]  buffer          - Data buffer
 * \param [IN]  size            - Data buffer size
 * \param [IN]  key             - AES key to be used
 * \param [IN]  address         - Frame address
 * \param [IN]  dir             - Frame direction [0: uplink, 1: downlink]
 * \param [IN]  sequenceCounter - Frame sequence counter
 * \param [OUT] encBuffer       - Encrypted buffer
 */
void LoRaMacPayloadEncrypt( const uint8_t *buffer, uint16_t size, const uint8_t *key, uint32_t address, uint8_t dir, uint32_t sequenceCounter, uint8_t *encBuffer );

/*!
 * Computes the LoRaMAC payload decryption
 *
 * \param [IN]  buffer          - Data buffer
 * \param [IN]  size            - Data buffer size
 * \param [IN]  key             - AES key to be used
 * \param [IN]  address         - Frame address
 * \param [IN]  dir             - Frame direction [0: uplink, 1: downlink]
 * \param [IN]  sequenceCounter - Frame sequence counter
 * \param [OUT] decBuffer       - Decrypted buffer
 */
void LoRaMacPayloadDecrypt( const uint8_t *buffer, uint16_t size, const uint8_t *key, uint32_t address, uint8_t dir, uint32_t sequenceCounter, uint8_t *decBuffer );

/*!
 * Computes the LoRaMAC Join Request frame MIC field
 *
 * \param [IN]  buffer          - Data buffer
 * \param [IN]  size            - Data buffer size
 * \param [IN]  key             - AES key to be used
 * \param [OUT] mic             - Computed MIC field
 */
void LoRaMacJoinComputeMic( const uint8_t *buffer, uint16_t size, const uint8_t *key, uint32_t *mic );

/*!
 * Computes the LoRaMAC join frame decryption
 *
 * \param [IN]  buffer          - Data buffer
 * \param [IN]  size            - Data buffer size
 * \param [IN]  key             - AES key to be used
 * \param [OUT] decBuffer       - Decrypted buffer
 */
void LoRaMacJoinDecrypt( const uint8_t *buffer, uint16_t size, const uint8_t *key, uint8_t *decBuffer );

/*!
 * Computes the LoRaMAC join frame decryption
 *
 * \param [IN]  key             - AES key to be used
 * \param [IN]  appNonce        - Application nonce
 * \param [IN]  devNonce        - Device nonce
 * \param [OUT] nwkSKey         - Network session key
 * \param [OUT] appSKey         - Application session key
 */
void LoRaMacJoinComputeSKeys( const uint8_t *key, const uint8_t *appNonce, uint16_t devNonce, uint8_t *nwkSKey, uint8_t *appSKey );

/*! \} defgroup LORAMAC */

#endif // __LORAMAC_CRYPTO_H__
