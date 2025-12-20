/*
 * Copyright (C) 2024-2025 Carl Seifert
 * Copyright (C) 2024-2025 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @file
 * @ingroup unittests
 * @brief   Unit tests for testing path matching functions
 * @author  Carl Seifert <carl.seifert1@mailbox.tu-dresden.de>
 */

#include <stdio.h>
#include <string.h>

#include "tests-unicoap.h"

#include "net/unicoap.h"

#define _MATCH_STRING(resource, string) \
    unicoap_resource_match_path_string(resource, string, static_strlen(string))

static void _test_root_with_string(bool match_subtree) {
    unicoap_resource_t r = {
        .path = "/",
        .flags = match_subtree ? UNICOAP_RESOURCE_FLAG_MATCH_SUBTREE : 0
    };

    _TEST_ASSERT_TRUE(_MATCH_STRING(&r, "/"));
    TEST_ASSERT_EQUAL_INT(_MATCH_STRING(&r, "/a"), match_subtree);
}

static void test_root_with_string(void) {
    _test_root_with_string(false);
}

static void test_root_with_string_subtree(void) {
    _test_root_with_string(true);
}

static void _test_root_with_options(bool match_subtree) {
    unicoap_resource_t r = {
        .path = "/",
        .flags = match_subtree ? UNICOAP_RESOURCE_FLAG_MATCH_SUBTREE : 0
    };

    UNICOAP_OPTIONS_ALLOC(options, 2);

    _TEST_ASSERT_TRUE(unicoap_resource_match_path_options(&r, &options));

    unicoap_options_add_uri_path_component_string(&options, "a");
    TEST_ASSERT_EQUAL_INT(unicoap_resource_match_path_options(&r, &options), match_subtree);
}

static void test_root_with_options(void) {
    _test_root_with_options(false);
}

static void test_root_with_options_subtree(void) {
    _test_root_with_options(true);
}

static void _test_simple_with_string(bool match_subtree) {
    unicoap_resource_t r = {
        .path = "/a",
        .flags = match_subtree ? UNICOAP_RESOURCE_FLAG_MATCH_SUBTREE : 0
    };

    _TEST_ASSERT_FALSE(_MATCH_STRING(&r, "/"));
    _TEST_ASSERT_FALSE(_MATCH_STRING(&r, "a"));
    _TEST_ASSERT_TRUE(_MATCH_STRING(&r, "/a"));
    _TEST_ASSERT_TRUE(_MATCH_STRING(&r, "/a/"));
    _TEST_ASSERT_FALSE(_MATCH_STRING(&r, "/aa"));
    TEST_ASSERT_EQUAL_INT(_MATCH_STRING(&r, "/a/a"), match_subtree);
}

static void test_simple_with_string(void) {
    _test_simple_with_string(false);
}

static void test_simple_with_string_subtree(void) {
    _test_simple_with_string(true);
}

static void _test_simple_with_options(bool match_subtree) {
    unicoap_resource_t r = {
        .path = "/a",
        .flags = match_subtree ? UNICOAP_RESOURCE_FLAG_MATCH_SUBTREE : 0
    };
    UNICOAP_OPTIONS_ALLOC(options, 10);

    _TEST_ASSERT_FALSE(unicoap_resource_match_path_options(&r, &options));

    unicoap_options_add_uri_path_component_string(&options, "a");
    _TEST_ASSERT_TRUE(unicoap_resource_match_path_options(&r, &options));

    unicoap_options_clear(&options);

    unicoap_options_add_uri_path_component_string(&options, "a");
    unicoap_options_add_uri_path_component_string(&options, "a");
    TEST_ASSERT_EQUAL_INT(unicoap_resource_match_path_options(&r, &options), match_subtree);
}

static void test_simple_with_options(void) {
    _test_simple_with_options(false);
}

static void test_simple_with_options_subtree(void) {
    _test_simple_with_options(true);
}

static void _test_long_with_string(bool match_subtree) {
    unicoap_resource_t r = {
        .path = "/a123/b567",
        .flags = match_subtree ? UNICOAP_RESOURCE_FLAG_MATCH_SUBTREE : 0
    };

    _TEST_ASSERT_FALSE(_MATCH_STRING(&r, "/"));
    _TEST_ASSERT_FALSE(_MATCH_STRING(&r, "a"));
    _TEST_ASSERT_FALSE(_MATCH_STRING(&r, "/a"));
    _TEST_ASSERT_FALSE(_MATCH_STRING(&r, "/a123"));
    _TEST_ASSERT_FALSE(_MATCH_STRING(&r, "/a123/"));
    _TEST_ASSERT_TRUE(_MATCH_STRING(&r, "/a123/b567"));
    _TEST_ASSERT_TRUE(_MATCH_STRING(&r, "/a123/b567/"));
    TEST_ASSERT_EQUAL_INT(_MATCH_STRING(&r, "/a123/b567/c89"), match_subtree);
    TEST_ASSERT_EQUAL_INT(_MATCH_STRING(&r, "/a123/b567/c89/"), match_subtree);
    TEST_ASSERT_EQUAL_INT(_MATCH_STRING(&r, "/a123/b567/c89/d00"), match_subtree);
}

