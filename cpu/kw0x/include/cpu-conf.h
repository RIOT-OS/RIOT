/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup        cpu_kw0x KW0x SiP
 * @ingroup         cpu
 * @brief           CPU specific implementations for the Freescale MKW0x SiP.
 *                  The SiP incorporates a low power Sub-1 GHz transceiver and a
 *                  Kinetis Cortex-M0+ MCU.
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

#ifdef CPU_MODEL_KW01Z128
#include "MKW01Z4.h"
#include "mcg.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name Kernel configuration
 *
 * @{
 */
#define KERNEL_CONF_STACKSIZE_PRINTF     (1024)

#ifndef KERNEL_CONF_STACKSIZE_DEFAULT
#define KERNEL_CONF_STACKSIZE_DEFAULT    (1024)
#endif

#define KERNEL_CONF_STACKSIZE_IDLE       (256)
/** @} */

/**
 * @brief Length for reading CPU_ID in octets
 */
#define CPUID_ID_LEN                     (12)

/**
 * @brief Pointer to CPU_ID
 */
#define CPUID_ID_PTR                     ((void *)(&(SIM->UIDMH)))

/**
 * @name UART0 buffer size definition for compatibility reasons.
 *
 * TODO: remove once the remodeling of the uart0 driver is done.
 * @{
 */
#ifndef UART0_BUFSIZE
#define UART0_BUFSIZE                    (128)
#endif
/** @} */

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
 * @name MKW0XD SiP internal interconnects between MCU and Modem.
 *
 * @{
 */
/* TODO */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __CPU_CONF_H */
/** @} */
