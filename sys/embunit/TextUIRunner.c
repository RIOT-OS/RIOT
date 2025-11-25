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
 * $Id: TextUIRunner.c,v 1.4 2004/02/10 16:20:43 arms22 Exp $
 */
#include "TextOutputter.h"
#include "TextUIRunner.h"

/*  Private
 */
static TestResult result_;
static OutputterRef outputterRef_ = 0;
static int wasfailure_ = 0;

static void TextUIRunner_startTest(TestListnerRef self,TestRef test)
{
    (void)self;
    (void)test;
    wasfailure_ = 0;
}

static void TextUIRunner_endTest(TestListnerRef self,TestRef test)
{
    (void)self;
    if (!wasfailure_)
        Outputter_printSuccessful(outputterRef_,test,result_.runCount);
}

static void TextUIRunner_addFailure(TestListnerRef self,TestRef test,char *msg,int line,char *file)
{
    (void)self;
    wasfailure_ = 1;
    Outputter_printFailure(outputterRef_,test,msg,line,file,result_.runCount);
}

static const TestListnerImplement TextUIRunnerImplement = {
    (TestListnerStartTestCallBack)  TextUIRunner_startTest,
    (TestListnerEndTestCallBack)    TextUIRunner_endTest,
    (TestListnerAddFailureCallBack) TextUIRunner_addFailure,
};

static const TestListner testuirunner_ = {
    (TestListnerImplement*)&TextUIRunnerImplement,
};

/*  Public
 */
void TextUIRunner_setOutputter(OutputterRef outputter)
{
    outputterRef_ = outputter;
}

void TextUIRunner_startWithOutputter(OutputterRef outputter)
{
    TestResult_init(&result_, (TestListnerRef)&testuirunner_);
    TextUIRunner_setOutputter(outputter);
    Outputter_printHeader(outputter);

}

void TextUIRunner_start(void)
{
    if (!outputterRef_)
        outputterRef_ = TextOutputter_outputter();
    TextUIRunner_startWithOutputter(outputterRef_);
}

void TextUIRunner_runTest(TestRef test)
{
    Outputter_printStartTest(outputterRef_,test);
    Test_run(test, &result_);
    Outputter_printEndTest(outputterRef_,test);
}

int TextUIRunner_end(void)
{
    Outputter_printStatistics(outputterRef_,&result_);
    return wasfailure_;
}
