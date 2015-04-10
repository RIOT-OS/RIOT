/*
 * Copyright (C) 2015, Janos Kutscherauer (noshky@gmail.com)
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef SYS_READLINE_SCAN_H_
#define SYS_READLINE_SCAN_H_

#ifdef __cplusplus
extern "C" {
#endif

#define LF      '\n' // 0x0A
#define CR      '\r' // 0x0D
#define TAB     '\t'
#define ESC     0x1B
#define BS      0x08
#define DEL     0x7F
#define BRK     '[' // 0x5B
#define CHO     'O' // 0x4F
#define TILD    '~' // 0x7E
#define SEMICOL ';' // 0x3B
#define UML     0xC3 // Ã

typedef enum
{
    CONTROL_NONE = 0,
    CONTROL_NL,
    CONTROL_TAB,
    CONTROL_ESC,
    CONTROL_BS,
    CONTROL_DEL,
    CONTROL_INS,
    CONTROL_HOME,
    CONTROL_END,
    CONTROL_PGUP,
    CONTROL_PGDWN,
    CONTROL_UP,
    CONTROL_DOWN,
    CONTROL_RIGHT,
    CONTROL_LEFT,
    CONTROL_CUT_ALL,
    CONTROL_CUT_ALL_BEHIND,
    CONTROL_CUT_WORD_BEFORE,
    CONTROL_CUT_WORD_AFTER,
    CONTROL_PASTE,
    CONTROL_CTRL_UP,
    CONTROL_CTRL_DOWN,
    CONTROL_CTRL_RIGHT,
    CONTROL_CTRL_LEFT,
    CONTROL_CTRL_DEL,
    CONTROL_UMLAUT,
    CONTROL_NONE_PRINTABLE,
    CONTROL_IN_PROGRESS
} control_e;

control_e readline_scan_char(unsigned char c);

#ifdef __cplusplus
}
#endif

#endif /* SYS_READLINE_SCAN_H_ */
