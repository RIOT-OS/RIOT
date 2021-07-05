/*
 * Copyright (C) 2021 Ishraq Ibne Ashraf <ishraq.i.ashraf@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup boards_raspberry-pi-pico
 * @{
 *
 * @file board.h
 * @brief Board specific definitions
 *
 * @author Ishraq Ibne Ashraf <ishraq.i.ashraf@gmail.com>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

// On-board green LED.
#define LED0_PIN GPIO_PIN(GPIO_BANK_USER, 25)

/**
 * @brief Initialise board specific hardware
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
