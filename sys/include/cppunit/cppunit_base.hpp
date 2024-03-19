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
 * @brief   RIOT unit tests for C++ base classes and macros
 * @author  Jens Wetterich <jens@wetterich-net.de>
 *
 */
#ifndef CPPUNIT_BASE_H
#define CPPUNIT_BASE_H
#if __cplusplus >= 201103L || defined(DOXYGEN)
#include <array>
#include <cstdio>
#include <cstring>
#include <type_traits>
#ifndef CPPUNIT_SUITE_CNT
/**
 * @brief Maximum amount of tests in one test suite
 */
#define CPPUNIT_SUITE_CNT (10)
#endif
/**
 * @brief RIOT C++ namespace
 */
namespace riot {
/**
 * @brief namespace for cpp unit tests
 */
namespace testing {
/**
 * @brief Test base class
 * @details Should not be instantiated directly.
 * @sa #TEST(suite, name) macro
 */
class test {
private:
    bool suc = true; ///< indicates success of the test after running
public:
    /**
     * @brief   Run the test
     * @details Should not be called directly, only via macros
     * @sa      #RUN_SUITE(name)
     * @return  whether the test completed without errors
     */
    virtual bool run() = 0;
    /**
     * @brief   Indicate failure during test run
     * @details Should be called by assertions macros
     * @sa      #EXPECT_EQ(expected, actual, msg)
     * @sa      #EXPECT_STREQ(expected, actual, msg)
     * @sa      #EXPECT_FFF_CALL_COUNT(name, count)
     * @sa      #EXPECT_FFF_CALL_PARAMS(mock, ...)
     */
    void fail() {
        suc = false;
    }
    /**
     * @brief   Check whether the test completed successfully
     * @return  whether the test completed without errors
     */
    bool success() const {
        return suc;
    }
};
/**
 * @brief   Test suite base class
 * @details Should not be instantiated directly.
 *          To customize a test suite with custom set_up and tear down methods,
 *          inherit from this class and override set_up() and/or tear_down().
 *          They will before / after every test.
 * @sa      #TEST_SUITE(name) macro
 * @sa      #TEST_SUITE_F(suite, name) macro
 * @sa      test_suite::set_up()
 * @sa      test_suite::tear_down()
 */
class test_suite {
protected:
    bool suc = true; ///< Indicates success of all tests after running the suite
    std::array<test*, CPPUNIT_SUITE_CNT> tests{}; ///< array of tests to run
public:
    /**
     * @brief method to run before each test
     */
    virtual void set_up() {
    }
    /**
     * @brief method to run after each test
     */
    virtual void tear_down() {
    }
    /**
     * @brief get the name of the test suite
     * @return name string
     */
    virtual const char* get_name() const {
        return "";
    };
    /**
     * @brief Run all tests in the suite
     */
    virtual void run() {
        printf("----\nStarting Test suite %s\n", get_name());
        for (auto test : tests) {
            if (test) {
                suc = test->run() && suc;
            }
        }
        printf("Suite %s completed: %s\n----\n", get_name(), suc ? "SUCCESS" : "FAILURE");
    }
    /**
     * @brief Run all tests in the suite
     */
    void addTest(test* test) {
        for (int i = 0; i < CPPUNIT_SUITE_CNT; i++) {
            if (!tests[i]) {
                tests[i] = test;
                break;
            }
        }
    }
};
}// namespace testing
}// namespace riot
/**
 * @brief Run the test suite \a name
 * @hideinitializer
 * @param[in] name Name of the suite
 */
#define RUN_SUITE(name) test_suite_##name.run();

/**
 * @brief Instantiate a test suite with custom test fixture
 * @hideinitializer
 * @param[in] suite Name of the custom test fixture class
 * @param[in] name Instantiation name of the suite
 */
#define TEST_SUITE_F(suite, name)                                              \
    static_assert(sizeof(#suite) > 1, "test fixture class must not be empty"); \
    static_assert(sizeof(#name) > 1, "test_suite name must not be empty");     \
    class test_suite_##name : public suite {                                   \
        const char* get_name() const override {                                \
            return #name;                                                      \
        };                                                                     \
    };                                                                         \
    test_suite_##name test_suite_##name;

/**
 * @brief Instantiate a test suite
 * @hideinitializer
 * @param[in] name Instantiation name of the suite
 */
#define TEST_SUITE(name) TEST_SUITE_F(::riot::testing::test_suite, name)

/**
 * @brief   Begin the definition of a test
 * @details Insert the test body after the macro
 * @hideinitializer
 * @param[in]   suite   Name of the suite to add the test to
 * @param[in]   name    Instantiation name of the test
 */
#define TEST(suite, name)                                                      \
    class Test_##name : public ::riot::testing::test {                         \
    private:                                                                   \
        void test_body();                                                      \
                                                                               \
    public:                                                                    \
        Test_##name() {                                                        \
            test_suite_##suite.addTest(this);                                  \
        }                                                                      \
        bool run() {                                                           \
            test_suite_##suite.set_up();                                       \
            printf("Running test " #name "...\n");                             \
            test_body();                                                       \
            printf("Test " #name ": %s\n", success() ? "SUCCESS" : "FAILURE"); \
            test_suite_##suite.tear_down();                                    \
            return success();                                                  \
        };                                                                     \
    };                                                                         \
    void Test_##name::test_body()
#else
#error This library needs C++11 and newer
#endif
#endif
/** @} */
