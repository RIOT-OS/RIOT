/*
 * Copyright (C) 2015 Engineering-Spirit
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_stm32f2
 * @{
 *
 * @file
 * @brief       Implementation of the kernels power management interface
 *
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 * @author      Nick v. IJzendoorn <nijzndoorn@engineering-spirit.nl>
 *
 * @}
 */

#include "cpu.h"
#include "arch/lpm_arch.h"

static enum lpm_mode current_mode = LPM_UNKNOWN;

void lpm_arch_init(void)
{
    current_mode = LPM_ON;
}

enum lpm_mode lpm_arch_set(enum lpm_mode target)
{
    enum lpm_mode last_mode = current_mode;

    switch (target) {
        case LPM_ON:                    /* STM Run mode */
            break;
        case LPM_IDLE:                  /* STM Sleep mode */
            break;
        case LPM_SLEEP:                 /* STM Stop mode */
            break;
        case LPM_POWERDOWN:             /* STM Standby mode */
            /* Fall-through */
        case LPM_OFF:                   /* STM Standby mode */
            break;
        default:
            break;
    }

    return last_mode;
}

enum lpm_mode lpm_arch_get(void)
{
    return current_mode;
}

void lpm_arch_awake(void)
{
    if (current_mode == LPM_SLEEP) {
        /* After stop mode, the clock system needs to be reconfigured */
        cpu_init();
    }
    current_mode = LPM_ON;
}

/** Not provided */
inline void lpm_arch_begin_awake(void) { }

/** Not provided */
inline void lpm_arch_end_awake(void) { }
