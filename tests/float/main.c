/*
 * Copyright (C) 2013 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief Float test application
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <math.h>

#include "board.h"

int main(void)
{
    double x = 1234567.0 / 1024.0;

    while (1) {
        x += 0.1;
        double z = x - floor(x);

        if (z >= 1) {
            putchar('+');
        }
        else {
            putchar('-');
        }
    }
}
