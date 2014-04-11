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
 * $Id: AssertImpl.c,v 1.5 2004/02/10 16:15:25 arms22 Exp $
 */
#include "config.h"
#include "stdImpl.h"
#include "AssertImpl.h"

void assertImplementationInt(int expected,int actual, long line, const char *file)
{
	char buffer[32];	/*"exp -2147483647 was -2147483647"*/
	char numbuf[12];	/*32bit int decimal maximum column is 11 (-2147483647~2147483647)*/

	stdimpl_strcpy(buffer, "exp ");

	{	stdimpl_itoa(expected, numbuf, 10);
		stdimpl_strncat(buffer, numbuf, 11);	}

	stdimpl_strcat(buffer, " was ");

	{	stdimpl_itoa(actual, numbuf, 10);
		stdimpl_strncat(buffer, numbuf, 11);	}

	addFailure(buffer, line, file);
}

void assertImplementationCStr(const char *expected,const char *actual, long line, const char *file)
{
	char buffer[ASSERT_STRING_BUFFER_MAX];
	#define exp_act_limit ((ASSERT_STRING_BUFFER_MAX-11-1)/2)/*	"exp'' was''" = 11 byte	*/
	int el;
	int al;

	if (expected) {
		el = stdimpl_strlen(expected);
	} else {
		el = 4;
		expected = "null";
	}

	if (actual) {
		al = stdimpl_strlen(actual);
	} else {
		al = 4;
		actual = "null";
	}
	if (el > exp_act_limit) {
		if (al > exp_act_limit) {
			al = exp_act_limit;
			el = exp_act_limit;
		} else {
			int w = exp_act_limit + (exp_act_limit - al);
			if (el > w) {
				el = w;
			}
		}
	} else {
		int w = exp_act_limit + (exp_act_limit - el);
		if (al > w) {
			al = w;
		}
	}
	stdimpl_strcpy(buffer, "exp \"");
	stdimpl_strncat(buffer, expected, el);
	stdimpl_strcat(buffer, "\" was \"");
	stdimpl_strncat(buffer, actual, al);
	stdimpl_strcat(buffer, "\"");

	addFailure(buffer, line, file);
}
