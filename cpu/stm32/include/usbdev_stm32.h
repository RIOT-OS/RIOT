/*
 * Copyright (C) 2019 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    cpu_stm32_usbdev stm32 USB OTG FS/HS peripheral
 * @ingroup     cpu_stm32
 * @brief       USB interface functions for the stm32 class devices
 *
 * @{
 *
 * @file
 * @brief       USB interface functions for the stm32 OTG FS/HS class devices
 *
 * The stm32f2, stm32f4 and stm32f7 have a common USB OTG FS capable USB
 * peripheral.
 *
 * Two versions are currently known to exist with subtle differences
 * in some registers. The CID register of the peripheral indicates this version,
 * 0x00001200 for one version of the full speed peripheral and 0x00002000 for
 * the other version of the full speed peripheral.
 * The main difference is in the GCCFG register, where the 1.2 version has a
 * NOVBUSSENS bit and the 2.0 version has a VBDEN bit. This difference is used
 * to detect the IP version.
 * The 2.0 version also has more advanced USB low power mode support.
 *
 * For the end user, the main difference is the 1.2 version  having 4 endpoints
 * and the 2.0 version having 6 endpoints. The 2.0 version also supports a
 * number of USB low power modes.
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#include <stdint.h>
#include <stdlib.h>
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Number of endpoints available with the OTG FS peripheral
 *        including the control endpoint
 */
#define DWC2_USB_OTG_FS_NUM_EP  STM32_USB_OTG_FS_NUM_EP

/**
 * @brief Number of endpoints available with the OTG HS peripheral
 *        including the control endpoint
 */
#define DWC2_USB_OTG_HS_NUM_EP  STM32_USB_OTG_HS_NUM_EP

/**
 * @brief USB OTG FS FIFO reception buffer space in 32-bit words
 *
 * Used as shared FIFO for reception of all OUT transfers from the host
 *
 * @note The application might have to increase this when dealing with large
 *       isochronous transfers
 */
#ifndef DWC2_USB_OTG_FS_RX_FIFO_SIZE
#define DWC2_USB_OTG_FS_RX_FIFO_SIZE   (128U)
#endif

/**
 * @brief USB OTG HS FIFO reception buffer space in 32-bit words
 *
 * Used as shared FIFO for reception of all OUT transfers from the host
 */
#ifndef DWC2_USB_OTG_HS_RX_FIFO_SIZE
#define DWC2_USB_OTG_HS_RX_FIFO_SIZE   (512U)
#endif

/**
 * @brief USB OTG FS FIFO total size in bytes
 *
 * Some device families (F7 and L4) forgot to define the FS device FIFO size
 * in their vendor headers. This define sets it to the value from the
 * reference manual.
 */
#ifndef USB_OTG_FS_TOTAL_FIFO_SIZE
#define USB_OTG_FS_TOTAL_FIFO_SIZE      (1280U)
#endif

/**
 * @brief USB OTG FS FIFO total size in bytes
 *
 * Some device families (F7 and L4) forgot to define the HS device FIFO size
 * in their vendor headers. This define sets it to the value from the
 * reference manual.
 */
#ifndef USB_OTG_HS_TOTAL_FIFO_SIZE
#define USB_OTG_HS_TOTAL_FIFO_SIZE      (4096U)
#endif

/**
 * @brief USB OTG FS FIFO total size in bytes
 */
#define DWC2_USB_OTG_FS_TOTAL_FIFO_SIZE USB_OTG_FS_TOTAL_FIFO_SIZE

/**
 * @brief USB OTG HS FIFO total size in bytes
 */
#define DWC2_USB_OTG_HS_TOTAL_FIFO_SIZE USB_OTG_HS_TOTAL_FIFO_SIZE

/* periph/usbdev.h is included after the definitions above by intention */
#include "periph/usbdev.h"

/**
 * @brief stm32 USB Device FS only peripheral device context
 */
typedef struct {
    usbdev_t usbdev;                            /**< Inherited usbdev struct */
    const stm32_usbdev_fs_config_t *config;     /**< USB peripheral config   */
    usbdev_ep_t *in;                            /**< In endpoints */
    usbdev_ep_t *out;                           /**< Out endpoints */
    size_t used;                                /**< Bytes used by usbdev stack */
} stm32_usbdev_fs_t;

#ifdef __cplusplus
}
#endif
/** @} */
