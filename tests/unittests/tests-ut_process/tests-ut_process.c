/*
 * SPDX-FileCopyrightText: 2021 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */

#include <errno.h>

#include "embUnit.h"

#include "kernel_defines.h"
#include "ut_process.h"

#include "tests-ut_process.h"

static char _res_buf[128U];
static const ut_process_var_t _test_vec[] = {
    /* Taken from https://datatracker.ietf.org/doc/html/rfc6570#section-3.2
     * minus the level 4 variables */
     { .name = "dub",   .value = "me/too", },
     { .name = "hello", .value = "Hello World!", },
     { .name = "half",  .value = "50%", },
     { .name = "var",   .value = "value", },
     { .name = "who",   .value = "fred", },
     { .name = "base",  .value = "http://example.com/home/", },
     { .name = "path",  .value = "/foo/bar", },
     { .name = "v",     .value = "6", },
     { .name = "x",     .value = "1024", },
     { .name = "y",     .value = "768", },
     { .name = "empty", .value = "", },
     { .name = "undef", .value = NULL },
};

static void _setup(void)
{
    memset(_res_buf, 0, sizeof(_res_buf));
}

static void test_expand_str__overview_example(void)
{
    /* Tests example from
     * https://datatracker.ietf.org/doc/html/rfc6570#section-1.1 */
    static const char *ut = "http://www.example.com/foo{?query,number}";
    static const ut_process_var_t vars[] = {
        { .name = "number", .value = "100", },
        { .name = "query", .value = "mycelium", },
    };
    size_t res_buf_len = sizeof(_res_buf);

    TEST_ASSERT_EQUAL_INT(
        sizeof("http://www.example.com/foo?query=mycelium&number=100") - 1,
        ut_process_str_expand(ut, vars, ARRAY_SIZE(vars), _res_buf, &res_buf_len)
    );
    TEST_ASSERT_EQUAL_STRING(
        "http://www.example.com/foo?query=mycelium&number=100", _res_buf
    );
    TEST_ASSERT_EQUAL_INT(
        sizeof("http://www.example.com/foo?query=mycelium&number=100") - 1,
        res_buf_len
    );

    TEST_ASSERT_EQUAL_INT(
        sizeof("http://www.example.com/foo?number=100") - 1,
        ut_process_str_expand(ut, vars, 1U, _res_buf, &res_buf_len)
    );
    TEST_ASSERT_EQUAL_STRING(
        "http://www.example.com/foo?number=100", _res_buf
    );
    TEST_ASSERT_EQUAL_INT(
        sizeof("http://www.example.com/foo?number=100") - 1,
        res_buf_len
    );

    TEST_ASSERT_EQUAL_INT(
        sizeof("http://www.example.com/foo") - 1,
        ut_process_str_expand(ut, NULL, 0U, _res_buf, &res_buf_len)
    );
    TEST_ASSERT_EQUAL_STRING(
        "http://www.example.com/foo", _res_buf
    );
    TEST_ASSERT_EQUAL_INT(
        sizeof("http://www.example.com/foo") - 1,
        res_buf_len
    );
}

#define ASSERT_EXPANSION(template, expansion) \
    res_buf_len = sizeof(_res_buf); \
    TEST_ASSERT_EQUAL_INT(sizeof(expansion) - 1, ut_process_str_expand( \
        template, _test_vec, ARRAY_SIZE(_test_vec), _res_buf, &res_buf_len \
    )); \
    TEST_ASSERT_EQUAL_STRING(expansion, _res_buf)

static void test_expand_str__simple(void)
{
    size_t res_buf_len;

    /* see https://datatracker.ietf.org/doc/html/rfc6570#section-3.2.2
     * except level 4 expansions */
    ASSERT_EXPANSION("{var}", "value");
    ASSERT_EXPANSION("{hello}", "Hello%20World%21");
    ASSERT_EXPANSION("{half}", "50%25");
    ASSERT_EXPANSION("O{empty}X", "OX");
    ASSERT_EXPANSION("O{undef}X", "OX");
    ASSERT_EXPANSION("{x,y}", "1024,768");
    ASSERT_EXPANSION("{x,hello,y}", "1024,Hello%20World%21,768");
    ASSERT_EXPANSION("?{x,empty}", "?1024,");
    ASSERT_EXPANSION("?{x,undef}", "?1024");
    ASSERT_EXPANSION("?{undef,y}", "?768");
}

