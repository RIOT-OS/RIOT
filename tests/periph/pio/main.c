/*
 * Copyright (C) 2021 Otto-von-Guericke Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test application for the PIO peripheral driver
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 *
 * @}
 */

#include <stddef.h>
#include "shell.h"
#include "periph_conf.h"
#include "periph/pio.h"
#include "pio/pio.h"
#define ENABLE_DEBUG    0
#include "debug.h"

#if ENABLE_DEBUG
#define DEBUG_TEST_FAILED(msg, ...) \
DEBUG("[Test PIO] %s failed at %d: " msg "\n", __func__, __LINE__, __VA_ARGS__);
#else
#define DEBUG_TEST_FAILED(msg, ...)
#endif

static int _test_pio_alloc_and_free(pio_t pio)
{
    int error = 1;
    pio_program_t pro;
    for (int i = 0; i < PIO_INSTR_NUMOF; i++) {
        pro = (pio_program_t){.location = PIO_PROGRAM_NOT_LOADED, .instr_numof = 1};
        if (pio_alloc_program(pio, &pro)) {
            DEBUG_TEST_FAILED("Could not allocate program at %d", i);
            goto CLEAN;
        }
    }
    if (!pio_alloc_program(pio, &pro)) {
        DEBUG_TEST_FAILED("Program impossibly allocated");
        goto CLEAN;
    }
    pio_free_program(pio, &pro);
    pro.location = PIO_PROGRAM_NOT_LOADED;
    if (pio_alloc_program(pio, &pro)) {
        DEBUG_TEST_FAILED("Program could not be allocated after free");
        goto CLEAN;
    }
    error = 0;
CLEAN:
    for (int i = 0; i < PIO_INSTR_NUMOF; i++) {
        pro = (pio_program_t){.location = i, .instr_numof = 1};
        pio_free_program(pio, &pro);
    }
    return error;
}

static int _test_pio_sm_lock_unlock(pio_t pio)
{
    int error = 1;
    pio_sm_t sm;
    for (pio_sm_t i = 0; i < PIO_SM_NUMOF; i++) {
        if ((sm = pio_sm_lock(pio)) < 0) {
            DEBUG_TEST_FAILED("Could not lock state machine %d", i);
            goto CLEAN;
        }
    }
    if (pio_sm_lock(pio) >= 0) {
        DEBUG_TEST_FAILED("State machine impossibly locked");
        goto CLEAN;
    }
    pio_sm_unlock(pio, sm);
    if (sm != pio_sm_lock(pio)) {
        DEBUG_TEST_FAILED("State machine could not be locked after unlock");
        goto CLEAN;
    }
    error = 0;
CLEAN:
    for (pio_sm_t i = 0; i < PIO_SM_NUMOF; i++) {
        pio_sm_unlock(pio, i);
    }
    return error;
}

static int _test_pio_sm_program_any(void)
{
    int error = 1;
    pio_t pio;
    pio_sm_t sm;
    pio_program_t pro = {.location = PIO_PROGRAM_NOT_LOADED, .instr_numof = PIO_INSTR_NUMOF};
    if (pio_alloc_program_sm_lock_any(&pio, &sm, &pro)) {
        DEBUG_TEST_FAILED("Could not allocate program for any state machine");
        goto CLEAN;
    }
    error = 0;
CLEAN:
    pio_sm_unlock(pio, sm);
    pio_free_program(pio, &pro);
    return error;
}

int main(void)
{
    int error = 0;

    for (pio_t pio = 0; pio < PIO_NUMOF; pio++) {
        error = _test_pio_alloc_and_free(pio) ? 1 : error;
        error = _test_pio_sm_lock_unlock(pio) ? 1 : error;
    }
    error = _test_pio_sm_program_any() ? 1 : error;

    puts(error ? "TEST FAILED!" : "TEST SUCCEEDED!");
    return 0;
}
