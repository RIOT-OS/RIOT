/*
 * SPDX-FileCopyrightText: 2003 Embedded Unit Project
 * SPDX-License-Identifier: MIT
 */
#ifndef EMBUNIT_TEST_H
#define EMBUNIT_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct __TestResult     TestResult;
typedef struct __TestResult*    TestResultRef;/*downward compatible*/

typedef struct __TestImplement  TestImplement;
typedef struct __TestImplement* TestImplementRef;/*downward compatible*/

typedef char*(*TestNameFunction)(void*);
typedef void(*TestRunFunction)(void*,TestResult*);
typedef int(*TestCountTestCasesFunction)(void*);

struct __TestImplement {
    TestNameFunction name;
    TestRunFunction run;
    TestCountTestCasesFunction countTestCases;
};

typedef struct __Test   Test;
typedef struct __Test*  TestRef;/*downward compatible*/

struct __Test {
    TestImplement* isa;
};

#define Test_name(s)            (s)->isa->name(s)
#define Test_run(s,r)           (s)->isa->run(s,r)
#define Test_countTestCases(s)  (s)->isa->countTestCases(s)

#ifdef  __cplusplus
}
#endif

#endif /* EMBUNIT_TEST_H */
