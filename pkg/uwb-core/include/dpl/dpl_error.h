/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_uwb_core
 * @{
 *
 * @file
 * @brief       uwb-core DPL (Decawave Porting Layer) error types
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#ifndef DPL_DPL_ERROR_H
#define DPL_DPL_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief DPL error types
 */
enum dpl_error {
    DPL_OK = 0,
    DPL_ENOMEM = 1,
    DPL_EINVAL = 2,
    DPL_INVALID_PARAM = 3,
    DPL_MEM_NOT_ALIGNED = 4,
    DPL_BAD_MUTEX = 5,
    DPL_TIMEOUT = 6,
    DPL_ERR_IN_ISR = 7,
    DPL_ERR_PRIV = 8,
    DPL_OS_NOT_STARTED = 9,
    DPL_ENOENT = 10,
    DPL_EBUSY = 11,
    DPL_ERROR = 12,
};

/**
 * @brief   dep error type
 */
typedef enum dpl_error dpl_error_t;

#ifdef __cplusplus
}
#endif

#endif /* DPL_DPL_ERROR_H */
