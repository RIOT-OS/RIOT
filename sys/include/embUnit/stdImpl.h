/*
 * SPDX-FileCopyrightText: 2003 Embedded Unit Project
 * SPDX-License-Identifier: MIT
 */
#ifndef EMBUNIT_STDIMPL_H
#define EMBUNIT_STDIMPL_H

#ifdef  __cplusplus
extern "C" {
#endif

#ifndef NULL
#define NULL    0
#endif

char* stdimpl_strcpy(char *s1, const char *s2);
char* stdimpl_strcat(char *dst, const char *src);
char* stdimpl_strncat(char *dst, const char *src,unsigned int count);
int stdimpl_strlen(const char *str);
int stdimpl_strcmp(const char *s1, const char *s2);
char* stdimpl_lltoa(long long v,char *string,int r);

static inline char* stdimpl_itoa(int v,char *string,int r)
{
    return stdimpl_lltoa(v,string,r);
}

#ifdef  __cplusplus
}
#endif

#endif /* EMBUNIT_STDIMPL_H */
