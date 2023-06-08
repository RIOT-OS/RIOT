/*
 * Copyright (C) 2023 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_cortexm_common
 * @{
 *
 * @file
 * @brief       Cortex-M specific busy wait
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>
 *
 */

#ifndef WAIT_ARCH_H
#define WAIT_ARCH_H

#include <stdint.h>

#include "board.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif
#ifndef DOXYGEN

#if defined(CPU_CORE_CORTEX_M4) || defined(CPU_CORE_CORTEX_M4F)
#  define CPU_CYCLES_PER_WAIT_LOOP_ITERATION 4U
#endif

#if defined(CPU_CORE_CORTEX_M3)
#  define CPU_CYCLES_PER_WAIT_LOOP_ITERATION 7U
#endif

/* Cortex M uses ztimer_spin for busy wait */

#endif

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* WAIT_ARCH_H */
