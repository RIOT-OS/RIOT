/*
 * COPYRIGHT AND PERMISSION NOTICE
 *
 * Copyright (c) 2003 Embedded Unit Project
 *
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, and/or sell copies of the Software, and to permit persons
 * to whom the Software is furnished to do so, provided that the above
 * copyright notice(s) and this permission notice appear in all copies
 * of the Software and that both the above copyright notice(s) and this
 * permission notice appear in supporting documentation.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT
 * OF THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * HOLDERS INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY
 * SPECIAL INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Except as contained in this notice, the name of a copyright holder
 * shall not be used in advertising or otherwise to promote the sale,
 * use or other dealings in this Software without prior written
 * authorization of the copyright holder.
 *
 * $Id: AssertImpl.h,v 1.6 2003/09/16 11:09:53 arms22 Exp $
 */
#ifndef __ASSERTIMPL_H__
#define __ASSERTIMPL_H__

#ifdef  __cplusplus
extern "C" {
#endif

void addFailure(const char *msg, long line, const char *file);  /*TestCase.c*/

void assertImplementationLongLong(long long expected,long long actual, long line, const char *file);
void assertImplementationCStr(const char *expected,const char *actual, long line, const char *file);

#define TEST_ASSERT_EQUAL_STRING(expected_, actual_) \
    do { \
        __typeof__(expected_) ____expected__ = expected_; \
        __typeof__(actual_) ____actual__ = actual_; \
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
            assertImplementationLongLong(____expected__, ____actual__, __LINE__, __FILE__); \
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

#endif/*__ASSERTIMPL_H__*/
