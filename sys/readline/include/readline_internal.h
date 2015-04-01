/*
 * Copyright (C) 2015, Janos Kutscherauer (noshky@gmail.com)
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef SYS_READLINE_READLINE_INTERNAL_H_
#define SYS_READLINE_READLINE_INTERNAL_H_

#include "readline.h"
#include "readline_scan.h"
#include "readline_history.h"

#ifdef __cplusplus
extern "C" {
#endif

void set_screen_cursor(shell_t *shell, int cursor);
void cut(shell_t *shell, int *cursor, char *buf, char *cut_buf, int count);
void paste(shell_t *shell, int *cursor, char *buf, char *paste_buf, int count);
char* find_word_before(int *cursor, char *buf);
char* find_word_behind(int *cursor, char *buf);
void cut_word_before(shell_t *shell, int *cursor, char *buf, char *cut_buf);
void cut_word_behind(shell_t *shell, int *cursor, char *buf, char *cut_buf);
void move_cursor_word_left(shell_t *shell, int *cursor, char *buf);
void move_cursor_word_right(shell_t *shell, int *cursor, char *buf);
control_e readline_control_process(shell_t *shell, int *cursor, char *buf, control_e control);

#if SHELL_HISTORY_SIZE > 0
void history_cycle_up(shell_t *shell, int *cursor, char *buf);
void history_cycle_down(shell_t *shell, int *cursor, char *buf);
#endif

#ifdef __cplusplus
}
#endif

#endif /* SYS_READLINE_READLINE_INTERNAL_H_ */
