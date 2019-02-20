/*
 * Copyright (C) 2017 Eistec AB
 *               2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    boards_common_kw41z KW41Z common
 * @ingroup     boards_common
 * @brief       Shared files and configuration for KW41Z based boards.
 * @{
 *
 * @file
 * @brief       Shared configuration for some kw41z-based boards
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef BOARD_COMMON_H
#define BOARD_COMMON_H

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief   Initialize board specific hardware, including clock, LEDs and standard I/O
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_COMMON_H */
/** @} */
