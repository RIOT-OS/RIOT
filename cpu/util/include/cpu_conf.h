/*
 * Copyright (C) 2020 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_util
 *
 * @{
 *
 * @file
 * @brief       Dummy file
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#ifndef CPU_CONF_H
#define CPU_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Dummy stack sizes
 *          These are not used, but can't be 0 / undefined.
 *
 * @{
 */
#define THREAD_STACKSIZE_DEFAULT        (1024)
#define THREAD_STACKSIZE_IDLE           (1024)
#define THREAD_EXTRA_STACKSIZE_PRINTF   (1024)
/** @} */


#ifdef __cplusplus
}
#endif

#endif /* CPU_CONF_H */
