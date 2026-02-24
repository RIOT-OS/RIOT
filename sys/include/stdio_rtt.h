/*
 * SPDX-FileCopyrightText: 2016 Michael Andersen <m.andersen@berkeley.edu>
 * SPDX-FileCopyrightText: 2018 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     sys_stdio_rtt
 *
 * @{
 * @file
 *
 * @author      Michael Andersen <m.andersen@cs.berkeley.edu>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include "stdio_base.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief enable stdin polling, at a power consumption cost. This is enabled
 *        by default unless STDIO_RTT_DISABLE_STDIN is defined.
 */
void rtt_stdio_enable_stdin(void);

/**
 * @brief enable stdout blocking and free space polling. This must be done
 *        with caution because if there is no RTT client attached, all
 *        writes to stdout will block indefinitely. This can be enabled
 *        automatically by defining STDIO_RTT_ENABLE_BLOCKING_STDOUT
 */
void rtt_stdio_enable_blocking_stdout(void);

#ifdef __cplusplus
}
#endif

/** @} */
