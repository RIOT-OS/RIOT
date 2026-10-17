/*
 * SPDX-FileCopyrightText: 2004 PJRC.COM, LLC, <paul@pjrc.com>
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ifndef UUENCODE_H
#define UUENCODE_H

extern void uuencode(char *str, const unsigned char *data, int num);
extern int uudecode(const char *str, unsigned char *data, int max);

#endif /* UUENCODE_H */
