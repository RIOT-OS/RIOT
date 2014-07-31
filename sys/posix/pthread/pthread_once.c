/*
 * POSIX implementation of threading.
 *
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
    if (*once_control == PTHREAD_ONCE_INIT) {
        init_routine();
    }

    *once_control = PTHREAD_ONCE_INIT + 1;

    return 0;
}
