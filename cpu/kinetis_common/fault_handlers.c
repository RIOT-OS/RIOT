/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_kinetis_common_fhandlers
 * @{
 *
 * @file
 * @brief       Fault Handlers for Freescale Kinetis MCUs
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Johann Fischer <j.fischer@phytec.de>
 *
 * @}
 */

#include <stdint.h>
#include "panic.h"
#include "fault_handlers.h"

void isr_nmi(void)
{
    core_panic(PANIC_NMI_HANDLER, "NMI HANDLER");
}

void isr_hard_fault(void)
{
    core_panic(PANIC_HARD_FAULT, "HARD FAULT");
}

void isr_mem_manage(void)
{
    core_panic(PANIC_MEM_MANAGE, "MEM MANAGE HANDLER");
}

void isr_bus_fault(void)
{
    core_panic(PANIC_BUS_FAULT, "BUS FAULT");
}

void isr_usage_fault(void)
{
    core_panic(PANIC_USAGE_FAULT, "ISR USAGE FAULT");
}

void isr_debug_mon(void)
{
    core_panic(PANIC_DEBUG_MON, "DEBUG MON HANDLER");
}

void isr_unhandled(void)
{
    core_panic(PANIC_UNHANDLED_ISR, "UNHANDLED ISR");
}
