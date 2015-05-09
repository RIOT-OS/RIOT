/*
 * Copyright (C) 2014, 2015 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  core_util
 * @{
 *
 * @file
 * @brief       Crash handling header
 *
 * Define a ::core_panic function that allows to stop/reboot the system
 * when an unrecoverable problem has occurred.
 *
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 */

#ifndef PANIC_H
#define PANIC_H

#include "kernel.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @brief Handle an unrecoverable error by halting or rebooting the system
 *
 * A numeric code indicating the failure reason can be given
 * as the *crash_code* parameter.
 *
 * Detailing the failure is possible using the *message* parameter.
 * This function should serve a similar purpose as the panic()
 * function of Unix/Linux kernels.
 *
 * If the DEVELHELP macro is defined, the system will be halted;
 * the system will be rebooted otherwise.
 *
 * @warning this function NEVER returns!
 *
 * @param[in] crash_code    a unique code for identifying the crash reason
 * @param[in] message       a human readable reason for the crash
 *
 * @return                  this function never returns
 * */
NORETURN void core_panic(int crash_code, const char *message);

#ifdef __cplusplus
}
#endif

#endif /* PANIC_H */
/** @} */
