/*
 * SPDX-FileCopyrightText: 2003 Embedded Unit Project
 * SPDX-License-Identifier: MIT
 */
#ifndef EMBUNIT_TEXTUIRUNNER_H
#define EMBUNIT_TEXTUIRUNNER_H

#include "Outputter.h"

#include <embUnit/embUnit.h>

#ifdef __cplusplus
extern "C" {
#endif

void TextUIRunner_setOutputter(OutputterRef outputter);
void TextUIRunner_startWithOutputter(OutputterRef outputter);
void TextUIRunner_start(void);
void TextUIRunner_runTest(TestRef test);
int TextUIRunner_end(void);

#ifdef __cplusplus
}
#endif

#endif /* EMBUNIT_TEXTUIRUNNER_H */
