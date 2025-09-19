/*
 * SPDX-FileCopyrightText: 2017 Eistec AB
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         cpu_kinetis
 * @{
 *
 * @file
 * @brief           Implementation specific CPU configuration options
 *
 * @author          Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

/* This is needed to homogenize the symbolic IRQ names across different versions
 * of the vendor headers. These must be defined before any vendor headers are
 * included */
#define FTFA_IRQn FTF_IRQn
#define FTFA_Collision_IRQn Read_Collision_IRQn
#define FTFE_IRQn FTF_IRQn
#define FTFE_Collision_IRQn Read_Collision_IRQn
#define FTFL_IRQn FTF_IRQn
#define FTFL_Collision_IRQn Read_Collision_IRQn
#define PMC_IRQn LVD_LVW_IRQn
#define Watchdog_IRQn WDOG_EWM_IRQn
#define LVD_LVW_DCDC_IRQn LVD_LVW_IRQn

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
#elif defined(KINETIS_SERIES_EA)
#include "cpu_conf_kinetis_ea.h"
#endif /* defined(KINETIS_SERIES_x) */

#ifndef MCU_MEM_MAP_VERSION
#error Missing vendor header for the chosen CPU_MODEL
#endif
#include "cpu_conf_kinetis.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief   Bit-Band configuration
 * @{
 */
#ifdef BITBAND_REG32
#define CPU_HAS_BITBAND         1
/* SRAM_L is mapped before the bit-banding region, only SRAM_U can be used for bit-banding */
#define CPU_HAS_SRAM_BITBAND    0
#endif
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
