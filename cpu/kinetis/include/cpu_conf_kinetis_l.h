/*
 * Copyright (C) 2017 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#pragma once

/**
 * @ingroup         cpu_kinetis
 * @brief           CPU specific implementations for the NXP Kinetis L series of
 *                  Cortex-M MCUs
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */


#if (KINETIS_FAMILY == 4)
#if (KINETIS_SUBFAMILY == 3)
#if defined(CPU_MODEL_MKL43Z128VLH4) || \
    defined(CPU_MODEL_MKL43Z128VMP4) || \
    defined(CPU_MODEL_MKL43Z256VLH4) || \
    defined(CPU_MODEL_MKL43Z256VMP4)
#include "vendor/MKL43Z4.h"
#endif
#endif /* (KINETIS_SUBFAMILY == y) */
#endif /* (KINETIS_FAMILY == x) */

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
}
#endif

/** @} */
