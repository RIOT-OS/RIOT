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
 * @brief RIOT unit tests for C++ assertion macros for @ref pkg_fff
 * @author Jens Wetterich <jens@wetterich-net.de>
 *
 */
#ifndef CPPUNIT_FFF_H
#define CPPUNIT_FFF_H
#if __cplusplus >= 201103L || defined(DOXYGEN)
/**
 * @brief Expect \a count calls to \a name mock
 * @details Needs the @ref pkg_fff for mocks
 * @hideinitializer
 * @param[in] name Name of the mock
 * @param[in] count Expected calls
 */
#define EXPECT_FFF_CALL_COUNT(name, count)                                                     \
    if (name##_fake.call_count != (count)) {                                                   \
        fail();                                                                                \
        printf("Expected %d calls to " #name ", but got %d\n", count, name##_fake.call_count); \
    }

/** @cond Helper macros for the EXPECT_FFF_CALL_PARAMS macro */
#define EXPECT_FFF_CALL_1(name, val1)                                          \
    if (name##_fake.arg0_val != (val1)) {                                      \
        fail();                                                                \
        puts("Argument 0 to mock " #name " doesn't match the expectation.\n"); \
    }
#define EXPECT_FFF_CALL_2(name, val1, val2)                                    \
    if (name##_fake.arg0_val != (val1)) {                                      \
        fail();                                                                \
        puts("Argument 1 to mock " #name " doesn't match the expectation.\n"); \
    }                                                                          \
    if (name##_fake.arg1_val != (val2)) {                                      \
        fail();                                                                \
        puts("Argument 2 to mock " #name " doesn't match the expectation.\n"); \
    }
#define EXPECT_FFF_CALL_3(name, val1, val2, val3)                              \
    if (name##_fake.arg0_val != (val1)) {                                      \
        fail();                                                                \
        puts("Argument 1 to mock " #name " doesn't match the expectation.\n"); \
    }                                                                          \
    if (name##_fake.arg1_val != (val2)) {                                      \
        fail();                                                                \
        puts("Argument 2 to mock " #name " doesn't match the expectation.\n"); \
    }                                                                          \
    if (name##_fake.arg2_val != (val3)) {                                      \
        fail();                                                                \
        puts("Argument 3 to mock " #name " doesn't match the expectation.\n"); \
    }
#define EXPECT_FFF_CALL_4(name, val1, val2, val3, val4)                        \
    if (name##_fake.arg0_val != (val1)) {                                      \
        fail();                                                                \
        puts("Argument 1 to mock " #name " doesn't match the expectation.\n"); \
    }                                                                          \
    if (name##_fake.arg1_val != (val2)) {                                      \
        fail();                                                                \
        puts("Argument 2 to mock " #name " doesn't match the expectation.\n"); \
    }                                                                          \
    if (name##_fake.arg2_val != (val3)) {                                      \
        fail();                                                                \
        puts("Argument 3 to mock " #name " doesn't match the expectation.\n"); \
    }                                                                          \
    if (name##_fake.arg3_val != (val4)) {                                      \
        fail();                                                                \
        puts("Argument 4 to mock " #name " doesn't match the expectation.\n"); \
    }
#define EXPECT_FFF_CALL_5(name, val1, val2, val3, val4, val5)                  \
    if (name##_fake.arg0_val != (val1)) {                                      \
        fail();                                                                \
        puts("Argument 1 to mock " #name " doesn't match the expectation.\n"); \
    }                                                                          \
    if (name##_fake.arg1_val != (val2)) {                                      \
        fail();                                                                \
        puts("Argument 2 to mock " #name " doesn't match the expectation.\n"); \
    }                                                                          \
    if (name##_fake.arg2_val != (val3)) {                                      \
        fail();                                                                \
        puts("Argument 3 to mock " #name " doesn't match the expectation.\n"); \
    }                                                                          \
    if (name##_fake.arg3_val != (val4)) {                                      \
        fail();                                                                \
        puts("Argument 4 to mock " #name " doesn't match the expectation.\n"); \
    }                                                                          \
    if (name##_fake.arg4_val != (val5)) {                                      \
        fail();                                                                \
        puts("Argument 5 to mock " #name " doesn't match the expectation.\n"); \
    }
#define EXPECT_FFF_CALL_6(name, val1, val2, val3, val4, val5, val6)            \
    if (name##_fake.arg0_val != (val1)) {                                      \
        fail();                                                                \
        puts("Argument 1 to mock " #name " doesn't match the expectation.\n"); \
    }                                                                          \
    if (name##_fake.arg1_val != (val2)) {                                      \
        fail();                                                                \
        puts("Argument 2 to mock " #name " doesn't match the expectation.\n"); \
    }                                                                          \
    if (name##_fake.arg2_val != (val3)) {                                      \
        fail();                                                                \
        puts("Argument 3 to mock " #name " doesn't match the expectation.\n"); \
    }                                                                          \
    if (name##_fake.arg3_val != (val4)) {                                      \
        fail();                                                                \
        puts("Argument 4 to mock " #name " doesn't match the expectation.\n"); \
    }                                                                          \
    if (name##_fake.arg5_val != (val6)) {                                      \
        fail();                                                                \
        puts("Argument 6 to mock " #name " doesn't match the expectation.\n"); \
    }
#define GET_FFF_MACRO(_1, _2, _3, _4, _5, _6, NAME, ...) NAME
/** @endcond */
/**
 * @brief Expect that the last call to \a mock was made with the given parameters
 * @details Needs the @ref pkg_fff for mocks
 * @hideinitializer
 * @param[in] mock Name of the mock
 * @param[in] ... params
 */
#define EXPECT_FFF_CALL_PARAMS(mock, ...)                                               \
    GET_FFF_MACRO(__VA_ARGS__, EXPECT_FFF_CALL_6, EXPECT_FFF_CALL_5, EXPECT_FFF_CALL_4, \
                  EXPECT_FFF_CALL_3, EXPECT_FFF_CALL_2, EXPECT_FFF_CALL_1)              \
    (mock, __VA_ARGS__)
#else
#error This library needs C++11 and newer
#endif
#endif
/** @} */
