/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <string.h>
#include <malloc.h>

#include "embUnit.h"

#include "optparse.h"

/**
 * Test an empty parser.
 *
 * According to the docs, all fields of opt_conf_t can be zero. In that case
 * the parser will fail except if argc = 0.
 *
 * Also, if argc=0, argv=NULL should be OK.
 */
static void test_optparse_trivial(void)
{
    opt_conf_t cfg;
    int parse_result;
    static const char *argv1[] = { "batman" };
    static const char *argv2[] = { "-y", "3" };
    static const char *argv_dash[] = { "--" };

    memset(&cfg, 0, sizeof(cfg));

    parse_result = optparse_cmd(&cfg, 0, NULL);
    TEST_ASSERT_EQUAL_INT(OPTPARSE_OK, parse_result);

    parse_result = optparse_cmd(&cfg, 1, argv1);
    TEST_ASSERT_EQUAL_INT(-2, parse_result);

    parse_result = optparse_cmd(&cfg, 2, argv2);
    TEST_ASSERT_EQUAL_INT(-OPTPARSE_BADSYNTAX, parse_result);

    /* This is OK because "--" is eaten up by the parser */
    parse_result = optparse_cmd(&cfg, 1, argv_dash);
    TEST_ASSERT_EQUAL_INT(OPTPARSE_OK, parse_result);
}

enum _rules {
    VERBOSITY,
    SETTABLE,
    UNSETTABLE,
    QTHING,
    KEY,
    HELP_OPT,
    FLOATTHING,
    DOUBLETHING,
    INTTHING,
    IGNORED1,
    IGNORED2,
    N_RULES
};

#define MAX_ARGS 3

typedef struct argparser_data {
    int n;
    const char *arguments[MAX_ARGS];
} argparser_data_t;

int arg_parser(int argn, const char *v, void *_data)
{
    argparser_data_t *data = _data;

    (void)argn;

    if (data->n >= MAX_ARGS) {
        return -OPTPARSE_BADSYNTAX;
    }

    data->arguments[data->n++] = v;

    return -OPTPARSE_OK;
}

/**
 * Test all actions.
 */
static void test_optparse_actions(void)
{
    opt_conf_t cfg;
    opt_rule_t rules[N_RULES];
    int parse_result;
    static const char *argv1[] = { "test", "-c", "3", "--key", "hello", "-vf5.5",
                                   "-qpasted", "-vv9", "--verbose", "-i", "-v", /* this -v is an argument to -i */
                                   "-s", "-u", "-", "--d", "8.5", "quack", "--124",
                                   "--", "-qwerty" };
    static const char *argv_help[] = { "test", "-c", "3", "-h", "--invalid-option" };
    argparser_data_t adata = { 0 };


    int c = 10;
    char *key = NULL;
    float f = 1.0f;
    const char *q = "nothing";
    int vlevel = 0;
    bool flag_s = false, flag_u = true;
    double d = -1.0;

    set_parse_meta(rules + VERBOSITY, 'v', "verbose", "Verbosity level (can be specified multiple times");
    set_parse_count(rules + VERBOSITY, &vlevel);

    set_parse_meta(rules + SETTABLE, 's', NULL, "Set a flag 's'");
    set_parse_bool(rules + SETTABLE, &flag_s);

    set_parse_meta(rules + UNSETTABLE, 'u', NULL, "Unset the flag 'u'");
    set_parse_bool_unset(rules + UNSETTABLE, &flag_u);

    set_parse_meta(rules + QTHING, 'q', "qthing", "Set the string q.");
    set_parse_str_nocopy(rules + QTHING, &q);

    set_parse_meta(rules + KEY, OPTPARSE_NO_SHORT, "key", "Choose a key");
    set_parse_str(rules + KEY, &key);

    /* (-f, --q) and (-k, --d), just to add some confusion! */
    set_parse_meta(rules + FLOATTHING, 'f', "q", "Set a float");
    set_parse_float(rules + FLOATTHING, &f);
    set_parse_meta(rules + DOUBLETHING, 'k', "d", "Set a double");
    set_parse_double(rules + DOUBLETHING, &d);

    set_parse_meta(rules + INTTHING, 'c', NULL, "Set an integer");
    set_parse_int(rules + INTTHING, &c);

    set_parse_meta(rules + IGNORED1, 'i', NULL, "No op (takes 1 arg)");
    set_parse_ignore(rules + IGNORED1);

    set_parse_meta(rules + IGNORED2, '9', "124", "No op switch");
    set_parse_ignore_sw(rules + IGNORED2);

    set_parse_meta(rules + HELP_OPT, 'h', "help", "Show this help");
    set_parse_help(rules + HELP_OPT);

    opt_conf_init(&cfg, rules, N_RULES,
                  "Test program", OPTPARSE_IGNORE_ARGV0,
                  arg_parser, (void *)&adata);


    parse_result = optparse_cmd(&cfg, sizeof(argv1) / sizeof(*argv1), argv1);

    TEST_ASSERT_EQUAL_INT(-OPTPARSE_OK, parse_result);

    TEST_ASSERT_EQUAL_INT(3, c);
    TEST_ASSERT_EQUAL_INT(4, vlevel);
    TEST_ASSERT_EQUAL_STRING("hello", key);
    TEST_ASSERT_EQUAL_STRING("pasted", q);
    TEST_ASSERT(f == 5.5f);
    TEST_ASSERT(d == 8.5);
    TEST_ASSERT(flag_s);
    TEST_ASSERT(!flag_u);

    TEST_ASSERT_EQUAL_INT(3, adata.n);
    TEST_ASSERT_EQUAL_STRING("-", adata.arguments[0]);
    TEST_ASSERT_EQUAL_STRING("quack", adata.arguments[1]);
    TEST_ASSERT_EQUAL_STRING("-qwerty", adata.arguments[2]);

    free(key);
    key = NULL;

    parse_result = optparse_cmd(&cfg, sizeof(argv_help) / sizeof(*argv_help), argv_help);

    TEST_ASSERT_EQUAL_INT(-OPTPARSE_REQHELP, parse_result);
}

Test *tests_optparse_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_optparse_trivial),
        new_TestFixture(test_optparse_actions),
    };

    EMB_UNIT_TESTCALLER(optparse_tests, NULL, NULL, fixtures);

    return (Test *)&optparse_tests;
}


void tests_optparse(void)
{
    TESTS_RUN(tests_optparse_tests());
}
