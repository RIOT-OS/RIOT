/*
 * SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         cpu_rp2350
 * @{
 *
 * @file
 * @brief           Compatibility definitions for the USB device driver
 *
 * The USB controller of the RP2350 is the same as the one of the RP2040, so
 * the USB device driver in `cpu/common/rp2/usbdev.c` is used. This
 * file provides the register definitions of the Pico SDK and the helper
 * functions of the RP2040 port used by the driver.
 *
 * @author          Christopher Buechse
 */

#include "compat_layer.h"
#include "periph_cpu.h"
#include "regs/resets.h"
#include "regs/usb.h"
#include "regs/usb_device_dpram.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Helper functions of the RP2040 port used by the driver
 * @{
 */
#define io_reg_atomic_set(reg, mask)    atomic_set(reg, mask)
#define io_reg_atomic_clear(reg, mask)  atomic_clear(reg, mask)
#define periph_reset(mask)              atomic_set(&RESETS->RESET, mask)
#define periph_reset_done(mask)         reset_component(mask, mask)
#ifdef RP2350_USE_RISCV
#  define NVIC_EnableIRQ(irq)           rp_irq_enable(irq)
#  define cortexm_isr_end()             rp_end_isr()
#endif
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
