/*
 * Copyright (C) 2016 Leon George
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */
/**
 * @ingroup         cpu_cc26x0_definitions
 * @{
 *
 * @file
 * @brief           Driver for the cc26x0 GPIO controller
 *
 * @author          Leon George <leon@georgemail.eu>
 *
 */

#ifndef CC26x0_GPIO_H
#define CC26x0_GPIO_H

#include "cc26x0.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
#define GPIO_BASE                   (0x40022000) /**< GPIO base address */
/** @} */

/**
 * GPIO registers
 */
typedef struct {
    reg32_t DOUT_FRAG[8]; /**< w the first bit of each byte corresponds to one bit of DOUT */
    reg32_t __reserved1[24]; /**< unused */
    reg32_t DOUT; /**< r/w state of the output */
    reg32_t __reserved2[3]; /**< unused */
    reg32_t DOUTSET; /**< w1s set output */
    reg32_t __reserved3[3]; /**< unused */
    reg32_t DOUTCLR; /**< w1c clear output */
    reg32_t __reserved4[3]; /**< unused */
    reg32_t DOUTTGL; /**< r/w toggle output */
    reg32_t __reserved5[3]; /**< unused */
    reg32_t DIN; /**< r input status */
    reg32_t __reserved6[3]; /**< unused */
    reg32_t DOE; /**< r/w enable output */
    reg32_t __reserved7[3]; /**< unused */
    reg32_t EVFLAGS; /**< r/w1c indicates an input event */
} gpio_regs_t;

#define GPIO ((gpio_regs_t *) (GPIO_BASE)) /**< GPIO register bank */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* CC26x0_GPIO_H */

/** @} */
