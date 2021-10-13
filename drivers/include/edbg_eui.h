/*
 * Copyright (C) 2020 Benjamin Valentin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_edbg_eui Driver for getting MAC address out of Atmel EDBG
 * @ingroup     drivers_misc
 * @brief       Device driver interface for the AT24MAC I2C chip
 * @{
 *
 * @file
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#ifndef EDBG_EUI_H
#define EDBG_EUI_H

#include <stdint.h>
#include "net/eui64.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Get the unique EUI64 address from a Atmel EDBG debugger
 *
 * @note    The EDBG firmware may take a while to start up, so on cold
 *          boot this function can return error when called to soon.
 *
 * @param[out] addr     memory location to copy the address into.
 *
 * @return              0 on success, error otherwise.
 */
int edbg_get_eui64(eui64_t *addr);

#ifdef __cplusplus
}
#endif

#endif /* EDBG_EUI_H */
/** @} */
