/*
 * Copyright (C) 2023 Gunar Schorcht <gunar@schorcht.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_sipeed_longan_nano
 * @{
 *
 * @file
 * @brief       Board specific definitions for the SeeedStudio GD32 RISC-V board
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"
#include "periph_common_conf.h"

#include "cfg_timer_default.h"
#include "cfg_uart_default.h"

#ifndef CONFIG_BOARD_HAS_HXTAL
#define CONFIG_BOARD_HAS_HXTAL  1   /**< The board provides a high frequency oscillator. */
#endif

#ifndef CONFIG_BOARD_HAS_LXTAL
#define CONFIG_BOARD_HAS_LXTAL  1   /**< The board provides a low frequency oscillator. */
#endif

#ifndef CONFIG_CLOCK_HXTAL
#define CONFIG_CLOCK_HXTAL      MHZ(8)      /**< HXTAL frequency */
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
