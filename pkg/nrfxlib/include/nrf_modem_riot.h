/*
 * Copyright (C) 2021-2022 Mesotic SAS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup    pkg_nrf_modem
 * @{
 *
 * @file
 * @brief       RIOT internal header for nrf_modem glue logic
 *
 * @note        Content of this file was taken from Nordic SDK
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 */

#ifndef NRF_MODEM_RIOT_H
#define NRF_MODEM_RIOT_H

#include "cpu.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   struct holding all params needed for modem initialization
 */

typedef struct nrf_modem_params {
    uint16_t shmem_size; /**< Size of each shared memory buffer used by IPC */
    bool trace_enable;  /**< Enable trace from Modem (debugging purpose) */
} nrf_modem_params_t;

#ifdef __cplusplus
}
#endif

#endif /* NRF_MODEM_RIOT_H */
/** @} */