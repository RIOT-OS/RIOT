/*
 * Copyright (C) 2015 Janos Kutscherauer (noshky@gmail.com)
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "include/readline_scan.h"

#include <stdio.h>

typedef enum
{
    STATE_START = 0, //
    STATE_ESC, //
    STATE_BRACKET, //
    STATE_O, //
    STATE_256, //
    STATE_UMLAUT,
    STATE_1, //
    STATE_3, //
    STATE_SEMICOLON, //
    STATE_5
} state_e;

static state_e state = STATE_START;
static char tmp = '\0';

#define CTRL(CHAR)  (CHAR - 'A' + 1)

control_e readline_scan_char(unsigned char c)
{
    switch (state) {
        case STATE_START:
            switch (c) {
                case DEL: // intentional fall-through
                case BS:
                    return CONTROL_BS;
                case LF: // intentional fall-through
                case CR:
                    return CONTROL_NL;
                case TAB:
                    return CONTROL_TAB;
                case ESC:
                    state = STATE_ESC;
                    return CONTROL_IN_PROGRESS;
                case UML:
                    state = STATE_UMLAUT;
                    return CONTROL_IN_PROGRESS;
                case CTRL('A'):
                    return CONTROL_HOME;
                case CTRL('B'):
                    return CONTROL_LEFT;
                case CTRL('D'):
                    return CONTROL_DEL;
                case CTRL('E'):
                    return CONTROL_END;
                case CTRL('F'):
                    return CONTROL_RIGHT;
                case CTRL('K'):
                    return CONTROL_CUT_ALL_BEHIND;
                case CTRL('N'):
                    return CONTROL_DOWN;
                case CTRL('P'):
                    return CONTROL_UP;
                case CTRL('U'):
                    return CONTROL_CUT_ALL;
                case CTRL('W'):
                    return CONTROL_CUT_WORD_BEFORE;
                case CTRL('Y'):
                    return CONTROL_PASTE;
            }
            break;
        case STATE_ESC:
            state = STATE_START;
            switch (c) {
                case ESC:
                    state = STATE_ESC;
                    return CONTROL_IN_PROGRESS;
                case DEL:
                    return CONTROL_DEL;
                case BS:
                    return CONTROL_CUT_WORD_BEFORE;
                case 'b':
                    return CONTROL_CTRL_LEFT;
                case 'd':
                    return CONTROL_CUT_WORD_AFTER;
                case 'f':
                    return CONTROL_CTRL_RIGHT;
                case BRK:
                    state = STATE_BRACKET;
                    return CONTROL_IN_PROGRESS;
                case CHO:
                    state = STATE_O;
                    return CONTROL_IN_PROGRESS;
            }
            break;
        case STATE_BRACKET:
            state = STATE_START;
            switch (c) {
                case '2': // intentional fall-through
                case '5': // intentional fall-through
                case '6':
                    tmp = c;
                    state = STATE_256;
                    return CONTROL_IN_PROGRESS;
                case '1':
                    state = STATE_1;
                    return CONTROL_IN_PROGRESS;
                case '3':
                    state = STATE_3;
                    return CONTROL_IN_PROGRESS;
                case 'A':
                    return CONTROL_UP;
                case 'B':
                    return CONTROL_DOWN;
                case 'C':
                    return CONTROL_RIGHT;
                case 'D':
                    return CONTROL_LEFT;
                case 'U':
                    return CONTROL_PGDWN;
                case 'V':
                    return CONTROL_PGUP;
                case 'H': // intentional fall-through
                case '@':
                    return CONTROL_HOME;
            }
            break;
        case STATE_256:
            state = STATE_START;
            if (c == TILD) {
                switch (tmp) {
                    case '2':
                        return CONTROL_INS;
                    case '5':
                        return CONTROL_PGUP;
                    case '6':
                        return CONTROL_PGDWN;
                }
            }
            break;
        case STATE_O:
            state = STATE_START;
            switch (c) {
                case 'F':
                    return CONTROL_END;
                case 'H':
                    return CONTROL_HOME;
            }
            break;
        case STATE_UMLAUT:
            state = STATE_START;
            switch (c) {
                case 0xA4: // ä
                case 0x84: // Ä
                case 0xB6: // ö
                case 0x96: // Ö
                case 0xBC: // ü
                case 0x9C: // Ü
                case 0x9F: // ß
                    return CONTROL_UMLAUT;
            }
            break;
        case STATE_1:
            state = STATE_START;
            switch (c) {
                case TILD:
                    return CONTROL_HOME;
                case SEMICOL:
                    state = STATE_SEMICOLON;
                    return CONTROL_IN_PROGRESS;
            }
            break;
        case STATE_3:
            state = STATE_START;
            switch (c) {
                case TILD:
                    return CONTROL_DEL;
                case SEMICOL:
                    state = STATE_SEMICOLON;
                    return CONTROL_IN_PROGRESS;
            }
            break;
        case STATE_SEMICOLON:
            state = STATE_START;
            switch (c) {
                case '5':
                    state = STATE_5;
                    return CONTROL_IN_PROGRESS;
            }
            break;
        case STATE_5:
            state = STATE_START;
            switch (c) {
                case 'A':
                    return CONTROL_CTRL_UP;
                case 'B':
                    return CONTROL_CTRL_DOWN;
                case 'C':
                    return CONTROL_CTRL_RIGHT;
                case 'D':
                    return CONTROL_CTRL_LEFT;
                case TILD:
                    return CONTROL_CTRL_DEL;
            }
            break;
    }
    if (c < 0x20 || c > 0x7E) {
        return CONTROL_NONE_PRINTABLE;
    }
    return CONTROL_NONE;
}
