/*
 * Copyright (C) 2018 Ishraq Ibne Ashraf
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_kinetis
 * @brief           CPU specific implementations for the NXP Kinetis E series of
 *                  Cortex-M MCUs
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Ishraq Ibne Ashraf <ishraq.i.ashraf@gmail.com>
 */

#ifndef CPU_CONF_KINETIS_E_H
#define CPU_CONF_KINETIS_E_H

#if (KINETIS_FAMILY == 0)
#if (KINETIS_SUBFAMILY == 2)
#if defined(CPU_MODEL_MKE02Z64VQH4)
#include "vendor/MKE02Z4.h"
#endif /* CPU_MODEL_MKE02Z64VQH4 */
#endif /* (KINETIS_SUBFAMILY == y) */
#endif /* (KINETIS_FAMILY == x) */

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_KINETIS_E_H */
/** @} */
