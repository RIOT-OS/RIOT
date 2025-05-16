/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    drivers_periph_hwrng HWRNG Abstraction
 * @ingroup     drivers_periph
 * @brief       Peripheral hardware random number generator interface
 *
 * The HWRNG interface abstracts means provided by MCU peripherals to create
 * random number. On most platforms, these peripherals are called something like
 * 'hardware random number generator' or 'pseudo random number generator'. The
 * quality of the created random data does differ quite a bit between different
 * MCUs, so please refer to your MCUs reference manual for information on this!
 *
 * @note    Refer to your platforms MCU reference manual for information on the
 *          quality of the used (pseudo) random number generator!
 *
 * # (Low-) Power Implications
 *
 * The HWRNG implementation **should** consume no additional power while no read
 * operation is in progress. This means, that the HWRNG peripheral should be
 * disabled (e.g. through peripheral clock gating) after the initialization and
 * that it **should** only be turned on while hwrng_read() is active.
 *
 * If the implementation puts the active thread to sleep during hwrng_read(), it
 * might need to block certain power modes on some platforms during this time.
 *
 * @{
 * @file
 * @brief       Hardware random number generator driver interface
 *
 * @author      Christian Mehlis <mehlis@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialize the hardware random number generator
 *
 * On some platforms, the random number generator needs some global
 * initialization before it can be used. This should happen in this function
 * if it would impose too much overhead to do this every time the hwrng_read
 * function is called. The device should however be put into power-off mode
 * after initialization and will be powered on and of when hwrng_read is called.
 */
void hwrng_init(void);

/**
 * @brief   Read N bytes of random data from the hardware device
 *
 * The read function should power on the HWRNG MCU peripheral, read the given
 * number of random bytes and than put the peripheral back to sleep.
 *
 * @param[in] buf   destination buffer to write the bytes to
 * @param[in] num   number of bytes to get from device
 */
void hwrng_read(void *buf, unsigned int num);

#ifdef __cplusplus
}
#endif

/** @} */
