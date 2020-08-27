/*
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_sifive_e
 * @brief       Board specific definitions for the Qemu sifive_e RISC-V board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Qemu sifive_e RISC-V board
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef BOARD_H
#define BOARD_H

#include "macros/units.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    xtimer configuration
 * @{
 */
#define XTIMER_HZ                   (MHZ(10))
/** @} */

/**
 * @brief   Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
