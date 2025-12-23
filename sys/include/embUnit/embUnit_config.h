/*
 * SPDX-FileCopyrightText: 2003 Embedded Unit Project
 * SPDX-License-Identifier: MIT
 */
#ifndef EMBUNIT_EMBUNIT_CONFIG_H
#define EMBUNIT_EMBUNIT_CONFIG_H

#ifdef  __cplusplus
extern "C" {
#endif

/*  #define NO_STDIO_PRINTF*/
    #ifdef  NO_STDIO_PRINTF
        extern void stdimpl_print(const char *string);
    #else
        #include<stdio.h>
        #define stdimpl_print(s) printf("%s", s);
    #endif

    #define ASSERT_STRING_BUFFER_MAX    64

#ifdef  __cplusplus
}
#endif

#endif /* EMBUNIT_EMBUNIT_CONFIG_H */
