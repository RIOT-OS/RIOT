/*
 * Copyright (C) 2014-2015  René Kijewski  <rene.kijewski@fu-berlin.de>
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
 * @ingroup     x86-multithreading
 * @{
 *
 * @file
 * @brief       Atomic arithmetic for the x86 port.
 *
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * @}
 */

#include <stdint.h>
#include "atomic.h"

int atomic_cas(atomic_int_t *dest, int known_value, int new_value)
{
    uint8_t successful;
    asm volatile ("lock cmpxchgl %2, %0\n"
                  "seteb %1"
                  : "+m"(ATOMIC_VALUE(*dest)), "=g"(successful)
                  : "r"(new_value), "a"(known_value)
                  : "flags");
    return successful;
}
