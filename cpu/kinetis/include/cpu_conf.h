/*
 * Copyright (C) 2017 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_kinetis
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifndef CPU_CONF_H
#define CPU_CONF_H

/* Dispatch to a separate file per family */
#if defined(KINETIS_SERIES_K)
#include "cpu_conf_kinetis_k.h"
#elif defined(KINETIS_SERIES_L)
#include "cpu_conf_kinetis_l.h"
#elif defined(KINETIS_SERIES_M)
#include "cpu_conf_kinetis_m.h"
#elif defined(KINETIS_SERIES_V)
#include "cpu_conf_kinetis_v.h"
#elif defined(KINETIS_SERIES_W)
#include "cpu_conf_kinetis_w.h"
#endif /* defined(KINETIS_SERIES_x) */

#ifndef MCU_MEM_MAP_VERSION
#error Missing vendor header for the chosen CPU_MODEL
#endif
#include "cpu_conf_kinetis.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
/** @} */