static void test_expand_str__reserved(void)
{
    size_t res_buf_len;

    /* see https://datatracker.ietf.org/doc/html/rfc6570#section-3.2.3
     * except level 4 expansions */
    ASSERT_EXPANSION("{+var}", "value");
    ASSERT_EXPANSION("{+hello}", "Hello%20World!");
    ASSERT_EXPANSION("{+half}", "50%25");
    ASSERT_EXPANSION("{base}index", "http%3A%2F%2Fexample.com%2Fhome%2Findex");
    ASSERT_EXPANSION("{+base}index", "http://example.com/home/index");
    ASSERT_EXPANSION("O{+empty}X", "OX");
    ASSERT_EXPANSION("O{+undef}X", "OX");
    ASSERT_EXPANSION("{+path}/here", "/foo/bar/here");
    ASSERT_EXPANSION("here?ref={+path}", "here?ref=/foo/bar");
    ASSERT_EXPANSION("up{+path}{var}/here", "up/foo/barvalue/here");
    ASSERT_EXPANSION("{+x,hello,y}", "1024,Hello%20World!,768");
    ASSERT_EXPANSION("{+path,x}/here", "/foo/bar,1024/here");
}

static void test_expand_str__fragment(void)
{
    size_t res_buf_len;

    /* see https://datatracker.ietf.org/doc/html/rfc6570#section-3.2.4
     * except level 4 expansions */
    ASSERT_EXPANSION("{#var}", "#value");
    ASSERT_EXPANSION("{#hello}", "#Hello%20World!");
    ASSERT_EXPANSION("{#half}", "#50%25");
    ASSERT_EXPANSION("foo{#empty}", "foo#");
    ASSERT_EXPANSION("foo{#undef}", "foo");
    ASSERT_EXPANSION("{#x,hello,y}", "#1024,Hello%20World!,768");
    ASSERT_EXPANSION("{#path,x}/here", "#/foo/bar,1024/here");
}

static void test_expand_str__label(void)
{
    size_t res_buf_len;

    /* see https://datatracker.ietf.org/doc/html/rfc6570#section-3.2.5
     * except level 4 expansions */
    ASSERT_EXPANSION("{.who}", ".fred");
    ASSERT_EXPANSION("{.who,who}", ".fred.fred");
    ASSERT_EXPANSION("{.half,who}", ".50%25.fred");
    ASSERT_EXPANSION("X{.var}", "X.value");
    ASSERT_EXPANSION("X{.empty}", "X.");
    ASSERT_EXPANSION("X{.undef}", "X");

    /* unreserved set is not really tested with provided set, so add a test for
     * that */
    ASSERT_EXPANSION("{.hello}", ".Hello%20World%21");
}

static void test_expand_str__path(void)
{
    size_t res_buf_len;

    /* see https://datatracker.ietf.org/doc/html/rfc6570#section-3.2.6
     * except level 4 expansions */
    ASSERT_EXPANSION("{/who}", "/fred");
    ASSERT_EXPANSION("{/who,who}", "/fred/fred");
    ASSERT_EXPANSION("{/half,who}", "/50%25/fred");
    ASSERT_EXPANSION("{/who,dub}", "/fred/me%2Ftoo");
    ASSERT_EXPANSION("{/var}", "/value");
    ASSERT_EXPANSION("{/var,empty}", "/value/");
    ASSERT_EXPANSION("{/var,undef}", "/value");
    ASSERT_EXPANSION("{/var,x}/here", "/value/1024/here");

    /* unreserved set is not really tested with provided set, so add a test for
     * that */
    ASSERT_EXPANSION("{/hello}", "/Hello%20World%21");
}

static void test_expand_str__path_param(void)
{
    size_t res_buf_len;

    /* see https://datatracker.ietf.org/doc/html/rfc6570#section-3.2.7
     * except level 4 expansions */
    ASSERT_EXPANSION("{;who}", ";who=fred");
    ASSERT_EXPANSION("{;half}", ";half=50%25");
    ASSERT_EXPANSION("{;empty}", ";empty");
    ASSERT_EXPANSION("{;v,empty,who}", ";v=6;empty;who=fred");
    ASSERT_EXPANSION("{;v,bar,who}", ";v=6;who=fred");
    ASSERT_EXPANSION("{;x,y}", ";x=1024;y=768");
    ASSERT_EXPANSION("{;x,y,empty}", ";x=1024;y=768;empty");
    ASSERT_EXPANSION("{;x,y,undef}", ";x=1024;y=768");

    /* unreserved set is not really tested with provided set, so add a test for
     * that */
    ASSERT_EXPANSION("{;hello}", ";hello=Hello%20World%21");
}

