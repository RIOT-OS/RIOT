/*
 * Copyright (C) 2015 Janos Kutscherauer (noshky@gmail.com)
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/* clib includes */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "embUnit/embUnit.h"
#include "tests-readline.h"

#include "readline.h"
#include "readline_internal.h"

static shell_t shell;
static char BUFFER[SHELL_BUFFER_SIZE];
static char CUT_PASTE_BUFFER[SHELL_BUFFER_SIZE];
static char TEST_BUFFER[SHELL_BUFFER_SIZE];

#define TEST_STRING "123456789"
#define TEST_STRING2 "abcdefghi"
#define LEN_TEST_STRING 9

static void set_up(void)
{
    shell.put_char = put_char_dummy;
    shell.readchar = get_char_dummy;
    shell.shell_buffer_size = SHELL_BUFFER_SIZE;
    memset(BUFFER, 0, SHELL_BUFFER_SIZE);
    memset(CUT_PASTE_BUFFER, 0, SHELL_BUFFER_SIZE);
}

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))

static void test_cut_loops(void)
{
    int cursor = 0;
    char *buf = BUFFER;
    char *cut_buf = CUT_PASTE_BUFFER;
    char *exp_cut_buf = TEST_BUFFER;

    memset(buf, 0, SHELL_BUFFER_SIZE);
    memset(cut_buf, 0, SHELL_BUFFER_SIZE);

    for (int cur = 0; cur < LEN_TEST_STRING; cur++) {
        for (int cut_len = 0; cut_len < LEN_TEST_STRING; cut_len++) {
            // Preparation
            cursor = cur;
            sprintf(buf, TEST_STRING);
            int exp_cut_len = MIN(cut_len, LEN_TEST_STRING - cur);
            strncpy(exp_cut_buf, buf + cur, exp_cut_len);
            exp_cut_buf[exp_cut_len] = '\0';

            // Action
            cut(&shell, &cursor, buf, cut_buf, cut_len);
////            printf("cur: %d, cut_len: %d, exp_cut_len: %d, buf: '%s', cut_buf: '%s'\n",
//                cur, cut_len, exp_cut_len, buf, cut_buf);

// Tests
            TEST_ASSERT_EQUAL_INT(LEN_TEST_STRING - exp_cut_len, strlen(buf));
            TEST_ASSERT_EQUAL_INT(exp_cut_len, strlen(cut_buf));
            TEST_ASSERT_EQUAL_INT(cur, cursor);
            TEST_ASSERT_EQUAL_STRING(exp_cut_buf, cut_buf);
        }
    }

}

static void test_paste_loops(void)
{
    int cursor = 0;
    char *buf = BUFFER;
    char *paste_buf = CUT_PASTE_BUFFER;
    char *exp_buf = TEST_BUFFER;

    sprintf(paste_buf, TEST_STRING2);
    memset(buf, 0, SHELL_BUFFER_SIZE);

    for (int cur = 0; cur < LEN_TEST_STRING; cur++) {
        for (int paste_len = 0; paste_len < LEN_TEST_STRING; paste_len++) {
            // Preparation
            cursor = cur;
            sprintf(buf, TEST_STRING);
            int exp_buf_len = paste_len + LEN_TEST_STRING;

            // Construct the expected buffer
            strncpy(exp_buf, buf, cur);
            strncpy(exp_buf + cur, paste_buf, paste_len);
            strncpy(exp_buf + cur + paste_len, buf + cur, LEN_TEST_STRING - cur);
            exp_buf[exp_buf_len] = '\0';

            // Action
            paste(&shell, &cursor, buf, paste_buf, paste_len);
//            printf("cur: %d, paste_len: %d, exp_buf_len: %d, buf: '%s', exp_buf: '%s'\n",
//                    cur, paste_len, exp_buf_len, buf, exp_buf);

// Tests
            TEST_ASSERT_EQUAL_INT(exp_buf_len, strlen(buf));
            TEST_ASSERT_EQUAL_INT(cur + paste_len, cursor);
            TEST_ASSERT_EQUAL_STRING(exp_buf, buf);
        }
    }

}

