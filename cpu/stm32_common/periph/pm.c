/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
 *               2015 Engineering-Spirit
 *               2016 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32_common
 * @{
 *
 * @file
 * @brief       Low-level PWM driver implementation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 * @author      Nick v. IJzendoorn <nijzndoorn@engineering-spirit.nl>
 *
 * @}
 */

#include "cpu.h"
#include "periph_cpu.h"

void pm_set(unsigned mode)
{
    int deepsleep = 1;

    switch (mode) {
        case PM_0_SLEEP:
            deepsleep = 0;
            break;
        case PM_1_STOP:
            /* enter stop mode on deepsleep with voltage regulator on */
            PWR->CR &= ~(PWR_CR_PDDS | PWR_CR_LPDS);
            break;
        case PM_2_STANDBY:
            /* enter standby mode on deepsleep with voltage regulator off and
             * clear flags */
            PWR->CR |= (PWR_CR_PDDS | PWR_CR_CWUF | PWR_CR_CSBF);
            /* Enable WKUP pin to use for wakeup from standby mode */
            PWR->CSR |= PWR_CSR_EWUP;
            break;
        default:
            /* unknown mode -> do nothing */
            return;
    }

    /* call the shared function to set the CPU into the selected sleep mode */
    cortexm_sleep(deepsleep);
}
