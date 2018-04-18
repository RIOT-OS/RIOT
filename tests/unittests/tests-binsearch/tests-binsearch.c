/**
 * @ingroup tests
 * @{
 * @file
 *
 * @brief   Test binary search.
 * @author  Juan Carrano <j.carrano@fu-berlin.de>
 *
 * @}
 */

#include <stddef.h>

#include "embUnit.h"

#include "binsearch.h"

struct my_record {
    int b;
    char z;
    const char *the_key;
    float dummy;
};

struct my_record my_table[] = {
    {45688,     'b', "antonio", 89.5},
    {25432,     'g', "carl",    2342389.3},
    {42356,     '3', "hans",    189.4},
    {1212,      'g', "jenny",   23489.5},
    {75,        '8', "lisandro",89.7},
    {224,       'o', "luke",    289.8},
    {6733,      '1', "massimo", 2342389.2},
    {23,       '\0', "mike",    389.6},
    {7535,      '1', "peter",   2489.7},
    {3452,      '9', "piotr",   489.9},
    {4342,      ' ', "raoult",  2389.2},
    {45424,(char)0xFF, "richard", 4289.5},
    {2234,      'b', "vittorio",989.2}
};

#define TABLE_LEN (sizeof(my_table)/sizeof(my_table[0]))
#define COMPARE_LEN 255

/**
 * Truncate the table to n elements and perform various tests.
 */
static void _test_n_elements(int n)
{
    int i;

    /* first test the smallest string (nonexistant) */
    int ix = BINSEARCH_STR(my_table, n, the_key, "", COMPARE_LEN);
    const struct my_record *norecord;

    TEST_ASSERT_EQUAL_INT(BINSEARCH_NOTFOUND, ix);

    /* test existing strings */
    for (i=0; i<n; i++) {
        const char *k = my_table[i].the_key;
        const struct my_record *r = BINSEARCH_STR_P(my_table, n, the_key, k, COMPARE_LEN);

        TEST_ASSERT(r != NULL);
        TEST_ASSERT_EQUAL_STRING(k, r->the_key); /* We should find it */
    }

    /* test a big string (nonexistant) */
    ix = BINSEARCH_STR(my_table, n, the_key, "zzzzzzzzzzz", COMPARE_LEN);
    TEST_ASSERT_EQUAL_INT(BINSEARCH_NOTFOUND, ix);

    norecord = BINSEARCH_STR_P(my_table, n, the_key, "invalidname", COMPARE_LEN);
    TEST_ASSERT_NULL(norecord);
}

static void test_n_elements(void)
{
    size_t n;

    for (n = 0; n <= TABLE_LEN; n++) {
        _test_n_elements(n);
    }
}


Test *tests_binsearch_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_n_elements),
    };

    EMB_UNIT_TESTCALLER(binsearch_tests, NULL, NULL, fixtures);

    return (Test *)&binsearch_tests;
}


void tests_binsearch(void)
{
    TESTS_RUN(tests_binsearch_tests());
}
