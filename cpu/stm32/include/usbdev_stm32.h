/*
 * Copyright (C) 2019 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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

#ifndef USBDEV_STM32_H
#define USBDEV_STM32_H

#include <stdint.h>
#include <stdlib.h>
#include "periph_cpu.h"
#include "periph/usbdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Detect the IP version based on the available register define */
#if defined(USB_OTG_GCCFG_NOVBUSSENS)
#define STM32_USB_OTG_CID_1x        /**< USB OTG FS version 0x00001200 */
#elif defined(USB_OTG_GCCFG_VBDEN)
#define STM32_USB_OTG_CID_2x        /**< USB OTG FS version 0x00002000 */
#elif defined(USB)
#define STM32_USB_FS_CID_1x         /**< USB FS version 0x00001200 */
#else
#error Unknown USB peripheral version
#endif

/**
 * @brief Number of endpoints available with the OTG FS peripheral
 *        including the control endpoint
 */
#ifdef STM32_USB_OTG_CID_1x
#define STM32_USB_OTG_FS_NUM_EP (4) /**< OTG FS with 4 endpoints */
#elif defined(STM32_USB_OTG_CID_2x)
#define STM32_USB_OTG_FS_NUM_EP (6) /**< OTG FS with 6 endpoints */
#endif

/**
 * @brief Number of endpoints available with the OTG HS peripheral
 *        including the control endpoint
 */
#ifdef STM32_USB_OTG_CID_1x
#define STM32_USB_OTG_HS_NUM_EP (6) /**< OTG HS with 6 endpoints */
#elif defined(STM32_USB_OTG_CID_2x)
#define STM32_USB_OTG_HS_NUM_EP (9) /**< OTG HS with 9 endpoints */
#endif

/**
 * @brief USB OTG FS FIFO reception buffer space in 32-bit words
 *
 * Used as shared FIFO for reception of all OUT transfers
 *
 * @note The application might have to increase this when dealing with large
 *       isochronous transfers
 */
#ifndef STM32_USB_OTG_FS_RX_FIFO_SIZE
#define STM32_USB_OTG_FS_RX_FIFO_SIZE   (128U)
#endif

/**
 * @brief USB OTG HS FIFO reception buffer space in 32-bit words
 *
 * Used as shared FIFO for reception of all OUT transfers from the host
 */
#ifndef STM32_USB_OTG_HS_RX_FIFO_SIZE
#define STM32_USB_OTG_HS_RX_FIFO_SIZE   (512U)
#endif

/**
 * @brief Use the built-in DMA controller of the HS peripheral when possible
 */
#ifndef STM32_USB_OTG_HS_USE_DMA
#ifdef STM32_USB_OTG_CID_1x
/* FIXME: It should be possible to use DMA with the 1.x version of the  *
 * peripheral, but somehow it doesn't work.                             */
#define STM32_USB_OTG_HS_USE_DMA        (0)
#else
#define STM32_USB_OTG_HS_USE_DMA        (1)
#endif
#endif

/**
 * @brief stm32 USB OTG peripheral device out endpoint struct
 */
typedef struct {
    usbdev_ep_t ep;     /**< Inherited usbdev endpoint struct */
    uint8_t *out_buf;   /**< Requested data output buffer */
} stm32_usb_otg_fshs_out_ep_t;

/**
 * @brief stm32 USB OTG peripheral device context
 */
typedef struct {
    usbdev_t usbdev;                            /**< Inherited usbdev struct */
    const stm32_usb_otg_fshs_config_t *config;  /**< USB peripheral config   */
    size_t fifo_pos;                            /**< FIFO space occupied */
    usbdev_ep_t *in;                            /**< In endpoints */
    stm32_usb_otg_fshs_out_ep_t *out;           /**< Out endpoints */
    bool suspend;                               /**< Suspend status */
} stm32_usb_otg_fshs_t;

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
#endif /* USBDEV_STM32_H */
/** @} */
