/*
 * Copyright (C) 2015 Janos Kutscherauer <noshky@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file    ColorOutputter.c
 */
#include <stdio.h>
#include "ColorOutputter.h"
#include "ColorTextColors.h"

static void ColorOutputter_printHeader(OutputterRef self)
{
    (void) self;
}

static void ColorOutputter_printStartTest(OutputterRef self, TestRef test)
{
    (void) self;
    (void) test;
}

static void ColorOutputter_printEndTest(OutputterRef self, TestRef test)
{
    (void) self;
    (void) test;
}

static void ColorOutputter_printSuccessful(OutputterRef self, TestRef test, int runCount)
{
    (void) self;
    (void) test;
    (void) runCount;
    printf(CGREEN "." CDEFAULT);
}

static void ColorOutputter_printFailure(OutputterRef self, TestRef test, char *msg, int line,
        char *file, int runCount)
{
    (void) self;
    (void) runCount;
    printf("\n" CRED "FAILED %s (%s:%d) %s" CDEFAULT "\n", Test_name(test), file, line, msg);
}

void ColorOutputter_printStatistics(OutputterRef self, TestResultRef result)
{
    (void) self;
    if (result->failureCount) {
        printf("\n" BGRED SBOLD "FAILED" SDEFAULT " (%d of %d failed)", result->failureCount,
                result->runCount);
    }
    else {
        printf("\n" BGGREEN SBOLD "OK" SDEFAULT " (%d tests)", result->runCount);
    }
    printf(LINEFILL BGDEFAULT "\n");
}

static const OutputterImplement ColorOutputterImplement = {
    (OutputterPrintHeaderFunction) ColorOutputter_printHeader,
    (OutputterPrintStartTestFunction) ColorOutputter_printStartTest,
    (OutputterPrintEndTestFunction) ColorOutputter_printEndTest,
    (OutputterPrintSuccessfulFunction) ColorOutputter_printSuccessful,
    (OutputterPrintFailureFunction) ColorOutputter_printFailure,
    (OutputterPrintStatisticsFunction) ColorOutputter_printStatistics
};

static const Outputter ColorOutputter = {
    (OutputterImplementRef) &ColorOutputterImplement
};

OutputterRef ColorOutputter_outputter(void)
{
    return (OutputterRef) &ColorOutputter;
}
/** @} */
