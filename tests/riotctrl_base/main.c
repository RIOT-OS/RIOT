/*
 * main.c
 * Copyright (C) 2021 Martine Lenders <mail@martine-lenders.eu>
 *
 * Distributed under terms of the MIT license.
 */

#include "shell.h"

int main(void)
{
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);
    return 0;
}
