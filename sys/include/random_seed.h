/*
 * Copyright (C) 2017 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_random
 *
 * @{
 *
 * @file
 * @brief       Interface to generate "unique" PRNGs seed
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 */

#ifndef RANDOM_SEED_H
#define RANDOM_SEED_H

#include <inttypes.h>
#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Set default ADC line for random seed
 *
 * @todo    Define by board
 */
#ifndef RANDOM_SEED_ADC_LINE
#define RANDOM_SEED_ADC_LINE        (0)
#endif

/**
 * @brief   Set default ADC resolution for random seed
 *
 * @todo    Define by board
 */
#ifndef RANDOM_SEED_ADC_RES
#define RANDOM_SEED_ADC_RES        ADC_RES_12BIT
#endif

/**
 * @brief   Generate a more or less unique seed for PRNGs.
 *          The quality of the seed, respectively the uniqueness, depends
 *          on the availability of hardware ressources. Sources used for seed
            generation:
 *          1. A hardware random number generator, if present
 *          2. A combination of single bits taken from multiple ADC samples and
               the CPU ID, in case an, ADC is present.
 *          3. A combination of the CPU iD and the system time value. Please note
               that the CPU ID might equal between similar MCUs for some families.

 *          While methods 1. and 2. are likely to result in unique values, method 3
            is prone to return equal results. Pleas check your hardware ressources!
 *
 * @return  the "unique" seed
 */
uint32_t random_prng_seed(void);

#ifdef __cplusplus
}
#endif

#endif /* RANDOM_SEED_H */
/** @} */
