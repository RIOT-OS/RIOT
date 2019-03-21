/*
 * Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_internal
 * @{
 *
 * @file
 * @brief       Platform-independent kernel initilization
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include "kernel_init.h"
#include "log.h"

#include "periph/pm.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

extern int main(void);
void kernel_init(void)
{
    LOG_INFO("main(): This is RIOT! (Version: " RIOT_VERSION ")\n");
    main();
    LOG_INFO("main() exited, interrupts will still be served\n");
    while (1) {
        pm_set_lowest();
    }
}
