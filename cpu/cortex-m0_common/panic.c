/*
 * Copyright (C) 2015 INRIA
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cortex-m0_common
 * @{
 *
 * @file
 * @brief       Crash handling functions implementation for ARM Cortex-based MCUs
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Joakim Gebart <joakim.gebart@eistec.se>
 */

#include "cpu.h"
#include "lpm.h"

void panic_arch(void)
{
#if DEVELHELP
    /* The bkpt instruction will signal to the debugger to break here. */
    __ASM("bkpt #0");
    /* enter infinite loop, into deepest possible sleep mode */
    while (1) {
        lpm_set(LPM_OFF);
    }
#endif
}
