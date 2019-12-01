/*
 * Copyright (C) 2019 OvGU Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_at86rf2xx
 * @{
 *
 * @file
 * @brief       Types of supported AT86RF2xx devices
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 */
#ifndef AT86RF2XX_DEV_TYPES_H
#define AT86RF2XX_DEV_TYPES_H

#include "kernel_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AT86RF212B_UNUSED       (-1)
#define AT86RF231_UNUSED        (-2)
#define AT86RF232_UNUSED        (-3)
#define AT86RF233_UNUSED        (-4)
#define AT86RFA1_UNUSED         (-5)
#define AT86RFR2_UNUSED         (-6)

#if IS_USED(MODULE_AT86RF212B)
#define AT86RF2XX_DT_COUNTER1   (1)
#else
#define AT86RF2XX_DT_COUNTER1   (0)
#endif

#if IS_USED(MODULE_AT86RF231)
#define AT86RF2XX_DT_COUNTER2   (AT86RF2XX_DT_COUNTER1 + 1)
#else
#define AT86RF2XX_DT_COUNTER2   AT86RF2XX_DT_COUNTER1
#endif

#if IS_USED(MODULE_AT86RF232)
#define AT86RF2XX_DT_COUNTER3   (AT86RF2XX_DT_COUNTER2 + 1)
#else
#define AT86RF2XX_DT_COUNTER3   AT86RF2XX_DT_COUNTER2
#endif

#if IS_USED(MODULE_AT86RF233)
#define AT86RF2XX_DT_COUNTER4   (AT86RF2XX_DT_COUNTER3 + 1)
#else
#define AT86RF2XX_DT_COUNTER4   AT86RF2XX_DT_COUNTER3
#endif

#if IS_USED(MODULE_AT86RFA1)
#define AT86RF2XX_DT_COUNTER5   (AT86RF2XX_DT_COUNTER4 + 1)
#else
#define AT86RF2XX_DT_COUNTER5   AT86RF2XX_DT_COUNTER4
#endif

#if IS_USED(MODULE_AT86RFA1)
#define AT86RF2XX_DT_COUNTER6   (AT86RF2XX_DT_COUNTER5 + 1)
#else
#define AT86RF2XX_DT_COUNTER6   AT86RF2XX_DT_COUNTER5
#endif

/**
 * @brief Supported device types.
 */
typedef enum at86rf2xx_dev_type {
#if IS_USED(MODULE_AT86RF212B) || defined(DOXYGEN)
    AT86RF2XX_DEV_TYPE_AT86RF212B   = 0,                    /**< Type AT86RF212B */
#else
    AT86RF2XX_DEV_TYPE_AT86RF212B   = AT86RF212B_UNUSED,
#endif
#if IS_USED(MODULE_AT86RF231) || defined(DOXYGEN)
    AT86RF2XX_DEV_TYPE_AT86RF231    = AT86RF2XX_DT_COUNTER1, /**< Type AT86RF231 */
#else
    AT86RF2XX_DEV_TYPE_AT86RF231    = AT86RF231_UNUSED,
#endif
#if IS_USED(MODULE_AT86RF232) || defined(DOXYGEN)
    AT86RF2XX_DEV_TYPE_AT86RF232    = AT86RF2XX_DT_COUNTER2, /**< Type AT86RF232 */
#else
    AT86RF2XX_DEV_TYPE_AT86RF232    = AT86RF232_UNUSED,
#endif
#if IS_USED(MODULE_AT86RF233) || defined(DOXYGEN)
    AT86RF2XX_DEV_TYPE_AT86RF233    = AT86RF2XX_DT_COUNTER3, /**< Type AT86RF233 */
#else
    AT86RF2XX_DEV_TYPE_AT86RF233    = AT86RF233_UNUSED,
#endif
#if IS_USED(MODULE_AT86RFA1) || defined(DOXYGEN)
    AT86RF2XX_DEV_TYPE_AT86RFA1     = AT86RF2XX_DT_COUNTER4, /**< Type AT86RFA1 */
#else
    AT86RF2XX_DEV_TYPE_AT86RFA1     = AT86RFA1_UNUSED,
#endif
#if IS_USED(MODULE_AT86RFR2) || defined(DOXYGEN)
    AT86RF2XX_DEV_TYPE_AT86RFR2     = AT86RF2XX_DT_COUNTER5, /**< Type AT86RFR2 */
#else
    AT86RF2XX_DEV_TYPE_AT86RFR2     = AT86RFR2_UNUSED,
#endif
    AT86RF2XX_DEV_TYPE_NUM_OF       = AT86RF2XX_DT_COUNTER6 /**< Number of used transceiver types */
} at86rf2xx_dev_type_t;

#ifdef __cplusplus
}
#endif

#endif /* AT86RF2XX_DEV_TYPES_H */
/** @} */
