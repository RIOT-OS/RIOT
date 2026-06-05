/*
 * SPDX-FileCopyrightText: 2019 Kaspar Schleiser <kaspar@schleiser.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     pkg_micropython
 * @{
 *
 * @file
 * @brief       MicroPython RIOT port core glue declarations
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#pragma once

#include "py/obj.h"

/**
 * @brief   Call a MicroPython callable from RIOT C code, safely
 *
 * Invokes @p callback with no arguments inside an NLR frame, so that any
 * uncaught Python exception is printed instead of propagating into the calling
 * C code. Must be called from thread context (not from an ISR); use
 * mp_sched_schedule_node() to defer ISR work to such a context.
 *
 * @param[in] callback    MicroPython callable to invoke with no arguments.
 */
void mp_riot_call_function_safe(mp_obj_t callback);
