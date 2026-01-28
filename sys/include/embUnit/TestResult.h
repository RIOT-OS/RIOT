/*
 * SPDX-FileCopyrightText: 2003 Embedded Unit Project
 * SPDX-License-Identifier: MIT
 */
#ifndef EMBUNIT_TESTRESULT_H
#define EMBUNIT_TESTRESULT_H

#ifdef  __cplusplus
extern "C" {
#endif

/*typedef struct __TestResult       TestResult;*//* -> Test.h*/
/*typedef struct __TestResult*      TestResultRef;*//* -> Test.h*/

typedef struct __TestListner    TestListner;
typedef struct __TestListner*   TestListnerRef;/*downward compatible*/

struct __TestResult {
    unsigned short runCount;
    unsigned short failureCount;
    TestListner* listener;
};

#define new_TestResult(listener)\
    {\
        0,\
        0,\
        (TestListner*)listener,\
    }

void TestResult_init(TestResult* self,TestListner* listener);
void TestResult_startTest(TestResult* self,Test* test);
void TestResult_endTest(TestResult* self,Test* test);
void TestResult_addFailure(TestResult* self,Test* test,const char* msg,int line,const char* file);

#ifdef  __cplusplus
}
#endif

#endif /* EMBUNIT_TESTRESULT_H */
