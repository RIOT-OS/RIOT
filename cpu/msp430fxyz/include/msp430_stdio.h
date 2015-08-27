/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_msp430fxyz
 *
 * @{
 * @file
 * @brief       STDIO over UART for MSP430 platforms
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef MSP430_STDIO_H
#define MSP430_STDIO_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialize the STDIO data structured and the underlying UART driver
 *          as define in board.h
 */
void msp430_stdio_init(void);

#ifdef __cplusplus
}
#endif

#endif /* MSP430_STDIO_H */
/** @} */
