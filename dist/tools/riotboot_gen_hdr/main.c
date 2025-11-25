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
int updatehdr(int argc, char *argv[]);
int readhdr(const char *file);

int main(int argc, char *argv[])
{
    char *usage = "genhdr generate [args]\n\t"
                  "genhdr update [file] [new_version]\n\t"
                  "genhdr show [file]";

    if (argc < 2) {
        goto usage;
    }
    else if (!strcmp(argv[1], "generate")) {
        return genhdr(argc - 1, &argv[1]);
    }
    else if (!strcmp(argv[1], "update")) {
        return updatehdr(argc - 1, &argv[1]);
    }
    else if (!strcmp(argv[1], "show")) {
        return readhdr(argv[2]);
    }

usage:
    fprintf(stderr, "usage:\t%s\n", usage);
    return 1;
}
