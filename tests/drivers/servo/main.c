/*
 * SPDX-FileCopyrightText: 2015 Eistec AB
 * SPDX-FileCopyrightText: 2022 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test for servo driver
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <stdlib.h>
#include <stdio.h>

#include "shell.h"

int main(void)
{
    puts("RIOT RC servo test");

    char buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, buf, sizeof(buf));

    return 0;
}
