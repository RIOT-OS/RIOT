/*
 * SPDX-FileCopyrightText: 2003 Embedded Unit Project
 * SPDX-License-Identifier: MIT
 */
#ifndef EMBUNIT_REPEATEDTEST_H
#define EMBUNIT_REPEATEDTEST_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct __RepeatedTest   RepeatedTest;
typedef struct __RepeatedTest*  RepeatedTestRef;    /*downward compatible*/

struct __RepeatedTest {
    TestImplement* isa;
    Test* test;
    int timesRepeat;
};

extern const TestImplement RepeatedTestImplement;

#define new_RepeatedTest(test,tmrp)\
    {\
        (TestImplement*)&RepeatedTestImplement,\
        (Test*)test,\
        tmrp,\
    }

#ifdef  __cplusplus
}
#endif

#endif /* EMBUNIT_REPEATEDTEST_H */
