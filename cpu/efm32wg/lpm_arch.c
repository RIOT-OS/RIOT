/*
 * lpm_arch.c
 *
 *  Created on: 3 wrz 2015
 *      Author: kkk
 */

/**
 * @ingroup     cpu_efm32wg
 * @{
 *
 * @file
 * @brief       Implementation of the kernels power management interface
 *
 * @author      Mateusz Kubaszek <mathir.km.riot@gmail.com>
 *
 * @}
 */

#include "cpu.h"
#include "arch/lpm_arch.h"
#include "emlib/inc/em_emu.h"
#include "board.h"

static enum lpm_mode current_mode = LPM_UNKNOWN;

void lpm_arch_init(void)
{
    current_mode = LPM_ON;
}

enum lpm_mode lpm_arch_set(enum lpm_mode target)
{
    enum lpm_mode last_mode = current_mode;

    switch(target) {
    case LPM_ON: /* EM0 mode */
        current_mode = LPM_ON;
        break;
    case LPM_IDLE: /* EM2 mode */
        /* Entering EM2 mode */
        /* High frequencies clocks are disabled, LFXO is still running */
        current_mode = LPM_IDLE;
#ifndef DISABLE_EM2
        EMU_EnterEM2(true);
#endif
        break;
    case LPM_SLEEP: /* EM2 mode */
        /* Entering EM2 mode */
        /* High frequencies clocks are disabled, LFXO is still running */
        current_mode = LPM_SLEEP;
#ifndef DISABLE_EM2
        EMU_EnterEM2(true);
#endif
        break;
    case LPM_POWERDOWN: /* not implemented */
        /* Fall-through */
    case LPM_OFF: /* not implemented */
        /* Fall-through */
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
    if(current_mode == LPM_SLEEP) {
        /* After stop mode, the clock system needs to be reconfigured */
        cpu_init();
    }
    current_mode = LPM_ON;
}

/** Not provided */
inline void lpm_arch_begin_awake(void)
{
}

/** Not provided */
inline void lpm_arch_end_awake(void)
{
}
