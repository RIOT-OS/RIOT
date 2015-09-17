/*
 * Copyright (C) 2015 Rakendra Thapa <rakendrathapa@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_lm4f120
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Rakendra Thapa <rakendrathapa@gmail.com>
 */
#ifndef PERIPH_CPU_H_
#define PERIPH_CPU_H_

#include <inttypes.h>
#include "periph/dev_enums.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HAVE_GPIO_T
typedef uint32_t gpio_t;

#define GPIO(x,y) ((gpio_t)(x<<4 | y))
/* nothing to do here, yet */

  enum {
    PORT_A = 0,       /**< port A */
    PORT_B = 1,       /**< port B */
    PORT_C = 2,       /**< port C */
    PORT_D = 3,       /**< port D */
    PORT_E = 4,       /**< port E */
    PORT_F = 5,       /**< port F */
  };

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_H_ */
/** @} */
