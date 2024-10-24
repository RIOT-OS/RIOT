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
 * @brief           Common CPU definitions for the STM32 family
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Vincent Dupont <vincent@otakeys.com>
 */

#ifndef PERIPH_CPU_COMMON_H
#define PERIPH_CPU_COMMON_H

#include <stdint.h>

#include "cpu_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef Doxygen
/**
 * @brief   Starting address of the ROM bootloader
 *          see application note AN2606
 */
#define STM32_BOOTLOADER_ADDR
#endif

/**
 * @brief   Length of the CPU_ID in octets
 *
 * This is the same for all members of the stm32 family
 */
#define CPUID_LEN           (12U)

/**
 * @brief   Starting address of the CPU ID
 */
#ifndef CPUID_ADDR
#define CPUID_ADDR          (UID_BASE)
#endif

/**
 * @brief   CPU specific LSI clock speed
 */
#if defined(CPU_FAM_STM32F0) || defined (CPU_FAM_STM32F1) || \
    defined(CPU_FAM_STM32F3)
#define CLOCK_LSI           (40000U)
#elif defined(CPU_FAM_STM32L0) || defined(CPU_FAM_STM32L1)
#define CLOCK_LSI           (37000U)
#elif defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4) || \
      defined(CPU_FAM_STM32F7) || defined(CPU_FAM_STM32L4) || \
      defined(CPU_FAM_STM32WB) || defined(CPU_FAM_STM32G4) || \
      defined(CPU_FAM_STM32G0) || defined(CPU_FAM_STM32L5) || \
      defined(CPU_FAM_STM32U5) || defined(CPU_FAM_STM32MP1) || \
      defined(CPU_FAM_STM32WL) || defined(CPU_FAM_STM32C0)
#define CLOCK_LSI           (32000U)
#else
#error "error: LSI clock speed not defined for your target CPU"
#endif

/* if CPU has APB1 bus */
#if     defined(CPU_FAM_STM32G4) || \
        defined(CPU_FAM_STM32L4) || \
        defined(CPU_FAM_STM32L5) || \
        defined(CPU_FAM_STM32U5) || \
        defined(CPU_FAM_STM32WB) || \
        defined(CPU_FAM_STM32WL)
            #define APB1_PERIPH_EN              RCC->APB1ENR1
            #define APB12_PERIPH_EN             RCC->APB1ENR2
#elif   defined(CPU_FAM_STM32C0) || \
        defined(CPU_FAM_STM32G0)
            #define APB1_PERIPH_EN              RCC->APBENR1
            #define APB12_PERIPH_EN             RCC->APBENR2
#elif   defined(CPU_FAM_STM32MP1)
            #define APB1_PERIPH_EN              RCC->MC_APB1ENSETR
            #define APB1_PERIPH_DIS             RCC->MC_APB1ENCLRR
#elif   defined(APB1PERIPH_BASE) || \
        defined(CPU_FAM_STM32F0) || \
        defined(CPU_FAM_STM32L0)
            #define APB1_PERIPH_EN              RCC->APB1ENR
#endif

/* if CPU has APB2 bus */
#if     defined(CPU_FAM_STM32MP1)
            #define APB2_PERIPH_EN              RCC->MC_APB2ENSETR
            #define APB2_PERIPH_DIS             RCC->MC_APB2ENCLRR
#elif   defined(APB2PERIPH_BASE) || \
        defined(CPU_FAM_STM32F0) || \
        defined(CPU_FAM_STM32L0)
            #define APB2_PERIPH_EN              RCC->APB2ENR
#endif

/* if CPU has APB3 bus */
#if     defined(CPU_FAM_STM32WB)
            /* CPU has APB3, but no periph enable registers for the bus. */
            #undef APB3_PERIPH_EN               /* not defined */
#elif   defined(APB3PERIPH_BASE) || \
        defined(APB3PERIPH_BASE_S)
            #define APB3_PERIPH_EN              RCC->APB3ENR
#endif

/* if CPU has AHB/AHB1 bus */
#if     defined(AHBPERIPH_BASE) || \
        defined(CPU_FAM_STM32F3)
            #define AHB_PERIPH_EN               RCC->AHBENR
#elif   defined(CPU_FAM_STM32MP1)
            /* CPU has AHB1, but no periph enable registers for the bus. */
            #undef AHB1_PERIPH_EN               /* not defined */
            #undef AHB1_PERIPH_DIS              /* not defined */
#elif   defined(AHB1PERIPH_BASE)
            #define AHB1_PERIPH_EN              RCC->AHB1ENR
#endif

/* if CPU has AHB2 bus */
#if     defined(CPU_FAM_STM32F0) || \
        defined(CPU_FAM_STM32F3)
            /* CPU has AHB2, but no periph enable registers for the bus. */
            #undef AHB2_PERIPH_EN               /* not defined */
#elif   defined(CPU_FAM_STM32U5)
            #define AHB2_PERIPH_EN              RCC->AHB2ENR1
            #define AHB22_PERIPH_EN             RCC->AHB2ENR2
