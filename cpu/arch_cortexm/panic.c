/*
 * Copyright (C) 2015 INRIA
 * Copyright (C) 2015 Eistec AB
 * Copyright (C) 2016 OTA keys
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cortex-m_common
 * @{
 *
 * @file
 * @brief       Crash handling functions implementation for ARM Cortex-based MCUs
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Toon Stegen <toon.stegen@altran.com>
 */

#include "cpu.h"
#include "log.h"

#ifdef DEVELHELP
static void print_ipsr(void)
{
    uint32_t ipsr = __get_IPSR() & IPSR_ISR_Msk;
    if(ipsr) {
        /* if you get here, you might have forgotten to implement the isr
         * for the printed interrupt number */
        LOG_ERROR("Inside isr %d\n", ((int)ipsr) - 16);
    }
}
#endif

void panic_arch(void)
{
#ifdef DEVELHELP
    print_ipsr();
    /* The bkpt instruction will signal to the debugger to break here. */
    __asm__("bkpt #0");
#endif
}
