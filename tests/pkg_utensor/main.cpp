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
 * @brief       uTensor example application
 *
 * Adapted from the uTensor sample application by Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <stdalign.h>
#include <stdio.h>
#include <inttypes.h>

alignas(float)
#include "blob/digit.h"         //contains a sample taken from the MNIST test set

#include "deep_mlp.hpp"         //generated model file
#include "tensor.hpp"           //useful tensor classes

int main()
{
    puts("Simple MNIST end-to-end uTensor cli example (device)\n");

    // create the context class, the stage where inferences take place
    Context ctx;

    // because we used alignas(float), we can rest assured that silencing
    // -Wcast-align with an intermediate cast to uintptr_t is fine
    float *digit_as_float = (float *)(uintptr_t)digit;

    // wrap the input digit in a tensor class
    auto input_x = new WrappedRamTensor<float>({1, digit_len >> 2}, digit_as_float);

    // pass ownership of the tensor to the context
    get_deep_mlp_ctx(ctx, input_x);

    // get a reference to the output tensor
    S_TENSOR pred_tensor = ctx.get("y_pred:0");
    // trigger the inference
    ctx.eval();

    // get the result back and display it
    uint8_t pred_label = *(pred_tensor->read<int>(0, 0));
    printf("Predicted label: %d\r\n", pred_label);

    return 0;
}
