/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2014 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup        cpu_kw2x KW2xD SiP
 * @ingroup         cpu
 * @brief           CPU specific implementations for the Freescale KW2xD SiP.
 *                  The SiP incorporates a low power 2.4 GHz transceiver and a
 *                  Kinetis Cortex-M4 MCU.
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Hauke Petersen <hauke.peterse@fu-berlin.de>
 * @author          Johann Fischer <j.fischer@phytec.de>
 */

#ifndef __CPU_CONF_H
#define __CPU_CONF_H

#ifdef CPU_MODEL_KW21D256
#include "MKW22D5.h"
#elif CPU_MODEL_KW21D512
#include "MKW22D5.h"
#elif CPU_MODEL_KW22D512
#include "MKW22D5.h"
#else
#error "undefined CPU_MODEL"
#endif

#include "mcg.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief   ARM Cortex-M specific CPU configuration
 * @{
 */
#define CPU_DEFAULT_IRQ_PRIO            (1U)
#define CPU_IRQ_NUMOF                   (65U)
#define CPU_FLASH_BASE                  (0x00000000)
/** @} */

/**
 * @brief Length for reading CPU_ID in octets
 */
#define CPUID_ID_LEN                     (16)

/**
 * @brief Pointer to CPU_ID
 */
#define CPUID_ID_PTR                     ((void *)(&(SIM_UIDH)))

#define TRANSCEIVER_BUFFER_SIZE          (3) /**< Buffer Size for Transceiver Module */

/**
 * @brief MCU specific Low Power Timer settings.
 */
#define LPTIMER_CLKSRC                   LPTIMER_CLKSRC_LPO
#define LPTIMER_DEV                      (LPTMR0) /**< LPTIMER hardware module */
#define LPTIMER_CLKEN()                  (SIM->SCGC5 |= SIM_SCGC5_LPTMR_MASK) /**< Enable LPTMR0 clock gate */
#define LPTIMER_CLKDIS()                 (SIM->SCGC5 &= ~SIM_SCGC5_PTMR_MASK) /**< Disable LPTMR0 clock gate */
#define LPTIMER_CNR_NEEDS_LATCHING       1 /**< LPTMR.CNR register do not need latching */

/**
 * @name KW2XD SiP internal interconnects between MCU and Modem.
 *
 * @{
 */
#define KW2XDRF_PORT_BASE       PORTB_BASE /**< MCU Port connected to Modem*/
#define KW2XDRF_PORT            PORTB /**< MCU Port connected to Modem*/
#define KW2XDRF_GPIO            GPIOB /**< GPIO Device connected to Modem */
#define KW2XDRF_PORT_IRQn       PORTB_IRQn
#define KW2XDRF_PORT_CLKEN()    (SIM->SCGC5 |= (SIM_SCGC5_PORTB_MASK)) /**< Clock Enable for PORTB*/
#define KW2XDRF_PIN_AF          2  /**< Pin Muxing Parameter for GPIO Device*/
#define KW2XDRF_PCS0_PIN        10 /**< SPI Slave Select Pin */
#define KW2XDRF_SCK_PIN         11 /**< SPI Clock Output Pin */
#define KW2XDRF_SOUT_PIN        16 /**< SPI Master Data Output Pin */
#define KW2XDRF_SIN_PIN         17 /**< SPI Master Data Input Pin */

#define KW2XDRF_IRQ_PIN         3  /**< Modem's IRQ Output (activ low) */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __CPU_CONF_H */
/** @} */
