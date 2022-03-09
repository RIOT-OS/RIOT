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

#include <inttypes.h>
#include <stdalign.h>
#include <stdio.h>

#include "model.h"

/* the digit array will be casted to float, so it has to meet that alignment */
alignas(float)
#include "blob/digit.h"

int main(void)
{
    /* Use intermediate cast to uintptr_t to silence -Wcast-align. Since
     * we add the alignas() attribute, the alignment is ensured */
    const float *digit_as_float = (const float *)(uintptr_t)digit;
    printf("Predicted digit: %" PRIi32 "\n",
           model_predict(digit_as_float, digit_len >> 2));

    return 0;
}
