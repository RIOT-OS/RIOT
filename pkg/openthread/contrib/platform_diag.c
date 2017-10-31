/*
 * Copyright (C) 2017 Fundacion Inria Chile
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @ingroup     net
 * @file
 * @brief       Implementation of OpenThread diagnostics platform abstraction
 *
 * @author      Jose Ignacio Alamos <jialamos@uc.cl>
 * @}
 */

#include <stdbool.h>
#include <stdio.h>

static bool sDiagMode = false;

void otPlatDiagProcess(int argc, char *argv[], char *aOutput, size_t aOutputMaxLen)
{
    /* add more plarform specific diagnostics features here */
    (void)argc;
    (void)argv;
    (void)aOutput;
    (void)aOutputMaxLen;
}

void otPlatDiagModeSet(bool aMode)
{
    sDiagMode = aMode;
}

bool otPlatDiagModeGet(void)
{
    return sDiagMode;
}
