/*
 * SPDX-FileCopyrightText: 2003 Embedded Unit Project
 * SPDX-License-Identifier: MIT
 */
#ifndef EMBUNIT_TESTSUITE_H
#define EMBUNIT_TESTSUITE_H

#ifdef  __cplusplus
extern "C" {
#endif

typedef struct __TestSuite  TestSuite;
typedef struct __TestSuite* TestSuiteRef;/*downward compatible*/

struct __TestSuite {
    TestImplement* isa;
    char *name;
    int numberOfTests;
    Test** tests;
};

extern const TestImplement TestSuiteImplement;

#define new_TestSuite(name,tests,numberOfTests)\
    {\
        (TestImplement*)&TestSuiteImplement,\
        name,\
        numberOfTests,\
        tests,\
    }

#ifdef  __cplusplus
}
#endif

#endif /* EMBUNIT_TESTSUITE_H */