static void test_cut_normal(void)
{
    int cursor = 0;
    char *buf = BUFFER;
    char *cut_buf = CUT_PASTE_BUFFER;

    memset(buf, 0, SHELL_BUFFER_SIZE);
    memset(cut_buf, 0, SHELL_BUFFER_SIZE);

    sprintf(buf, TEST_STRING);
    cursor = 0;
    cut(&shell, &cursor, buf, cut_buf, 1);
    TEST_ASSERT_EQUAL_INT(8, strlen(buf));
    TEST_ASSERT_EQUAL_INT(1, strlen(cut_buf));
    TEST_ASSERT_EQUAL_STRING("23456789", buf);
    TEST_ASSERT_EQUAL_STRING("1", cut_buf);

    memset(buf, 0, SHELL_BUFFER_SIZE);
    memset(cut_buf, 0, SHELL_BUFFER_SIZE);
    sprintf(buf, TEST_STRING);
    cursor = 0;
    cut(&shell, &cursor, buf, cut_buf, 3);
    TEST_ASSERT_EQUAL_INT(6, strlen(buf));
    TEST_ASSERT_EQUAL_INT(3, strlen(cut_buf));
    TEST_ASSERT_EQUAL_STRING("456789", buf);
    TEST_ASSERT_EQUAL_STRING("123", cut_buf);

    memset(buf, 0, SHELL_BUFFER_SIZE);
    memset(cut_buf, 0, SHELL_BUFFER_SIZE);
    sprintf(buf, TEST_STRING);
    cursor = 3;
    cut(&shell, &cursor, buf, cut_buf, 3);
    TEST_ASSERT_EQUAL_INT(6, strlen(buf));
    TEST_ASSERT_EQUAL_INT(3, strlen(cut_buf));
    TEST_ASSERT_EQUAL_STRING("123789", buf);
    TEST_ASSERT_EQUAL_STRING("456", cut_buf);
}

