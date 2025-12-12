/*
 * SPDX-FileCopyrightText: 2003 Embedded Unit Project
 * SPDX-License-Identifier: MIT
 */
#ifndef EMBUNIT_TESTRUNNER_H
#define EMBUNIT_TESTRUNNER_H

#ifdef  __cplusplus
extern "C" {
#endif

void TestRunner_start(void);
void TestRunner_runTest(Test* test);
int TestRunner_end(void);

extern int TestRunnerHadErrors;

#ifdef  __cplusplus
}
#endif

#endif /* EMBUNIT_TESTRUNNER_H */
