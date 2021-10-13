/*
* Copyright (C) 2019 HAW Hamburg
*
* This file is subject to the terms and conditions of the GNU Lesser
* General Public License v2.1. See the file LICENSE in the top level
* directory for more details.
*/

#include <string.h>
#include "embUnit.h"
#include "tests-clif.h"

#include "clif.h"

#ifdef TESTS_CLIF_PRINT
#include <stdio.h>
static void _print_attr(clif_attr_t *attr)
{
    if (attr->key) {
        printf("-- Attr: ");
        printf("%.*s", attr->key_len, attr->key);
        if (attr->value) {
            printf(" = %.*s\n", attr->value_len, attr->value);
        }
        else {
            puts("");
        }
    }
}
#endif /* TESTS_CLIF_PRINT */

#define _STR_LEN(s) (sizeof(s)-1)
#define _NEW_ATTR(k, v) { .key = k, .key_len = _STR_LEN(k), .value = v, \
                          .value_len = _STR_LEN(v) }
#define _NEW_ATTR_NO_VAL(k) { .key = k, .key_len = _STR_LEN(k), .value_len = 0 }

/**
 * @brief Compares two link format attributes
 *
 * @param[in] p1 first attribute to compare
 * @param[in] p2 second attribute to compare
 *
 * @return 0 if attributes are equal
 * @return 1 otherwise
 */
static unsigned _compare_attrs(clif_attr_t *p1, clif_attr_t *p2)
{
    unsigned result = 1;
    int res;

    if (p1->key_len != p2->key_len) {
        goto out;
    }

    if (strncmp(p1->key, p2->key, p1->key_len)) {
        goto out;
    }

    if (!p1->value && !p2->value) {
        goto success_out;
    }

    if (!p1->value || !p2->value || (p1->value_len != p2->value_len)) {
        goto out;
    }

    res = strncmp(p1->value, p2->value, p1->value_len);
    if (res != 0) {
        goto out;
    }
success_out:
    result = 0;
out:
    return result;
}

/* This also tests the functions `clif_add_target` and
 * `clif_add_attr`. */
static void test_clif_encode_links(void)
{
    const char exp_string[] = "</sensor/temp>;rt=\"temperature\";if=\"sensor\","
                              "</node/info>,</node/ep>;ct=\"40\"";
    clif_attr_t attrs[] = {
        { .key = "rt", .value = "temperature" },
        { .key = "if", .value = "sensor" },
        { .key = "ct", .value = "40" }
    };

    clif_t links[] = {
        { .target = "/sensor/temp", .attrs = attrs, .attrs_len = 2 },
        { .target = "/node/info", .attrs_len = 0 },
        { .target = "/node/ep", .attrs = &attrs[2], .attrs_len = 1 }
    };

    const size_t exp_size = sizeof(exp_string) - 1;
    char output[sizeof(exp_string) + 1];
    size_t pos = 0;
    ssize_t res = 0;

    /* first test with NULL output to check the needed bytes */
    res = clif_encode_link(&links[0], NULL, 0);
    pos += res;

    for (unsigned i = 1; i < ARRAY_SIZE(links); i++) {
        res = clif_add_link_separator(NULL, 0);
        if (res <= 0) {
            break;
        }
        pos += res;

        res = clif_encode_link(&links[i],NULL, 0);
        if (res <= 0) {
            break;
        }
        pos += res;
    }

    TEST_ASSERT_EQUAL_INT(exp_size, pos);

    /* now actually encode the links */
    pos = 0;
    res = clif_encode_link(&links[0], output, sizeof(output));
    pos += res;

    for (unsigned i = 1; i < ARRAY_SIZE(links); i++) {
        res = clif_add_link_separator(&output[pos], sizeof(output) - pos);
        if (res <= 0) {
            break;
        }
        pos += res;

        res = clif_encode_link(&links[i], &output[pos], sizeof(output) - pos);
        if (res <= 0) {
            break;
        }
        pos += res;
    }
    output[pos++] = '\0';

#ifdef TESTS_CLIF_PRINT
    puts("\n========================================");
    puts("[Test: encode_links]");
    puts("---------------------");
    printf("Encoded links: %s\n", output);
#endif

    TEST_ASSERT_EQUAL_STRING(exp_string, output);
    TEST_ASSERT_EQUAL_INT(exp_size, pos - 1); /* do not count '\0' */
}

