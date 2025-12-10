/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     pkg_uwb_core
 * @{
 *
 * @file
 * @brief       uwb-dw1000 radio configurations
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#include "dw1000/dw1000_regs.h"

#ifdef __cplusplus
extern "C" {
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
#ifndef CONFIG_DW1000_ROLE_DEFAULT
#define CONFIG_DW1000_ROLE_DEFAULT         0x0
#endif

/**
 * @brief   Default Number of symbols in start of frame delimiter
 */
#ifndef CONFIG_DW1000_NSFD_DEFAULT
#define CONFIG_DW1000_NSFD_DEFAULT         8
#endif

/**
 * @brief   Default Number of symbols in preamble sequence
 */
#ifndef CONFIG_DW1000_NSYNC_DEFAULT
#define CONFIG_DW1000_NSYNC_DEFAULT         128
#endif

/**
 * @brief   Default Number of symbols in phy header
 */
#ifndef CONFIG_DW1000_NPHR_DEFAULT
#define CONFIG_DW1000_NPHR_DEFAULT         21
#endif

/**
 * @brief   Default channel
 */
#ifndef CONFIG_DW1000_CHANNEL_DEFAULT
#define CONFIG_DW1000_CHANNEL_DEFAULT      5
#if CONFIG_DW1000_CHANNEL_DEFAULT > 7 || CONFIG_DW1000_CHANNEL_DEFAULT < 1 || \
    CONFIG_DW1000_CHANNEL_DEFAULT == 6
#error "CONFIG_DW1000_CHANNEL_DEFAULT must be 1..7, 6 excluded"
#endif
#endif

/**
 * @brief   Default Pulse generator delay
 */
#ifndef CONFIG_DW1000_TX_PGDELAY_DEFAULT
#if CONFIG_DW1000_CHANNEL_DEFAULT == 1
#define CONFIG_DW1000_TX_PGDELAY_DEFAULT   TC_PGDELAY_CH1
#elif CONFIG_DW1000_CHANNEL_DEFAULT == 2
#define CONFIG_DW1000_TX_PGDELAY_DEFAULT   TC_PGDELAY_CH2
#elif CONFIG_DW1000_CHANNEL_DEFAULT == 3
#define CONFIG_DW1000_TX_PGDELAY_DEFAULT   TC_PGDELAY_CH3
#elif CONFIG_DW1000_CHANNEL_DEFAULT == 4
#define CONFIG_DW1000_TX_PGDELAY_DEFAULT   TC_PGDELAY_CH4
#elif CONFIG_DW1000_CHANNEL_DEFAULT == 5
#define CONFIG_DW1000_TX_PGDELAY_DEFAULT   TC_PGDELAY_CH5
#elif CONFIG_DW1000_CHANNEL_DEFAULT == 7
#define CONFIG_DW1000_TX_PGDELAY_DEFAULT   TC_PGDELAY_CH7
#endif
#endif

/**
 * @brief   Default UWB Pulse Repetition Frequency (MHz)
 *
 * - DWT_PRF_16M
 * - DWT_PRF_64M
 */
#ifndef CONFIG_DW1000_PRF_DEFAULT
#define CONFIG_DW1000_PRF_DEFAULT          DWT_PRF_64M
#endif

/**
 * @brief   Default UWB Datarate (110k, 850k, 6m8)
 *
 * - DWT_BR_110K
 * - DWT_BR_850K
 * - DWT_BR_6M8
 */
#ifndef CONFIG_DW1000_DATARATE_DEFAULT
#define CONFIG_DW1000_DATARATE_DEFAULT     DWT_BR_6M8
#endif

/**
 * @brief   Default UWB Acquisition Chunk Size (Relates to RX preamble length)
 *
 * - 8
 * - 16
 * - 32
 * - 64
 */
#ifndef CONFIG_DW1000_PACLEN_DEFAULT
#define CONFIG_DW1000_PACLEN_DEFAULT       DWT_PAC8
#endif

/**
 * @brief Default UWB RX Preamble Code Index
 */
#ifndef CONFIG_DW1000_RX_PREAM_CIDX_DEFAULT
#define CONFIG_DW1000_RX_PREAM_CIDX_DEFAULT    9
#endif

/**
 * @brief Default UWB SFD Type
 *
 * - true: use non standard SFD for better performance
 * - false: use standard SFD
 */
#ifndef CONFIG_DW1000_RX_SFD_TYPE_DEFAULT
#define CONFIG_DW1000_RX_SFD_TYPE_DEFAULT  true
#endif

/**
 * @brief Default UWB SFD Timeout (-1=auto, timeout in symbols)
 *
 */
#ifndef CONFIG_DW1000_RX_SFD_TO_DEFAULT
#define CONFIG_DW1000_RX_SFD_TO_DEFAULT   (128 + 1 + 8 - 8) /* (preamble length + 1 + SFD length - PAC size) */
#endif

/**
 * @brief Default UWB PHR Mode
 *
 * - 0x0 - standard DWT_PHRMODE_STD
 * - 0x3 - extended frames DWT_PHRMODE_EXT
 */
#ifndef CONFIG_DW1000_RX_PHR_MODE_DEFAULT
#define CONFIG_DW1000_RX_PHR_MODE_DEFAULT      DWT_PHRMODE_EXT
#endif

/**
 * @brief Enable RX Frame Quality diagnositics (rssi, fppl, etc.)
 */
#ifndef CONFIG_DW1000_RX_DIAGNOSTIC
#define CONFIG_DW1000_RX_DIAGNOSTIC    0
#endif

/**
 * @brief Default UWB RX Antenna separation distance in m
 */
#ifndef CONFIG_DW1000_TX_PREAM_CIDX_DEAULT
#define CONFIG_DW1000_TX_PREAM_CIDX_DEAULT     9
#endif

/**
 * @brief Default UWB Preamble Length
 *
 * - DWT_PLEN_4096 : Standard preamble length 4096 symbols
 * - DWT_PLEN_2048 : Non-standard preamble length 2048 symbols
 * - DWT_PLEN_1536 : Non-standard preamble length 1536 symbols
 * - DWT_PLEN_1024 : Standard preamble length 1024 symbols
 * - DWT_PLEN_512  : Non-standard preamble length 512 symbols
 * - DWT_PLEN_256  : Non-standard preamble length 256 symbols
 * - DWT_PLEN_128  : Non-standard preamble length 128 symbols
 * - DWT_PLEN_64   : Standard preamble length 64 symbols
 * - DWT_PLEN_32   : When setting length 32 symbols this is 0x0,  which is programmed to byte 2 of the TX_FCTRL register
 * - DWT_PLEN_72   : Non-standard length 72
 */
#ifndef CONFIG_DW1000_TX_PREAM_LEN_DEFAULT
#define CONFIG_DW1000_TX_PREAM_LEN_DEFAULT      DWT_PLEN_128
#endif

/**
 * @brief Default UWB RX Antenna separation distance in m
 */
#ifndef CONFIG_DW1000_RX_ANTSEP_DEFAULT
#define CONFIG_DW1000_RX_ANTSEP_DEFAULT       0.0205
#endif

/**
 * @brief   Default MAC FrameFilter (0x00ff = frame filtering enabled, all frames
 *          matching PANID and destination address accepted)
 */
#ifndef CONFIG_DW1000_FRAME_FILTER_DEFAULT
#define CONFIG_DW1000_FRAME_FILTER_DEFAULT     0x00ff
#endif

/**
 * @brief   Default MAC FrameFilter Crystal Trim value, 0xff == not set
 */
#ifndef CONFIG_DW1000_XTAL_TRIM_DEFAULT
#define CONFIG_DW1000_XTAL_TRIM_DEFAULT        0x10
#endif

/**
 * @brief    Time until the Receiver is stable, (in us)
 */
#ifndef CONFIG_DW1000_RX_STABLE_TIME_US
#define CONFIG_DW1000_RX_STABLE_TIME_US        6
#endif

/**
 * @brief Enables forced TRXOFF in start_tx and start_tx interface
 */
#ifndef CONFIG_DW1000_TRXOFF_ENABLE
#define CONFIG_DW1000_TRXOFF_ENABLE            1
#endif

/**
 * @brief Enables double buffer
 */
#define DW1000_DOUBLE_BUFFER_ENABLE    false

/**
 * @brief Load LDE microcode on wake up
 */
#ifndef CONFIG_DW1000_LDE_ENABLE
#define CONFIG_DW1000_LDE_ENABLE   true
#endif

/**
 * @brief Load the LDO tune value on wake up
 */
#ifndef CONFIG_DW1000_LDO_ENABLE
#define CONFIG_DW1000_LDO_ENABLE   false
#endif

/**
 * @brief Enable sleep
 */
#ifndef CONFIG_DW1000_SLEEP_ENABLE
#define CONFIG_DW1000_SLEEP_ENABLE     true
#endif

/**
 * @brief Wakeup to Rx state
 */
#ifndef CONFIG_DW1000_WAKEUP_RX_ENABLE
#define CONFIG_DW1000_WAKEUP_RX_ENABLE true
#endif

/**
 * @brief On error re-enable
 */
#ifndef CONFIG_DW1000_RX_AUTO_ENABLE
#define CONFIG_DW1000_RX_AUTO_ENABLE    true
#endif

#ifdef __cplusplus
}
#endif
