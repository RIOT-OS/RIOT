/*
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */
/**
 * @ingroup         cpu_gd32v
 * @{
 *
 * @file
 * @brief           Extra definitions for the GD32VF103 core missing from the
 *                  other vendor definitions
 *
 * @author          Koen Zandberg <koen@bergzand.net>
 */
#ifndef GD32VF103_CORE_H
#define GD32VF103_CORE_H

#include "vendor/gd32vf103.h"

#ifdef cplusplus
 extern "C" {
#endif

#define _REG32(p, i) 			    (*(volatile uint32_t *) ((p) + (i)))
#define _REG32P(p, i) 			    ((volatile uint32_t *) ((p) + (i)))

/**
 * @brief RISC-V ECLIC per interrupt configuration registers
 */
typedef struct __attribute((packed)) {
    volatile uint8_t ip;     /**< Interrupt pending */
    volatile uint8_t ie;     /**< Interrupt enable */
    volatile uint8_t attr;   /**< Interrupt attributes */
    volatile uint8_t ctl;    /**< Interrupt control */
} eclic_clicint_t;

/**
 * @name RISC-V ECLIC definitions
 * @{
 */
#define ECLIC_CTRL_ADDR      0xd2000000
#define ECLIC_REG(offset)    _REG32(ECLIC_CTRL_ADDR, offset)
#define ECLIC_REGP(offset)   _REG32P(ECLIC_CTRL_ADDR, offset)

#define ECLIC_CLICCFG        *((volatile uint8_t*)ECLIC_REGP(0x0))
#define ECLIC_CLICINFO       *((volatile uint32_t*)ECLIC_REGP(0x4)
#define ECLIC_CLICMTH        *((volatile uint8_t*)ECLIC_REGP(0xb)
#define ECLIC_CLICINT_ADDR   ECLIC_REGP(0x1000)
#define ECLIC_CLICINT        ((volatile eclic_clicint_t *)ECLIC_CLICINT_ADDR)
/** @} */


/**
 * @name RISC-V core timer definitions on the GD32V. No actual CLINT is present,
 * but the core timer implements similar register definitions.
 * @{
 */
#define CLINT_CTRL_ADDR             0xd1000000

#define CLINT_MTIME                 (0x0000U)
#define CLINT_MTIMECMP              (0x0008U)
#define CLINT_MSTOP                 (0x0FF8U)
#define CLINT_MSIP                  (0x0FFCU)

#define CLINT_REG(offset)           _REG32(CLINT_CTRL_ADDR, offset)
#define CLINT_FREQ_DIV              (4U)

/** @} */

#ifdef cplusplus
}
#endif

#endif /* GD32VF103_CORE_H */