static void test_expand_str__query(void)
{
    size_t res_buf_len;

    /* see https://datatracker.ietf.org/doc/html/rfc6570#section-3.2.8
     * except level 4 expansions */
    ASSERT_EXPANSION("{?who}", "?who=fred");
    ASSERT_EXPANSION("{?half}", "?half=50%25");
    ASSERT_EXPANSION("{?x,y}", "?x=1024&y=768");
    ASSERT_EXPANSION("{?x,y,empty}", "?x=1024&y=768&empty=");
    ASSERT_EXPANSION("{?x,y,undef}", "?x=1024&y=768");

    /* unreserved set is not really tested with provided set, so add a test for
     * that */
    ASSERT_EXPANSION("{?hello}", "?hello=Hello%20World%21");
}

static void test_expand_str__query_cont(void)
{
    size_t res_buf_len;

    /* see https://datatracker.ietf.org/doc/html/rfc6570#section-3.2.8
     * except level 4 expansions */
    ASSERT_EXPANSION("{&who}", "&who=fred");
    ASSERT_EXPANSION("{&half}", "&half=50%25");
    ASSERT_EXPANSION("?fixed=yes{&x}", "?fixed=yes&x=1024");
    ASSERT_EXPANSION("{&x,y,empty}", "&x=1024&y=768&empty=");
    ASSERT_EXPANSION("{&x,y,undef}", "&x=1024&y=768");

    /* unreserved set is not really tested with provided set, so add a test for
     * that */
    ASSERT_EXPANSION("{&hello}", "&hello=Hello%20World%21");
}

static void test_expand_str__dup_open(void)
{
    size_t res_buf_len;

    ASSERT_EXPANSION("{half{who}", "{halffred");
    ASSERT_EXPANSION("{{half{who}", "{{halffred");
    ASSERT_EXPANSION("half{who}{{", "halffred{{");
}

static void test_expand_str__dup_close(void)
{
    size_t res_buf_len;

    ASSERT_EXPANSION("{half{who}}", "{halffred}");
    ASSERT_EXPANSION("half{who}}}", "halffred}}");
}

#define ASSERT_ENOBUFS_EXPANSION(template, res_len) \
    res_buf_len = (res_len); \
    TEST_ASSERT_EQUAL_INT(-ENOBUFS, ut_process_str_expand( \
        template, _test_vec, ARRAY_SIZE(_test_vec), _res_buf, &res_buf_len \
    ))

static void test_expand_str__enobufs(void)
{
    size_t res_buf_len;

    ASSERT_ENOBUFS_EXPANSION("{half{who}}", sizeof("{half") - 1);
    ASSERT_ENOBUFS_EXPANSION("foo{#empty}", sizeof("fo") - 1);
    ASSERT_ENOBUFS_EXPANSION("{thisisonlyatest", sizeof("{thisi") - 1);
    ASSERT_ENOBUFS_EXPANSION("foo", sizeof("foo") - 1);
    ASSERT_ENOBUFS_EXPANSION("{?x,y,empty}", sizeof("?x=1024&") - 1);
    ASSERT_ENOBUFS_EXPANSION("{?half}", sizeof("?hal") - 1);
    ASSERT_ENOBUFS_EXPANSION("{?half}", sizeof("?half") - 1);
    ASSERT_ENOBUFS_EXPANSION("{?half}", sizeof("?half=50") - 1);
    ASSERT_ENOBUFS_EXPANSION("foo{#empty}", sizeof("foo") - 1);
}

#define ASSERT_EINVAL_EXPANSION(template) \
    res_buf_len = sizeof(_res_buf); \
    TEST_ASSERT_EQUAL_INT(-EINVAL, ut_process_str_expand( \
        template, _test_vec, ARRAY_SIZE(_test_vec), _res_buf, &res_buf_len \
    ))

static void test_expand_str__einval(void)
{
    size_t res_buf_len;

    ASSERT_EINVAL_EXPANSION("{?x,,empty}");
    ASSERT_EINVAL_EXPANSION("{=x,y,empty}");
    ASSERT_EINVAL_EXPANSION("{?x,$,empty}");
}

static Test *_ut_process_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_expand_str__overview_example),
        new_TestFixture(test_expand_str__simple),
        new_TestFixture(test_expand_str__reserved),
        new_TestFixture(test_expand_str__fragment),
        new_TestFixture(test_expand_str__label),
        new_TestFixture(test_expand_str__path),
        new_TestFixture(test_expand_str__path_param),
        new_TestFixture(test_expand_str__query),
        new_TestFixture(test_expand_str__query_cont),
        new_TestFixture(test_expand_str__dup_open),
        new_TestFixture(test_expand_str__dup_close),
        new_TestFixture(test_expand_str__enobufs),
        new_TestFixture(test_expand_str__einval),
    };

    EMB_UNIT_TESTCALLER(ut_process_tests, _setup, NULL, fixtures);

    return (Test *)&ut_process_tests;
}

void tests_ut_process(void)
{
    TESTS_RUN(_ut_process_tests());
}

/** @} */
