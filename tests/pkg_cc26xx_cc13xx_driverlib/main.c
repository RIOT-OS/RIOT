/*
 * Copyright (C) 2020 Locha Inc
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
 * @brief       Verifies that the library builds and links correctly
 *
 * @author      Jean Pierre Dudey <jeandudey@hotmail.com>
 *
 * @}
 */

#include <stdio.h>

#include "driverlib/chipinfo.h"

int main(void)
{
    PackageType_t package = ChipInfo_GetPackageType();
    ChipType_t chip = ChipInfo_GetChipType();

    printf("package = %x\n", package);
    printf("chip = %x\n", chip);

    return 0;
}
