/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_uwb_core
 * @{
 *
 * @file
 * @brief       uwb-core uwbcfg module configurations
 *              taken from decawave-uwb-core/sys/uwbcfg/syscfg.yml
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#ifndef DPL_SYSCFG_SYSCFG_UWBCFG_H
#define DPL_SYSCFG_SYSCFG_UWBCFG_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Enable uwbcfg module
 */
#ifndef MYNEWT_VAL_UWBCFG_ENABLED
#define MYNEWT_VAL_UWBCFG_ENABLED (IS_ACTIVE(MODULE_UWB_CORE_UWBCFG))
#endif

/**
 * @brief   Apply configuration on uwbcfg module setup
 */
#ifndef MYNEWT_VAL_UWBCFG_APPLY_AT_INIT
#define MYNEWT_VAL_UWBCFG_APPLY_AT_INIT (1)
#endif

/**
 * @brief   Default channel
 */
#ifndef MYNEWT_VAL_UWBCFG_DEF_CH
#define MYNEWT_VAL_UWBCFG_DEF_CH ("5")
#endif

/**
 * @brief   Default UWB PRF (MHz)
 */
#ifndef MYNEWT_VAL_UWBCFG_DEF_PRF
#define MYNEWT_VAL_UWBCFG_DEF_PRF ("64")
#endif

/**
 * @brief   Default UWB Datarate (110k, 850k, 6m8)
 */
#ifndef MYNEWT_VAL_UWBCFG_DEF_DATARATE
#define MYNEWT_VAL_UWBCFG_DEF_DATARATE ("6m8")
#endif

/**
 * @brief   Default UWB PAC Length (8, 16, 32, 64)
 */
#ifndef MYNEWT_VAL_UWBCFG_DEF_PACLEN
#define MYNEWT_VAL_UWBCFG_DEF_PACLEN ("8")
#endif

/**
 * @brief   Default UWB External clock delay
 */
#ifndef MYNEWT_VAL_UWBCFG_DEF_EXT_CLKDLY
#define MYNEWT_VAL_UWBCFG_DEF_EXT_CLKDLY ("0")
#endif

/**
 * @brief   Default MAC FrameFilter (0x0000 = no filter)
 */
#ifndef MYNEWT_VAL_UWBCFG_DEF_FRAME_FILTER
#define MYNEWT_VAL_UWBCFG_DEF_FRAME_FILTER ("0x0000")
#endif

/**
 * @brief   Default UWB Role
 *
 *  - Tag          "0x00"
 *  - Node         "0x01"
 *  - Pan master   "0x02"
 *  - Anchor       "0x04"
 *  - Panmaster    "0x07"
 */
#ifndef MYNEWT_VAL_UWBCFG_DEF_ROLE
#define MYNEWT_VAL_UWBCFG_DEF_ROLE ("0x0")
#endif

/**
 * @brief Default UWB RX Antenna delay
 */
#ifndef MYNEWT_VAL_UWBCFG_DEF_RX_ANTDLY
#define MYNEWT_VAL_UWBCFG_DEF_RX_ANTDLY ("0x4050")
#endif

/**
 * @brief Default UWB RX Antenna separation distance in m
 */
#ifndef MYNEWT_VAL_UWBCFG_DEF_RX_ANTSEP
#define MYNEWT_VAL_UWBCFG_DEF_RX_ANTSEP ("0.0205")
#endif

/**
 * @brief UWBCFG_DEF_RX_DIAG_EN
 */
#ifndef MYNEWT_VAL_UWBCFG_DEF_RX_DIAG_EN
#define MYNEWT_VAL_UWBCFG_DEF_RX_DIAG_EN ("0x1")
#endif

/**
 * @brief Default UWB PDOA Mode (0, 1, 2, 3)
 */
#ifndef MYNEWT_VAL_UWBCFG_DEF_RX_PDOA_MODE
#define MYNEWT_VAL_UWBCFG_DEF_RX_PDOA_MODE ("0")
#endif

/**
 * @brief Default UWB PHR Mode (s, e)
 */
#ifndef MYNEWT_VAL_UWBCFG_DEF_RX_PHR_MODE
#define MYNEWT_VAL_UWBCFG_DEF_RX_PHR_MODE ("e")
#endif

/**
 * @brief Default UWB PHR Rate (0 = std, 1 = data)
 */
#ifndef MYNEWT_VAL_UWBCFG_DEF_RX_PHR_RATE
#define MYNEWT_VAL_UWBCFG_DEF_RX_PHR_RATE ("0")
#endif

/**
 * @brief Default UWB RX Preamble Code Index
 */
#ifndef MYNEWT_VAL_UWBCFG_DEF_RX_PREAM_CIDX
#define MYNEWT_VAL_UWBCFG_DEF_RX_PREAM_CIDX ("9")
#endif

/**
 * @brief Default UWB SFD Timeout (-1=auto, timeout in symbols)
 */
#ifndef MYNEWT_VAL_UWBCFG_DEF_RX_SFD_TO
#define MYNEWT_VAL_UWBCFG_DEF_RX_SFD_TO ("-1")
#endif

/**
 * @brief Default UWB SFD Type (0, 1)
 */
#ifndef MYNEWT_VAL_UWBCFG_DEF_RX_SFD_TYPE
#define MYNEWT_VAL_UWBCFG_DEF_RX_SFD_TYPE ("1")
#endif

/**
 * @brief Default UWB Sts Length (32-2040 in steps of 8)
 */
#ifndef MYNEWT_VAL_UWBCFG_DEF_RX_STS_LEN
#define MYNEWT_VAL_UWBCFG_DEF_RX_STS_LEN ("64")
#endif

/**
 * @brief Default UWB Sts Mode (0, 1, 2, sdc, 4z)
 */
#ifndef MYNEWT_VAL_UWBCFG_DEF_RX_STS_MODE
#define MYNEWT_VAL_UWBCFG_DEF_RX_STS_MODE ("0")
#endif

/**
 * @brief Default UWB Coarse TX Power (0,3,6,..,18)
 */
#ifndef MYNEWT_VAL_UWBCFG_DEF_TXRF_POWER_COARSE
#define MYNEWT_VAL_UWBCFG_DEF_TXRF_POWER_COARSE ("15")
#endif

/**
 * @brief Default UWB FINE TX Power (0,1,2,..,31)
 */
#ifndef MYNEWT_VAL_UWBCFG_DEF_TXRF_POWER_FINE
#define MYNEWT_VAL_UWBCFG_DEF_TXRF_POWER_FINE ("22")
#endif

/**
 * @brief Default UWB FINE TX Power (0,1,2,..,31)
 */
#ifndef MYNEWT_VAL_UWBCFG_DEF_TXRF_VCM_LO
#define MYNEWT_VAL_UWBCFG_DEF_TXRF_VCM_LO ("15")
#endif

/**
 * @brief Default UWB TX Antenna delay
 */
#ifndef MYNEWT_VAL_UWBCFG_DEF_TX_ANTDLY
#define MYNEWT_VAL_UWBCFG_DEF_TX_ANTDLY ("0x4050")
#endif

/**
 * @brief Default UWB RX Antenna separation distance in m
 */
#ifndef MYNEWT_VAL_UWBCFG_DEF_TX_PREAM_CIDX
#define MYNEWT_VAL_UWBCFG_DEF_TX_PREAM_CIDX ("9")
#endif

/**
 * @brief Default UWB Preamble Length
 */
#ifndef MYNEWT_VAL_UWBCFG_DEF_TX_PREAM_LEN
#define MYNEWT_VAL_UWBCFG_DEF_TX_PREAM_LEN ("128")
#endif

/**
 * @brief Default XTAL Trim value (0xff = no trim, or use OTP value)
 */
#ifndef MYNEWT_VAL_UWBCFG_DEF_XTAL_TRIM
#define MYNEWT_VAL_UWBCFG_DEF_XTAL_TRIM ("0xff")
#endif

/**
 * @brief   Offset relative leading edge to start extracting CIR from
 */
#ifndef MYNEWT_VAL_UWBCFG_DEF_CIR_OFFSET
#define MYNEWT_VAL_UWBCFG_DEF_CIR_OFFSET ("0")
#endif

/**
 * @brief   Number of bins to extract from CIR
 */
#ifndef MYNEWT_VAL_UWBCFG_DEF_CIR_SIZE
#define MYNEWT_VAL_UWBCFG_DEF_CIR_SIZE ("0")
#endif

#ifdef __cplusplus
}
#endif

#endif /* DPL_SYSCFG_SYSCFG_UWBCFG_H */
