/*
 * Copyright (C) 2023 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_msp430_common
 * @{
 *
 * @file
 * @brief       MSP430 specific busy wait
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

/* MSP430 uses ztimer_spin for busy wait */

#endif

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* WAIT_ARCH_H */
