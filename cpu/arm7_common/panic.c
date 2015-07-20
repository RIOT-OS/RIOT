/*
 * Copyright (C) 2014 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup cpu_arm7_common
 * @{
 *
 * @file
 * @brief       Crash handling functions implementation for ARM-based MCUs
 *
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 */

#include "lpm.h"

void panic_arch(void)
{
#if DEVELHELP
    /* enter infinite loop, into deepest possible sleep mode */
    while (1) {
        lpm_set(LPM_OFF);
    }
#endif
}
