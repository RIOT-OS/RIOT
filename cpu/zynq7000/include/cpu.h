/*
 * Copyright (C) 2015 Xsilon Ltd
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_zynq7000
 * @{
 *
 * @file            cpu.h
 * @brief           CPU specific
 *
 * @author          Simon Vincent <simon.vincent@xsilon.com>
 */

#ifndef __CPU_H
#define __CPU_H

#include "zynq7000.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief   UART0 buffer size definition for compatibility reasons
     *
     * TODO: remove once the remodeling of the uart0 driver is done
     * @{
     */
#ifndef UART0_BUFSIZE
#define UART0_BUFSIZE                   (128)
#endif
    /** @} */

#define ARCH_HAS_ATOMIC_COMPARE_AND_SWAP 1

    /* Zynq bus frequency */
#define F_BUS               (100000000UL)

void cpu_init (void);

void NVIC_SystemReset(void);

extern volatile int _in_isr;

#ifdef __cplusplus
}
#endif

#endif
