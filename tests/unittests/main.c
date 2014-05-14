/*
 * Copyright (C) 2014 Martin Lenders
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

#include "embUnit/embUnit.h"

#include "lpm.h"

#ifdef OUTPUT
#define OUTPUT_XML      (1)
#define OUTPUT_TEXT     (2)
#define OUTPUT_COMPILER (4)

#if (OUTPUT==OUTPUT_XML)
#include "textui/XMLOutputter.h"

#define OUTPUTTER   (XMLOutputter_outputter())
#endif

#if (OUTPUT==OUTPUT_TEXT)
#include "textui/TextOutputter.h"

#define OUTPUTTER   (TextOutputter_outputter())
#endif

#if (OUTPUT==OUTPUT_COMPILER)
#include "textui/CompilerOutputter.h"

#define OUTPUTTER   (CompilerOutputter_outputter())
#endif

#include "textui/TextUIRunner.h"

#define TESTS_START()   TextUIRunner_start()
#define TESTS_RUN(t)    TextUIRunner_runTest(t)
#define TESTS_END()     TextUIRunner_end()

#else

#define TESTS_START()   TestRunner_start()
#define TESTS_RUN(t)    TestRunner_runTest(t)
#define TESTS_END()     TestRunner_end()

#endif


int main(void)
{
#ifdef OUTPUT
    TextUIRunner_setOutputter(OUTPUTTER);
#endif

    TESTS_START();
    /* put test TEST_RUN() calls here */
    TESTS_END();

#if defined (BOARD_NATIVE) && !defined (OUTPUT)
    void _exit(int);
    _exit(TestRunnerHadErrors);
#endif

    lpm_set(LPM_POWERDOWN);

    return 0;
}
