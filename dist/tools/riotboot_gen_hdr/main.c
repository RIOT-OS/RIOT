/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *               2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License v2. See the file LICENSE for more details.
 */

/**
 * @file
 * @brief       Header generation tool for RIOT firmware images
 *
 * @author      Francisco Acosta <francisco.acosta@inria.fr>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#include <stdio.h>
#include <string.h>

int genhdr(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    char *usage = "genhdr generate [args]";

    if (argc < 2) {
        goto usage;
    }
    else if (!strcmp(argv[1], "generate")) {
        return genhdr(argc - 1, &argv[1]);
    }

usage:
    fprintf(stderr, "usage: %s\n", usage);
    return 1;
}
