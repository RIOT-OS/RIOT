/*
 * Copyright (C) 2017 Gilles DOFFE <g.doffe@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @ingroup cpu_native
 * @ingroup drivers_periph_qdec
 * @brief   Low-level QDEC driver implementation
 * @author  Gilles DOFFE <g.doffe@gmail.com>
 */

#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include <errno.h>
#include <assert.h>

#include "cpu.h"
#include "cpu_conf.h"
#include "native_internal.h"
#include "periph/qdec.h"
#include "periph/gpio.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifdef QDEC_NUMOF

#define NATIVE_QDEC_MAX     (0x7FFFFFFFL)

/* QDEC devices */
uint32_t qdecs[QDEC_NUMOF];
/* QDEC values for each device, should be set externally */
int32_t  qdecs_value[QDEC_NUMOF];

int32_t qdec_init(qdec_t qdec, qdec_mode_t mode, qdec_cb_t cb, void *arg)
{
    /* no interrupt needed since it is externally incremented */
    (void)cb;
    (void)arg;

    /* Verify parameters */
    assert((qdec < QDEC_NUMOF));

    /* Count on A (TI1) signal edges, B (TI2) signal edges or both,
     * default to EINVAL (Invalid argument).
     */
    switch (mode) {
        /* X1 mode */
        case QDEC_X1:
            break;
        case QDEC_X2:
        case QDEC_X4:
        default:
            errno = EINVAL;
            goto err_invalid_mode;
    }

    /* Initialize qdec channels */
    qdecs[qdec] = 0;

    /* Reset counter and start qdec */
    qdec_start(qdec);

    return 0;

/* Error management */
err_invalid_mode:
    return errno;
}

/* Return QDEC value */
inline int32_t qdec_read_and_reset(qdec_t qdec)
{
    int32_t count = 0;

    count = qdecs_value[qdec];
    qdecs_value[qdec] = 0;

    return count;
}

/* Return QDEC value */
inline int32_t qdec_read(qdec_t qdec)
{
    return qdecs_value[qdec];
}

/* Empty functions to keep API */
void qdec_start(qdec_t qdec)
{
    (void)qdec;
}

void qdec_stop(qdec_t qdec)
{
    (void)qdec;
}

#endif /* QDEC_NUMOF */
