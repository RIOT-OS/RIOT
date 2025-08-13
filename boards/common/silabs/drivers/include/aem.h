/*
 * SPDX-FileCopyrightText: 2018 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    boards_common_silabs_drivers_aem Advanced energy monitor driver
 * @ingroup     boards_common_silabs_drivers
 * @{
 *
 * @file
 * @brief       Implementations of the advanced energy monitor driver.
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   AEM specific values for the Data Watchpoint and Trace (DWT).
 */
#define AEM_DWT_CTRL        (0x400113FF)

/**
 * @name    AEM specific values for theTrace Port Interface Unit (TPIU).
 * @{
 */
#define AEM_TPI_ACPR        (0xf)
#define AEM_TPI_SPPR        (0x2)
#define AEM_TPI_FFCR        (0x100)
/** @} */

/**
 * @name    AEM specific values for the Instrumentation Trace Microcel (ITM).
 * @{
 */
#define AEM_ITM_LAR         (0xc5acce55)
#define AEM_ITM_TCR         (0x10009)
/** @} */

/**
 * @brief   Initialize the advanced energy monitor.
 *
 * This must be performed as one of the first initializations, to provide
 * output as soon as possible.
 */
void aem_init(void);

#ifdef __cplusplus
}
#endif

/** @} */
