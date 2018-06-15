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
 * $Id: TestCase.c,v 1.6 2004/02/10 16:19:29 arms22 Exp $
 */
#include "Test.h"
#include "TestCase.h"
#include "TestResult.h"

static TestResult* result_;
static TestCase* self_;

const char* TestCase_name(TestCase* self)
{
    return self->name;
}

void TestCase_run(TestCase* self,TestResult* result)
{
    TestResult_startTest(result, (Test*)self);
    if (self->setUp) {
        self->setUp();
    }
    if (self->runTest) {
        TestResult* wr =result_;    /*push*/
        TestCase* ws = self_;   /*push*/
        result_ = result;
        self_ = self;
        self->runTest();
        result_ = wr;   /*pop*/
        self_ = ws; /*pop*/
    }
    if (self->tearDown) {
        self->tearDown();
    }
    TestResult_endTest(result, (Test*)self);
}

int TestCase_countTestCases(TestCase* self)
{
    (void)self;
    return 1;
}

const TestImplement TestCaseImplement = {
    (TestNameFunction)          TestCase_name,
    (TestRunFunction)           TestCase_run,
    (TestCountTestCasesFunction)TestCase_countTestCases,
};

void addFailure(const char *msg, long line, const char *file)
{
    TestResult_addFailure(result_, (Test*)self_, (char*)msg, line, (char*)file);
}
