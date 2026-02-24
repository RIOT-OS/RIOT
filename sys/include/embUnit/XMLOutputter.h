/*
 * SPDX-FileCopyrightText: 2003 Embedded Unit Project
 * SPDX-License-Identifier: MIT
 */
#ifndef EMBUNIT_XMLOUTPUTTER_H
#define EMBUNIT_XMLOUTPUTTER_H

#include "Outputter.h"

#ifdef __cplusplus
extern "C" {
#endif

void XMLOutputter_setStyleSheet(char *style);
OutputterRef XMLOutputter_outputter(void);

#ifdef __cplusplus
}
#endif

#endif /* EMBUNIT_XMLOUTPUTTER_H */
