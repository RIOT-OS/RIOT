/*
 * COPYRIGHT AND PERMISSION NOTICE
 *
 * Copyright (c) 2003 Embedded Unit Project
 *
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, and/or sell copies of the Software, and to permit persons
 * to whom the Software is furnished to do so, provided that the above
 * copyright notice(s) and this permission notice appear in all copies
 * of the Software and that both the above copyright notice(s) and this
 * permission notice appear in supporting documentation.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT
 * OF THIRD PARTY RIGHTS. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * HOLDERS INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM, OR ANY
 * SPECIAL INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Except as contained in this notice, the name of a copyright holder
 * shall not be used in advertising or otherwise to promote the sale,
 * use or other dealings in this Software without prior written
 * authorization of the copyright holder.
 *
 * $Id: stdImpl.c,v 1.3 2004/02/10 16:15:25 arms22 Exp $
 */
#include "stdImpl.h"

char* stdimpl_strcpy(char *dst, const char *src)
{
    char *start = dst;
    char c;
    do {
        c = *src;
        *dst = c;
        src++;
        dst++;
    } while (c);
    return start;
}

char* stdimpl_strcat(char *dst, const char *src)
{
    char *start = dst;
    char c;
    do {
        c = *dst;
        dst++;
    } while (c);
    dst--;
    do {
        c = *src;
        *dst = c;
        src++;
        dst++;
    } while (c);
    return start;
}

char* stdimpl_strncat(char *dst, const char *src,unsigned int count)
{
    char *start = dst;
    char c;
    do {
        c = *dst;
        dst++;
    } while (c);
    dst--;
    if (count) {
        do {
            c = *src;
            *dst = c;
            src++;
            dst++;
            count--;
        } while (c && count);
        *dst = '\0';
    }
    return start;
}

int stdimpl_strlen(const char *str)
{
    const char *estr = str;
    char c;
    do {
        c = *estr;
        estr++;
    } while (c);
    return ((int)(estr - str - 1));
}

int stdimpl_strcmp(const char *s1, const char *s2)
{
    char c1,c2;
    do {
        c1 = *s1++;
        c2 = *s2++;
    } while ((c1) && (c2) && (c1==c2));
    return c1 - c2;
}

static char* _xtoa(unsigned long v,char *string, int r, int is_neg)
{
    char *start = string;
    char buf[33],*p;

    p = buf;

    do {
        *p++ = "0123456789abcdef"[(v % r) & 0xf];
    } while (v /= r);

    if (is_neg) {
        *p++ = '-';
    }

    do {
        *string++ = *--p;
    } while (buf != p);

    *string = '\0';

    return start;
}

char* stdimpl_itoa(int v,char *string,int r)
{
    if ((r == 10) && (v < 0)) {
        return _xtoa((unsigned long)(-v), string, r, 1);
    }
    return _xtoa((unsigned long)(v), string, r, 0);
}
