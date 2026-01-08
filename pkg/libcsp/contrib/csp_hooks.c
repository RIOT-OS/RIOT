/*
 * Copyright (C) 2022 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @file
 * @brief       LibCSP
 *
 * @author      Pierre Millot <pierre.millot@grenoble-inp.org>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 */

#include <csp/csp_hooks.h>
#include "cpu.h"

__attribute__((weak)) uint32_t csp_memfree_hook(void)
{
    return 0;
}

__attribute__((weak)) unsigned int csp_ps_hook(csp_packet_t *packet)
{
    (void)packet;
    return 0;
}

__attribute__((weak)) void csp_reboot_hook(void)
{}

__attribute__((weak)) void csp_shutdown_hook(void)
{}
