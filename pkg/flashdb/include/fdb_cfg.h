/*
 * Copyright (c) 2020, Armink, <armink.ztl@gmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief configuration file
 */

#ifndef FDB_CFG_H
#define FDB_CFG_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief disable all debug output unless explicitly requested
 */
#ifndef FDB_DEBUG_ENABLE
#define FDB_PRINT(...)
#endif

#ifdef __cplusplus
}
#endif
#endif /* FDB_CFG_H */
