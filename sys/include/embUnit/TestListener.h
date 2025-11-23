/*
 * SPDX-FileCopyrightText: 2003 Embedded Unit Project
 * SPDX-License-Identifier: MIT
 */
#ifndef EMBUNIT_TESTLISTENER_H
#define EMBUNIT_TESTLISTENER_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct __TestListnerImplement   TestListnerImplement;
typedef struct __TestListnerImplement*  TestListnerImplementRef;/*downward compatible*/

typedef void(*TestListnerStartTestCallBack)(void*,void*);
typedef void(*TestListnerEndTestCallBack)(void*,void*);
typedef void(*TestListnerAddFailureCallBack)(void*,void*,const char*,int,const char*);

struct __TestListnerImplement {
    TestListnerStartTestCallBack startTest;
    TestListnerEndTestCallBack endTest;
    TestListnerAddFailureCallBack addFailure;
};

/*typedef struct __TestListner  TestListner;*/  /*->TestResult.h*/
/*typedef struct __TestListner* TestListnerRef;*/   /*->TestResult.h*/

struct __TestListner {
    TestListnerImplement* isa;
};

#define TestListner_startTest(s,t)          ((TestListner*)s)->isa->startTest(s,t)
#define TestListner_endTest(s,t)            ((TestListner*)s)->isa->endTest(s,t)
#define TestListner_addFailure(s,t,m,l,f)   ((TestListner*)s)->isa->addFailure(s,t,m,l,f)

#ifdef  __cplusplus
}
#endif

#endif /* EMBUNIT_TESTLISTENER_H */
