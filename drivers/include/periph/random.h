/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     driver_periph
 * @{
 *
 * @file
 * @brief       (Pseudo) random number generator low-level driver interface
 *
 * NOTE: The quality of the random data read from this interface is highly
 * dependent on hardware dependent implementation. Most platforms utilize a
 * hardware (Pseudo) Random Number Generator. The quality of the generated
 * random data can be however very different.
 *
 * @note REFER TO YOUR PLATFORMS IMPLEMENTATION ABOUT INFORMATION ABOUT THE
 *       QUALITY OF RANDOMNES!
 *
 * @author      Christian Mehlis <mehlis@inf.fu-berlin.de>
 */

#ifndef __RANDOM_H
#define __RANDOM_H

#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/* only include this file if a random number generator is defined */
#if RANDOM_NUMOF

/**
 * @brief Initializes the source of randomness
 *
 * In case of a hardware random number generator, this peripheral
 * is initialized and powered on. If such a device is not present,
 * it depends on the implementation how a source for randomness
 * is created and initialized.
 */
void random_init(void);

/**
 * @brief Reads num or less bytes of randomness from the source, will
 *        block until random data is available
 *
 * @param[in] buf   destination buffer to write the bytes to
 * @param[in] num   number of bytes to get from device,
 *                  only values >0 are valid
 *
 * @return          the number of bytes written to buf
 * @return          0 on error
 */
int random_read(char *buf, unsigned int num);

/**
 * @brief Power on the random number generator
 */
void random_poweron(void);

/**
 * @brief Power off the random number generator
 */
void random_poweroff(void);

#endif /* RANDOM_NUMOF */

#ifdef __cplusplus
}
#endif

#endif /* __RANDOM_H */
/** @} */
