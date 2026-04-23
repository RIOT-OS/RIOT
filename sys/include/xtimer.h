/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 * Copyright (C) 2016 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    sys_xtimer xtimer high level timer abstraction layer (deprecated)
 * @ingroup     sys
 * @brief       Provides a high level timer module to register
 *              timers, get current system time, and let a thread sleep for
 *              a certain amount of time.
 *
 * The implementation takes one low-level timer and multiplexes it.
 *
 * Insertion and removal of timers has O(n) complexity with (n) being the
 * number of active timers.  The reason for this is that multiplexing is
 * realized by next-first singly linked lists.
 *
 * @deprecated  xtimer has been deprecated in favor of the @ref sys_ztimer
 * @note        With @ref sys_ztimer_xtimer_compat a compatibility wrapper is
 *              provided that in the vast majority of cases can function as a
 *              drop-in replacement. This compatibility wrapper is expected to
 *              replace `xtimer` in the near future and ensure that code still
 *              depending on the `xtimer` API continues to function.
 * @details     Note that at least for long running timers, using
 *              @ref sys_ztimer instead of the compatibility layer can yield
 *              lower clock drift and lower power consumption compared to
 *              using the compatibility layer. For details on how to achieve
 *              lower clock drift and lower power consumption please consult the
 *              @ref sys_ztimer documentation.
 *
 * @{
 * @file
 * @brief   xtimer interface definitions
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 * @author  Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

/**
 * @brief This macro is used to check whether internal headers have been
 * included via the public header
 */
#define XTIMER_H

#ifdef MODULE_ZTIMER64_XTIMER_COMPAT
#include "ztimer64/xtimer_compat.h" /* IWYU pragma: export */
#else
#include "ztimer/xtimer_compat.h" /* IWYU pragma: export */
#endif

/** @} */
