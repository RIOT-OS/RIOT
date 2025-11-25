/*
 * Copyright (C) 2021 Niklaus Leuenberger
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       This is a test for the EXTERNAL_PKG_DIRS variable.
 *
 * @author      Niklaus Leuenberger <niklaus.leuenb@gmail.com>
 *
 * @}
 */

#include <stdio.h>

#ifndef TEST_EXTERNAL_PKG
    #error "Required external package not included."
#endif

#ifdef TEST_EXTERNAL_PKG_NOT_USED
    #error "External package included that shouldn't be."
#endif

int main(void)
{
    puts("If it compiles, it works!");
    return 0;
}
