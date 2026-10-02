/*
 * SPDX-FileCopyrightText: 2015-2021 The Apache Software Foundation
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

/**
 * @ingroup     pkg_mynewt_core
 * @{
 *
 * @file
 * @brief       mynewt-core system configurations
 *
 * @}
 */

#include "kernel_defines.h"

/**
 * @name    MyNewt header inclusion macro definitions
 * @{
 *
 * PLEASE NOTE: Following macro definitions where copied directly from
 *              apache/mynewt-core and are under the copyright specified in
 *              the header.
 *
 * This macro exists to ensure code includes this header when needed.  If code
 * checks the existence of a setting directly via ifdef without including this
 * header, the setting macro will silently evaluate to 0.  In contrast, an
 * attempt to use these macros without including this header will result in a
 * compiler error.
 */
#define MYNEWT_VAL(_name)                       MYNEWT_VAL_ ## _name
#define MYNEWT_VAL_CHOICE(_name, _val)          MYNEWT_VAL_ ## _name ## __ ## _val
/** @} */

/**
 * @brief TIMER 3 or 5 (RTC_DEV0) will be mynewt-core OS_CPUTIME timer,
 *        depending on used CPU family
 */
#ifndef MYNEWT_VAL_OS_CPUTIME_TIMER_NUM
#ifdef CPU_FAM_NRF51
#define MYNEWT_VAL_OS_CPUTIME_TIMER_NUM (3)
#else
#define MYNEWT_VAL_OS_CPUTIME_TIMER_NUM (5)
#endif
#endif

/**
 * @brief Enable TIMER 3 or TIMER 5 (RTC_DEV0) depending on used CPU family
 */
#ifdef CPU_FAM_NRF51
#ifndef MYNEWT_VAL_TIMER_3
#define MYNEWT_VAL_TIMER_3 (1)
#endif
#else
#ifndef MYNEWT_VAL_TIMER_5
#define MYNEWT_VAL_TIMER_5 (1)
#endif
#endif

#if IS_USED(MODULE_NIMBLE)
/*** @mynewt-nimble */
#undef MYNEWT_VAL
#undef MYNEWT_VAL_CHOICE
#include "npl_sycfg.h"
#endif

#if IS_USED(MODULE_UWB_CORE)
/*** @decawave-mynewt-core/hw/drivers/uwb */
#include "dpl_syscfg/syscfg_uwb.h"

/*** @decawave-mynewt-core/lib/twr_ds */
#include "dpl_syscfg/syscfg_twr_ds.h"

/*** @decawave-mynewt-core/lib/twr_ds_ext */
#include "dpl_syscfg/syscfg_twr_ds_ext.h"

/*** @decawave-mynewt-core/lib/twr_ss */
#include "dpl_syscfg/syscfg_twr_ss.h"

/*** @decawave-mynewt-core/lib/twr_ss_ack */
#include "dpl_syscfg/syscfg_twr_ss_ack.h"

/*** @decawave-mynewt-core/lib/twr_ss_ext */
#include "dpl_syscfg/syscfg_twr_ss_ext.h"

/*** @decawave-mynewt-core/lib/uwb_rng */
#include "dpl_syscfg/syscfg_uwb_rng.h"

#endif

#if IS_USED(MODULE_UWB_DW1000)
/*** @decawave-uwb-dw1000/hw/drivers/uwb/uwb_dw1000 */
#include "syscfg_uwb_dw1000.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif
