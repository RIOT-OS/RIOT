/*
 * SPDX-FileCopyrightText: 2003 Embedded Unit Project
 * SPDX-License-Identifier: MIT
 */
#ifndef EMBUNIT_TESTCASE_H
#define EMBUNIT_TESTCASE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct __TestCase   TestCase;
typedef struct __TestCase*  TestCaseRef;/*compatible embUnit1.0*/

struct __TestCase {
    TestImplement* isa;
    const char *name;
    void(*setUp)(void);
    void(*tearDown)(void);
    void(*runTest)(void);
};

extern const TestImplement TestCaseImplement;

#define new_TestCase(name,setUp,tearDown,runTest)\
    {\
        (TestImplement*)&TestCaseImplement,\
        name,\
        setUp,\
        tearDown,\
        runTest,\
    }

#ifdef  __cplusplus
}
#endif

#endif /* EMBUNIT_TESTCASE_H */
