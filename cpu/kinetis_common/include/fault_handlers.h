/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    cpu_kinetis_common_fhandlers Kinetis Fault Handlers
 * @ingroup     cpu_kinetis_common
 * @brief       Fault Handlers for Freescale Kinetis MCUs.
 *
 * @{

 * @file
 * @brief       Interface definition for the Kinetis Fault Handlers.
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Johann Fischer <j.fischer@phytec.de>
 */

#ifndef FAULT_HANDLERS_H
#define FAULT_HANDLERS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Non Maskable Interrupt
 */
void isr_nmi(void);

/**
 * @brief HardFault
 */
void isr_hard_fault(void);

/**
 * @brief MemManage
 */
void isr_mem_manage(void);

/**
 * @brief BusFault
 */
void isr_bus_fault(void);

/**
 * @brief UsageFault
 */
void isr_usage_fault(void);

/**
 * @brief Debug Interrupt
 */
void isr_debug_mon(void);

/**
 * @brief Default handler, called in case no interrupt handler was defined.
 */
void isr_unhandled(void);

#ifdef __cplusplus
}
#endif

#endif /* FAULT_HANDLERS_H */
/** @} */
