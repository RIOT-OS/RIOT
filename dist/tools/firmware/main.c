/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License v2. See the file LICENSE for more details.
 */

#include <stdio.h>
#include <string.h>

int verify(int argc, char *argv[]);
extern const char verify_usage[];

int genkeys(int argc, char *argv[]);
extern const char genkeys_usage[];

int sign(int argc, char *argv[]);
extern const char sign_usage[];

int main(int argc, char *argv[])
{
    if (argc < 2) {
        goto usage;
    }
    if (!strcmp(argv[1], "verify")) {
        return verify(argc, argv);
    } else if (!strcmp(argv[1], "genkeys")) {
        return genkeys(argc - 1, &argv[1]);
    } else if (!strcmp(argv[1], "sign")) {
        return sign(argc - 1, &argv[1]);
    }

usage:
    fprintf(stderr, "usage: %s\n"  \
                    "       %s\n"  \
                    "       %s\n", \
                    genkeys_usage, sign_usage, verify_usage);
    return 1;
}