static void test_cut_bounds_cases(void)
{
    int cursor = 0;
    char *buf = BUFFER;
    char *cut_buf = CUT_PASTE_BUFFER;

    /*
     * Test empty buffer behaviour
     */
    memset(buf, 0, SHELL_BUFFER_SIZE);
    memset(cut_buf, 0, SHELL_BUFFER_SIZE);
    cursor = 0;
    cut(&shell, &cursor, buf, cut_buf, 0);
    TEST_ASSERT_EQUAL_INT(0, strlen(buf));
    TEST_ASSERT_EQUAL_INT(0, strlen(cut_buf));

    memset(buf, 0, SHELL_BUFFER_SIZE);
    memset(cut_buf, 0, SHELL_BUFFER_SIZE);
    cursor = 0;
    cut(&shell, &cursor, buf, cut_buf, 1);
    TEST_ASSERT_EQUAL_INT(0, strlen(buf));
    TEST_ASSERT_EQUAL_INT(0, strlen(cut_buf));

    memset(buf, 0, SHELL_BUFFER_SIZE);
    memset(cut_buf, 0, SHELL_BUFFER_SIZE);
    cursor = 0;
    cut(&shell, &cursor, buf, cut_buf, LEN_TEST_STRING - 1);
    TEST_ASSERT_EQUAL_INT(0, strlen(buf));
    TEST_ASSERT_EQUAL_INT(0, strlen(cut_buf));

    memset(buf, 0, SHELL_BUFFER_SIZE);
    memset(cut_buf, 0, SHELL_BUFFER_SIZE);
    cursor = 0;
    cut(&shell, &cursor, buf, cut_buf, LEN_TEST_STRING * 2);
    TEST_ASSERT_EQUAL_INT(0, strlen(buf));
    TEST_ASSERT_EQUAL_INT(0, strlen(cut_buf));

    /*
     * Test empty cutting behaviour
     */
    memset(buf, 0, SHELL_BUFFER_SIZE);
    memset(cut_buf, 0, SHELL_BUFFER_SIZE);
    sprintf(buf, TEST_STRING);
    cursor = 0;
    cut(&shell, &cursor, buf, cut_buf, 0);
    TEST_ASSERT_EQUAL_INT(LEN_TEST_STRING, strlen(buf));
    TEST_ASSERT_EQUAL_INT(0, strlen(cut_buf));

    memset(buf, 0, SHELL_BUFFER_SIZE);
    memset(cut_buf, 0, SHELL_BUFFER_SIZE);
    cursor = 5;
    sprintf(buf, TEST_STRING);
    cut(&shell, &cursor, buf, cut_buf, 0);
    TEST_ASSERT_EQUAL_INT(LEN_TEST_STRING, strlen(buf));
    TEST_ASSERT_EQUAL_INT(0, strlen(cut_buf));

    memset(buf, 0, SHELL_BUFFER_SIZE);
    memset(cut_buf, 0, SHELL_BUFFER_SIZE);
    cursor = LEN_TEST_STRING - 1;
    sprintf(buf, TEST_STRING);
    cut(&shell, &cursor, buf, cut_buf, 0);
    TEST_ASSERT_EQUAL_INT(LEN_TEST_STRING, strlen(buf));
    TEST_ASSERT_EQUAL_INT(0, strlen(cut_buf));

    memset(buf, 0, SHELL_BUFFER_SIZE);
    memset(cut_buf, 0, SHELL_BUFFER_SIZE);
    cursor = LEN_TEST_STRING;
    sprintf(buf, TEST_STRING);
    cut(&shell, &cursor, buf, cut_buf, 0);
    TEST_ASSERT_EQUAL_INT(LEN_TEST_STRING, strlen(buf));
    TEST_ASSERT_EQUAL_INT(0, strlen(cut_buf));

    memset(buf, 0, SHELL_BUFFER_SIZE);
    memset(cut_buf, 0, SHELL_BUFFER_SIZE);
    cursor = LEN_TEST_STRING + 1;
    sprintf(buf, TEST_STRING);
    cut(&shell, &cursor, buf, cut_buf, 0);
    TEST_ASSERT_EQUAL_INT(LEN_TEST_STRING, strlen(buf));
    TEST_ASSERT_EQUAL_INT(0, strlen(cut_buf));

    memset(buf, 0, SHELL_BUFFER_SIZE);
    memset(cut_buf, 0, SHELL_BUFFER_SIZE);
    cursor = LEN_TEST_STRING * 2;
    sprintf(buf, TEST_STRING);
    cut(&shell, &cursor, buf, cut_buf, 0);
    TEST_ASSERT_EQUAL_INT(LEN_TEST_STRING, strlen(buf));
    TEST_ASSERT_EQUAL_INT(0, strlen(cut_buf));
}

static void test_cut_overcutting(void)
{
    int cursor = 0;
    char *buf = BUFFER;
    char *cut_buf = CUT_PASTE_BUFFER;

    /**
     * Test overcutting behaviour
     */
    memset(buf, 0, SHELL_BUFFER_SIZE);
    memset(cut_buf, 0, SHELL_BUFFER_SIZE);
    sprintf(buf, TEST_STRING);
    cursor = 0;
    cut(&shell, &cursor, buf, cut_buf, LEN_TEST_STRING);
    TEST_ASSERT_EQUAL_INT(0, strlen(buf));
    TEST_ASSERT_EQUAL_INT(LEN_TEST_STRING, strlen(cut_buf));
    TEST_ASSERT_EQUAL_STRING("", buf);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING, cut_buf);

    memset(buf, 0, SHELL_BUFFER_SIZE);
    memset(cut_buf, 0, SHELL_BUFFER_SIZE);
    sprintf(buf, TEST_STRING);
    cursor = 0;
    cut(&shell, &cursor, buf, cut_buf, LEN_TEST_STRING * 2);
    TEST_ASSERT_EQUAL_INT(0, strlen(buf));
    TEST_ASSERT_EQUAL_INT(LEN_TEST_STRING, strlen(cut_buf));
    TEST_ASSERT_EQUAL_STRING("", buf);
    TEST_ASSERT_EQUAL_STRING(TEST_STRING, cut_buf);

    memset(buf, 0, SHELL_BUFFER_SIZE);
    memset(cut_buf, 0, SHELL_BUFFER_SIZE);
    sprintf(buf, TEST_STRING);
    cursor = 4;
    cut(&shell, &cursor, buf, cut_buf, LEN_TEST_STRING);
    TEST_ASSERT_EQUAL_INT(4, strlen(buf));
    TEST_ASSERT_EQUAL_INT(5, strlen(cut_buf));
    TEST_ASSERT_EQUAL_STRING("1234", buf);
    TEST_ASSERT_EQUAL_STRING("56789", cut_buf);

    memset(buf, 0, SHELL_BUFFER_SIZE);
    memset(cut_buf, 0, SHELL_BUFFER_SIZE);
    sprintf(buf, TEST_STRING);
    cursor = LEN_TEST_STRING - 1;
    cut(&shell, &cursor, buf, cut_buf, LEN_TEST_STRING);
    TEST_ASSERT_EQUAL_INT(8, strlen(buf));
    TEST_ASSERT_EQUAL_INT(1, strlen(cut_buf));
    TEST_ASSERT_EQUAL_STRING("12345678", buf);
    TEST_ASSERT_EQUAL_STRING("9", cut_buf);

    memset(buf, 0, SHELL_BUFFER_SIZE);
    memset(cut_buf, 0, SHELL_BUFFER_SIZE);
    sprintf(buf, TEST_STRING);
    cursor = LEN_TEST_STRING;
    cut(&shell, &cursor, buf, cut_buf, LEN_TEST_STRING);
    TEST_ASSERT_EQUAL_INT(LEN_TEST_STRING, strlen(buf));
    TEST_ASSERT_EQUAL_INT(0, strlen(cut_buf));
    TEST_ASSERT_EQUAL_STRING(TEST_STRING, buf);
    TEST_ASSERT_EQUAL_STRING("", cut_buf);
}

