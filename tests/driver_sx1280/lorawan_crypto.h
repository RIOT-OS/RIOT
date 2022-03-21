/*
 * Copyright (C) 2020-2022 Université Grenoble Alpes
 */

/*
 * Utils for the MIC control
 *
 * @Remark: the MIC is similar to the LoRaWAN frames MIC (which is based on CMAC).
 *
 */

#ifndef LORA_CRYPTO_H
#define LORA_CRYPTO_H

#include <inttypes.h>
#include <stdbool.h>

#define CMAC_KEY_LEN                                (16)

/*!
 * @brief Computes the LoRaMAC frame MIC field
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
                             uint32_t address, uint8_t dir, uint32_t sequenceCounter,
                             uint32_t *mic);

#endif
