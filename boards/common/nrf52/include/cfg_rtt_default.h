/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_common_nrf52
 * @{
 *
 * @file
 * @brief       Default RTT configuration for nRF52 based boards
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 */

#ifndef CFG_RTT_DEFAULT_H
#define CFG_RTT_DEFAULT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Real time counter configuration
 * @{
 */
#define RTT_NUMOF           (1U)
#define RTT_DEV             (1)             /* NRF_RTC1 */
#define RTT_MAX_VALUE       (0x00ffffff)
#define RTT_FREQUENCY       (1024)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* CFG_RTT_DEFAULT_H */
/** @} */