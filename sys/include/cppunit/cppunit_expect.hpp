/*
 * Copyright (C) 2021 Jens Wetterich <jens@wetterich-net.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @addtogroup unittests_cpp
 * @{
 * @file
 * @brief   RIOT unit tests for C++ assertion macros
 * @author  Jens Wetterich <jens@wetterich-net.de>
 *
 */
#ifndef CPPUNIT_EXPECT_H
#define CPPUNIT_EXPECT_H
#if __cplusplus >= 201103L || defined(DOXYGEN)
/**
 * @brief   Expect equality of the \a actual and \a expected value
 * @hideinitializer
 * @param[in]   expected    Expected value
 * @param[in]   actual      Actual value
 * @param[in]   msg         Message to print in case of failure
 */
#define EXPECT_EQ(expected, actual, msg)                                                   \
    static_assert(std::is_integral<decltype(expected)>::value,                             \
                  "EXPECT_EQ requires an integral type ");                                 \
    if ((actual) != (expected)) {                                                          \
        fail();                                                                            \
        if (std::is_same<decltype(expected), bool>::value) {                               \
            printf("Expected: %s, actual: %s\n" msg "\n", (expected) ? "true" : "false",   \
                   (actual) ? "true" : "false");                                           \
        }                                                                                  \
        else if (std::is_unsigned<decltype(expected)>::value) {                            \
            printf("Expected: %u, actual: %u\n" msg "\n", static_cast<unsigned>(expected), \
                   static_cast<unsigned>(actual));                                         \
        }                                                                                  \
        else {                                                                             \
            printf("Expected: %d, actual: %d\n" msg "\n", static_cast<int>(expected),      \
                   static_cast<int>(actual));                                              \
        }                                                                                  \
    }
/**
 * @brief   Expect string equality of the \a actual and \a expected value
 * @details Interprets both values as const char* string
 * @hideinitializer
 * @param[in]   expected    Expected value
 * @param[in]   actual      Actual value
 * @param[in]   msg         Message to print in case of failure
 */
#define EXPECT_STREQ(expected, actual, msg)                                             \
    auto expected_str = static_cast<const char*>(expected);                             \
    auto actual_str = static_cast<const char*>(actual);                                 \
    if (strcmp(expected_str, actual_str) != 0) {                                        \
        fail();                                                                         \
        printf(msg " not equal! Expected: %s, actual: %s\n", expected_str, actual_str); \
    }
#else
#error This library needs C++11 and newer
#endif
#endif
/** @} */
