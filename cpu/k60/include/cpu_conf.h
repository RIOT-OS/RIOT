/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup        cpu_k60 NXP Kinetis K60
 * @ingroup         cpu
 * @brief           CPU specific implementations for the NXP Kinetis K60
 *                  Cortex-M4 MCU
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef CPU_CONF_H
#define CPU_CONF_H

#if defined(CPU_MODEL_MK60DN512VLL10) || defined(CPU_MODEL_MK60DN256VLL10)
#include "vendor/MK60D10.h"

/* K60 rev 2.x replaced the RNG module in 1.x by the RNGA PRNG module */
#define KINETIS_RNGA            (RNG)
#else
#error Unknown CPU model. Update Makefile.include in the board directory.
#endif

#include "cpu_conf_kinetis.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief This CPU provides an additional ADC clock divider as CFG1[ADICLK]=1
 */
#define KINETIS_HAVE_ADICLK_BUS_DIV_2 1

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */
