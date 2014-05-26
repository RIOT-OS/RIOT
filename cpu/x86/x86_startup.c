/*
 * Copyright (C) 2014  René Kijewski  <rene.kijewski@fu-berlin.de>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * @ingroup     x86
 * @{
 *
 * @file
 * @brief       The common startup code for the x86 port.
 *
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include "x86_hwtimer.h"
#include "x86_interrupts.h"
#include "x86_memory.h"
#include "x86_pci.h"
#include "x86_pic.h"
#include "x86_pit.h"
#include "x86_rtc.h"
#include "x86_threading.h"
#include "x86_uart.h"

#include <cpu.h>
#include <kernel_internal.h>
#include <tlsf-malloc.h>

#include <stdio.h>

/* Must be <= 0x1000 because otherwise x86_map_physical_pages() might get a page out of this pool.
 * Because static memory has the PT_G flag, flushing the TLB would cause a stale PT
 * after calling add_pages_to_pool() in x86_map_physical_pages().
 */
static char early_malloc_pool[0x1000] __attribute__((aligned(4)));

void x86_startup(void)
{
    tlsf_add_pool(early_malloc_pool, sizeof early_malloc_pool);

    x86_early_init_uart();
    x86_init_threading();
    x86_init_interrupts();
    x86_init_pic();
    x86_init_uart();
    x86_init_memory();
    x86_init_rtc();
    x86_init_pit();
    x86_init_hwtimer();
    x86_init_pci();
    puts("RIOT x86 hardware initialization complete.");

    x86_init_board();
    puts("RIOT board initialization complete.");

    kernel_init(); /* should not return */
    puts("kernel_init returned");
    x86_hlt();
}
