/*
 * Copyright (C) 2015 Janos Kutscherauer (noshky@gmail.com)
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef SYS_READLINE_HISTORY_H_
#define SYS_READLINE_HISTORY_H_

void history_put(char *input);
int history_getprev(char *buf);
int history_getnext(char *buf);
void history_reset(void);

/*
 * For testing
 */
void history_reset_internal(void);
void history_printbuf(void);

#endif /* SYS_READLINE_HISTORY_H_ */
