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

#include "os/os_error.h"

/**
 * @brief DPL error types
 */
enum dpl_error {
    DPL_OK  = OS_OK,
    DPL_ENOMEM = OS_ENOMEM,
    DPL_EINVAL = OS_EINVAL,
    DPL_INVALID_PARAM = OS_INVALID_PARM,
    DPL_MEM_NOT_ALIGNED = OS_MEM_NOT_ALIGNED,
    DPL_BAD_MUTEX = OS_BAD_MUTEX,
    DPL_TIMEOUT = OS_TIMEOUT,
    DPL_ERR_IN_ISR = OS_ERR_IN_ISR,
    DPL_ERR_PRIV = OS_ERR_PRIV,
    DPL_OS_NOT_STARTED = OS_NOT_STARTED,
    DPL_ENOENT = OS_ENOENT,
    DPL_EBUSY = OS_EBUSY,
    DPL_ERROR = OS_ERROR ,
};

/**
 * @brief dpl error type
 */
typedef os_error_t dpl_error_t;

#ifdef __cplusplus
}
#endif

#endif /* DPL_DPL_ERROR_H */
