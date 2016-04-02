/*
 * Copyright (C) 2014  René Kijewski  <rene.kijewski@fu-berlin.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * Rebooting x86 boards.
 *
 * @ingroup x86
 * @{
 * @file
 * @author  René Kijewski <rene.kijewski@fu-berlin.de>
 */

#ifndef CPU__X86__REBOOT__H__
#define CPU__X86__REBOOT__H__

#include <stdbool.h>
#include "kernel_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Prototype for a x86 reboot function.
 *
 * The function may return, then a naive reboot is tried.
 * It is safe to call x86_kbc_reboot() inside the handler,
 * then a naive reboot will be done immediately.
 *
 * The handler is called with interrupts disabled.
 * You should keep the interrupts disabled and use busy loops.
 */
typedef void (*x86_reboot_t)(void);

/**
 * @brief   Reboots the system.
 *
 * First the function supplied if x86_set_reboot_fun() is called (iff applies).
 * Then a reboot using the keyboard controller is tried.
 * And if everything fails, a tripple fault is provoked.
 */
void NORETURN x86_kbc_reboot(void);

/**
 * @brief   Set specialized reboot function.
 *
 * This function should be utilized by the ACPI subsystem.
 */
void x86_set_reboot_fun(x86_reboot_t fun);

/**
 * @brief   Loads an empty interrupt descriptor table.
 *
 * Any interrupt that will occur causes a tripple fault, i.e. a reboot.
 * Must be called with interrupts disabled.
 */
void x86_load_empty_idt(void);

/**
 * @brief     Prototype of the board specific x86 shutdown function.
 * @returns   `false` if the shutdown could not be scheduled.
 */
typedef bool (*x86_shutdown_t)(void);

/**
 * @brief     Shutdown and power off
 * @details   May return. The shutdown only gets scheduled.
 * @returns   `false` if shutting down is not possible at this moment.
 */
bool x86_shutdown(void);

/**
 * @brief   Function to call on x86_shutdown()
 */
void x86_set_shutdown_fun(x86_shutdown_t fun);

#ifdef __cplusplus
}
#endif

#endif

/** @} */
