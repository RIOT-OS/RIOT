/*
 * Copyright (C) 2017 Eistec AB
 * Copyright (C) 2018 HERE Deutschland GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#pragma once

/**
 * @ingroup         cpu_kinetis
 * @brief           CPU specific implementations for the NXP Kinetis EA series of
 *                  Cortex-M MCUs
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author          Anton Gerasimov <anton.gerasimov@here.com>
 */


#if defined(KINETIS_CORE_Z)
#if (KINETIS_ROMSIZE == 128)
/* Kinetis KEAZ128 */
#include "vendor/SKEAZ1284.h"

#define KINETIS_SINGLE_UART_IRQ
#endif  /* (KINETIS_ROMSIZE == y) */
#endif  /* (KINETIS_CORE_x) */

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
}
#endif

/** @} */
