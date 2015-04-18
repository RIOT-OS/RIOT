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
 * $Id: TestResult.c,v 1.4 2004/02/10 16:19:29 arms22 Exp $
 */
#include "Test.h"
#include "TestListener.h"
#include "TestResult.h"

void TestResult_init(TestResult* self,TestListner* listner)
{
    self->runCount = 0;
    self->failureCount = 0;
    self->listener = listner;
}

void TestResult_startTest(TestResult* self,Test* test)
{
    self->runCount++;
    if (self->listener) {
        TestListner_startTest(self->listener, test);
    }
}

void TestResult_endTest(TestResult* self,Test* test)
{
    if (self->listener) {
        TestListner_endTest(self->listener, test);
    }
}

void TestResult_addFailure(TestResult* self,Test* test,const char* msg,int line,const char* file)
{
    self->failureCount++;
    if (self->listener) {
        TestListner_addFailure(self->listener, test, msg, line, file);
    }
}