static void test_paste_bounds_cases(void)
{
    int cursor = 0;
    char *buf = BUFFER;
    char *paste_buf = CUT_PASTE_BUFFER;

    /* Emty buffer, empty paste, pastecount=0 */
    memset(buf, 0, SHELL_BUFFER_SIZE);
    memset(paste_buf, 0, SHELL_BUFFER_SIZE);
    cursor = 0;
    paste(&shell, &cursor, buf, paste_buf, 0);
    TEST_ASSERT_EQUAL_INT(0, strlen(buf));
    TEST_ASSERT_EQUAL_INT(0, cursor);

    /* Emty buffer, paste=teststring, pastecount=0 */
    memset(buf, 0, SHELL_BUFFER_SIZE);
    memset(paste_buf, 0, SHELL_BUFFER_SIZE);
    sprintf(paste_buf, TEST_STRING);
    cursor = 0;
    paste(&shell, &cursor, buf, paste_buf, 0);
    TEST_ASSERT_EQUAL_INT(0, strlen(buf));
    TEST_ASSERT_EQUAL_INT(0, cursor);

    /* buffer=teststring, empty paste, pastecount=0 */
    memset(buf, 0, SHELL_BUFFER_SIZE);
    memset(paste_buf, 0, SHELL_BUFFER_SIZE);
    sprintf(buf, TEST_STRING);
    cursor = 0;
    paste(&shell, &cursor, buf, paste_buf, 0);
    TEST_ASSERT_EQUAL_INT(LEN_TEST_STRING, strlen(buf));
    TEST_ASSERT_EQUAL_INT(0, cursor);

    /* buffer=teststring, empty paste, pastecount=0, cursor=<end>*/
    memset(buf, 0, SHELL_BUFFER_SIZE);
    memset(paste_buf, 0, SHELL_BUFFER_SIZE);
    sprintf(buf, TEST_STRING);
    cursor = LEN_TEST_STRING;
    paste(&shell, &cursor, buf, paste_buf, 0);
    TEST_ASSERT_EQUAL_INT(LEN_TEST_STRING, strlen(buf));
    TEST_ASSERT_EQUAL_INT(LEN_TEST_STRING, cursor);
}

