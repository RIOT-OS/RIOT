/*
 * SPDX-FileCopyrightText: 2021 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#ifndef RESULT_OUTPUT_TYPES_H
#define RESULT_OUTPUT_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   turo type
 * @internal
 */
struct turo {
    void *empty; /**< Empty type needed due to C11-§6.7.2.1/5 */
};

#ifdef __cplusplus
}
#endif
#endif /* RESULT_OUTPUT_TYPES_H */
