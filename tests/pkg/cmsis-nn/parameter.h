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
 * @brief           CNN parameters
 */

#ifndef PARAMETER_H
#define PARAMETER_H

#ifdef __cplusplus
extern "C" {
#endif

#define CONV1_IM_DIM 32
#define CONV1_IM_CH 3
#define CONV1_KER_DIM 5
#define CONV1_PADDING 2
#define CONV1_STRIDE 1
#define CONV1_OUT_CH 32
#define CONV1_OUT_DIM 32

#define POOL1_KER_DIM 3
#define POOL1_STRIDE 2
#define POOL1_PADDING 0
#define POOL1_OUT_DIM 16

#define CONV2_IM_DIM 16
#define CONV2_IM_CH 32
#define CONV2_KER_DIM 5
#define CONV2_PADDING 2
#define CONV2_STRIDE 1
#define CONV2_OUT_CH 16
#define CONV2_OUT_DIM 16

#define POOL2_KER_DIM 3
#define POOL2_STRIDE 2
#define POOL2_PADDING 0
#define POOL2_OUT_DIM 8

#define CONV3_IM_DIM 8
#define CONV3_IM_CH 16
#define CONV3_KER_DIM 5
#define CONV3_PADDING 2
#define CONV3_STRIDE 1
#define CONV3_OUT_CH 32
#define CONV3_OUT_DIM 8

#define POOL3_KER_DIM 3
#define POOL3_STRIDE 2
#define POOL3_PADDING 0
#define POOL3_OUT_DIM 4

#define IP1_DIM 4*4*32
#define IP1_IM_DIM 4
#define IP1_IM_CH 32
#define IP1_OUT 10

#ifdef __cplusplus
} /* end extern "C" */
#endif

/** @} */
#endif /* PARAMETER_H */
