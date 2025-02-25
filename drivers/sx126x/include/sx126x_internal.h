/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    drivers_sx126x_internal SX1261/2/8 and LLCC68 internal functions
 * @ingroup     drivers_sx126x
 * @brief       Internal functions for the SX1261/2/8 and LLCC68
 *
 * @{
 *
 * @file
 *
 * @author      José I. Alamos <jose.alamos@haw-hamburg.de>
 */

#include <assert.h>

#include "net/lora.h"
#include "sx126x.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Get the current chip state
 *
 * @param[in]   dev                     Device descriptor of the driver
 *
 * @return  Chip state
 */
sx126x_chip_modes_t sx126x_get_state(const sx126x_t *dev);

/**
 * @brief   Set the internal chip state
 *
 * @param[in]   dev                     Device descriptor of the driver
 * @param[in]   state                   State to set
 */
void sx126x_set_state(sx126x_t *dev, sx126x_chip_modes_t state);

/**
 * @brief   Check whether the device model is sx1261
 *
 * @param[in] dev                      Device descriptor of the driver
 *
 * @retval    true if the device is sx1261
 * @retval    false otherwise
 */
static inline bool sx126x_is_sx1261(sx126x_t *dev)
{
    assert(dev);
    if (SX126X_SINGLE) {
        return IS_USED(MODULE_SX1261);
    }
    else {
        return dev->params->type == SX126X_TYPE_SX1261;
    }
}

/**
 * @brief   Check whether the device model is sx1262
 *
 * @param[in] dev                      Device descriptor of the driver
 *
 * @retval    true if the device is sx1262
 * @retval    false otherwise
 */
static inline bool sx126x_is_sx1262(sx126x_t *dev)
{
    assert(dev);
    if (SX126X_SINGLE) {
        return IS_USED(MODULE_SX1262);
    }
    else {
        return dev->params->type == SX126X_TYPE_SX1262;
    }
}

/**
 * @brief   Check whether the device model is llcc68
 *
 * @param[in] dev                      Device descriptor of the driver
 *
 * @retval    true if the device is llcc68
 * @retval    false otherwise
 */
static inline bool sx126x_is_llcc68(sx126x_t *dev)
{
    assert(dev);
    if (SX126X_SINGLE) {
        return IS_USED(MODULE_LLCC68);
    }
    else {
        return dev->params->type == SX126X_TYPE_LLCC68;
    }
}

/**
 * @brief   Check whether the device model is sx1268
 *
 * @param[in] dev                      Device descriptor of the driver
 *
 * @retval    true if the device is sx1268
 * @retval    false otherwise
 */
static inline bool sx126x_is_sx1268(sx126x_t *dev)
{
    assert(dev);
    if (SX126X_SINGLE) {
        return IS_USED(MODULE_SX1268);
    }
    else {
        return dev->params->type == SX126X_TYPE_SX1268;
    }
}

/**
 * @brief   Check whether the device model is stm32wl (onboard radio)
 *
 * @param[in] dev                      Device descriptor of the driver
 *
 * @retval    true if the device is stm32wl
 * @retval    false otherwise
 */
static inline bool sx126x_is_stm32wl(sx126x_t *dev)
{
    assert(dev);
    if (SX126X_SINGLE) {
        return IS_USED(MODULE_SX126X_STM32WL);
    }
    else {
        return dev->params->type == SX126X_TYPE_STM32WL;
    }
}

/**
 * @brief   Mapping of general LoRA bandwidth in lora.h to sx126x_lora_bw_t
 *
 * @param[in]   lora_bw     LoRa bandwidth LORA_BW_
 *
 * @return  Corresponding sx126x_lora_bw_t value or default value
 */
static inline sx126x_lora_bw_t _sx126x_lora_bw_from(int lora_bw)
{
    switch (lora_bw) {
        case LORA_BW_125_KHZ: return SX126X_LORA_BW_125;
        case LORA_BW_250_KHZ: return SX126X_LORA_BW_250;
        case LORA_BW_500_KHZ: return SX126X_LORA_BW_500;
        default:
            return SX126X_LORA_BW_125;
    }
}

/**
 * @brief   Mapping of sx126x_lora_bw_t to general LoRA bandwidth in lora.h
 *
 * @param[in]   bw      sx126x_lora_bw_t
 *
 * @return  Corresponding LoRa bandwidth
 */
static inline int _sx126x_lora_bw_to(sx126x_lora_bw_t bw)
{
    switch (bw) {
        case SX126X_LORA_BW_125: return LORA_BW_125_KHZ;
        case SX126X_LORA_BW_250: return LORA_BW_250_KHZ;
        case SX126X_LORA_BW_500: return LORA_BW_500_KHZ;
        default:
            return -1;
    }
}

/**
 * @brief   Mapping of general LoRA spreading factor in lora.h to sx126x_lora_sf_t
 *
 * @param[in]   lora_sf     LoRa spreading factor LORA_SF
 *
 * @return  Corresponding sx126x_lora_sf_t value or default value
 */
static inline sx126x_lora_sf_t _sx126x_lora_sf_from(int lora_sf)
{
    switch (lora_sf) {
        case LORA_SF5: return SX126X_LORA_SF5;
        case LORA_SF6: return SX126X_LORA_SF6;
        case LORA_SF7: return SX126X_LORA_SF7;
        case LORA_SF8: return SX126X_LORA_SF8;
        case LORA_SF9: return SX126X_LORA_SF9;
        case LORA_SF10: return SX126X_LORA_SF10;
        case LORA_SF11: return SX126X_LORA_SF11;
        case LORA_SF12: return SX126X_LORA_SF12;
        default:
            return SX126X_LORA_SF7;
    }
}

/**
 * @brief   Mapping of sx126x_lora_sf_t to general LoRA spreading factor in lora.h
 *
 * @param[in]   sf      sx126x_lora_sf_t
 *
 * @return  Corresponding LoRa spreading factor
 */
static inline int _sx126x_lora_sf_to(sx126x_lora_sf_t sf)
{
    switch (sf) {
        case SX126X_LORA_SF5: return LORA_SF5;
        case SX126X_LORA_SF6: return LORA_SF6;
        case SX126X_LORA_SF7: return LORA_SF7;
        case SX126X_LORA_SF8: return LORA_SF8;
        case SX126X_LORA_SF9: return LORA_SF9;
        case SX126X_LORA_SF10: return LORA_SF10;
        case SX126X_LORA_SF11: return LORA_SF11;
        case SX126X_LORA_SF12: return LORA_SF12;
        default:
            return -1;
    }
}

/**
 * @brief   Mapping of general LoRA coding rate in lora.h to sx126x_lora_cr_t
 *
 * @param[in]   lora_cr     LoRa coding rate LORA_CR_
 *
 * @return  Corresponding sx126x_lora_cr_t value or default value
 */
static inline sx126x_lora_cr_t _sx126x_lora_cr_from(int lora_cr)
{
    switch (lora_cr) {
        case LORA_CR_4_5: return SX126X_LORA_CR_4_5;
        case LORA_CR_4_6: return SX126X_LORA_CR_4_6;
        case LORA_CR_4_7: return SX126X_LORA_CR_4_7;
        case LORA_CR_4_8: return SX126X_LORA_CR_4_8;
        default:
            return SX126X_LORA_CR_4_5;
    }
}

/**
 * @brief   Mapping of sx126x_lora_cr_t to general LoRA coding rate in lora.h
 *
 * @param[in]   cr      sx126x_lora_cr_t
 *
 * @return  Corresponding LoRa coding rate
 */
static inline int _sx126x_lora_cr_to(sx126x_lora_cr_t cr)
{
    switch (cr) {
        case SX126X_LORA_CR_4_5: return LORA_CR_4_5;
        case SX126X_LORA_CR_4_6: return LORA_CR_4_6;
        case SX126X_LORA_CR_4_7: return LORA_CR_4_7;
        case SX126X_LORA_CR_4_8: return LORA_CR_4_8;
        default:
            return -1;
    }
}

#ifdef __cplusplus
}
#endif

/** @} */