/* This also tests the functions `clif_get_target` and `clif_get_attr` */
static void test_clif_decode_links(void)
{
    /* string to decode */
    char input_string[] = "</sensors>;ct=40;title=\"\\\"Sensor\\\" Index, collection\","
                          "</sensors/temp>;rt=\"temperature-c\";if=\"sensor\","
                          "</sensors/light>;rt=\"light-lux\";if=sensor,"
                          "<http://www.example.com/sensors/t123>;"
                          "anchor=\"/sensors/temp\";rel=\"describedby\";sz=1234,"
                          "</t>;anchor=\"/sensors/temp\";rel=\"alternate\";a;s=\"This is \\\"escaped and has , \\\"\","
                          "</riot/board>,</riot/info>;obs";

    /* ordered expected types to be decoded */
    clif_attr_type_t exp_types[] = {
        CLIF_ATTR_CT, CLIF_ATTR_TITLE, CLIF_ATTR_RT, CLIF_ATTR_IF,
        CLIF_ATTR_RT, CLIF_ATTR_IF, CLIF_ATTR_ANCHOR, CLIF_ATTR_REL,
        CLIF_ATTR_SZ, CLIF_ATTR_ANCHOR, CLIF_ATTR_REL, CLIF_ATTR_EXT,
        CLIF_ATTR_EXT, CLIF_ATTR_OBS
    };

    /* ordered amount of expected attributes per link to be decoded */
    unsigned exp_attr_numof[] = { 2, 2, 2, 3, 4, 0, 1 };

    /* ordered expected attributes to be decoded */
    clif_attr_t exp_attrs[] = {
        _NEW_ATTR("ct", "40"),
        _NEW_ATTR("title", "\\\"Sensor\\\" Index, collection"),
        _NEW_ATTR("rt", "temperature-c"),
        _NEW_ATTR("if", "sensor"),
        _NEW_ATTR("rt", "light-lux"),
        _NEW_ATTR("if", "sensor"),
        _NEW_ATTR("anchor", "/sensors/temp"),
        _NEW_ATTR("rel", "describedby"),
        _NEW_ATTR("sz", "1234"),
        _NEW_ATTR("anchor", "/sensors/temp"),
        _NEW_ATTR("rel", "alternate"),
        _NEW_ATTR_NO_VAL("a"),
        _NEW_ATTR("s", "This is \\\"escaped and has , \\\""),
        _NEW_ATTR_NO_VAL("obs"),
    };

    /* ordered expected targets to be decoded */
    const char *exp_targets[] = {
        "/sensors", "/sensors/temp", "/sensors/light",
        "http://www.example.com/sensors/t123", "/t", "/riot/board", "/riot/info"
    };

    const unsigned exp_links_numof = ARRAY_SIZE(exp_targets);
    const unsigned exp_attrs_numof = ARRAY_SIZE(exp_attrs);
    const size_t input_len = sizeof(input_string) - 1;

    clif_t out_link;
    char *pos = input_string;
    unsigned links_numof = 0;

    /* first test without attributes array, to test the expected attributes
     * functionality */
    do {
        ssize_t res = clif_decode_link(&out_link, NULL, 0, pos,
                                       input_len - (pos - input_string));
        if (res < 0) {
            break;
        }
        pos += res;

        /* check expected target */
        TEST_ASSERT(!strncmp(exp_targets[links_numof], out_link.target, out_link.target_len));

        /* check expected amount of attributes */
        TEST_ASSERT_EQUAL_INT(exp_attr_numof[links_numof], out_link.attrs_len);
        links_numof++;
    } while (pos < input_string + sizeof(input_string));

#ifdef TESTS_CLIF_PRINT
    puts("\n========================================");
    puts("[Test: decode_links]");
    printf("- Amount of decoded links: %u\n", links_numof);
#endif
    TEST_ASSERT(exp_links_numof == links_numof);

    /* now decode again but saving the attributes */
    clif_attr_t out_attrs[ARRAY_SIZE(exp_attrs)];
    pos = input_string;
    unsigned attrs_numof = 0;
    do {
        ssize_t res = clif_decode_link(&out_link, &out_attrs[attrs_numof],
                                       exp_attrs_numof - attrs_numof, pos,
                                       input_len - (pos - input_string));

        if (res < 0) {
            break;
        }
        pos += res;
#ifdef TESTS_CLIF_PRINT
        puts("---------------------");
        puts("New link:");
        printf("- Target: %.*s\n", out_link.target_len, out_link.target);
        printf("- Number of attributes: %d\n", out_link.attrs_len);
#endif

        for (unsigned i = 0; i < out_link.attrs_len; i++) {
#ifdef TESTS_CLIF_PRINT
            _print_attr(&out_link.attrs[i]);
#endif
            /* compare the attribute structure with the expected one */
            TEST_ASSERT(!_compare_attrs(&out_link.attrs[i],
                                         &exp_attrs[attrs_numof]));
            clif_attr_type_t type = clif_get_attr_type(out_link.attrs[i].key,
                                                         out_link.attrs[i].key_len);

            /* check that the returned type is the expected one */
            TEST_ASSERT_EQUAL_INT(exp_types[attrs_numof], type);

            /* test type to string conversion */
            const char *t;
            if (clif_attr_type_to_str(type, &t) < 0) {
                t = NULL;
            }
            TEST_ASSERT_EQUAL_STRING(type == CLIF_ATTR_EXT ?
                                     NULL : exp_attrs[attrs_numof].key, t);
            attrs_numof++;
        }
    } while (pos < input_string + sizeof(input_string));
    TEST_ASSERT_EQUAL_INT(exp_attrs_numof, attrs_numof);
}

