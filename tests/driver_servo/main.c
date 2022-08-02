/*
 * Copyright (C) 2015 Eistec AB
 * Copyright (C) 2022 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
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
