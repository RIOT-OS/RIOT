/*
 * Copyright (C) 2020 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_stdio_semihosting STDIO over Semihosting
 * @ingroup     sys
 *
 * @brief       Standard input/output backend using ARM Semihosting
 *
 * The ARM Semihosting provides an STDIO backend using the ARM Semihosting
 * protocol. The main advantage of Semihosting is that is allows STDIO over the
 * SWD/JTAG debugging interface already available on ARM microcontrollers.
 *
 * ARM Semihosting works by using the breakpoint instructing to trigger the
 * debugger to read the output or to write the input chars. Please be aware that
 * this might skew the timing of your application.
 *
 * The main disadvantage of Semihosting is that it is relative slow (even when
 * compared to serial uart), and that it requires an active debug session to
 * handle the breakpoint instructions. Without an active debug session the CPU
 * will halt on the first STDIO activity until the breakpoint is handled by the
 * debugger. Don't forget to disable the Semihosting module or replace it with
 * stdio_null when switching to production builds.
 *
 * As this is an ARM specific protocol, this module will only work on ARM-based
 * microcontrollers.
 *
 * ## Usage
 *
 * Enable Semihosting-based stdio by adding the following module to your
 * makefile:
 *
 * ```
 * USEMODULE += stdio_semihosting
 * ```
 *
 * If semihosting is not the default stdio mechanism of your board, the
 * `RIOT_TERMINAL` variable has to be set to `semihosting`:
 *
 * ```
 * make term RIOT_TERMINAL=semihosting
 * ```
 *
 * Launching the terminal will start an OpenOCD session with semihosting
 * enabled. This can be used for both STDIO interaction and for debugging the
 * firmware.
 *
 * @{
 * @file
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef STDIO_SEMIHOSTING_H
#define STDIO_SEMIHOSTING_H

#include "stdio_base.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enable reception for Semihosting
 *
 * Automatically enabled when including the `stdin` module
 */
#define STDIO_SEMIHOSTING_RX            (IS_USED(MODULE_STDIN))

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* STDIO_SEMIHOSTING_H */