static void test_paste_illegal_input(void)
{
    int cursor = 0;
    char *buf = BUFFER;
    char *paste_buf = CUT_PASTE_BUFFER;

    /* Empty buffer, paste=teststring, pastecount=0, cursor=somewhere illegal */
    memset(buf, 0, SHELL_BUFFER_SIZE);
    memset(paste_buf, 0, SHELL_BUFFER_SIZE);
    sprintf(paste_buf, TEST_STRING);
    cursor = 100;
    paste(&shell, &cursor, buf, paste_buf, 0);
    TEST_ASSERT_EQUAL_INT(0, strlen(buf));
    TEST_ASSERT_EQUAL_INT(100, cursor);

    /* Emty buffer, empty paste, pastecount=100 */
    memset(buf, 0, SHELL_BUFFER_SIZE);
    memset(paste_buf, 0, SHELL_BUFFER_SIZE);
    cursor = 0;
    paste(&shell, &cursor, buf, paste_buf, 100);
    TEST_ASSERT_EQUAL_INT(0, strlen(buf));
    TEST_ASSERT_EQUAL_INT(0, cursor);

    /* Emty buffer, empty paste, pastecount=100, cursor=somewhere illegal */
    memset(buf, 0, SHELL_BUFFER_SIZE);
    memset(paste_buf, 0, SHELL_BUFFER_SIZE);
    cursor = 100;
    paste(&shell, &cursor, buf, paste_buf, 100);
    TEST_ASSERT_EQUAL_INT(0, strlen(buf));
    TEST_ASSERT_EQUAL_INT(100, cursor);
}

static void test_paste_normal(void)
{
    int cursor = 0;
    char *buf = BUFFER;
    char *paste_buf = CUT_PASTE_BUFFER;

    /* Empty buffer, paste=teststring, pastecount=0, cursor=somewhere illegal */
    memset(buf, 0, SHELL_BUFFER_SIZE);
    memset(paste_buf, 0, SHELL_BUFFER_SIZE);
    sprintf(paste_buf, "abc");
    cursor = 0;
    paste(&shell, &cursor, buf, paste_buf, 1);
    TEST_ASSERT_EQUAL_INT(1, strlen(buf));
    TEST_ASSERT_EQUAL_STRING("a", buf);
    TEST_ASSERT_EQUAL_INT(1, cursor);

    /* Empty buffer, paste=teststring, pastecount=0, cursor=somewhere illegal */
    memset(buf, 0, SHELL_BUFFER_SIZE);
    memset(paste_buf, 0, SHELL_BUFFER_SIZE);
    sprintf(paste_buf, "abc");
    cursor = 0;
    paste(&shell, &cursor, buf, paste_buf, 3);
    TEST_ASSERT_EQUAL_INT(3, strlen(buf));
    TEST_ASSERT_EQUAL_STRING("abc", buf);
    TEST_ASSERT_EQUAL_INT(3, cursor);

    /* Empty buffer, paste=teststring, pastecount=0, cursor=somewhere illegal */
    memset(buf, 0, SHELL_BUFFER_SIZE);
    memset(paste_buf, 0, SHELL_BUFFER_SIZE);
    sprintf(buf, "xyz");
    sprintf(paste_buf, "abc");
    cursor = 0;
    paste(&shell, &cursor, buf, paste_buf, 3);
    TEST_ASSERT_EQUAL_INT(6, strlen(buf));
    TEST_ASSERT_EQUAL_STRING("abcxyz", buf);
    TEST_ASSERT_EQUAL_INT(3, cursor);

    /* Empty buffer, paste=teststring, pastecount=0, cursor=somewhere illegal */
    memset(buf, 0, SHELL_BUFFER_SIZE);
    memset(paste_buf, 0, SHELL_BUFFER_SIZE);
    sprintf(buf, "xyz");
    sprintf(paste_buf, "abc");
    cursor = 3;
    paste(&shell, &cursor, buf, paste_buf, 3);
    TEST_ASSERT_EQUAL_INT(6, strlen(buf));
    TEST_ASSERT_EQUAL_STRING("xyzabc", buf);
    TEST_ASSERT_EQUAL_INT(6, cursor);

    /* Empty buffer, paste=teststring, pastecount=0, cursor=somewhere illegal */
    memset(buf, 0, SHELL_BUFFER_SIZE);
    memset(paste_buf, 0, SHELL_BUFFER_SIZE);
    sprintf(buf, "xyz123");
    sprintf(paste_buf, "abc");
    cursor = 3;
    paste(&shell, &cursor, buf, paste_buf, 2);
    TEST_ASSERT_EQUAL_INT(8, strlen(buf));
    TEST_ASSERT_EQUAL_STRING("xyzab123", buf);
    TEST_ASSERT_EQUAL_INT(5, cursor);

}

