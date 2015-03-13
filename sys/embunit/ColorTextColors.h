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
 * $Id: ColorTextOutputter.h,v 1.2 2003/09/06 13:28:27 arms22 Exp $
 */
#ifndef __COLORTEXTCOLORS_H__
#define __COLORTEXTCOLORS_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Some terminal color definitions:
 * C<color>:    text color
 * CDEFAULT:    default text color
 * BG<color>:   background color
 * BGDEFAULT:   default background color
 * S<style>:    text style
 * SDEFAULT:    default text style
 */
#define CRED        "\033[31m"
#define CGREEN      "\033[32m"
#define CDEFAULT    "\033[39m"
#define BGRED       "\033[41m"
#define BGGREEN     "\033[42m"
#define BGDEFAULT   "\033[49m"
#define SBOLD       "\033[1m"
#define SDEFAULT    "\033[21m"
#define LINEFILL    "\033[K"

#ifdef __cplusplus
}
#endif

#endif/*__COLORTEXTCOLORS_H__*/
