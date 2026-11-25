/*
 * SPDX-FileCopyrightText: 2026 Klim Evdokimov <klimevdokimov@mail.ru>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         cpu_riscv_common
 * @{
 *
 * @file
 * @brief           WCH QingKe PFIC (Programmable Fast Interrupt Controller)
 *                  definitions
 *
 * The PFIC is used by the WCH CH32V family. The trap vector is used in
 * non-vectored mode: every interrupt enters the common trap handler and
 * `mcause` holds the PFIC interrupt number (0-15 core, 16+ peripheral).
 *
 * RISCV implementations using this peripheral must define
 * `PFIC_NUM_INTERRUPTS`. `PFIC_BASE_ADDR` defaults to the address used on all
 * QingKe cores.
 *
 * @author          Klim Evdokimov <klimevdokimov@mail.ru>
 */

#include <stdint.h>

#include "cpu_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PFIC_BASE_ADDR
/**
 * @brief   PFIC base address
 */
#  define PFIC_BASE_ADDR            (0xE000E000UL)
#endif

/**
 * @brief   PFIC callback declaration
 *
 * @param   irq     Interrupt number
 */
typedef void (*pfic_isr_cb_t)(unsigned irq);

/**
 * @brief   PFIC register map
 */
typedef struct {
    volatile const uint32_t ISR[8];     /**< Interrupt enable status */
    volatile const uint32_t IPR[8];     /**< Interrupt pending status */
    volatile uint32_t ITHRESDR;         /**< Interrupt priority threshold */
    volatile uint32_t RESERVED;         /**< Reserved */
    volatile uint32_t CFGR;             /**< Interrupt configuration */
    volatile const uint32_t GISR;       /**< Global interrupt status */
    volatile uint8_t VTFIDR[4];         /**< VTF interrupt ID */
    uint8_t RESERVED0[12];              /**< Reserved */
    volatile uint32_t VTFADDR[4];       /**< VTF interrupt address */
    uint8_t RESERVED1[0x90];            /**< Reserved */
    volatile uint32_t IENR[8];          /**< Interrupt enable set */
    uint8_t RESERVED2[0x60];            /**< Reserved */
    volatile uint32_t IRER[8];          /**< Interrupt enable clear */
    uint8_t RESERVED3[0x60];            /**< Reserved */
    volatile uint32_t IPSR[8];          /**< Interrupt pending set */
    uint8_t RESERVED4[0x60];            /**< Reserved */
    volatile uint32_t IPRR[8];          /**< Interrupt pending clear */
    uint8_t RESERVED5[0x60];            /**< Reserved */
    volatile uint32_t IACTR[8];         /**< Interrupt active status */
    uint8_t RESERVED6[0xE0];            /**< Reserved */
    volatile uint8_t IPRIOR[256];       /**< Interrupt priority */
    uint8_t RESERVED7[0x810];           /**< Reserved */
    volatile uint32_t SCTLR;            /**< System control */
} pfic_t;

/**
 * @brief   PFIC register access
 */
#define PFIC                        ((pfic_t *)PFIC_BASE_ADDR)

/**
 * @brief Initialize the PFIC interrupt controller
 */
void pfic_init(void);

/**
 * @brief Enable a single interrupt
 *
 * @param   irq         Interrupt number to enable
 * @param   priority    Priority level to configure (lower value means
 *                      higher priority)
 */
void pfic_enable_interrupt(unsigned irq, unsigned priority);

/**
 * @brief Disable a single interrupt
 *
 * @param   irq     Interrupt number to disable
 */
void pfic_disable_interrupt(unsigned irq);

/**
 * @brief Set the priority of an interrupt
 *
 * @param   irq         Interrupt number to configure
 * @param   priority    Priority level to configure
 */
void pfic_set_priority(unsigned irq, unsigned priority);

/**
 * @brief Set an interrupt pending
 *
 * @param   irq     Interrupt number
 */
void pfic_set_pending(unsigned irq);

/**
 * @brief Clear the pending state of an interrupt
 *
 * @param   irq     Interrupt number
 */
void pfic_clear_pending(unsigned irq);

/**
 * @brief Set the handler for an interrupt
 *
 * @param   irq     Interrupt number to configure
 * @param   cb      Callback handler to configure
 */
void pfic_set_handler(unsigned irq, pfic_isr_cb_t cb);

/**
 * @brief PFIC interrupt handler
 *
 * @internal
 *
 * @param   irq     Interrupt number to call the handler for
 */
void pfic_isr_handler(uint32_t irq);

#ifdef __cplusplus
}
#endif

/** @} */