static void test_paste_overpasting(void)
{
    int cursor = 0;
    char *buf = BUFFER;
    char *paste_buf = CUT_PASTE_BUFFER;

    /* Pasting more characters than are in the paste_buf */
    memset(buf, 0, SHELL_BUFFER_SIZE);
    memset(paste_buf, 0, SHELL_BUFFER_SIZE);
    sprintf(paste_buf, "abc");
    sprintf(buf, "12345");
    cursor = 2;
    paste(&shell, &cursor, buf, paste_buf, 5);
    TEST_ASSERT_EQUAL_INT(8, strlen(buf));
    TEST_ASSERT_EQUAL_STRING("12abc345", buf);

    /* Paste the last character */
    memset(buf, 0, SHELL_BUFFER_SIZE);
    memset(paste_buf, 0, SHELL_BUFFER_SIZE);
    sprintf(buf, "0123456789ABCDEF0123456789ABCDE"); // 31 chars, limit is 32
    sprintf(paste_buf, "_+-");
    cursor = 0;
    paste(&shell, &cursor, buf, paste_buf, 1);
    TEST_ASSERT_EQUAL_INT(32, strlen(buf));
    TEST_ASSERT_EQUAL_STRING("_0123456789ABCDEF0123456789ABCDE", buf);

    /* Paste the last character at the end */
    memset(buf, 0, SHELL_BUFFER_SIZE);
    memset(paste_buf, 0, SHELL_BUFFER_SIZE);
    sprintf(buf, "0123456789ABCDEF0123456789ABCDE"); // 31 chars, limit is 32
    sprintf(paste_buf, "_+-");
    cursor = 31;
    paste(&shell, &cursor, buf, paste_buf, 1);
    TEST_ASSERT_EQUAL_INT(32, strlen(buf));
    TEST_ASSERT_EQUAL_STRING("0123456789ABCDEF0123456789ABCDE_", buf);
    TEST_ASSERT_EQUAL_INT(32, cursor);

    /* Paste the last character inbetween */
    memset(buf, 0, SHELL_BUFFER_SIZE);
    memset(paste_buf, 0, SHELL_BUFFER_SIZE);
    sprintf(buf, "0123456789ABCDEF0123456789ABCDE"); // 31 chars, limit is 32
    sprintf(paste_buf, "_+-");
    cursor = 16;
    paste(&shell, &cursor, buf, paste_buf, 1);
    TEST_ASSERT_EQUAL_INT(32, strlen(buf));
    TEST_ASSERT_EQUAL_STRING("0123456789ABCDEF_0123456789ABCDE", buf);
    TEST_ASSERT_EQUAL_INT(17, cursor);

    /****************************/

    /* Paste in the full buffer */
    memset(buf, 0, SHELL_BUFFER_SIZE);
    memset(paste_buf, 0, SHELL_BUFFER_SIZE);
    sprintf(buf, "0123456789ABCDEF0123456789ABCDEF"); // 32 chars, limit is 32
    sprintf(paste_buf, "_+-");
    cursor = 8;
    paste(&shell, &cursor, buf, paste_buf, 1);
    TEST_ASSERT_EQUAL_INT(32, strlen(buf));
    TEST_ASSERT_EQUAL_STRING("0123456789ABCDEF0123456789ABCDEF", buf);
    TEST_ASSERT_EQUAL_INT(8, cursor);
}

Test *tests_readline_internal_cut_paste_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_cut_normal),
        new_TestFixture(test_cut_bounds_cases),
        new_TestFixture(test_cut_overcutting),
        new_TestFixture(test_cut_loops),
        new_TestFixture(test_paste_normal),
        new_TestFixture(test_paste_bounds_cases),
        new_TestFixture(test_paste_overpasting),
        new_TestFixture(test_paste_illegal_input),
        new_TestFixture(test_paste_loops),
    };

    EMB_UNIT_TESTCALLER(readline_internal_cut_paste_tests, set_up, NULL, fixtures);

    return (Test *)&readline_internal_cut_paste_tests;
}
