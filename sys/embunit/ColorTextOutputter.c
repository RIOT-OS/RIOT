/*
 * Copyright (C) 2015 Janos Kutscherauer <noshky@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdio.h>
#include "ColorTextOutputter.h"
#include "ColorOutputter.h"
#include "ColorTextColors.h"

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
