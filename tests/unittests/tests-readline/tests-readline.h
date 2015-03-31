/*
 * Copyright (C) 2015 Janos Kutscherauer (noshky@gmail.com)
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  unittests
 * @{
 *
 * @file        tests-readline.h
 * @brief       Unittests for the ``readline`` module
 *
 * @author      Janos Kutscherauer (noshky@gmail.com)
 */
#ifndef __TESTS_READLINE_H_
#define __TESTS_READLINE_H_

#include "embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Shell buffer, +1 for the \0 character */
#define SHELL_BUFFER_SIZE (32 + 1)

void put_char_dummy(int c);
int get_char_dummy(void);

void _replace_cursor_marks(char *buf);

/**
 * @brief   The entry point of this test suite.
 */
void tests_readline(void);

/**
 * @brief   Generates tests for readline-internal.h
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_readline_internal_cut_paste_tests(void);
Test *tests_readline_internal_cursor_tests(void);
Test *tests_readline_internal_cut_words_tests(void);
Test *tests_readline_internal_history_tests(void);
Test *tests_readline_internal_history_tests_internal(void);

/* ... */

#ifdef __cplusplus
}
#endif

#endif /* __TESTS_READLINE_H_ */
/** @} */
