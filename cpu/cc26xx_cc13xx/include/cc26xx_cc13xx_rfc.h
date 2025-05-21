/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup         cpu_cc26xx_cc13xx_definitions
 * @{
 *
 * @file
 * @brief           CC26xx/CC13xx MCU I/O register definitions
 *
 * @author          Jean Pierre Dudey <jeandudey@hotmail.com>
 */

#include "cc26xx_cc13xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   RFC_DBELL registers
 */
typedef struct {
    reg32_t CMDR; /**< Doorbell Command Register */
    reg32_t CMDSTA; /**< Doorbell Command Status Register */
    reg32_t RFHWIFG; /**< Interrupt Flags From RF Hardware Modules */
    reg32_t RFHWIEN; /**< Interrupt Enable For RF Hardware Modules */
    reg32_t RFCPEIFG; /**< Interrupt Flags For Command and Packet Engine
                           Generated Interrupts */
    reg32_t RFCPEIEN; /**< Interrupt Enable For Command and Packet Engine
                           Generated Interrupts */
    reg32_t RFCPEISL; /**< Interrupt Vector Selection For Command and Packet
                          Engine Generated Interrupts */
    reg32_t RFACKIFG; /**< Doorbell Command Acknowledgement Interrupt Flag */
    reg32_t SYSGPOCTL; /**< RF Core General Purpose Output Control */
} rfc_dbell_regs_t;

/**
 * @brief   RFC_DBELL definitions
 * @{
 */

/**
 * @brief   RFCHWIFG/RFCHWIEN interrupt flags
 */
typedef enum {
    HW_IRQ_FSCA     = (1 << 1),
    HW_IRQ_MDMDONE  = (1 << 2),
    HW_IRQ_MDMIN    = (1 << 3),
    HW_IRQ_MDMOUT   = (1 << 4),
    HW_IRQ_MDMSOFT  = (1 << 5),
    HW_IRQ_TRCTK    = (1 << 6),
    HW_IRQ_RFEDONE  = (1 << 8),
    HW_IRQ_RFESOFT0 = (1 << 9),
    HW_IRQ_RFESOFT1 = (1 << 10),
    HW_IRQ_RFESOFT2 = (1 << 11),
    HW_IRQ_RATCH0   = (1 << 12),
    HW_IRQ_RATCH1   = (1 << 13),
    HW_IRQ_RATCH2   = (1 << 14),
    HW_IRQ_RATCH3   = (1 << 15),
    HW_IRQ_RATCH4   = (1 << 16),
    HW_IRQ_RATCH5   = (1 << 17),
    HW_IRQ_RATCH6   = (1 << 18),
    HW_IRQ_RATCH7   = (1 << 19)
} rf_hw_irq_t;

/**
 * @brief   RFCPEIEN/RFCPEIFG/RFCPEISL interrupt flags
 */
typedef enum {
    CPE_IRQ_COMMAND_DONE         = (1 << 0),
    CPE_IRQ_LAST_COMMAND_DONE    = (1 << 1),
    CPE_IRQ_FG_COMMAND_DONE      = (1 << 2),
    CPE_IRQ_LAST_FG_COMMAND_DONE = (1 << 3),
    CPE_IRQ_TX_DONE              = (1 << 4),
    CPE_IRQ_TX_ACK               = (1 << 5),
    CPE_IRQ_TX_CTRL              = (1 << 6),
    CPE_IRQ_TX_CTRL_ACK          = (1 << 7),
    CPE_IRQ_TX_CTRL_ACK_ACK      = (1 << 8),
    CPE_IRQ_TX_RETRANS           = (1 << 9),
    CPE_IRQ_TX_ENTRY_DONE        = (1 << 10),
    CPE_IRQ_TX_BUFFER_CHANGED    = (1 << 11),
#ifdef CPU_VARIANT_X2
    CPE_IRQ_COMMAND_STARTED      = (1 << 12),
    CPE_IRQ_FG_COMMAND_STARTED   = (1 << 13),
#else
    CPE_IRQ_IRQ12                = (1 << 12),
    CPE_IRQ_IRQ13                = (1 << 13),
#endif
    CPE_IRQ_IRQ14                = (1 << 14),
    CPE_IRQ_IRQ15                = (1 << 15),
    CPE_IRQ_RX_OK                = (1 << 16),
    CPE_IRQ_RX_NOK               = (1 << 17),
    CPE_IRQ_RX_IGNORED           = (1 << 18),
    CPE_IRQ_RX_EMPTY             = (1 << 19),
    CPE_IRQ_RX_CTRL              = (1 << 20),
    CPE_IRQ_RX_CTRL_ACK          = (1 << 21),
    CPE_IRQ_RX_BUF_FULL          = (1 << 22),
    CPE_IRQ_RX_ENTRY_DONE        = (1 << 23),
    CPE_IRQ_RX_DATA_WRITTEN      = (1 << 24),
    CPE_IRQ_RX_N_DATA_WRITTEN    = (1 << 25),
    CPE_IRQ_RX_ABORTED           = (1 << 26),
    CPE_IRQ_IRQ27                = (1 << 27),
    CPE_IRQ_SYNTH_NO_LOCK        = (1 << 28),
    CPE_IRQ_MODULES_UNLOCKED     = (1 << 29),
    CPE_IRQ_BOOT_DONE            = (1 << 30),
    CPE_IRQ_INTERNAL_ERROR       = (1 << 31),
} rf_cpe_irq_t;

#define RFACKIFG_ACKFLAG 0x1
/** @} */

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
#define RFC_DBELL_BASE (PERIPH_BASE + 0x41000) /**< RFC_DBELL base address */
#define RFC_DBELL_BASE_NONBUF (PERIPH_BASE_NONBUF + 0x41000) /**< RFC_DBELL base address */
/** @} */

#define RFC_DBELL ((rfc_dbell_regs_t *) (RFC_DBELL_BASE)) /**< RFC_DBELL register bank */
#define RFC_DBELL_NONBUF ((rfc_dbell_regs_t *) (RFC_DBELL_BASE_NONBUF)) /**< RFC_DBELL register bank */

/**
 * @brief   RFC_PWR registers
 */
typedef struct {
    reg32_t PWMCLKEN; /**< RF Core Power Management and Clock Enable */
} rfc_pwr_regs_t;

/**
 * @brief   RFC_PWR definitions
 * @{
 */
#define PWMCLKEN_RFCTRC 0x400
#define PWMCLKEN_FSCA   0x200
#define PWMCLKEN_PHA    0x100
#define PWMCLKEN_RAT    0x80
#define PWMCLKEN_RFERAM 0x40
#define PWMCLKEN_MDMRAM 0x10
#define PWMCLKEN_MDM    0x8
#define PWMCLKEN_CPERAM 0x4
#define PWMCLKEN_CPE    0x2
#define PWMCLKEN_RFC    0x1
/** @} */

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
#define RFC_PWR_BASE (PERIPH_BASE + 0x40000) /**< RFC_PWR base address */
#define RFC_PWR_BASE_NONBUF (PERIPH_BASE_NONBUF + 0x40000) /**< RFC_PWR base address */
/** @} */

#define RFC_PWR ((rfc_pwr_regs_t *) (RFC_PWR_BASE)) /**< RFC_PWR register bank */
#define RFC_PWR_NONBUF ((rfc_pwr_regs_t *) (RFC_PWR_BASE_NONBUF)) /**< RFC_PWR register bank */

#ifdef __cplusplus
}
#endif

/** @} */
