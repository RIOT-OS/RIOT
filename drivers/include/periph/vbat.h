/*
 * Copyright (C) 2022 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    drivers_periph_vbat backup battery monitoring
 * @ingroup     drivers_periph
 * @brief       Driver to monitor the internal backup battery status
 *
 * The backup battery must be connected with the VBAT pin and GND.
 * On most development boards you will have to remove a solder bridge
 * that connects VDD with VBAT. The battery is used to retain the backup domain
 * register and RTC register content while power (VDD) is off.
 *
 * The actual driver is implemented as a CPU feature.
 * A board that supports
 * backup battery monitoring via ADC, must `#define VBAT_ADC ADC_LINE(x)`,
 * where `x` is the index of the internal VBAT ADC line in `adc_config[]`
 * in periph_conf.h.
 *
 * @{
 *
 * @file
 * @brief       Interface of backup battery (VBAT) monitoring
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 */

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief       Initialize backup battery monitoring
 *
 * @retval      != 0: Failure
 * @retval      0: Success
 */
int vbat_init(void);

/**
 * @brief       Sample backup battery
 *
 * @return      Backup battery load in mV
 */
int32_t vbat_sample_mv(void);

/**
 * @brief       Sample the backup battery and compare the result with
 *              the minimum value required to power the backup domain
 *
 * @retval      true: empty
 * @retval      false: not empty
 */
bool vbat_is_empty(void);

#ifdef __cplusplus
}
#endif

/** @} */
