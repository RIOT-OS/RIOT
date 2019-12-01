/*
 * Copyright (C) 2019 OvGU Magdeburg <fabian.huessler@ovgu.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_at86rf2xx
 * @{
 * @file
 * @brief       AT86RF2xx devices from paameters
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 */

#ifndef AT86RF2XX_DEVS_H
#define AT86RF2XX_DEVS_H

#include <assert.h>

#include "kernel_defines.h"
#include "at86rf2xx.h"
#include "at86rf2xx_params.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Data structure to store AT86RF2XX device structures
 */
typedef union at86rf2xx_devs {
    /**
     * @brief Access AT86RF2XX device array by their type
     */
    struct nemed_devs {
#if IS_USED(MODULE_AT86RF212B)
        /**
         * @brief   AT86RF212B device array
         */
        at86rf212b_t at86rf212b_devs[AT86RF212B_NUM_OF];
#endif
#if IS_USED(MODULE_AT86RF231)
        /**
         * @brief   AT86RF231 device array
         */
        at86rf231_t at86rf231_devs[AT86RF231_NUM_OF];
#endif
#if IS_USED(MODULE_AT86RF232)
        /**
         * @brief   AT86RF232 device array
         */
        at86rf232_t at86rf232_devs[AT86RF232_NUM_OF];
#endif
#if IS_USED(MODULE_AT86RF233)
        /**
         * @brief   AT86RF233 device array
         */
        at86rf233_t at86rf233_devs[AT86RF233_NUM_OF];
#endif
#if IS_USED(MODULE_AT86RFA1)
        /**
         * @brief   AT86RFA1 device array
         */
        at86rfa1_t at86rfa1_devs[AT86RFA1_NUM_OF];
#endif
#if IS_USED(MODULE_AT86RFR2)
        /**
         * @brief   AT86RFR2 device array
         */
        at86rfr2_t at86rfr2_devs[AT86RFR2_NUM_OF];
#endif
    } named_devs;
    /**
     * @brief Raw memory of AT86RF2XX device structures
     */
    uint8_t mem_devs[
        AT86RF212B_NUM_OF * sizeof(struct at86rf212b) +
        AT86RF231_NUM_OF  * sizeof(struct at86rf231)  +
        AT86RF232_NUM_OF  * sizeof(struct at86rf232)  +
        AT86RF233_NUM_OF  * sizeof(struct at86rf233)  +
        AT86RFA1_NUM_OF   * sizeof(struct at86rfa1)   +
        AT86RFR2_NUM_OF   * sizeof(struct at86rfr2)
    ];
} at86rf2xx_devs_t;

/**
 * @brief   Dispatch *_setup() functions for all AT86RF2xx devices
 *
 * @param[in] at86rf2xx_devs        data structure of all AT86RF2xx devices
 */
static inline
void at86rf2xx_setup_devs(at86rf2xx_devs_t *at86rf2xx_devs)
{
#if IS_USED(MODULE_AT86RF212B)
    at86rf212b_setup(at86rf2xx_devs->named_devs.at86rf212b_devs,
                     at86rf212b_params, AT86RF212B_NUM_OF);
#endif
#if IS_USED(MODULE_AT86RF231)
    at86rf231_setup(at86rf2xx_devs->named_devs.at86rf231_devs,
                    at86rf231_params, AT86RF231_NUM_OF);
#endif
#if IS_USED(MODULE_AT86RF232)
    at86rf232_setup(at86rf2xx_devs->named_devs.at86rf232_devs,
                    at86rf232_params, AT86RF232_NUM_OF);
#endif
#if IS_USED(MODULE_AT86RF233)
    at86rf233_setup(at86rf2xx_devs->named_devs.at86rf233_devs,
                    at86rf233_params, AT86RF233_NUM_OF);
#endif
#if IS_USED(MODULE_AT86RFA1)
    at86rfa1_setup(at86rf2xx_devs->named_devs.at86rfa1_devs);
#endif
#if IS_USED(MODULE_AT86RFR2)
    at86rfr2_setup(at86rf2xx_devs->named_devs.at86rfr2_devs);
#endif
}

/**
 * @brief Get an AT86Rf2xx dev by it´s index
 *
 * @param[in] at86rf2xx_devs        device data structure
 * @param[in] idx                   index
 *
 * @return                          device at index
 */
static inline
at86rf2xx_t *at86rf2xx_get_dev(at86rf2xx_devs_t *at86rf2xx_devs,
                               unsigned idx)
{
    assert(at86rf2xx_devs);
    uint8_t *out_dev = at86rf2xx_devs->mem_devs;

#if IS_USED(MODULE_AT86RF212B)
    if (idx >= AT86RF212B_NUM_OF) {
        out_dev += (AT86RF212B_NUM_OF * sizeof(at86rf212b_t));
        idx -= AT86RF212B_NUM_OF;
    }
    else {
        return (at86rf2xx_t *)(out_dev + (idx * sizeof(at86rf212b_t)));
    }
#endif
#if IS_USED(MODULE_AT86RF231)
    if (idx >= AT86RF231_NUM_OF) {
        out_dev += (AT86RF231_NUM_OF * sizeof(at86rf231_t));
        idx -= AT86RF231_NUM_OF;
    }
    else {
        return (at86rf2xx_t *)(out_dev + (idx * sizeof(at86rf231_t)));
    }
#endif
#if IS_USED(MODULE_AT86RF232)
    if (idx >= AT86RF232_NUM_OF) {
        out_dev += (AT86RF232_NUM_OF * sizeof(at86rf232_t));
        idx -= AT86RF232_NUM_OF;
    }
    else {
        return (at86rf2xx_t *)(out_dev + (idx * sizeof(at86rf232_t)));
    }
#endif
#if IS_USED(MODULE_AT86RF233)
    if (idx >= AT86RF233_NUM_OF) {
        out_dev += (AT86RF233_NUM_OF * sizeof(at86rf233_t));
        idx -= AT86RF233_NUM_OF;
    }
    else {
        return (at86rf2xx_t *)(out_dev + (idx * sizeof(at86rf233_t)));
    }
#endif
#if IS_USED(MODULE_AT86RFA1)
    if (idx >= AT86RFA1_NUM_OF) {
        out_dev += (AT86RFA1_NUM_OF * sizeof(at86rfa1_t));
        idx -= AT86RFA1_NUM_OF;
    }
    else {
        return (at86rf2xx_t *)(out_dev + (idx * sizeof(at86rfa1_t)));
    }
#endif
#if IS_USED(MODULE_AT86RFR2)
    if (idx >= AT86RFR2_NUM_OF) {
        out_dev += (AT86RFR2_NUM_OF * sizeof(at86rfr2_t));
        idx -= AT86RFR2_NUM_OF;
    }
    else {
        return (at86rf2xx_t *)(out_dev + (idx * sizeof(at86rfr2_t)));
    }
#endif
    return (at86rf2xx_t *)out_dev;
}

#ifdef __cplusplus
}
#endif

#endif /* AT86RF2XX_DEVS_H */
/** @} */
