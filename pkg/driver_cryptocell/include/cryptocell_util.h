/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_driver_cryptocell
 * @{
 *
 * @brief       Utility functions to setup and terminate the CryptoCell 310 driver
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */
#ifndef CRYPTOCELL_UTIL_H
#define CRYPTOCELL_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Enable CryptoCell module and IRQs.
 *
 *          Must be called before using crypto functions.
 *          CryptoCell peripheral will be enabled after this call.
 */
void cryptocell_enable(void);

/**
 * @brief   Disable CryptoCell module and IRQs.
 *
 *          Should be called after using crypto functions.
 *          CryptoCell peripheral will be disabled after this call.
 */
void cryptocell_disable(void);

/**
 * @brief   Enables CryptoCell module, IRQs and crypto libraries on nrf52840.
 *
 *          Must be called once before using the CryptoCell lib.
 */
void cryptocell_setup(void);

/**
 * @brief   Finishes the use of the CryptoCell library.
 *
 *          Should be called after using the CryptoCell lib.
 */
void cryptocell_terminate(void);

#ifdef __cplusplus
}
#endif

#endif /* CRYPTOCELL_UTIL_H */
/** @} */
