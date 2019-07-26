/*
 * Copyright (C) 2014-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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
 * @{
 * @file
 * @brief       Hardware random number generator driver interface
 *
 * @author      Christian Mehlis <mehlis@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_HWRNG_H
#define PERIPH_HWRNG_H

#include <stdint.h>
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @def     HWRNG_HAS_INIT
 *
 * @brief   Set to 1 if the platform implements hwrng_init(), 0 otherwise
 */
#ifndef HWRNG_HAS_INIT
#define HWRNG_HAS_INIT            (0)
#endif

/**
 * @def     HWRNG_HAS_POWERONOFF
 *
 * @brief   Set to 1 if the platform implements hwrng_poweron() / hwrng_poweroff(), 0 otherwise
 */
#ifndef HWRNG_HAS_POWERONOFF
#define HWRNG_HAS_POWERONOFF      (0)
#endif

/**
 * @def     HWRNG_HAS_GENERIC_READ
 *
 * @brief   Set to 1 if the platform does not implement a custom hwrng_read() function, 0 otherwise
 */
#ifndef HWRNG_HAS_GENERIC_READ
#define HWRNG_HAS_GENERIC_READ    (0)
#endif

/**
 * @brief   Initialize the hardware random number generator
 *
 * On some platforms, the random number generator needs some global
 * initialization before it can be used. This should happen in this function
 * if it would impose too much overhead to do this everytime the hwrng_read
 * function is called. The device should however be put into power-off mode
 * after initialization and will be powered on and of when hwrng_read is called.
 *
 * @note     Only implemented and called for platforms with HWRNG_HAS_INIT = 1.
 *
 */
void hwrng_init(void);

/**
 * @brief    Power on the HWRNG module
 *
 * @note     Only implemented and called for platforms with HWRNG_HAS_POWERONOFF = 1.
 *
 */
void hwrng_poweron(void);

/**
 * @brief    Power off the HWRNG module
 *
 * @note     Only implemented and called for platforms with HWRNG_HAS_POWERONOFF = 1.
 *
 */
void hwrng_poweroff(void);

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

/**
 * @brief   Read a single random word.
 *
 * The read function does not power on the HWRNG MCU peripheral, the caller
 * is responsible for calling hwrng_poweron() / hwrng_poweroff() manually if
 * HWRNG_HAS_POWERONOFF = 1.
 *
 * @return  The random number.
 */
uint32_t hwrng_uint32(void);

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_HWRNG_H */
/** @} */
