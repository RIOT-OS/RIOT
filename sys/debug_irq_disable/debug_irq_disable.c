/*
 * Copyright (C) 2022 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     debug_irq_disable
 * @{
 *
 * @file
 * @brief       Helper for debug_irq_disable
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @}
 */

#include <stdbool.h>
#include "fmt.h"

void debug_irq_disable_print(const char *file, unsigned line, uint32_t ticks)
{
    static bool is_printing;

    if (is_printing) {
        return;
    }

    /* prevent infinite recursion if stdio driver uses irq_disable()  */
    is_printing = true;

    print_str("irq disabled for ");
    print_u32_dec(ticks);
    print_str(" ticks in ");
    print_str(file);
    print_str(":");
    print_u32_dec(line);
    print_str("\n");

    is_printing = false;
}