static void test_long_with_string(void) {
    _test_long_with_string(false);
}

static void test_long_with_string_subtree(void) {
    _test_long_with_string(true);
}

static void _test_long_with_options(bool match_subtree) {
    unicoap_resource_t r = {
        .path = "/a123/b567",
        .flags = match_subtree ? UNICOAP_RESOURCE_FLAG_MATCH_SUBTREE : 0
    };

    UNICOAP_OPTIONS_ALLOC(options, 20);
    _TEST_ASSERT_FALSE(unicoap_resource_match_path_options(&r, &options));

    unicoap_options_add_uri_path_component_string(&options, "a");
    _TEST_ASSERT_FALSE(unicoap_resource_match_path_options(&r, &options));

    unicoap_options_clear(&options);

    unicoap_options_add_uri_path_component_string(&options, "a123");
    _TEST_ASSERT_FALSE(unicoap_resource_match_path_options(&r, &options));

    unicoap_options_clear(&options);

    unicoap_options_add_uri_path_component_string(&options, "a123");
    unicoap_options_add_uri_path_component_string(&options, "b56");
    _TEST_ASSERT_FALSE(unicoap_resource_match_path_options(&r, &options));

    unicoap_options_clear(&options);

    unicoap_options_add_uri_path_component_string(&options, "a123");
    unicoap_options_add_uri_path_component_string(&options, "b567");
    _TEST_ASSERT_TRUE(unicoap_resource_match_path_options(&r, &options));

    unicoap_options_clear(&options);

    unicoap_options_add_uri_path_component_string(&options, "a123");
    unicoap_options_add_uri_path_component_string(&options, "b567");
    unicoap_options_add_uri_path_component_string(&options, "c89");
    TEST_ASSERT_EQUAL_INT(unicoap_resource_match_path_options(&r, &options), match_subtree);

    unicoap_options_clear(&options);

    unicoap_options_add_uri_path_component_string(&options, "a123");
    unicoap_options_add_uri_path_component_string(&options, "b567");
    unicoap_options_add_uri_path_component_string(&options, "c89");
    unicoap_options_add_uri_path_component_string(&options, "d00");
    TEST_ASSERT_EQUAL_INT(unicoap_resource_match_path_options(&r, &options), match_subtree);
}

static void test_long_with_options(void) {
    _test_long_with_options(false);
}

static void test_long_with_options_subtree(void) {
    _test_long_with_options(true);
}

static inline bool path_is_equal(unicoap_path_t lhs, unicoap_path_t rhs) {
    return unicoap_path_is_equal(&lhs, &rhs);
}

static void test_path_object(void) {
    const char* str_foo = "foo";
    const char* str_bar = "bar";
    const char* str_zoo = "zoo";

    const unicoap_path_t my_path1 = UNICOAP_PATH(str_foo);
    const char* my_path1_layout[] = { str_foo, NULL };
    TEST_ASSERT_EQUAL_INT(memcmp(my_path1._components, my_path1_layout, sizeof(my_path1_layout)), 0);


    const unicoap_path_t my_path2 = UNICOAP_PATH(str_foo, str_bar, str_zoo);
    const char* my_path2_layout[] = { str_foo, str_bar, str_zoo, NULL };
    TEST_ASSERT_EQUAL_INT(memcmp(my_path2._components, my_path2_layout, sizeof(my_path2_layout)), 0);

    const unicoap_path_t my_root = UNICOAP_PATH_ROOT;
    TEST_ASSERT_EQUAL_INT(my_root._components, NULL);

    const unicoap_path_t my_root2 = UNICOAP_PATH(NULL);
    const char* my_root2_layout[] = { NULL };
    TEST_ASSERT_EQUAL_INT(memcmp(my_root2._components, my_root2_layout, sizeof(my_root2_layout)), 0);
}

static void test_path_object_is_root(void) {
    const unicoap_path_t my_path1 = UNICOAP_PATH("a");
    _TEST_ASSERT_FALSE(unicoap_path_is_root(&my_path1));

    const unicoap_path_t my_root = UNICOAP_PATH_ROOT;
    _TEST_ASSERT_TRUE(unicoap_path_is_root(&my_root));

    const unicoap_path_t my_root2 = UNICOAP_PATH(NULL);
    _TEST_ASSERT_TRUE(unicoap_path_is_root(&my_root2));

    const unicoap_path_t my_path2 = UNICOAP_PATH("foo", "bar", "zoo");
    _TEST_ASSERT_FALSE(unicoap_path_is_root(&my_path2));
}

