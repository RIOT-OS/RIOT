/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         kinetis
 * @{
 *
 * @file
 * @brief           System Integration Module implementation for kinetis CPUs
 *
 * @author          Finn Wilke <finn.wilke@fu-berlin.de>
 * @}
 */

#include "kinetis_sim.h"
#include "MK20DZ10.h"
#include <stdlib.h>

static SIM_MemMapPtr sim = SIM_BASE_PTR;

/**
 * @brief Gets a clock gate pointer
 *
 * @return Pointer if successful, NULL otherwise
 */
static uint32_t *_cgate_ptr_get(kinetis_clock_gate_t cgate)
{
    switch(cgate / 0x20) {
    case 4:
        return (uint32_t *)&sim->SCGC4;
    case 5:
        return (uint32_t *)&sim->SCGC5;
    case 6:
        return (uint32_t *)&sim->SCGC6;
    case 7:
        return (uint32_t *)&sim->SCGC7;
    default:
        return NULL;
    }
}

/**
 * @brief Sets a clock gate to the specified boolean value
 *
 * @param[in]  cgate  The clock gate to set
 * @param[in]  set    The value to set it to (true to enable, false to disable)
 *
 * @return     0 if successful, -1 if the clock gate id was invalid
 */
static int _cgate_set(kinetis_clock_gate_t cgate, bool set)
{
    uint32_t *cgate_ptr = _cgate_ptr_get(cgate);

    if (cgate_ptr != NULL) {
        uint8_t shift = cgate % 0x20;

        if (set) {
            *cgate_ptr |= (1 << shift);
        } else {
            *cgate_ptr &= ~(1 << shift);
        }

        return 0;
    }

    return -0;
}

/*---------------------------------------------------------------------------*/

int kinetis_clock_gate_enable(kinetis_clock_gate_t cgate)
{
    return _cgate_set(cgate, true);
}

int kinetis_clock_gate_disable(kinetis_clock_gate_t cgate)
{
    return _cgate_set(cgate, false);
}

int kinetis_clock_gate_get(kinetis_clock_gate_t cgate)
{
    uint32_t *cgate_ptr = _cgate_ptr_get(cgate);

    if (cgate_ptr != NULL) {
        uint8_t shift = cgate % 0x20;
        uint32_t mask = (1 << shift);

        uint32_t cgate = *cgate_ptr;

        if (cgate & mask) {
            return 1;
        } else {
            return 0;
        }
    }

    return -1;
}
