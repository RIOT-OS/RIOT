/*
 * Copyright (C) 2022 Mesotic SAS
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
 * @brief       RIOT internal header for nrf_modem configuration
 *
 * @author      Dylan Laduranty <dylan.laduranty@mesotic.com>
 *
 */

#ifndef NRF_MODEM_PARAMS_H
#define NRF_MODEM_PARAMS_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NRF_MODEM_SHMEM_SIZE
#define NRF_MODEM_SHMEM_SIZE 8192 /* bytes */
#endif /* NRF_MODEM_SHMEM_SIZE */

#ifndef NRF_MODEM_TRACE_ENABLE
#define NRF_MODEM_TRACE_ENABLE 0
#endif /* NRF_MODEM_TRACE_ENABLE */

#ifdef __cplusplus
}
#endif

#endif /* NRF_MODEM_PARAMS_H */
/** @} */