static void test_path_object_component_count(void) {
    const unicoap_path_t my_path1 = UNICOAP_PATH("a");
    TEST_ASSERT_EQUAL_INT(unicoap_path_component_count(&my_path1), 1);

    const unicoap_path_t my_root = UNICOAP_PATH_ROOT;
    TEST_ASSERT_EQUAL_INT(unicoap_path_component_count(&my_root), 0);

    const unicoap_path_t my_root2 = UNICOAP_PATH(NULL);
    TEST_ASSERT_EQUAL_INT(unicoap_path_component_count(&my_root2), 0);

    const unicoap_path_t my_path2 = UNICOAP_PATH("foo", "bar", "zoo");
    TEST_ASSERT_EQUAL_INT(unicoap_path_component_count(&my_path2), 3);
}

static void test_path_object_root_is_equal(void) {
    _TEST_ASSERT_TRUE(path_is_equal(UNICOAP_PATH_ROOT, UNICOAP_PATH_ROOT));
    _TEST_ASSERT_TRUE(path_is_equal(UNICOAP_PATH_ROOT, UNICOAP_PATH(NULL)));
    _TEST_ASSERT_TRUE(path_is_equal(UNICOAP_PATH_ROOT, UNICOAP_PATH(NULL, "a")));
    _TEST_ASSERT_TRUE(path_is_equal(UNICOAP_PATH(NULL), UNICOAP_PATH_ROOT));
    _TEST_ASSERT_TRUE(path_is_equal(UNICOAP_PATH(NULL, "a"), UNICOAP_PATH_ROOT));
    _TEST_ASSERT_TRUE(path_is_equal(UNICOAP_PATH(NULL), UNICOAP_PATH(NULL)));
    _TEST_ASSERT_TRUE(path_is_equal(UNICOAP_PATH(NULL, "x"), UNICOAP_PATH(NULL, "a")));
    _TEST_ASSERT_FALSE(path_is_equal(UNICOAP_PATH_ROOT, UNICOAP_PATH("a")));
    _TEST_ASSERT_FALSE(path_is_equal(UNICOAP_PATH(NULL), UNICOAP_PATH("a")));
}

static void test_path_object_subtree_is_equal(void) {
    _TEST_ASSERT_TRUE(path_is_equal(UNICOAP_PATH("a"), UNICOAP_PATH("a")));
    _TEST_ASSERT_FALSE(path_is_equal(UNICOAP_PATH("a"), UNICOAP_PATH("a", "b")));
    _TEST_ASSERT_FALSE(path_is_equal(UNICOAP_PATH("a", NULL), UNICOAP_PATH("a", "b")));
    _TEST_ASSERT_FALSE(path_is_equal(UNICOAP_PATH("a", "c"), UNICOAP_PATH("a", "b")));
    _TEST_ASSERT_TRUE(path_is_equal(UNICOAP_PATH("a", "b"), UNICOAP_PATH("a", "b")));
    _TEST_ASSERT_FALSE(path_is_equal(UNICOAP_PATH("a", "b", "c"), UNICOAP_PATH("a", "b")));
    _TEST_ASSERT_FALSE(path_is_equal(UNICOAP_PATH("a", "bb"), UNICOAP_PATH("a", "b")));
    _TEST_ASSERT_FALSE(path_is_equal(UNICOAP_PATH("a", "bb"), UNICOAP_PATH("a", "b")));
}


Test* tests_unicoap_matching(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures){
        new_TestFixture(test_root_with_string),
        new_TestFixture(test_root_with_string_subtree),
        new_TestFixture(test_root_with_options),
        new_TestFixture(test_root_with_options_subtree),
        new_TestFixture(test_simple_with_string),
        new_TestFixture(test_simple_with_string_subtree),
        new_TestFixture(test_simple_with_options),
        new_TestFixture(test_simple_with_options_subtree),
        new_TestFixture(test_long_with_string),
        new_TestFixture(test_long_with_string_subtree),
        new_TestFixture(test_long_with_options),
        new_TestFixture(test_long_with_options_subtree),
        new_TestFixture(test_path_object),
        new_TestFixture(test_path_object_is_root),
        new_TestFixture(test_path_object_component_count),
        new_TestFixture(test_path_object_root_is_equal),
        new_TestFixture(test_path_object_subtree_is_equal),
    };

    EMB_UNIT_TESTCALLER(test_unicoap, NULL, NULL, fixtures);

    return (Test*)&test_unicoap;
}
