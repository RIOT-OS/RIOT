/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef TEST_UTILS_RESULT_OUTPUT_H
#define TEST_UTILS_RESULT_OUTPUT_H

/**
 * @defgroup    test_utils_result_output Test result output
 * @ingroup     sys
 * @brief       Utility function for abstraction of test result output format
 *
 * The TURO module provides an abstraction layer allowing salient data to be
 * provided for tests independent of format or medium. The intention is to have
 * a test that expects some data, for example, reading some registers,
 * output the results in a know way, for example json.  This should help
 * keeping the test results stable and not lock anyone into a particular
 * format. If JSON is too heavy all tests using this can be swapped out for
 * something lighter, for example CBOR. Then the tests should not have to be
 * adapted. There can also be python layers that coordinate the output results,
 * ideally done with riotctrl.
 *
 * Only one implementation should be selected, for example,
 * test_utils_result_output_json.
 *
 * Some of the design decisions include:
 * - ability to flush immediately to prevent the need for large buffers
 * - selectable output format based on `USEMODULE`:
 *   + `test_utils_result_output_check`: @ref test_utils_result_output_check
 *   + `test_utils_result_output_json`: @ref test_utils_result_output_json
 *   + `test_utils_result_output_txt`: @ref test_utils_result_output_txt
 * - exit status similar to a linux exit status.
 * - readable raw output used in the CI to assist with reproducing errors
 * - structure doesn't need to be enforced in every implementation to save
 * bytes, see test_utils_result_output_check for structure assertions
 *
 * Some limitations are:
 * - Only one type of result output implementation can be used at a time
 * - Errors may be caused by the specific result output implementation making
 * it difficult to debug
 *
 * @{
 * @file
 * @brief       Provides abstraction and convention for output of test results
 *
 * @author      Kevin Weiss <kevin.weiss@haw-hamburg.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "result_output_types.h"

/** @defgroup    turo_API Test Utils Result Output Implementation API
 * @brief   The turo API that must have an implementation.
 * @{
 */

/**
 * @brief   Type for a TURO object
 *
 * @note    API implementers: `struct turo` needs to be defined by
 *         implementation-specific `result_output_types.h`.
 */
typedef struct turo turo_t;

/**
 * @brief  Provides initial values for the turo context.
 *
 * @param[out] ctx       The implementation specific turo context.
 */
void turo_init(turo_t *ctx);

/**
 * @brief  Outputs a container open.
 *
 * Must be used when starting formatted result output.
 *
 * @param[in] ctx       The implementation specific turo context.
 */
void turo_container_open(turo_t *ctx);

/**
 * @brief  Outputs a signed 32 bit integer.
 *
 * @pre `turo_container_open` called
 *
 * @param[in] ctx       The implementation specific turo context.
 * @param[in] val       The value to output.
 */
void turo_s32(turo_t *ctx, int32_t val);

/**
 * @brief  Outputs an unsigned 32 bit integer.
 *
 * @pre `turo_container_open` called
 *
 * @param[in] ctx       The implementation specific turo context.
 * @param[in] val       The value to output.
 */
void turo_u32(turo_t *ctx, uint32_t val);

/**
 * @brief  Outputs a signed 64 bit integer.
 *
 * @pre `turo_container_open` called
 *
 * @param[in] ctx       The implementation specific turo context.
 * @param[in] val       The value to output.
 */
void turo_s64(turo_t *ctx, int64_t val);

/**
 * @brief  Outputs a formatted result unsigned 64 bit integer.
 *
 * @pre `turo_container_open` called
 *
 * @param[in] ctx       The implementation specific turo context.
 * @param[in] val       The value to output.
 */
void turo_u64(turo_t *ctx, uint64_t val);

/**
 * @brief  Outputs a formatted float result of varied precision.
 *
 * @pre `turo_container_open` called
 *
 * @param[in] ctx       The implementation specific turo context.
 * @param[in] val       The value to output.
 */
void turo_float(turo_t *ctx, float val);

/**
 * @brief  Outputs a formatted string string.
 *
 * @pre `turo_container_open` called
 *
 * @param[in] ctx       The implementation specific turo context.
 * @param[in] str       The string to output.
 */
void turo_string(turo_t *ctx, const char *str);

/**
 * @brief  Outputs a formatted boolean result.
 *
 * @pre `turo_container_open` called
 *
 * @param[in] ctx       The implementation specific turo context.
 * @param[in] val       The value to output.
 */
void turo_bool(turo_t *ctx, bool val);

/**
 * @brief  Outputs a formatted open of a dictionary result.
 *
 * A `turo_dict_close` must match.
 *
 * @pre `turo_container_open` called
 *
 * @param[in] ctx       The implementation specific turo context.
 */
void turo_dict_open(turo_t *ctx);

/**
 * @brief  Outputs a formatted open of a dictionary result.
 *
 * A turo value must follow.
 *
 * @pre `turo_container_open` called
 *
 * @param[in] ctx       The implementation specific turo context.
 * @param[in] key       The key of the dictionary.
 */
void turo_dict_key(turo_t *ctx, const char *key);

