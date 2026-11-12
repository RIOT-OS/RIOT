/*
 * SPDX-FileCopyrightText: 2019 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
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

/* the digit array will be casted to int16_t, so it has to meet that alignment */
alignas(int16_t)
#include "blob/digit.h"

/* the digit is an image of 8x8 pixels, stored as int16_t, the type of the
   features of the model */
#define DIGIT_PIXELS    (sizeof(digit) / sizeof(int16_t))

int main(void)
{
    /* Use intermediate cast to uintptr_t to silence -Wcast-align. Since
     * we add the alignas() attribute, the alignment is ensured */
    const int16_t *features = (const int16_t *)(uintptr_t)digit;

    printf("Predicted digit: %" PRIi32 "\n",
           model_predict(features, DIGIT_PIXELS));

    return 0;
}
