/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_test_helper TEST_HELPER
 * @ingroup     sys
 * @brief       Helpers that can be used to write tests
 * @{
 *
 * @file        test_helper.h
 * @brief       A collection of useful helpers when writing tests
 *
 * @author      Kevin Weiss <kevin.weiss@haw-hamburg.de>
 */

#ifndef TEST_HELPER_H
#define TEST_HELPER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   A printf wrapper that creates a JSON formatted name and printf
 *          value
 *
 * Prints a string name then the standard printf arguments.  For example the
 * name may be "function" and the printf arguments may be ("function_call(%d)",
 * 42), the result will print {"function":"function_call(42)"}\n.
 * @note    Only use if needed, other functions don't use printf and could save
            bytes.
 */
#define JSON_PRINTF_WRAPPER(name, ...) \
do { \
    printf("{\"%s\":\"", name); \
    printf(__VA_ARGS__); \
    printf("\"}\n"); \
} while (0)

/**
 * @brief   Prints an echo from the shell command
 *
 * @param[in] argc  number of arguments
 * @param[in] argv  array of arguments
 */
void print_echo(int argc, char **argv);

/**
 * @brief   Prints byte array value and name key in json formatting
 *
 * For example name="data" and a byte array of 1, 2, 3 were passed in then
 * {"name":[1,2,3]} would print
 *
 * @param[in] name  string of the key of the json element
 * @param[in] bytes byte array to be printed
 * @param[in] size  size of the byte array
 */
void print_json_byte_array(const char* name, uint8_t* bytes, size_t size);

/**
 * @brief   Prints an int value and name key in json formatting
 *
 * For example name="data" and a integer of 42 were passed in then {"name":42}
 * would print
 *
 * @param[in] name  string of the key of the json element
 * @param[in] val   integer value to print
 */
void print_json_i32_dec(const char* name, int32_t val);

/**
 * @brief   Prints an string value and name key in json formatting
 *
 * For example of name="answer_of_life" and a string value of "forty_two"
 * passed in then {"answer_of_life":"forty_two"} would print
 *
 * @param[in] name  string of the key of the json element
 * @param[in] val   integer value to print
 */
void print_json_str(const char* name, const char* val);

#ifdef __cplusplus
}
#endif

#endif /* TEST_HELPER_H */
/** @} */
