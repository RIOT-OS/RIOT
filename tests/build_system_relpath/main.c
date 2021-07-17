/*
 * Copyright (C) 2020 Kaspar Schleiser <kaspar@schleiser.de>
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
 * @brief       __FILE__ test application
 *
 * This test is supposed to check if __FILE__ evaluates to the desired value.
 * Should evaluate to the relative path of a filename.
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <stdio.h>

int main(void)
{
    printf("__FILE__: %s\n", __FILE__);

    return 0;
}
