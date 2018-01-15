/*
 * Copyright (C) 2017 TU Berlin DAI Labor
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   driver_cc1200
 * @{
 *
 * @file
 * @brief     cc1200 default settings override
 *
 * By setting either CC1200_DEFAULT_PATABLE or CC1200_DEFAULT_FREQ in board.h,
 * it is possible to override the default pa table or base frequency registers
 * on a per-device basis.
 *
 * @author      Anon Mall <anon.mall@gt-arc.com>
 */
#ifndef CC1200_DEFAULTSETTINGS_H
#define CC1200_DEFAULTSETTINGS_H

#include "board.h"
#include "cc1200-defines.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CC1200_DEFAULT_PATABLE
#define CC1200_DEFAULT_PATABLE cc1200_default_pa_table
extern const char cc1200_default_pa_table[8];
#endif

#ifndef CC1200_DEFAULT_FREQ
#define CC1200_DEFAULT_FREQ cc1200_default_base_freq
extern const char cc1200_default_base_freq[3];
#endif


#define CC1200_PANID_DEFAULT     (IEEE802154_DEFAULT_PANID)

/**
 * @brief   PATABLE with available output powers
 * @note    If changed in size, adjust MAX_OUTPUT_POWER definition
 *          in CC1200 interface
*/
const char cc1200_default_pa_table[8] = {
    0x00,   /*< -52 dBm */
    0x0D,   /*< -20 dBm */
    0x34,   /*< -10 dBm */
    0x57,   /*< - 5 dBm */
    0x8E,   /*<   0 dBm */
    0x85,   /*< + 5 dBm */
    0xCC,   /*< + 7 dBm */
    0xC3    /*< +10 dBm */
};

/**
 * @brief   VCO frequency programming for the CC1200
 * @note    The frequency is given by 2 factors, VCO frequency programming
 *          and the LO divider Programming. Relation is given by
 *          f_RF = (f_VCO) / (LO Divider)
 *          f_VCO = (FREQ/2^16) * f_XOSC + (FREQOFF/2^18)*f_XOSC
 *          FREQ is configured through a 24 bit unsigned frequency word stored
 *          here in cc1200_default_base_freq and has to be written to the FREQ0,
 *          FREQ1 and FREQ2 register
 */
const char cc1200_default_base_freq[3] = { 0xCC, 0xCC, 0x56 };

/**
 * @brief cc1200 default settings
 */
const char cc1200_default_conf[] = {
    0x30, /* IOCFG3 */
    0x06, /* IOCFG2 */
    0x30, /* IOCFG1 */
    0x01, /* IOCFG0 */
    0x6E, /* SYNC3 */
    0x4E, /* SYNC2 */
    0x90, /* SYNC1 */
    0x4E, /* SYNC0 */
    0xE5, /* SYNC CFG1 */
    0x23, /* SYNC CFG0 */
    0x47, /* DEVIATION M */
    0x0B, /* MODCFG DEV E */
    0x56, /* DCFILT CFG */
    0x19, /* PREAMBLE CFG1 */
    0xBA, /* PREAMBLE CFG0 */
    0xC8, /* IQIC */
    0x84, /* CHAN BW */
    0x42, /* MDMCFG1 */
    0x05, /* MDMCFG0 */
    0x94, /* SYMBOL RATE2 */
    0x7A, /* SYMBOL RATE1 */
    0xE1, /* SYMBOL RATE0 */
    0x27, /* AGC REF */
    (uint8_t) -91, /* AGC CS THR */
    (int8_t)CC1200_RF_CFG_RSSI_OFFSET, /* AGC GAIN ADJUST */
    0xB1, /* AGC CFG3 */
    0x20, /* AGC CFG2 */
    0x11, /* AGC CFG1 */
    0x90, /* AGC CFG0 */
    0x84, /* FIFO CFG */
    0x00, /* DEV ADDR */
    0x0B, /* SETTLING CFG */
    0x12, /* FS CFG */
    0x08, /* WOR CFG1 */
    0x21, /* WOR CFG0 */
    0x00, /* WOR EVENT0 MSB */
    0x00, /* WOR EVENT0 LSB */
    0x00, /* RXDCM TIME */
    0x04, /* PKT CFG2 */
    0x05, /* PKT CFG1 */
    0x20, /* PKT CFG0 */
    0x0F, /* RFEND CFG1 */
    0x30, /* RFEND CFG0 */
    0x3F, /* PA CFG1 */
    0x56, /* PA CFG0 */
    0x0F, /* ASK CFG */
    0xFF  /* PKT LEN */
};

/**
 * @brief The size of the configuration array for CC1200 in bytes
 * */
const uint8_t cc1200_default_conf_size = sizeof(cc1200_default_conf);

/**
 * @brief struct for extended register default settings
 * */
typedef struct {
    uint16_t address;
    uint8_t value;
} cc1200_register_setting;

/**
 * @brief default extended register settings
 * */
const cc1200_register_setting cc1200_ext_default_conf[] = {
    {CC1200_IF_MIX_CFG, 0x18},
    {CC1200_TOC_CFG, 0x03},
    {CC1200_MDMCFG2, 0x02},
    {CC1200_FREQ2, 0x56},
    {CC1200_FREQ1, 0xCC},
    {CC1200_FREQ0, 0xCC},
    {CC1200_IF_ADC1, 0xEE},
    {CC1200_IF_ADC0, 0x10},
    {CC1200_FS_DIG1, 0x04},
    {CC1200_FS_DIG0, 0x50},
    {CC1200_FS_CAL1, 0x40},
    {CC1200_FS_CAL0, 0x0E},
    {CC1200_FS_DIVTWO, 0x03},
    {CC1200_FS_DSM0, 0x33},
    {CC1200_FS_DVC1, 0xF7},
    {CC1200_FS_DVC0, 0x0F},
    {CC1200_FS_PFD, 0x00},
    {CC1200_FS_PRE, 0x6E},
    {CC1200_FS_REG_DIV_CML, 0x1C},
    {CC1200_FS_SPARE, 0xAC},
    {CC1200_FS_VCO0, 0xB5},
    {CC1200_IFAMP, 0x05},
    {CC1200_XOSC5, 0x0E},
    {CC1200_XOSC1, 0x03}
};

/**
 * @brief The size of the extended configuration array for CC1200 in bytes
 * */
const uint8_t cc1200_ext_default_conf_size = (sizeof(cc1200_ext_default_conf)/sizeof(cc1200_register_setting));

#ifdef __cplusplus
}
#endif

#endif /* CC1200_DEFAULTSETTINGS_H */
/** @} */
