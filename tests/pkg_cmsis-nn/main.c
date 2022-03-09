/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         tests
 * @{
 *
 * @file
 * @brief           Sample application for ARM CMSIS-NN package
 *
 * This example is adapted from ARM CMSIS CIFAR10 example to RIOT by Alexandre Abadie
 * https://github.com/ARM-software/CMSIS_5/tree/develop/CMSIS/NN/Examples/ARM/arm_nn_examples/cifar10
 *
 * @author          Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#include <stdint.h>
#include <stdio.h>
#include "arm_math.h"
#include "parameter.h"
#include "weights.h"

#include "arm_nnfunctions.h"

#include "blob/input.h"

/* There are 10 different classes of objects in the CIFAR10 dataset */
#define CLASSES_NUMOF   10

/* include the input and weights */
static const q7_t conv1_wt[CONV1_IM_CH * CONV1_KER_DIM * CONV1_KER_DIM * CONV1_OUT_CH] = CONV1_WT;
static const q7_t conv1_bias[CONV1_OUT_CH] = CONV1_BIAS;

static const q7_t conv2_wt[CONV2_IM_CH * CONV2_KER_DIM * CONV2_KER_DIM * CONV2_OUT_CH] = CONV2_WT;
static const q7_t conv2_bias[CONV2_OUT_CH] = CONV2_BIAS;

static const q7_t conv3_wt[CONV3_IM_CH * CONV3_KER_DIM * CONV3_KER_DIM * CONV3_OUT_CH] = CONV3_WT;
static const q7_t conv3_bias[CONV3_OUT_CH] = CONV3_BIAS;

static const q7_t ip1_wt[IP1_DIM * IP1_OUT] = IP1_WT;
static const q7_t ip1_bias[IP1_OUT] = IP1_BIAS;

/* Here the image_data should be the raw uint8 type RGB image in [RGB, RGB, RGB ... RGB] format */
// static const uint8_t image_data[CONV1_IM_CH * CONV1_IM_DIM * CONV1_IM_DIM] = IMG_DATA;
static q7_t output_data[IP1_OUT];

/* vector buffer: max(im2col buffer,average pool buffer, fully connected buffer) */
static q7_t col_buffer[2 * 5 * 5 * 32 * 2];
static q7_t img_buffer1[32 * 32 * 10 * 4];
static q7_t *img_buffer2 = (q7_t *)(img_buffer1 + (32 * 32 * 32));

static const char classes[CLASSES_NUMOF][6] = {
    "plane", "car", "bird", "cat", "deer", "dog", "frog", "horse", "ship", "truck" };

int main(void)
{
    printf("start execution\n");

    uint8_t *image_data = (uint8_t *)input;

    /* input pre-processing */
    int mean_data[3] = INPUT_MEAN_SHIFT;
    unsigned int scale_data[3] = INPUT_RIGHT_SHIFT;
    for (unsigned i = 0; i < input_len; i += 3) {
        img_buffer2[i]     = (q7_t)__SSAT( ((((int)image_data[i]     - mean_data[0]) << 7) + (0x1 << (scale_data[0] - 1)))
                                 >> scale_data[0], 8);
        img_buffer2[i + 1] = (q7_t)__SSAT( ((((int)image_data[i + 1] - mean_data[1]) << 7) + (0x1 << (scale_data[1] - 1)))
                                 >> scale_data[1], 8);
        img_buffer2[i + 2] = (q7_t)__SSAT( ((((int)image_data[i + 2] - mean_data[2]) << 7) + (0x1 << (scale_data[2] - 1)))
                                 >> scale_data[2], 8);
    }

    /* conv1 img_buffer2 -> img_buffer1 */
    arm_convolve_HWC_q7_RGB(img_buffer2, CONV1_IM_DIM, CONV1_IM_CH, conv1_wt, CONV1_OUT_CH, CONV1_KER_DIM, CONV1_PADDING,
                            CONV1_STRIDE, conv1_bias, CONV1_BIAS_LSHIFT, CONV1_OUT_RSHIFT, img_buffer1, CONV1_OUT_DIM,
                            (q15_t *)col_buffer, NULL);

    arm_relu_q7(img_buffer1, CONV1_OUT_DIM * CONV1_OUT_DIM * CONV1_OUT_CH);

    /* pool1 img_buffer1 -> img_buffer2 */
    arm_maxpool_q7_HWC(img_buffer1, CONV1_OUT_DIM, CONV1_OUT_CH, POOL1_KER_DIM,
                       POOL1_PADDING, POOL1_STRIDE, POOL1_OUT_DIM, NULL, img_buffer2);

    /* conv2 img_buffer2 -> img_buffer1 */
    arm_convolve_HWC_q7_fast(img_buffer2, CONV2_IM_DIM, CONV2_IM_CH, conv2_wt, CONV2_OUT_CH, CONV2_KER_DIM,
                             CONV2_PADDING, CONV2_STRIDE, conv2_bias, CONV2_BIAS_LSHIFT, CONV2_OUT_RSHIFT, img_buffer1,
                             CONV2_OUT_DIM, (q15_t *)col_buffer, NULL);

    arm_relu_q7(img_buffer1, CONV2_OUT_DIM * CONV2_OUT_DIM * CONV2_OUT_CH);

    /* pool2 img_buffer1 -> img_buffer2 */
    arm_maxpool_q7_HWC(img_buffer1, CONV2_OUT_DIM, CONV2_OUT_CH, POOL2_KER_DIM,
                       POOL2_PADDING, POOL2_STRIDE, POOL2_OUT_DIM, col_buffer, img_buffer2);

  /* conv3 img_buffer2 -> img_buffer1 */
    arm_convolve_HWC_q7_fast(img_buffer2, CONV3_IM_DIM, CONV3_IM_CH, conv3_wt, CONV3_OUT_CH, CONV3_KER_DIM,
                             CONV3_PADDING, CONV3_STRIDE, conv3_bias, CONV3_BIAS_LSHIFT, CONV3_OUT_RSHIFT, img_buffer1,
                             CONV3_OUT_DIM, (q15_t *)col_buffer, NULL);

    arm_relu_q7(img_buffer1, CONV3_OUT_DIM * CONV3_OUT_DIM * CONV3_OUT_CH);

    /* pool3 img_buffer-> img_buffer2 */
    arm_maxpool_q7_HWC(img_buffer1, CONV3_OUT_DIM, CONV3_OUT_CH, POOL3_KER_DIM,
                       POOL3_PADDING, POOL3_STRIDE, POOL3_OUT_DIM, col_buffer, img_buffer2);

    arm_fully_connected_q7_opt(img_buffer2, ip1_wt, IP1_DIM, IP1_OUT, IP1_BIAS_LSHIFT, IP1_OUT_RSHIFT, ip1_bias,
                               output_data, (q15_t *)img_buffer1);

    arm_softmax_q7(output_data, CLASSES_NUMOF, output_data);

    int val = -1;
    uint8_t class_idx = 0;
    for (unsigned i = 0; i < CLASSES_NUMOF; i++) {
        if (output_data[i] > val) {
            val = output_data[i];
            class_idx = i;
        }
    }

    if (val > 0) {
        printf("Predicted class: %s\n", classes[class_idx]);
    }
    else {
        puts("No match found");
    }

    return 0;
}
