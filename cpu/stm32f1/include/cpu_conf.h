/*
 * Copyright (C) 2013, 2016 Inria
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup        cpu_stm32f1 STM32F1
 * @ingroup         cpu
 * @brief           CPU specific implementations for the STM32F1
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Alaeddine Weslati <alaeddine.weslati@intia.fr>
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Francisco Acosta <francisco.acosta@inria.fr>
 */

#ifndef CPU_CONF_H
#define CPU_CONF_H

#include "cpu_conf_common.h"

#if defined(CPU_MODEL_STM32F103CB) || defined(CPU_MODEL_STM32F103RB)
#include "vendor/stm32f103xb.h"
#elif defined(CPU_MODEL_STM32F103RE)
#include "vendor/stm32f103xe.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Flash page configuration
 * @{
 */
#define FLASHPAGE_SIZE      (2048U)

#if defined(CPU_MODEL_STM32F103CB) || defined(CPU_MODEL_STM32F103RB)
#define FLASHPAGE_NUMOF     (64U)
#elif defined(CPU_MODEL_STM32F103RE)
#define FLASHPAGE_NUMOF     (256U)
#endif
/** @} */

/*
 * @brief   Offset to reset handler on VTOR
 */
#define VTOR_RESET_HANDLER     0x4

#if defined(CPU_MODEL_STM32F103RE)
/*
 * @brief   Flash partitioning for FW slots
 * @{
 */

#ifndef FW_METADATA_SPACE
#define FW_METADATA_SPACE          (0x100)
#endif

#define MAX_FW_SLOTS               (2)
#define FW_SLOT_PAGES              (120)
#define BOOTLOADER_SPACE           (0x4000)
#define FW_SLOT_SIZE               FLASHPAGE_SIZE * FW_SLOT_PAGES
#define FW_SLOT_1                  FLASH_BASE + BOOTLOADER_SPACE
#define FW_SLOT_1_END              FW_SLOT_1 + FW_SLOT_SIZE
#define FW_SLOT_1_PAGE             (8)
#define FW_SLOT_2                  FW_SLOT_1_END
#define FW_SLOT_2_END              FW_SLOT_2 + FW_SLOT_SIZE
#define FW_SLOT_2_PAGE             (128)

#ifdef FW_SLOTS
    #if FW_SLOT == 1
    #define CURRENT_FIRMWARE_ADDR       FW_SLOT_1
    #define CURRENT_FIRMWARE_PAGE       FW_SLOT_1_PAGE
    #define CURRENT_FIRMWARE_END        FW_SLOT_1_END
    #endif

    #if FW_SLOT == 2
    #define CURRENT_FIRMWARE_ADDR       FW_SLOT_2
    #define CURRENT_FIRMWARE_PAGE       FW_SLOT_2_PAGE
    #define CURRENT_FIRMWARE_END        FW_SLOT_2_END
    #endif

#endif /* FW_SLOTS */

/** @} */

/**
 * @brief Get FW internal address for a given slot
 *
 * @param[in] slot    FW slot
 *
 * @return            FW slot address
 */
static inline uint32_t get_slot_address(uint8_t slot)
{
    switch (slot) {
        case 1:
            return FW_SLOT_1;
            break;

        case 2:
            return FW_SLOT_2;
            break;
    }

    return 0;
}

/**
 * @brief Get internal page for a given slot
 *
 * @param[in] slot    FW slot
 *
 * @return            FW slot page
 */
static inline uint32_t get_slot_page(uint8_t slot)
{
    switch (slot) {
        case 1:
            return FW_SLOT_1_PAGE;
            break;

        case 2:
            return FW_SLOT_2_PAGE;
            break;
    }

    return 0;
}

#endif /* defined(CPU_MODEL_STM32F103RE) */
/** @} */

/**
 * @brief   ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO         (1U)
#define CPU_IRQ_NUMOF                (60U)

#ifdef FW_SLOTS
#define CPU_FLASH_BASE               (CURRENT_FIRMWARE_ADDR + FW_METADATA_SPACE)
#else
#define CPU_FLASH_BASE               FLASH_BASE
#endif
/** @} */

/**
 * @brief   Flash page configuration
 * @{
 */
#define FLASHPAGE_SIZE      (2048U)

#if defined(CPU_MODEL_STM32F103C8)
#define FLASHPAGE_NUMOF     (32U)
#elif defined(CPU_MODEL_STM32F103CB) || defined(CPU_MODEL_STM32F103RB)
#define FLASHPAGE_NUMOF     (64U)
#elif defined(CPU_MODEL_STM32F103RE)
#define FLASHPAGE_NUMOF     (256U)
#endif
/** @} */

/**
 * @brief Configure the CPU's clock system
 *
 * @param[in] source    source clock frequency
 * @param[in] target    target clock frequency
 * @param[in] prescale  prescaler to use
 */
void cpu_clock_scale(uint32_t source, uint32_t target, uint32_t *prescale);

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */
