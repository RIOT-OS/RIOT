/*
 * Copyright (C) 2015 Sebastian Sontberg <sebastian@sontberg.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_sam3s
 * @{
 *
 * @file
 * @brief           CPU specific values
 *
 * @author          Sebastian Sontberg <sebastian@sontberg.de>
 */

#ifndef CPU_CONF_H
#define CPU_CONF_H

#include "sam3s.h"

#define CPU_FLASH_BASE                  IFLASH_ADDR
#define CPU_DEFAULT_IRQ_PRIO            (1U)
#define CPU_IRQ_NUMOF                   (UDP_IRQn + 1)

#define BITBAND_SRAM_REF                (0x20000000)
#define BITBAND_SRAM_BASE               (0x22000000)

#define BITBAND_PERI_REF                (0x40000000)
#define BITBAND_PERI_BASE               (0x42000000)

/* Access to a peripheral register through bit banding */
#define BITBAND_REGADDR(Reg,Bit)                                        \
    (BITBAND_PERI_BASE +                                                \
     (32u*((uint32_t)&(Reg) - (uint32_t)BITBAND_PERI_REF)) +            \
     (4u*((uint32_t)(Bit))))

#endif /* CPU_CONF_H */
