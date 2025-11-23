/*
 * SPDX-FileCopyrightText: 2003 Embedded Unit Project
 * SPDX-License-Identifier: MIT
 */
#ifndef EMBUNIT_OUTPUTTER_H
#define EMBUNIT_OUTPUTTER_H

#include <embUnit/embUnit.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct __OutputterImplement     OutputterImplement;
typedef struct __OutputterImplement*    OutputterImplementRef;

typedef void(*OutputterPrintHeaderFunction)(void*);
typedef void(*OutputterPrintStartTestFunction)(void*,TestRef);
typedef void(*OutputterPrintEndTestFunction)(void*,TestRef);
typedef void(*OutputterPrintSuccessfulFunction)(void*,TestRef,int);
typedef void(*OutputterPrintFailureFunction)(void*,TestRef,char*,int,char*,int);
typedef void(*OutputterPrintStatisticsFunction)(void*,TestResultRef);

struct __OutputterImplement {
    OutputterPrintHeaderFunction        printHeader;
    OutputterPrintStartTestFunction     printStartTest;
    OutputterPrintEndTestFunction       printEndTest;
    OutputterPrintSuccessfulFunction    printSuccessful;
    OutputterPrintFailureFunction       printFailure;
    OutputterPrintStatisticsFunction    printStatistics;
};

typedef struct __Outputter  Outputter;
typedef struct __Outputter* OutputterRef;

struct __Outputter {
    OutputterImplementRef isa;
};

#define Outputter_printHeader(o)            (o)->isa->printHeader(o)
#define Outputter_printStartTest(o,t)       (o)->isa->printStartTest(o,t)
#define Outputter_printEndTest(o,t)         (o)->isa->printEndTest(o,t)
#define Outputter_printSuccessful(o,t,c)    (o)->isa->printSuccessful(o,t,c)
#define Outputter_printFailure(o,t,m,l,f,c) (o)->isa->printFailure(o,t,m,l,f,c)
#define Outputter_printStatistics(o,r)      (o)->isa->printStatistics(o,r)

#ifdef __cplusplus
}
#endif

#endif /* EMBUNIT_OUTPUTTER_H */
