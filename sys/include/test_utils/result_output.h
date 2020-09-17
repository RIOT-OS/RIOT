/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    test_utils_result_output Test result output
 * @ingroup     sys
 * @brief       Utility function for abstraction of test result output format
 *
 * @{
 * @file
 * @brief       Provides abstraction and convention for output of test results.
 *
 * @author      Kevin Weiss <kevin.weiss@haw-hamburg.de>
 */

#ifndef TEST_UTILS_RESULT_OUTPUT_H
#define TEST_UTILS_RESULT_OUTPUT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Outputs integer data.
 *
 * The exact output depends on the parser but it will contain formatted
 * integer data.
 *
 * @param[in] turo  parsing instance
 * @param[in] data  integer data to output
 */
void turo_data_int(int turo, int32_t data);

/**
 * @brief   Outputs boolean data.
 *
 * The exact output depends on the parser but it will contain formatted
 * boolean data.
 *
 * @param[in] turo  parsing instance
 * @param[in] data  boolean data to output
 */
void turo_data_bool(int turo, bool data);

/**
 * @brief   Outputs string data.
 *
 * The exact output depends on the parser but it will contain formatted
 * string data.
 *
 * @param[in] turo  parsing instance
 * @param[in] data  string data to output
 */
void turo_data_string(int turo, char* data);

/**
 * @brief   Outputs a uint8_t array data.
 *
 * The exact output depends on the parser but it will contain formatted
 * size of uint8_t data elements.
 *
 * @param[in] turo      parsing instance
 * @param[in] data      buffer of data to output
 * @param[in] size_t    amount of elements to output
 */
void turo_data_u8_array(int turo, uint8_t* data, size_t size);

/**
 * @brief   Outputs an int32_t array data.
 *
 * The exact output depends on the parser but it will contain formatted
 * size of int32_t data elements.
 *
 * @param[in] turo      parsing instance
 * @param[in] data      buffer of data to output
 * @param[in] size_t    amount of elements to output
 */
void turo_data_i32_array(int turo, int32_t* data, size_t size);

/**
 * @brief   Outputs a dict with string data.
 *
 * The exact output depends on the parser but it will contain a formatted
 * dict with a string value.
 *
 * @param[in] turo      parsing instance
 * @param[in] key       dictionary key
 * @param[in] value     string value of the dictionary
 */
void turo_data_string_dict(int turo, char* key, char* value);

/**
 * @brief   Outputs a dict with integer data.
 *
 * The exact output depends on the parser but it will contain a formatted
 * dict with an integer value.
 *
 * @param[in] turo      parsing instance
 * @param[in] key       dictionary key
 * @param[in] value     integer value of the dictionary
 */
void turo_data_int_dict(int turo, char* key, int32_t value);

/**
 * @brief   Outputs a successful result.
 *
 * The exact output depends on the parser but it will contain a formatted
 * success result.
 *
 * @param[in] turo      parsing instance
 */
void turo_result_success(int turo);

/**
 * @brief   Outputs a failure result.
 *
 * The exact output depends on the parser but it will contain a formatted
 * failure result.
 *
 * @param[in] turo      parsing instance
 */
void turo_result_error(int turo);

/**
 * @brief   Outputs a failure result and error code.
 *
 * The exact output depends on the parser but it will contain a formatted
 * error code and result.
 *
 * @param[in] turo      parsing instance
 * @param[in] error     error code to output
 */
void turo_result_error_with_code(int turo, int error);

#ifdef __cplusplus
}
#endif
#endif /* TEST_UTILS_RESULT_OUTPUT_H */
/** @} */
