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
 * $Id: ColorTextOutputter.c,v 1.4 2003/09/06 13:28:27 arms22 Exp $
 */
#include <stdio.h>
#include "ColorTextOutputter.h"
#include "ColorOutputter.h"

static void ColorTextOutputter_printHeader(OutputterRef self)
{
    (void)self;
}

static void ColorTextOutputter_printStartTest(OutputterRef self,TestRef test)
{
    (void)self;
    printf("- %s\n",Test_name(test));
}

static void ColorTextOutputter_printEndTest(OutputterRef self,TestRef test)
{
    (void)self;
    (void)test;
}

static void ColorTextOutputter_printSuccessful(OutputterRef self,TestRef test,int runCount)
{
    (void)self;
    printf(CGREEN "%d) OK %s" CDEFAULT "\n", runCount, Test_name(test));
}

static void ColorTextOutputter_printFailure(OutputterRef self,TestRef test,char *msg,int line,char *file,int runCount)
{
    (void)self;
    printf(CRED "%d) NG %s" CDEFAULT " (%s:%d) %s\n", runCount, Test_name(test), file, line, msg);
}

void ColorTextOutputter_printStatistics(OutputterRef self,TestResultRef result)
{
    ColorOutputter_printStatistics(self, result);
}

static const OutputterImplement ColorTextOutputterImplement = {
    (OutputterPrintHeaderFunction)      ColorTextOutputter_printHeader,
    (OutputterPrintStartTestFunction)   ColorTextOutputter_printStartTest,
    (OutputterPrintEndTestFunction)     ColorTextOutputter_printEndTest,
    (OutputterPrintSuccessfulFunction)  ColorTextOutputter_printSuccessful,
    (OutputterPrintFailureFunction)     ColorTextOutputter_printFailure,
    (OutputterPrintStatisticsFunction)  ColorTextOutputter_printStatistics,
};

static const Outputter ColorTextOutputter = {
    (OutputterImplementRef)&ColorTextOutputterImplement,
};

OutputterRef ColorTextOutputter_outputter(void)
{
    return (OutputterRef)&ColorTextOutputter;
}
