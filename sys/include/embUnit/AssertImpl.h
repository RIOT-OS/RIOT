/*
 * SPDX-FileCopyrightText: 2003 Embedded Unit Project
 * SPDX-License-Identifier: MIT
 */
#ifndef EMBUNIT_ASSERTIMPL_H
#define EMBUNIT_ASSERTIMPL_H

#ifdef  __cplusplus
extern "C" {
#endif

void addFailure(const char *msg, long line, const char *file);  /*TestCase.c*/

void assertImplementationLongLong(long long expected,long long actual, long line, const char *file);
void assertImplementationCStr(const char *expected,const char *actual, long line, const char *file);

#define TEST_ASSERT_EQUAL_STRING(expected_, actual_) \
    do { \
        const char *____expected__ = expected_; \
        const char *____actual__ = actual_; \
        if (stdimpl_strcmp(____expected__, ____actual__) != 0) { \
            assertImplementationCStr(____expected__, ____actual__, __LINE__, __FILE__); \
            return; \
        } \
    } while (0)

#define TEST_ASSERT_EQUAL_INT(expected_, actual_) \
    do { \
        long long ____expected__ = (long long) (expected_); \
        long long ____actual__ = (long long) (actual_); \
        if (____expected__ != ____actual__) { \
            assertImplementationLongLong(____expected__, ____actual__, \
                                         __LINE__, __FILE__); \
            return; \
        } \
    } while (0)

#define TEST_ASSERT_NULL(pointer_) \
    do { \
        __typeof__(pointer_) ____pointer__ = (pointer_); \
        TEST_ASSERT_MESSAGE(____pointer__ == NULL, #pointer_ " was not null."); \
    } while (0)

#define TEST_ASSERT_NOT_NULL(pointer_) \
    do { \
        __typeof__(pointer_) ____pointer__ = (pointer_); \
        TEST_ASSERT_MESSAGE(____pointer__ != NULL, #pointer_ " was null."); \
    } while (0)

#define TEST_ASSERT_MESSAGE(condition_, message) \
    do { \
        __typeof__(condition_) ____condition__ = (condition_); \
        if (!____condition__) { \
            TEST_FAIL((message)); \
        } \
    } while (0)

#define TEST_ASSERT(condition) \
    do { \
        TEST_ASSERT_MESSAGE((condition), #condition); \
    } while (0)

#define TEST_FAIL(message) \
    do { \
        addFailure((message), __LINE__, __FILE__); \
        return; \
    } while (0)

#ifdef  __cplusplus
}
#endif

#endif /* EMBUNIT_ASSERTIMPL_H */
