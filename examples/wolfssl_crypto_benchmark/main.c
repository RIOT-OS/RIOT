/*
 * Copyright (C) 2006-2017 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA 
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       wolfSSL cryptographic benchmark application
 *
 * @author      Kaleb J. Himes <kaleb@wolfssl.com>
 *
 * @}
 */

#include <stdio.h>

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfcrypt/test/test.h>

//include wolfCrypt benchmark app in-line
#include <wolfcrypt/benchmark/benchmark.c>

typedef struct func_args {
    int    argc;
    char** argv;
    int    return_code;
} func_args;


int main(void)
{
    func_args args;

    puts("wolfSSL Crypto Benchmarks!");

    benchmark_test(&args);

    return 0;
}
