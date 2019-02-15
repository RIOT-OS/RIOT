/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_ad7746
 * @{
 *
 * @file
 * @brief       Internal definitions for AD7746 capacitance sensor
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#ifndef AD7746_INTERNAL_H
#define AD7746_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    AD7746 registers
 * @{
 */
#define AD7746_REG_STATUS          (0x00) /**< Status */
#define AD7746_REG_CAP_DATA_H      (0x01) /**< Capacitive channel data High */
#define AD7746_REG_CAP_DATA_M      (0x02) /**< Capacitive channel data Med */
#define AD7746_REG_CAP_DATA_L      (0x03) /**< Capacitive channel data Low */
#define AD7746_REG_VT_DATA_H       (0x04) /**< Voltage/Temp channel data High */
#define AD7746_REG_VT_DATA_M       (0x05) /**< Voltage/Temp channel data Med */
#define AD7746_REG_VT_DATA_L       (0x06) /**< Voltage/Temp channel data Low */
#define AD7746_REG_CAP_SETUP       (0x07) /**< Capacitive channel setup */
#define AD7746_REG_VT_SETUP        (0x08) /**< Voltage/Temp channel setup */
#define AD7746_REG_EXC_SETUP       (0x09) /**< Capacitive channel excitation setup */
#define AD7746_REG_CONFIGURATION   (0x0A) /**< Configuration */
#define AD7746_REG_CAP_DAC_A       (0x0B) /**< Capacitive DAC A setup */
#define AD7746_REG_CAP_DAC_B       (0x0C) /**< Capacitive DAC B setup */
#define AD7746_REG_CAP_OFF_H       (0x0D) /**< Capacitive offset High */
#define AD7746_REG_CAP_OFF_L       (0x0E) /**< Capacitive offset Low */
#define AD7746_REG_CAP_GAIN_H      (0x0F) /**< Capacitive gain High */
#define AD7746_REG_CAP_GAIN_L      (0x10) /**< Capacitive gain Low */
#define AD7746_REG_VOLT_GAIN_H     (0x11) /**< Voltage gain High */
#define AD7746_REG_VOLT_GAIN_L     (0x12) /**< Voltage gain Low */
/** @} */

/**
 * @brief   AD7746 reset command
 */
#define AD7746_RESET_CMD             (0xBF)

/**
 * @name    AD7746 Status register bits
 * @{
 */
#define AD7746_STATUS_EXCERR_BIT         (3) /**< excitation output error */
#define AD7746_STATUS_RDY_BIT            (2) /**< conversion ready */
#define AD7746_STATUS_RDYVT_BIT          (1) /**< voltage/temperature ready */
#define AD7746_STATUS_RDYCAP_BIT         (0) /**< capacitance ready */
/** @} */

/**
 * @name    AD7746 Capacitive channel setup register bits
 * @{
 */
#define AD7746_CAP_SETUP_CAPEN_BIT       (7) /**< capacitive channel enable */
#define AD7746_CAP_SETUP_CIN2_BIT        (6) /**< second capacitive channel */
#define AD7746_CAP_SETUP_CAPDIFF_BIT     (5) /**< differential mode enable*/
#define AD7746_CAP_SETUP_CACHOP_BIT      (0) /**< capacitive channel chopping */
/** @} */

/**
 * @name    AD7746 Voltage/Temperature channel setup register bits
 * @{
 */
#define AD7746_VT_SETUP_VTEN_BIT         (7)
#define AD7746_VT_SETUP_VTMD1_BIT        (6)
#define AD7746_VT_SETUP_VTMD0_BIT        (5)
#define AD7746_VT_SETUP_EXTREF_BIT       (4)
#define AD7746_VT_SETUP_VTSHORT_BIT      (1)
#define AD7746_VT_SETUP_VTCHOP_BIT       (0)
/** @} */

/**
 * @name    AD7746 Capacitive channel excitation setup register bits
 * @{
 */
#define AD7746_EXC_SETUP_CLKCTRL_BIT     (7) /**< Clock control */
#define AD7746_EXC_SETUP_EXCON_BIT       (6) /**< Excitation signal control */
#define AD7746_EXC_SETUP_EXCB_BIT        (5) /**< EXCB pin enable */
#define AD7746_EXC_SETUP_INV_EXCB_BIT    (4) /**< EXCB pin disable */
#define AD7746_EXC_SETUP_EXCA_BIT        (3) /**< EXCA pin enable */
#define AD7746_EXC_SETUP_INV_EXCA_BIT    (2) /**<EXCA pin disable */
#define AD7746_EXC_SETUP_EXCLVL1_BIT     (1) /**< Excitation voltage level 1 */
#define AD7746_EXC_SETUP_EXCLVL0_BIT     (0) /**< Excitation voltage level 0 */
/** @} */

/**
 * @name    AD7746 Configuration register bits
 * @{
 */
#define AD7746_CONFIGURATION_VTF1_BIT    (7)
#define AD7746_CONFIGURATION_VTF0_BIT    (6)
#define AD7746_CONFIGURATION_CAPF2_BIT   (5)
#define AD7746_CONFIGURATION_CAPF1_BIT   (4)
#define AD7746_CONFIGURATION_CAPF0_BIT   (3)
#define AD7746_CONFIGURATION_MD2_BIT     (2)
#define AD7746_CONFIGURATION_MD1_BIT     (1)
#define AD7746_CONFIGURATION_MD0_BIT     (0)
/** @} */

/**
 * @name    AD7746 DAC A register bits
 * @{
 */
#define AD7746_DACAEN_BIT                7
/** @} */

/**
 * @name    AD7746 DAC B register bits
 * @{
 */
#define AD7746_DACBEN_BIT                7
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* AD7746_INTERNAL_H */
/** @} */
