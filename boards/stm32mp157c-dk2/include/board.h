/*
 * Copyright (C) 2020 Savoir-faire Linux
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_stm32mp157c-dk2
 * @{
 *
 * @file
 * @brief       Board specific definitions for the STM32MP157C-DK2 board
 *
 * @author      Gilles DOFFE <gilles.doffe@savoirfairelinux.com>
 *
 * @}
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize board specific hardware
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
