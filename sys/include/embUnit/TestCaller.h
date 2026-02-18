/*
 * SPDX-FileCopyrightText: 2003 Embedded Unit Project
 * SPDX-License-Identifier: MIT
 */
#ifndef EMBUNIT_TESTCALLER_H
#define EMBUNIT_TESTCALLER_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct __TestFixture    TestFixture;
typedef struct __TestFixture*   TestFixtureRef;/*downward compatible*/

struct __TestFixture {
    const char *name;
    void(*test)(void);
};

#define new_TestFixture(test) { #test, test }

typedef struct __TestCaller     TestCaller;
typedef struct __TestCaller*    TestCallerRef;/*downward compatible*/

struct __TestCaller {
    TestImplement* isa;
    const char *name;
    void(*setUp)(void);
    void(*tearDown)(void);
    int numberOfFixtuers;
    TestFixture *fixtuers;
};

extern const TestImplement TestCallerImplement;

#define new_TestCaller(name,sup,tdw,numberOfFixtuers,fixtuers)\
    {\
        (TestImplement*)&TestCallerImplement,\
        name,\
        sup,\
        tdw,\
        numberOfFixtuers,\
        fixtuers,\
    }

#ifdef  __cplusplus
}
#endif

#endif /* EMBUNIT_TESTCALLER_H */
