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
 * $Id: HelperMacro.h,v 1.3 2004/02/10 16:19:29 arms22 Exp $
 */
#ifndef EMBUNIT_HELPERMACRO_H
#define EMBUNIT_HELPERMACRO_H

#ifdef __cplusplus
extern "C" {
#endif

#define EMB_UNIT_TESTCASE(ca,sup,tdw,run) \
    static const TestCase   ca = new_TestCase(#ca,sup,tdw,run)

#define EMB_UNIT_TESTSUITE(su,array) \
    static const TestSuite  su = new_TestSuite(#su,(Test**)array,sizeof(array)/sizeof(array[0]))

#define EMB_UNIT_TESTREFS(tests) \
    static Test* const tests[] =

#define EMB_UNIT_ADD_TESTREF(testref) \
                (Test*) testref

#define EMB_UNIT_TESTCALLER(caller,sup,tdw,fixtures) \
    static const TestCaller caller = new_TestCaller(#caller,sup,tdw,sizeof(fixtures)/sizeof(fixtures[0]),(TestFixture*)fixtures)

#define EMB_UNIT_TESTFIXTURES(fixtures) \
    static const TestFixture    fixtures[] =

#define EMB_UNIT_REPEATEDTEST(repeater,test,tmrp) \
    static const RepeatedTest   repeater = new_RepeatedTest(test,tmrp)

#ifdef  __cplusplus
}
#endif

#endif/* EMBUNIT_HELPERMACRO_H */
