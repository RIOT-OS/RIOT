/*
 * Copyright (C) 2014 Simon Brummer
 *               2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_periph_dac DAC
 * @ingroup     drivers_periph
 * @brief       Low-level DAC peripheral driver
 *
 * @{
 * @file
 * @brief       Low-level DAC peripheral driver interface definition
 *
 * @author      Simon Brummer <simon.brummer@haw-hamburg.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_DAC_H
#define PERIPH_DAC_H

#include <stdint.h>

#include "periph_cpu.h"
#include "periph_conf.h"
/* TODO: remove once all platforms are ported to this interface */
#include "periph/dev_enums.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Make sure the number of available DAC lines is defined
 * @{
 */
#ifndef DAC_NUMOF
#define "DAC_NUMOF undefined"
#endif
/** @} */

/**
 * @brief   Define default DAC type identifier
 * @{
 */
#ifndef HAVE_DAC_T
typedef unsigned int dac_t;
#endif
/** @} */

/**
 * @brief   Default DAC undefined value
 * @{
 */
#ifndef DAC_UNDEF
#define DAC_UNDEF           (0xffff)
#endif
/** @} */

/**
 * @brief   Default DAC access macro
 * @{
 */
#ifndef DAC_LINE
#define DAC_LINE(x)         (x)
#endif
/** @} */

/**
 * @brief   Initialize the given DAC line
 *
 * The DAC will be initialized with all possilble channels active.
 * On each channel will be initialized with a Zero on it.
 *
 * @param[in] dev          the device to initialize
 * @param[in] precision    the precision to use for conversion
 *
 * @return                 0 on success
 * @return                 -1 on unknown DAC Device
 * @return                 -2 on precision not available
 */
int8_t dac_init(dac_t dev);

/**
 * @brief Write a value onto DAC Device on a given Channel.
 *
 * @param[in] dev          the DAC device to use
 * @param[in] channel      the channel used for Digital/Analoge conversion
 * @param[in] value        the value to write onto DAC.
 *
 * @return                 0 on success
 * @return                 -1 on unknown DAC Device
 * @return                 -2 on invalid channel
 * @return                 -3 if value is out of range.
 */
int8_t dac_write(dac_t dev, uint8_t channel, uint16_t value);

/**
 * @brief Enable power for the given DAC Device
 *
 * @param[in] dev          the DAC device to power up.
 *
 * @return                 0 on success
 * @return                 -1 on unknown DAC Device
 */
int8_t dac_poweron(dac_t dev);

/**
 * @brief Disable power for the given DAC Device
 *
 * @param[in] dev          the DAC device to power down
 *
 * @return                 0 on success
 * @return                 -1 on unknown DAC Device
 */
int8_t dac_poweroff(dac_t dev);

/**
 * @brief Helper function to map a given integer range to a valid DAC value.
 *
 * This function is useful for converting ranges of values to a valid DAC output value.
 *
 * The min value is assumed to be smaller than max value and value is assumed
 * to be between min and max.
 *
 * @param[in] dev          the DAC Device to read precision value from
 * @param[in] value        the value to map onto min and max
 * @param[in] min          the lower bound of the target interval
 * @param[in] max          the upper bound of the target interval
 *
 * @return                 the mapped value, in valid DAC range
 */
uint16_t dac_map(dac_t dev, int value, int min, int max);

/**
 * @brief Helper function to map a given float value range to a valid DAC value.
 *
 * @see dac_map
 *
 * @param[in] dev          the DAC Device to read precision value from
 * @param[in] value        the value to map onto min and max
 * @param[in] min          the lower bound of the target interval
 * @param[in] max          the upper bound of the target interval
 *
 * @return                 the mapped value, in valid DAC range
 */
uint16_t dac_mapf(dac_t dev, float value, float min, float max);

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_DAC_H */
/** @} */
