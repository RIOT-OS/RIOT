/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Emlearn test application
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include <inttypes.h>

#include "model.h"

/* the digit array included must be 4-byte aligned */
__attribute__((__aligned__(4)))
#include "blob/digit.h"

int main(void)
{
    printf("Predicted digit: %" PRIi32 "\n",
           model_predict((const float *)digit, digit_len >> 2));

    return 0;
}
