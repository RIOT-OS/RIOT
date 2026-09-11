/*
 * SPDX-FileCopyrightText: 2026 Ajit Upadhyay <u.ajiit@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     sys_msi
 * @{
 *
 * @file
 * @brief       Cortex-M NVIC backend for MSI-like doorbells
 *
 * @author      Ajit Upadhyay <u.ajiit@gmail.com>
 *
 * @}
 */

#include <errno.h>

#include "cpu.h"
#include "msi_arch.h"

static IRQn_Type _doorbell_irq;
static int _initialized;

/**
 * @brief   Configure an external NVIC interrupt as the doorbell
 *
 * @details
 * The doorbell identifier is interpreted as a Cortex-M external interrupt
 * number. When replacing an existing doorbell, its NVIC line is disabled and
 * any pending state is cleared before the new line is enabled.
 *
 * Global interrupt exclusion is provided by the architecture-independent
 * caller.
 *
 * @param[in] doorbell  Cortex-M external interrupt number
 *
 * @retval  0           The NVIC doorbell was configured
 * @retval  -EINVAL     @p doorbell is outside the CPU IRQ range
 */
int msi_arch_init(int doorbell)
{
    if ((doorbell < 0) || (doorbell >= (int)CPU_IRQ_NUMOF)) {
        return -EINVAL;
    }

    if (_initialized) {
        NVIC_DisableIRQ(_doorbell_irq);
        NVIC_ClearPendingIRQ(_doorbell_irq);
    }

    _doorbell_irq = (IRQn_Type)doorbell;
    NVIC_ClearPendingIRQ(_doorbell_irq);
    NVIC_EnableIRQ(_doorbell_irq);
    _initialized = 1;

    return 0;
}

/**
 * @brief   Pend the configured NVIC interrupt
 */
void msi_arch_trigger(void)
{
    NVIC_SetPendingIRQ(_doorbell_irq);
}