/**
 * @brief  Outputs a formatted close of a dictionary result.
 *
 * @pre `turo_container_open` called
 * @pre `turo_dict_open` called
 *
 * @param[in] ctx       The implementation specific turo context.
 */
void turo_dict_close(turo_t *ctx);

/**
 * @brief  Outputs a formatted open of an array result.
 *
 * A `turo_array_close` must match.
 *
 * @pre `turo_container_open` called
 *
 * @param[in] ctx       The implementation specific turo context.
 */
void turo_array_open(turo_t *ctx);

/**
 * @brief  Outputs a formatted close of an array result.
 *
 * @pre `turo_container_open` called
 * @pre `turo_array_open` called
 *
 * @param[in] ctx       The implementation specific turo context.
 */
void turo_array_close(turo_t *ctx);

/**
 * @brief  Outputs a formatted close of a container result.
 *
 * @pre `turo_container_open` called
 *
 * @param[in] ctx           The implementation specific turo context.
 * @param[in] exit_status   Exit status code for the result, 0 is success.
 */
void turo_container_close(turo_t *ctx, int exit_status);
/** @} */

/** @defgroup    turo_helpers Test Utils Result Output Helpers
 * @brief   Common functions and helpers that all implementations can use.
 * @{
 */

/**
 * @brief   Outputs a formatted uint8 array result.
 *
 * @pre `turo_container_open` called
 *
 * @param[in] ctx       The implementation specific turo context.
 * @param[in] vals      A buffer of data to output.
 * @param[in] size      The amount of elements to output.
 */
void turo_array_u8(turo_t *ctx, uint8_t *vals, size_t size);

/**
 * @brief   Outputs a int32 array result.
 *
 * @pre `turo_container_open` called
 *
 * @param[in] ctx       The implementation specific turo context.
 * @param[in] vals      A buffer of data to output.
 * @param[in] size      The amount of elements to output.
 */
void turo_array_s32(turo_t *ctx, int32_t *vals, size_t size);

/**
 * @brief   Outputs a dict with string data.
 *
 * @pre `turo_container_open` called
 *
 * @param[in] ctx       The implementation specific turo context.
 * @param[in] key       A dictionary key.
 * @param[in] val       A string value of the dictionary
 */
void turo_dict_string(turo_t *ctx, const char *key, const char *val);

/**
 * @brief   Outputs a dict with integer data.
 *
 * @pre `turo_container_open` called
 *
 * @param[in] ctx       The implementation specific turo context.
 * @param[in] key       A dictionary key.
 * @param[in] val       The integer value of the dictionary.
 */
void turo_dict_s32(turo_t *ctx, const char *key, int32_t val);

/**
 * @brief   Outputs a full successful int32 turo result.
 *
 * This includes all opening and closing of turo elements.
 *
 * @pre turo ctx initialized
 *
 * @param[in] ctx       The implementation specific turo context.
 * @param[in] val       The value to output.
 */
void turo_simple_s32(turo_t *ctx, int32_t val);

/**
 * @brief   Outputs a full successful uint8 array turo result.
 *
 * This includes all opening and closing of turo elements.
 *
 * @pre turo ctx initialized
 *
 * @param[in] ctx       The implementation specific turo context.
 * @param[in] vals      The buffer of the integers.
 * @param[in] size      Number of elements in the array.
 */
void turo_simple_array_u8(turo_t *ctx, uint8_t *vals, size_t size);

/**
 * @brief   Outputs a full successful int32 array turo result.
 *
 * This includes all opening and closing of turo elements.
 *
 * @pre turo ctx initialized
 *
 * @param[in] ctx       The implementation specific turo context.
 * @param[in] vals      The buffer of the integers.
 * @param[in] size      Number of elements in the array.
 */
void turo_simple_array_s32(turo_t *ctx, int32_t *vals, size_t size);

/**
 * @brief   Outputs a full successful dict with string turo result.
 *
 * This includes all opening and closing of turo elements.
 *
 * @pre turo ctx initialized
 *
 * @param[in] ctx       The implementation specific turo context.
 * @param[in] key       The dictionary key.
 * @param[in] val       The string value.
 */
void turo_simple_dict_string(turo_t *ctx, const char *key, const char *val);

/**
 * @brief   Outputs a full successful dict with an integer turo result.
 *
 * This includes all opening and closing of turo elements.
 *
 * @pre turo ctx initialized
 *
 * @param[in] ctx       The implementation specific turo context.
 * @param[in] key       The dictionary key.
 * @param[in] val       The integer value.
 */
void turo_simple_dict_s32(turo_t *ctx, const char *key, int32_t val);

/**
 * @brief   Outputs a full turo result with exit code.
 *
 * This includes all opening and closing of turo elements.
 *
 * @pre turo ctx initialized
 *
 * @param[in] ctx           The implementation specific turo context.
 * @param[in] exit_status   The exit status to output.
 */
void turo_simple_exit_status(turo_t *ctx, int exit_status);
/** @} */

#ifdef __cplusplus
}
#endif
/** @} */
#endif /* TEST_UTILS_RESULT_OUTPUT_H */
