/*
 * Copyright (C) 2015 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup x86_cpu
 * @{
 *
 * @file
 * @brief       Crash handling functions implementation for x86 port
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 */

#include "x86_reboot.h"
#include "lpm.h"

void panic_arch(void)
{
#if DEVELHELP
    /* enter infinite loop, into deepest possible sleep mode */
    while (1) {
        lpm_set(LPM_OFF);
    }
#else
    x86_shutdown();
#endif
}
