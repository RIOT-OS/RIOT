/*
 * Copyright (C) 2015 Rakendra Thapa <rakendrathapa@gmail.com
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_lm4f120
 * @{
 *
 * @file
 * @brief       Implementation of the kernels atomic interface
 *
 * @author      Rakendra Thapa <rakendrathapa@gmail.com>
 *
 * @}
 */

#include "arch/atomic_arch.h"
#include "irq.h"

unsigned int atomic_arch_set_return(unsigned int *to_set, unsigned int value)
{
    irq_arch_disable();
    unsigned int old = *to_set;
    *to_set = value;
    irq_arch_enable();
    return old;
}
