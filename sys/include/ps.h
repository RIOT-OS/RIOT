/*
 * SPDX-FileCopyrightText: 2010 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    sys_ps PS
 * @ingroup     sys
 * @brief       Show list with all threads
 * @{
 *
 * @file
 * @brief       List information about all active threads
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Print information to all active threads to stdout.
 */
void ps(void);

#ifdef __cplusplus
}
#endif

/** @} */
