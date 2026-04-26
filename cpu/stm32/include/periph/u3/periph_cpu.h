/*
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     cpu_stm32
 * @{
 *
 * @file
 * @brief       STM32U3 family specific peripheral CPU definitions
 *
 * @author      Adarsh Nair
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DOXYGEN

/**
 * @brief   Starting address of the ROM bootloader
 *          see application note AN2606
 */
#define STM32_BOOTLOADER_ADDR   (0x0BF8F000)

/**
 * @name   Constants for internal VBAT ADC line
 * @{
 */
#define VBAT_ADC_RES        ADC_RES_12BIT
#define VBAT_ADC_MAX        4095
/** @} */

/**
 * @name   RCC clock gating for USB full-speed (USB DRD FS) on STM32U3
 * @{
 * @note RM0487, memory map: USB is on the **APB2** bus; the enable bit is in
 * @c RCC->APB2ENR. ST CMSIS usually uses @c RCC_APB2ENR_USBEN (or similar).
 * The APB1 variants below are only fallbacks for unusual header pack versions.
 * Board @c apb in @c stm32_usbdev_fs_config_t must match (typically @c APB2).
 */
#if !defined(RCC_U3_USBDEV_FS_RMASK)
#  if defined(RCC_APB2ENR_USB1EN)
#    /* STM32U3 CMSIS: USB DRD FS (bit name includes instance "1" — see stm32u385xx.h) */
#    define RCC_U3_USBDEV_FS_RMASK  RCC_APB2ENR_USB1EN
#  elif defined(RCC_APB2ENR_USBEN)
#    define RCC_U3_USBDEV_FS_RMASK  RCC_APB2ENR_USBEN
#  elif defined(RCC_APB2ENR_USBFSDEN)
#    define RCC_U3_USBDEV_FS_RMASK  RCC_APB2ENR_USBFSDEN
#  elif defined(RCC_APB2ENR_USBFSEN)
#    define RCC_U3_USBDEV_FS_RMASK  RCC_APB2ENR_USBFSEN
#  elif defined(RCC_APB2ENR1_USBEN)
#    define RCC_U3_USBDEV_FS_RMASK  RCC_APB2ENR1_USBEN
#  elif defined(RCC_APB1ENR1_USBEN)
#    define RCC_U3_USBDEV_FS_RMASK  RCC_APB1ENR1_USBEN
#  elif defined(RCC_APB1ENR1_USBFSDEN)
#    define RCC_U3_USBDEV_FS_RMASK  RCC_APB1ENR1_USBFSDEN
#  elif defined(RCC_APB1ENR1_USBFSEN)
#    define RCC_U3_USBDEV_FS_RMASK  RCC_APB1ENR1_USBFSEN
#  elif defined(RCC_APB1ENR1_USBDEN)
#    define RCC_U3_USBDEV_FS_RMASK  RCC_APB1ENR1_USBDEN
#  else
#    error "Add USB full-speed clock enable: grep RCC_ *USB* in your stm32u3xx.h (APB2ENR in RM0487) and set RCC_U3_USBDEV_FS_RMASK in board Makefile CFLAGS, or add the macro name here"
#  endif
#endif
/** @} */

/**
 * @name   USB DRD FS peripheral handle for driver code
 * @{
 * STM32U3 CMSIS may expose the instance as @c USB, @c USB_DRD, @c USB_DRD_FS, or
 * a base address macro only. Provide @c USB when missing so @c periph_conf.h can
 * use a single @c (uintptr_t)USB like other boards.
 */
#if !defined(USB)
#  if defined(USB_DRD)
#    define USB  USB_DRD
#  elif defined(USB_DRD_FS)
#    define USB  USB_DRD_FS
#  elif defined(USB_DRD_BASE)
#    define USB  ((USB_DRD_TypeDef *)USB_DRD_BASE)
#  endif
#endif
/** @} */

/**
 * @name   RCC APB3 clock enable (cpu_common.h bus @c APB3)
 * @{
 * @note RM0487: @c RCC_APB3ENR gates SYSCFG, LPUART1, I2C3, LPTIM, etc. CMSIS
 *       exposes @c RCC->APB3ENR (not a made-up name).
 */
#if defined(CPU_FAM_STM32U3) && !defined(APB3_PERIPH_EN)
#  define APB3_PERIPH_EN   (RCC->APB3ENR)
#endif
/** @} */

/**
 * @name   Non-secure flash key symbols (@c FLASH_KEY1 / @c FLASH_KEY2)
 * @{
 * @note   STM32U3 CMSIS names @c FLASH->KEYR and the mask
 *         @c FLASH_KEYR_KEY_Msk, not 32-bit unlock values for the key
 *         sequence. RM0487 uses the same words as e.g. STM32G4 (see vendor
 *         @c FLASH_KEY1 / @c FLASH_KEY2 there); the U3 device headers do not
 *         define them.
 */
#if defined(CPU_FAM_STM32U3) && !defined(FLASH_KEY1) && !defined(FLASH_KEY2)
#  define FLASH_KEY1   ((uint32_t)0x45670123U)
#  define FLASH_KEY2   ((uint32_t)0xCDEF89ABU)
#endif
/** @} */

#endif /* ndef DOXYGEN */

#ifdef __cplusplus
}
#endif
