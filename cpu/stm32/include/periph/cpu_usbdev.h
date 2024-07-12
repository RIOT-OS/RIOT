/*
 * Copyright (C) 2016 Freie Universität Berlin
 *               2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_stm32
 * @{
 *
 * @file
 * @brief           USB CPU specific definitions for the STM32 family
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Vincent Dupont <vincent@otakeys.com>
 */

#ifndef PERIPH_CPU_USBDEV_H
#define PERIPH_CPU_USBDEV_H

#include <stdint.h>

#include "periph/cpu_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief USBDEV buffers must be word aligned because of DMA restrictions
 */
#define USBDEV_CPU_DMA_ALIGNMENT       (4)

/**
 * @brief USBDEV buffer instantiation requirement
 */
#define USBDEV_CPU_DMA_REQUIREMENTS    __attribute__((aligned(USBDEV_CPU_DMA_ALIGNMENT)))

#if !DOXYGEN    /* hide implementation details */

/**
 * @name    USB device definitions
 * @{
 */
/* Detect the IP version based on the available register define */
#if defined(USB_OTG_GCCFG_NOVBUSSENS)
#define STM32_USB_OTG_CID_1x        /**< USB OTG FS version 0x00001200 */
#elif defined(USB_OTG_GCCFG_VBDEN)
#define STM32_USB_OTG_CID_2x        /**< USB OTG FS version 0x00002000 */
#elif defined(USB)
#define STM32_USB_FS_CID_1x         /**< USB FS version 0x00001200 */
#endif

/**
 * @brief Number of endpoints available with the OTG FS peripheral
 *        including the control endpoint
 */
#if defined(USB_OTG_FS_MAX_IN_ENDPOINTS)
#define STM32_USB_OTG_FS_NUM_EP     (USB_OTG_FS_MAX_IN_ENDPOINTS)
#elif defined(STM32_USB_OTG_CID_1x)
#define STM32_USB_OTG_FS_NUM_EP     (4)    /**< OTG FS with 4 endpoints */
#elif defined(STM32_USB_OTG_CID_2x)
#define STM32_USB_OTG_FS_NUM_EP     (6)    /**< OTG FS with 6 endpoints */
#endif

/**
 * @brief Number of endpoints available with the OTG HS peripheral
 *        including the control endpoint
 */
#if defined(USB_OTG_HS_MAX_IN_ENDPOINTS)
#define STM32_USB_OTG_HS_NUM_EP     (USB_OTG_HS_MAX_IN_ENDPOINTS)
#elif defined(STM32_USB_OTG_CID_1x)
#define STM32_USB_OTG_HS_NUM_EP     (6)     /**< OTG HS with 6 endpoints */
#elif defined(STM32_USB_OTG_CID_2x)
#define STM32_USB_OTG_HS_NUM_EP     (9)     /**< OTG HS with 9 endpoints */
#endif

/**
 * @brief Number of IN/OUT endpoints including EP0 as used by USBUS
 *
 * @note USBUS allows only one definition of the number of available EPs, which
 *       is then used for all devices. To be able to use all EPs for devices
 *       with more EPs, the largest possible number of available EPs for
 *       several USB devices is defined here. The driver has to ensure that the
 *       number of allocated EPs does not exceed the number of available EPs if
 *       a device has less EPs.
 */
#if defined(MODULE_PERIPH_USBDEV_HS) && defined(STM32_USB_OTG_HS_NUM_EP)
#define USBDEV_NUM_ENDPOINTS            STM32_USB_OTG_HS_NUM_EP
#elif defined(STM32_USB_OTG_FS_NUM_EP)
#define USBDEV_NUM_ENDPOINTS            STM32_USB_OTG_FS_NUM_EP
#else
#define USBDEV_NUM_ENDPOINTS            8
#endif

#endif /* !DOXYGEN */

/**
 * @brief stm32 USB device FS configuration
 */
typedef struct {
    uintptr_t base_addr;            /**< USB peripheral base address */
    uint32_t rcc_mask;              /**< bit in clock enable register */
    uint8_t irqn;                   /**< IRQ channel */
    uint8_t apb;                    /**< APB bus */
    gpio_t dm;                      /**< Data- gpio */
    gpio_t dp;                      /**< Data+ gpio */
    gpio_af_t af;                   /**< Alternative function */
    gpio_t disconn;                 /**< GPIO if used for USB disconnect */
} stm32_usbdev_fs_config_t;

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_USBDEV_H */
/** @} */
