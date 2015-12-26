/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
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
 * @brief       fmt print test application
 *
 * This test is supposed to check for "compilabilty" of the fmt print_* instructions.
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "fmt.h"

int main(void)
{
    print_str("If you can read this:\n");
    print_str("Test successful.\n");

    return 0;
}
