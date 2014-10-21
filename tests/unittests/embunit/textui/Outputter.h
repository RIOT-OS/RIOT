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
 * $Id: Outputter.h,v 1.2 2003/09/06 13:28:27 arms22 Exp $
 */
#ifndef __OUTPUTTER_H__
#define __OUTPUTTER_H__

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

#endif/*__OUTPUTTER_H__*/
