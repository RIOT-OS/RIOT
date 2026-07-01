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

/**
 * @brief   RIOT ISR callback for MicroPython
 *
 * @param[in] callback    Info to pass on to the callback.
 */
void mp_riot_isr_callback(void *callback);
