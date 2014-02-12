/*
 * Copyright (C) 2014 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @addtogroup  core_util
 * @{
 *
 * @file        crash.h
 * @brief       Crash handling header
 *
 * Define a panic() function that allows to stop/reboot the system
 *  when an unrecoverable problem has occured.
 *
 * @author      Kévin Roussel <Kevin.Roussel@inria.fr>
 */

#ifndef __CRASH_H
#define __CRASH_H

#include "kernel.h"

/** Handle an unrecoverable error by halting or rebooting the system.
    A numeric code indicating the failure reason can be given
    as the ::crash_code parameter.
    Detailing the failure is possible using the ::message parameter.
    This function should serve a similar purpose than the panic()
    function of Unix/Linux kernels.

    if DEVELHELP macro is defined, system will be halted;
    system will be rebooted otherwise.

    WARNING: this function NEVER returns! */
NORETURN void core_panic(int crash_code, const char *message);

/** @} */
#endif /* __CRASH_H */
