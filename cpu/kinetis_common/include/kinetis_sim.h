/*
 * Copyright (C) 2015 Freie Universität Berlin
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


#ifndef __K20_SIM_H
#define __K20_SIM_H

#include <stdint.h>
#include <stdbool.h>

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Enables a clock gate
 *
 * @see k20_clock_gate_disable(k20_clock_gate_t)
 * @param[in]  cgate    The clock gate to enable
 *
 * @return 1 if setting the state was successful, 0 otherwise.
 */
int kinetis_clock_gate_enable(kinetis_clock_gate_t cgate);

/**
 * @brief Disables a clock gate
 *
 * @see k20_clock_gate_enable(k20_clock_gate_t)
 * @param[in]  cgate    The clock gate to disable
 *
 * @return 1 if setting the state was successful, 0 otherwise.
 */
int kinetis_clock_gate_disable(kinetis_clock_gate_t cgate);

/**
 * @brief Gets a clock gate value
 *
 * @return 1 if the clock gate is open, 0 if it is closed, -1 if an error occured (clockgate does not exist)
 */
int kinetis_clock_gate_get(kinetis_clock_gate_t cgate);

#ifdef __cplusplus
}
#endif

#endif /* __K20_SIM_H */
