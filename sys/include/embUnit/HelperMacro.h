/*
 * SPDX-FileCopyrightText: 2003 Embedded Unit Project
 * SPDX-License-Identifier: MIT
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

#endif /* EMBUNIT_HELPERMACRO_H */