#elif   defined(CPU_FAM_STM32F4) && defined(RCC_AHB2_SUPPORT)
            #define AHB2_PERIPH_EN              RCC->AHB2ENR
#elif   defined(CPU_FAM_STM32MP1)
            #define AHB2_PERIPH_EN              RCC->MC_AHB2ENSETR
            #define AHB2_PERIPH_DIS             RCC->MC_AHB2ENCLRR
#elif   defined(AHB2PERIPH_BASE)
            #define AHB2_PERIPH_EN              RCC->AHB2ENR
#endif

/* if CPU has AHB3 bus */
#if     defined(CPU_FAM_STM32F3)
            /* CPU has AHB3, but no periph enable registers for the bus. */
            #undef AHB3_PERIPH_EN               /* not defined */
#elif   defined(CPU_FAM_STM32F4) && defined(RCC_AHB3_SUPPORT)
            #define AHB3_PERIPH_EN              RCC->AHB3ENR
#elif   defined(CPU_FAM_STM32MP1)
            #define AHB3_PERIPH_EN              RCC->MC_AHB3ENSETR
            #define AHB3_PERIPH_DIS             RCC->MC_AHB3ENCLRR
#elif   defined(AHB3PERIPH_BASE) || \
        defined(AHB3PERIPH_BASE_S) || \
        defined(CPU_FAM_STM32F2) || \
        defined(CPU_FAM_STM32F7) || \
        defined(CPU_FAM_STM32G4) || \
        defined(CPU_FAM_STM32L4)
            #define AHB3_PERIPH_EN              RCC->AHB3ENR
#endif

/* if CPU has AHB4 bus */
#if     defined(CPU_FAM_STM32MP1)
            #define AHB4_PERIPH_EN              RCC->MC_AHB4ENSETR
            #define AHB4_PERIPH_DIS             RCC->MC_AHB4ENCLRR
#elif   defined(AHB4PERIPH_BASE)
            /* AHB3ENR is not a typo here. It controls both AHB3 and AHB4. */
            #define AHB4_PERIPH_EN              RCC->AHB3ENR
#endif

/* if CPU has IOP bus */
#if     defined(IOPPERIPH_BASE) || \
        defined(RCC_IOPENR_GPIOAEN) || \
        defined(RCC_IOPENR_IOPAEN)
            #define IOP_PERIPH_EN               RCC->IOPENR
#endif

/**
 * @brief   Available peripheral buses
 */
typedef enum {
#if defined(APB1_PERIPH_EN)
    APB1,           /**< APB1 bus */
#endif
#if defined(APB12_PERIPH_EN)
    APB12,          /**< AHB1 bus, second register */
#endif
#if defined(APB2_PERIPH_EN)
    APB2,           /**< APB2 bus */
#endif
#if defined(APB3_PERIPH_EN)
    APB3,           /**< APB3 bus */
#endif
#if defined(AHB_PERIPH_EN)
    AHB,            /**< AHB bus */
#endif
#if defined(AHB1_PERIPH_EN)
    AHB1,           /**< AHB1 bus */
#endif
#if defined(AHB2_PERIPH_EN)
    AHB2,           /**< AHB2 bus */
#endif
#if defined(AHB22_PERIPH_EN)
    AHB22,          /**< AHB2 bus, second register */
#endif
#if defined(AHB3_PERIPH_EN)
    AHB3,           /**< AHB3 bus */
#endif
#if defined(AHB4_PERIPH_EN)
    AHB4,           /**< AHB4 bus */
#endif
#if defined(IOP_PERIPH_EN)
    IOP,            /**< IOP bus */
#endif
    BUS_NUMOF       /**< number of buses */
} bus_t;

/**
 * @brief   Get the actual bus clock frequency for the APB buses
 *
 * @param[in] bus       target APBx bus
 *
 * @return              bus clock frequency in Hz
 */
uint32_t periph_apb_clk(bus_t bus);

/**
 * @brief   Get the actual timer clock frequency
 *
 * @param[in] bus       corresponding APBx bus
 *
 * @return              timer clock frequency in Hz
 */
uint32_t periph_timer_clk(bus_t bus);

/**
 * @brief   Enable the given peripheral clock
 *
 * @param[in] bus       bus the peripheral is connected to
 * @param[in] mask      bit in the RCC enable register
 */
void periph_clk_en(bus_t bus, uint32_t mask);

/**
 * @brief   Disable the given peripheral clock
 *
 * @param[in] bus       bus the peripheral is connected to
 * @param[in] mask      bit in the RCC enable register
 */
void periph_clk_dis(bus_t bus, uint32_t mask);

/**
 * @brief   Enable the given peripheral clock in low power mode
 *
 * @param[in] bus       bus the peripheral is connected to
 * @param[in] mask      bit in the RCC enable register
 */
void periph_lpclk_en(bus_t bus, uint32_t mask);

/**
 * @brief   Disable the given peripheral clock in low power mode
 *
 * @param[in] bus       bus the peripheral is connected to
 * @param[in] mask      bit in the RCC enable register
 */
void periph_lpclk_dis(bus_t bus, uint32_t mask);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* PERIPH_CPU_COMMON_H */
