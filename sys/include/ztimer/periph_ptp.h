/*
 * Copyright (C)    2018 Kaspar Schleiser <kaspar@schleiser.de>
                    2021 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#pragma once

/**
 * @defgroup    sys_ztimer_periph_ptp  ztimer periph/ptp backend
 * @ingroup     sys_ztimer
 * @brief       ztimer periph/ptp backend
 *
 * This ztimer module implements a ztimer virtual clock on top of periph/ptp.
 *
 * @{
 *
 * @file
 * @brief       ztimer periph/ptp backend API
 *
 * @author      Jana Eisoldt <jana.eisoldt@ovgu.de>
 */

#include "ztimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief ztimer_periph_ptp structure definition
 *
 * The periph/ptp backend has no private fields, thus this is just a typedef
 * to ztimer_clock_t.
 */
typedef ztimer_clock_t ztimer_periph_ptp_t;

/**
 * @brief   ztimer periph/ptp backend initialization function
 *
 * @param[in, out]  clock   ztimer_periph_ptp object to initialize
 */
void ztimer_periph_ptp_init(ztimer_periph_ptp_t *clock);

#ifdef __cplusplus
}
#endif

/** @} */
