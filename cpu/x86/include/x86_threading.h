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
 * Using multiple threads on x86 boards.
 *
 * @ingroup    x86
 * @defgroup   x86-multithreading   i586 multithreading support
 * @{
 * @file
 * @author     René Kijewski <rene.kijewski@fu-berlin.de>
 */

#ifndef CPU__X86__THREADING__H__
#define CPU__X86__THREADING__H__

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialize threading.
 *
 * This function is called during initialization by x86_startup().
 * You must not call this function on your own accord.
 */
void x86_init_threading(void);

/**
 * @brief   The getter/setter for irq_is_in() for the x86 port.
 */
extern bool x86_in_isr;

#ifdef __cplusplus
}
#endif

#endif

/** @} */
