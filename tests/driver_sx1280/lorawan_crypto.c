/*
 * Copyright (C) 2020-2022 Université Grenoble Alpes
 */

/*
 * Utils for the MIC control
 */

#include <stdlib.h>
#include <string.h>

#include "hashes/cmac.h"

#include "lorawan_crypto.h"

static cmac_context_t CmacContext;

static uint8_t digest[CMAC_KEY_LEN];

/*!
 * CMAC/AES Message Integrity Code (MIC) Block B0 size
 */
#define LORAMAC_MIC_BLOCK_B0_SIZE                   16

/*!
 * MIC field computation initial data
 */
static uint8_t MicBlockB0[] = { 0x49, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };


/*!
 * \brief Computes the LoRaMAC frame MIC field (from https://github.com/Lora-net/LoRaMac-node)
 *
 * \param [IN]  buffer          Data buffer
 * \param [IN]  size            Data buffer size
 * \param [IN]  key             AES key to be used
 * \param [IN]  address         Frame address
 * \param [IN]  dir             Frame direction [0: uplink, 1: downlink]
 * \param [IN]  sequenceCounter Frame sequence counter
 * \param [OUT] mic Computed MIC field
 */
static void LoRaMacComputeMic( const uint8_t *buffer, uint16_t size, const uint8_t *key,
                               uint32_t address, uint8_t dir, uint32_t sequenceCounter,
                               uint32_t *mic )
{
    MicBlockB0[0] = 0x49;
    MicBlockB0[1] = 0;
    MicBlockB0[2] = 0;
    MicBlockB0[3] = 0;
    MicBlockB0[4] = 0;

    MicBlockB0[5] = dir;

    MicBlockB0[6] = (address) & 0xFF;
    MicBlockB0[7] = (address >> 8) & 0xFF;
    MicBlockB0[8] = (address >> 16) & 0xFF;
    MicBlockB0[9] = (address >> 24) & 0xFF;

    MicBlockB0[10] = (sequenceCounter) & 0xFF;
    MicBlockB0[11] = (sequenceCounter >> 8) & 0xFF;
    MicBlockB0[12] = (sequenceCounter >> 16) & 0xFF;
    MicBlockB0[13] = (sequenceCounter >> 24) & 0xFF;

    MicBlockB0[14] = 0;

    MicBlockB0[15] = size & 0xFF;

    cmac_init(&CmacContext, key, CMAC_KEY_LEN);

    cmac_update(&CmacContext, MicBlockB0, LORAMAC_MIC_BLOCK_B0_SIZE);

    cmac_update(&CmacContext, buffer, size & 0xFF);

    cmac_final(&CmacContext, digest); // TODO reuse MicBlockB0

    *mic =
        ( uint32_t )(( uint32_t )digest[3] << 24 | ( uint32_t )digest[2] <<
                     16 | ( uint32_t )digest[1] << 8 | ( uint32_t )digest[0]);
}


/*!
 * \brief Computes the LoRaMAC frame MIC field (from https://github.com/Lora-net/LoRaMac-node)
 *
 * \param [IN]  buffer          Data buffer
 * \param [IN]  size            Data buffer size
 * \param [IN]  key             AES key to be used
 * \param [IN]  address         Frame address
 * \param [IN]  dir             Frame direction [0: uplink, 1: downlink]
 * \param [IN]  sequenceCounter Frame sequence counter
 * \param [OUT] mic Computed MIC field
 */
void lorawan_cmac_calculate_mic(const uint8_t *buffer, uint16_t size, const uint8_t *key,
                             uint32_t address, uint8_t dir, uint32_t sequenceCounter, uint32_t *mic)
{
    LoRaMacComputeMic(buffer, size, key, address, dir, sequenceCounter, mic);
}
