/*
 * Copyright (C) 2023 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_arm7_common
 * @{
 *
 * @file
 * @brief       ARM7 specific busy wait
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

/* ARM7 uses ztimer_spin for busy wait */

#endif

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* WAIT_ARCH_H */
