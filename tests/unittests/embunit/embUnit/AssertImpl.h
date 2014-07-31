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

void assertImplementationInt(int expected,int actual, long line, const char *file);
void assertImplementationCStr(const char *expected,const char *actual, long line, const char *file);

#define TEST_ASSERT_EQUAL_STRING(expected,actual)\
    if (expected && actual && (stdimpl_strcmp(expected,actual)==0)) {} else {assertImplementationCStr(expected,actual,__LINE__,__FILE__);return;}

#define TEST_ASSERT_EQUAL_INT(expected,actual)\
    if (expected == actual) {} else {assertImplementationInt(expected,actual,__LINE__,__FILE__);return;}

#define TEST_ASSERT_NULL(pointer)\
    TEST_ASSERT_MESSAGE(pointer == NULL,#pointer " was not null.")

#define TEST_ASSERT_NOT_NULL(pointer)\
    TEST_ASSERT_MESSAGE(pointer != NULL,#pointer " was null.")

#define TEST_ASSERT_MESSAGE(condition, message)\
    if (condition) {} else {TEST_FAIL(message);}

#define TEST_ASSERT(condition)\
    if (condition) {} else {TEST_FAIL(#condition);}

#define TEST_FAIL(message)\
    if (0) {} else {addFailure(message,__LINE__,__FILE__);return;}

#ifdef  __cplusplus
}
#endif

#endif/*__ASSERTIMPL_H__*/
