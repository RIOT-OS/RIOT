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
 * @ingroup     tests
 * @{
 * @file
 * @brief       Test bitarithm.
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "bitarithm.h"

int main(void)
{
    puts(".Start.");
    
    for (uint16_t i = 0xffffu; i > 0; --i) {
        unsigned m = bitarithm_msb(i);
        unsigned l = bitarithm_lsb(i);
        printf("%" PRIu16 " msb=%u lsb=%u\n", i, m, l);
    }

    puts(".Done.");
    return 0;
}