static void test_clif_get_attr_missing_value(void)
{
    clif_attr_t attr;
    char *input = ";ct=";

    /* Used to result in a spatial memory safety violation.
     * See: https://github.com/RIOT-OS/RIOT/pull/15945 */
    int r = clif_get_attr(input, strlen(input), &attr);
    TEST_ASSERT_EQUAL_INT(CLIF_NOT_FOUND, r);
}

static void test_clif_get_attr_missing_quote(void)
{
    clif_attr_t attr;
    char *input = ";rt=\"temp";

    int r = clif_get_attr(input, strlen(input), &attr);
    TEST_ASSERT_EQUAL_INT(CLIF_NOT_FOUND, r);
}

static void test_clif_get_empty_attr_value(void)
{
    clif_attr_t attr;
    char *input = ";rt=\"\"";

    int r = clif_get_attr(input, strlen(input), &attr);
    TEST_ASSERT_EQUAL_INT(CLIF_NOT_FOUND, r);
}

static void test_clif_get_attr_empty(void)
{
    clif_attr_t attr;

    /* clif_get_attr used to access data even if input was empty.
     * See: https://github.com/RIOT-OS/RIOT/pull/15947 */
    int r = clif_get_attr(NULL, 0, &attr);
    TEST_ASSERT_EQUAL_INT(CLIF_NOT_FOUND, r);
}

Test *tests_clif_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_clif_encode_links),
        new_TestFixture(test_clif_decode_links),
        new_TestFixture(test_clif_get_attr_missing_value),
        new_TestFixture(test_clif_get_attr_missing_quote),
        new_TestFixture(test_clif_get_empty_attr_value),
        new_TestFixture(test_clif_get_attr_empty)
    };

    EMB_UNIT_TESTCALLER(clif_tests, NULL, NULL, fixtures);

    return (Test *)&clif_tests;
}

void tests_clif(void)
{
    TESTS_RUN(tests_clif_tests());
}
