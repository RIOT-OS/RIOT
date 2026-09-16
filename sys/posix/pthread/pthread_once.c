/*
 * SPDX-FileCopyrightText: 2013 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup pthread
 * @{
 * @file
 * @brief   Singletons features / single-shot execution.
 * @author  Christian Mehlis <mehlis@inf.fu-berlin.de>
 * @author  René Kijewski <kijewski@inf.fu-berlin.de>
 * @}
 */

#include "pthread.h"

int pthread_once(pthread_once_t *once_control, void (*init_routine)(void))
{
    if (!once_control->init_executed) {
        init_routine();
    }

    once_control->init_executed = 1;

    return 0;
}
