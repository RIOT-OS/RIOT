/*
 * Copyright (C) 2018 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_cc110x
 * @{
 *
 * @file
 * @brief       Default settings of the TI CC1100/CC1101 transceiver
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */


#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The size of the configuration array for CC110X in bytes
 */
#define CC110X_CONF_SIZE 41

/**
 * @brief   Length of the PATABLE
 */
#define CC110X_PATABLE_LEN 8

/**
 * @brief   Configuration register values for CC1100/CC1101 transceivers
 */
extern const uint8_t cc110x_conf[CC110X_CONF_SIZE];

/**
 * @brief   Magic numbers to write to the TEST2, TEST1 and TEST0 configuration
 *          registers
 *
 * Those registers are poorly documented. The data sheet refers to SmartRF
 * Studio to obtain the correct magic numbers. Those values were identical
 * for the 433MHz, 868MHz and 915MHz band at GFSK, 250kBaud and 127kHz
 * deviation. Maybe by luck? So when adding a new base band or adjusting the
 * transceiver configuration, those numbers should be checked again with the
 * SmartRF Studio
 */
extern const uint8_t cc110x_magic_registers[3];

/**
 * @name    Configuration data that specify the 8 available output power levels
 *
 * Source: Table 39 in the data sheet on page 60.
 *
 * | Output Power [dBm] | Setting 433 MHz | Setting 868 MHz | Setting 915 MHz |
 * |--------------------|-----------------|-----------------|-----------------|
 * |                -30 |            0x12 |            0x03 |            0x03 |
 * |                -20 |            0x0E |            0x0F |            0x0E |
 * |                -15 |            0x1D |            0x1E |            0x1E |
 * |                -10 |            0x34 |            0x27 |            0x27 |
 * |                  0 |            0x60 |            0x50 |            0x8E |
 * |                  5 |            0x84 |            0x81 |            0xCD |
 * |                  7 |            0xC8 |            0xCB |            0xC7 |
 * |                 10 |            0xC0 |            0xC2 |            0xC0 |
 *
 * @{
 */
/**
 * @brief   PATABLE values for the 433 MHz band
 *
 * Source: Table 39 in the data sheet on page 60.
 */
extern const cc110x_patable_t cc110x_patable_433mhz;
/**
 * @brief   PATABLE values for the 868 MHz band
 *
 * @note    This is the default PATABLE. There is no need to upload this table
 *          unless switching between different PATABLEs is required.
 *
 * Source: Table 39 in the data sheet on page 60.
 */
extern const cc110x_patable_t cc110x_patable_868mhz;
/**
 * @brief   PATABLE values for the 915 MHz band
 *
 * Source: Table 39 in the data sheet on page 60.
 */
extern const cc110x_patable_t cc110x_patable_915mhz;
/** @} */

/**
 * @name    Channel mappings for CC110x transceivers
 *
 * @{
 */
/**
 * @brief   Channel map for 50 kHz channels with a distance of 200 kHz in the
 *          433 MHz band
 *
 * This channel mapping assumes @ref cc110x_config_433mhz_38kbps_50khz is
 * used as configuration. @ref cc110x_chanmap_433mhz_50khz_alt is an alternative
 * channel map that does not overlap with this map. (This means devices with
 * the alternative channel map cannot communicate with devices using this one,
 * but also no interference is expected.)
 */
extern const cc110x_chanmap_t cc110x_chanmap_433mhz_50khz;
/**
 * @brief   Alternative channel map for 50 kHz channels with a distance of 200 kHz
 *          in the 433 MHz band
 * @see     cc110x_chanmap_433mhz_50khz
 *
 * This channel mapping assumes @ref cc110x_config_433mhz_38kbps_50khz is
 * used as configuration.
 */
extern const cc110x_chanmap_t cc110x_chanmap_433mhz_50khz_alt;
/**
 * @brief   Channel map for 5 non-overlapping 300 kHz channels in the 433 MHz band
 *
 * This channel mapping assumes @ref cc110x_config_433mhz_250kbps_300khz is
 * used as configuration. (Note: The distance between channels is in fact
 * 350 kHz instead of 300. This increased distance allows to cover the full
 * license free range and reduce the possibility of interference of adjacent
 * channels. Reducing the distance to 300kHz would still allow only 5 channels.)
 */
extern const cc110x_chanmap_t cc110x_chanmap_433mhz_300khz;
/**
 * @brief   Channel map for LoRa 868MHz channels 10 to 17 (available as 0 to 7).
 *
 * This channel mapping assumes @ref cc110x_config_868mhz_250kbps_300khz is used
 * as configuration.
 */
extern const cc110x_chanmap_t cc110x_chanmap_868mhz_lora;
/** @} */

/**
 * @name    Base frequency, channel bandwidth and data rate configurations for
 *          CC110x transceivers
 *
 * @{
 */
/**
 * @brief   CC110x configuration: 433MHz band, 38.4kbps data rate, 50 kHz
 *          channel width
 */
extern const cc110x_config_t cc110x_config_433mhz_38kbps_50khz;
/**
 * @brief   CC110x configuration: 433MHz band, 250kbps data rate, 300 kHz
 *          channel width
 */
extern const cc110x_config_t cc110x_config_433mhz_250kbps_300khz;
/**
 * @brief   CC110x configuration: 868MHz band, 250kbps data rate, 300 kHz
 *          channel width
 */
extern const cc110x_config_t cc110x_config_868mhz_250kbps_300khz;
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
