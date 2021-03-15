/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
