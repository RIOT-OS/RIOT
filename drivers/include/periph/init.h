/*
 * Copyright (C) 2017 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_periph_init Common peripheral initialization
 * @ingroup     drivers_periph
 * @brief       Common static peripheral driver initialization
 *
 * This interface provides a central hook for any static peripheral
 * initialization that might be needed. Typical drivers that need this are
 * shared peripherals like SPI and I2C.
 *
 * @{
 * @file
 * @brief       Common peripheral driver initialization interface
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_INIT_H
#define PERIPH_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Common peripheral initialization function
 *
 * This function should call all needed static initialization functions for
 * configured peripheral drivers like SPI or I2C. This function SHOULD be called
 * early in the boot process, e.g. before the actual kernel initialization is
 * started.
 */
void periph_init(void);

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_INIT_H */
/** @} */
