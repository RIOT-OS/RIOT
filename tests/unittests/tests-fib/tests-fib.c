/*
* Copyright (C) 2015 Martin Landsmann
*
* This file is subject to the terms and conditions of the GNU Lesser
* General Public License v2.1. See the file LICENSE in the top level
* directory for more details.
*/

#define TEST_FIB_SHOW_OUTPUT (0) /**< set  */

#include <stdio.h> /**< required for snprintf() */
#include <string.h>
#include <errno.h>
#include "embUnit.h"
#include "tests-fib.h"
#include "vtimer.h"

#include "thread.h"
#include "ng_fib.h"
#include "ng_fib/ng_universal_address.h"

/*
* @brief helper to fill FIB with unique entries
*/
static void _fill_FIB_unique(size_t entries)
{
    size_t add_buf_size = 16;
    char addr_dst[add_buf_size];
    char addr_nxt[add_buf_size];
    uint32_t addr_dst_flags = 0x77777777;
    uint32_t addr_nxt_flags = 0x77777777;

    for (size_t i = 0; i < entries; ++i) {
        /* construct "addresses" for the FIB */
        snprintf(addr_dst, add_buf_size, "Test address %02d", (int)i);
        snprintf(addr_nxt, add_buf_size, "Test address %02d", entries + i);
        /* the terminating \0 is unnecessary here */
        fib_add_entry(42, (uint8_t *)addr_dst, add_buf_size - 1, addr_dst_flags,
                      (uint8_t *)addr_nxt, add_buf_size - 1, addr_nxt_flags, 10000);
    }
}

/*
* @brief helper to fill FIB with multiple used entries
* The modulus adjusts the number of reused addresses
*/
static void _fill_FIB_multiple(size_t entries, size_t modulus)
{
    size_t add_buf_size = 16;
    char addr_dst[add_buf_size];
    char addr_nxt[add_buf_size];
    uint32_t addr_dst_flags = 0x33333333;
    uint32_t addr_nxt_flags = 0x33333333;

    for (size_t i = 0; i < entries; ++i) {
        /* construct "addresses" for the FIB */
        snprintf(addr_dst, add_buf_size, "Test address %02d", (int)i);
        snprintf(addr_nxt, add_buf_size, "Test address %02d", i % modulus);
        fib_add_entry(42, (uint8_t *)addr_dst, add_buf_size - 1, addr_dst_flags,
                      (uint8_t *)addr_nxt, add_buf_size - 1, addr_nxt_flags, 10000);
    }
}

/*
* @brief filling the FIB with entries
* It is expected to have 20 FIB entries and 40 used universal address entries
*/
static void test_fib_01_fill_unique_entries(void)
{
    _fill_FIB_unique(20);
#if (TEST_FIB_SHOW_OUTPUT == 1)
    fib_print_fib_table();
    puts("");
    universal_address_print_table();
    puts("");
#endif
    TEST_ASSERT_EQUAL_INT(20, fib_get_num_used_entries());
    TEST_ASSERT_EQUAL_INT(40, universal_address_get_num_used_entries());
    fib_deinit();
}

/*
 * @brief filling the FIB with reusable entries
 * It is expected to have 20 FIB entries and 20 universal address entries
 */
static void test_fib_02_fill_multiple_entries(void)
{
    size_t entries = 20;
    _fill_FIB_multiple(entries, 11);

#if (TEST_FIB_SHOW_OUTPUT == 1)
    fib_print_fib_table();
    puts("");
    universal_address_print_table();
    puts("");
#endif
    TEST_ASSERT_EQUAL_INT(20, fib_get_num_used_entries());
    TEST_ASSERT_EQUAL_INT(20, universal_address_get_num_used_entries());
    fib_deinit();
}

/*
* @brief filling the FIB with entries and removing all entries
* It is expected to have 0 FIB entries and 0 universal address entries after remove
*/
static void test_fib_03_removing_all_entries(void)
{
    size_t add_buf_size = 16;
    char addr_dst[add_buf_size];

    size_t entries = 20;
    _fill_FIB_unique(entries);

    TEST_ASSERT_EQUAL_INT(20, fib_get_num_used_entries());
    TEST_ASSERT_EQUAL_INT(40, universal_address_get_num_used_entries());

    for (size_t i = 0; i < entries; ++i) {
        /* construct "addresses" to remove */
        snprintf(addr_dst, add_buf_size, "Test address %02d", (int)i);
        fib_remove_entry((uint8_t *)addr_dst, add_buf_size - 1);
    }

    TEST_ASSERT_EQUAL_INT(0, fib_get_num_used_entries());
    TEST_ASSERT_EQUAL_INT(0, universal_address_get_num_used_entries());

#if (TEST_FIB_SHOW_OUTPUT == 1)
    fib_print_fib_table();
    puts("");
    universal_address_print_table();
    puts("");
#endif

    fib_deinit();
}

/*
* @brief filling the FIB with entries and removing the lower 1/2 entries (0..9)
* It is expected to have 10 FIB entries and 19 used universal address entries after remove
*/
static void test_fib_04_remove_lower_half(void)
{
    size_t add_buf_size = 16;
    char addr_dst[add_buf_size];

    size_t entries = 20;
    _fill_FIB_multiple(entries, 11);

    TEST_ASSERT_EQUAL_INT(20, fib_get_num_used_entries());
    TEST_ASSERT_EQUAL_INT(20, universal_address_get_num_used_entries());

    for (size_t i = 0; i < entries / 2; ++i) {
        /* construct "addresses" to remove */
        snprintf(addr_dst, add_buf_size, "Test address %02d", (int)i);
        fib_remove_entry((uint8_t *)addr_dst, add_buf_size - 1);
    }

#if (TEST_FIB_SHOW_OUTPUT == 1)
    fib_print_fib_table();
    puts("");
    universal_address_print_table();
    puts("");
#endif
    TEST_ASSERT_EQUAL_INT(10, fib_get_num_used_entries());
    TEST_ASSERT_EQUAL_INT(19, universal_address_get_num_used_entries());
    fib_deinit();
}

/*
* @brief filling the FIB with entries and removing the upper 1/2 entries (10..19)
* It is expected to have 10 FIB entries and 10 universal address entries after remove
*/
static void test_fib_05_remove_upper_half(void)
{
    size_t add_buf_size = 16;
    char addr_dst[add_buf_size];

    size_t entries = 20;
    _fill_FIB_multiple(entries, 11);

    TEST_ASSERT_EQUAL_INT(20, fib_get_num_used_entries());
    TEST_ASSERT_EQUAL_INT(20, universal_address_get_num_used_entries());

    for (size_t i = 0; i < entries / 2; ++i) {
        /* construct "addresses" to remove */
        snprintf(addr_dst, add_buf_size, "Test address %02d", ((entries / 2) + i));
        fib_remove_entry((uint8_t *)addr_dst, add_buf_size - 1);
    }

#if (TEST_FIB_SHOW_OUTPUT == 1)
    fib_print_fib_table();
    puts("");
    universal_address_print_table();
    puts("");
#endif
    TEST_ASSERT_EQUAL_INT(10, fib_get_num_used_entries());
    TEST_ASSERT_EQUAL_INT(10, universal_address_get_num_used_entries());
    fib_deinit();
}

/*
* @brief filling the FIB with entries and removing one entry
* It is expected to have 19 FIB entries and still 20 universal address entries
* after removing 02
* (the use count for 02 is reduced to 1 after remove)
*/
static void test_fib_06_remove_one_entry(void)
{
    size_t add_buf_size = 16;
    char addr_dst[] = "Test address 02";

    size_t entries = 20;
    _fill_FIB_multiple(entries, 11);

    TEST_ASSERT_EQUAL_INT(20, fib_get_num_used_entries());
    TEST_ASSERT_EQUAL_INT(20, universal_address_get_num_used_entries());

    fib_remove_entry((uint8_t *)addr_dst, add_buf_size - 1);

#if (TEST_FIB_SHOW_OUTPUT == 1)
    fib_print_fib_table();
    puts("");
    universal_address_print_table();
    puts("");
#endif
    TEST_ASSERT_EQUAL_INT(19, fib_get_num_used_entries());
    TEST_ASSERT_EQUAL_INT(20, universal_address_get_num_used_entries());
    fib_deinit();
}

/*
* @brief filling the FIB with entries and removing one entry several times
* It is expected to have 19 FIB entries and 19 universal address entries
* after removing 13
*/
static void test_fib_07_remove_one_entry_multiple_times(void)
{
    size_t add_buf_size = 16; /* includes space for terminating \0 */
    char addr_dst[] = "Test address 13";

    size_t entries = 20;
    _fill_FIB_multiple(entries, 11);

    TEST_ASSERT_EQUAL_INT(20, fib_get_num_used_entries());
    TEST_ASSERT_EQUAL_INT(20, universal_address_get_num_used_entries());

    fib_remove_entry((uint8_t *)addr_dst, add_buf_size - 1);
    fib_remove_entry((uint8_t *)addr_dst, add_buf_size - 1);
    fib_remove_entry((uint8_t *)addr_dst, add_buf_size - 1);
    fib_remove_entry((uint8_t *)addr_dst, add_buf_size - 1);

#if (TEST_FIB_SHOW_OUTPUT == 1)
    fib_print_fib_table();
    puts("");
    universal_address_print_table();
    puts("");
#endif
    TEST_ASSERT_EQUAL_INT(19, fib_get_num_used_entries());
    TEST_ASSERT_EQUAL_INT(19, universal_address_get_num_used_entries());
    fib_deinit();
}

/*
* @brief filling the FIB with entries and removing an unknown entry
* It is expected to have 20 FIB entries and 20 universal address entries after removing
*/
static void test_fib_08_remove_unknown(void)
{
    size_t add_buf_size = 16; /* includes space for terminating \0 */
    char addr_dst[] = "Test address 99";

    size_t entries = 20;
    _fill_FIB_multiple(entries, 11);

    TEST_ASSERT_EQUAL_INT(20, fib_get_num_used_entries());
    TEST_ASSERT_EQUAL_INT(20, universal_address_get_num_used_entries());

    fib_remove_entry((uint8_t *)addr_dst, add_buf_size - 1);
    fib_remove_entry((uint8_t *)addr_dst, add_buf_size - 1);
    fib_remove_entry((uint8_t *)addr_dst, add_buf_size - 1);

#if (TEST_FIB_SHOW_OUTPUT == 1)
    fib_print_fib_table();
    puts("");
    universal_address_print_table();
    puts("");
#endif
    TEST_ASSERT_EQUAL_INT(20, fib_get_num_used_entries());
    TEST_ASSERT_EQUAL_INT(20, universal_address_get_num_used_entries());
    fib_deinit();
}

/*
* @brief filling the FIB with entries and update an entry
* It is expected to have FIB entry 13 with updated lifetime of 9999
* and entry 7 with updated iface ID of 7, lifetime of 7777 and next hop "Test address 77"
*/
static void test_fib_09_update_entry(void)
{
    size_t add_buf_size = 16; /* includes space for terminating \0 */
    char addr_dst13[] = "Test address 13";
    char addr_dst07[] = "Test address 07";
    char addr_nxt2[] = "Test address 99";
    char addr_nxt77[] = "Test address 77";

    size_t entries = 20;
    _fill_FIB_multiple(entries, 11);

    TEST_ASSERT_EQUAL_INT(20, fib_get_num_used_entries());
    TEST_ASSERT_EQUAL_INT(20, universal_address_get_num_used_entries());

    fib_update_entry((uint8_t *)addr_dst13, add_buf_size - 1,
                     (uint8_t *)addr_nxt2, add_buf_size - 1, 0x99, 9999);
    fib_update_entry((uint8_t *)addr_dst07, add_buf_size - 1,
                     (uint8_t *)addr_nxt77, add_buf_size - 1, 0x77, 7777);

#if (TEST_FIB_SHOW_OUTPUT == 1)
    fib_print_fib_table();
    puts("");
    universal_address_print_table();
    puts("");
#endif
    fib_deinit();
}

/*
* @brief filling the FIB with entries and adding an additional one (not fitting)
* It is expected to have 20 FIB entries and to receive FPC_ERROR on adding an additional one
*/
static void test_fib_10_add_exceed(void)
{
    size_t add_buf_size = 16; /* includes space for terminating \0 */
    char addr_dst[] = "Test address 98";
    char addr_nxt[] = "Test address 99";

    size_t entries = 20;
    _fill_FIB_unique(entries);

    TEST_ASSERT_EQUAL_INT(20, fib_get_num_used_entries());
    TEST_ASSERT_EQUAL_INT(40, universal_address_get_num_used_entries());

    int ret = fib_add_entry(42, (uint8_t *)addr_dst, add_buf_size - 1, 0x98,
                            (uint8_t *)addr_nxt, add_buf_size - 1, 0x99, 9999);

    TEST_ASSERT_EQUAL_INT(-ENOMEM, ret);
    TEST_ASSERT_EQUAL_INT(20, fib_get_num_used_entries());
    TEST_ASSERT_EQUAL_INT(40, universal_address_get_num_used_entries());

#if (TEST_FIB_SHOW_OUTPUT == 1)
    fib_print_fib_table();
    puts("");
    universal_address_print_table();
    puts("");
#endif
    fib_deinit();
}

/*
* @brief get next hop for known destination
* It is expected to get the next hop 02 and receive 0
*/
static void test_fib_11_get_next_hop_success(void)
{
    size_t add_buf_size = 16; /* includes space for terminating \0 */
    char addr_dst[] = "Test address 13";
    char addr_expect[] = "Test address 02";
    kernel_pid_t iface_id = KERNEL_PID_UNDEF;
    uint32_t next_hop_flags = 0;
    char addr_nxt[add_buf_size];

    size_t entries = 20;
    _fill_FIB_multiple(entries, 11);

    TEST_ASSERT_EQUAL_INT(20, fib_get_num_used_entries());
    TEST_ASSERT_EQUAL_INT(20, universal_address_get_num_used_entries());
    int ret = fib_get_next_hop(&iface_id,
                               (uint8_t *)addr_nxt, &add_buf_size, &next_hop_flags,
                               (uint8_t *)addr_dst, add_buf_size - 1, 0x13);

    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(20, fib_get_num_used_entries());
    TEST_ASSERT_EQUAL_INT(20, universal_address_get_num_used_entries());

    ret = strncmp(addr_expect, addr_nxt, add_buf_size);
    TEST_ASSERT_EQUAL_INT(0, ret);

#if (TEST_FIB_SHOW_OUTPUT == 1)
    fib_print_fib_table();
    puts("");
    universal_address_print_table();
    puts("");
#endif
    fib_deinit();
}

/*
* @brief get next hop for unknown destination
* It is expected to get no next hop and receive -EHOSTUNREACH
*/
static void test_fib_12_get_next_hop_fail(void)
{
    size_t add_buf_size = 16; /* includes space for terminating \0 */
    char addr_dst[] = "Test address 99";
    kernel_pid_t iface_id = KERNEL_PID_UNDEF;
    uint32_t next_hop_flags = 0;
    char addr_nxt[add_buf_size];

    size_t entries = 20;
    _fill_FIB_multiple(entries, 11);
    TEST_ASSERT_EQUAL_INT(20, fib_get_num_used_entries());
    TEST_ASSERT_EQUAL_INT(20, universal_address_get_num_used_entries());
    int ret = fib_get_next_hop(&iface_id,
                               (uint8_t *)addr_nxt, &add_buf_size, &next_hop_flags,
                               (uint8_t *)addr_dst, add_buf_size - 1, 0x99);

    TEST_ASSERT_EQUAL_INT(-EHOSTUNREACH, ret);

#if (TEST_FIB_SHOW_OUTPUT == 1)
    fib_print_fib_table();
    puts("");
    universal_address_print_table();
    puts("");
#endif
    fib_deinit();
}

/*
* @brief get next hop for known destination but unsufficient size for the output
* It is expected to get no next hop and receive -ENOBUFS
*/
static void test_fib_13_get_next_hop_fail_on_buffer_size(void)
{
    size_t add_buf_size = 16; /* includes space for terminating \0 */
    char addr_dst[] = "Test address 13";
    kernel_pid_t iface_id = KERNEL_PID_UNDEF;
    uint32_t next_hop_flags = 0;
    size_t add_buf_size_nxt = 12;
    char addr_nxt[add_buf_size];

    size_t entries = 20;
    _fill_FIB_multiple(entries, 11);
    TEST_ASSERT_EQUAL_INT(20, fib_get_num_used_entries());
    TEST_ASSERT_EQUAL_INT(20, universal_address_get_num_used_entries());

    int ret = fib_get_next_hop(&iface_id,
                               (uint8_t *)addr_nxt, &add_buf_size_nxt, &next_hop_flags,
                               (uint8_t *)addr_dst, add_buf_size - 1, 0x13);

    TEST_ASSERT_EQUAL_INT(-ENOBUFS, ret);
    TEST_ASSERT_EQUAL_INT(add_buf_size_nxt, add_buf_size - 1);

#if (TEST_FIB_SHOW_OUTPUT == 1)
    fib_print_fib_table();
    puts("");
    universal_address_print_table();
    puts("");
#endif
    fib_deinit();
}

/*
* @brief testing prefix and exact match
* It is expected receive 23 for addr123 as exact match and
* 12 for addr124
*/
static void test_fib_14_exact_and_prefix_match(void)
{
    size_t add_buf_size = 16;
    char addr_dst[add_buf_size];
    char addr_nxt[add_buf_size];
    kernel_pid_t iface_id = KERNEL_PID_UNDEF;
    uint32_t next_hop_flags = 0;
    char addr_lookup[add_buf_size];
    memset(addr_dst, 0, add_buf_size);
    memset(addr_nxt, 0, add_buf_size);

    snprintf(addr_dst, add_buf_size, "Test addr12");
    snprintf(addr_nxt, add_buf_size, "Test address %02d", 12);
    fib_add_entry(42, (uint8_t *)addr_dst, add_buf_size - 1, 0x12,
                  (uint8_t *)addr_nxt, add_buf_size - 1, 0x12, 100000);

    snprintf(addr_dst, add_buf_size, "Test addr123");
    snprintf(addr_nxt, add_buf_size, "Test address %02d", 23);
    fib_add_entry(42, (uint8_t *)addr_dst, add_buf_size - 1, 0x123,
                  (uint8_t *)addr_nxt, add_buf_size - 1, 0x23, 100000);

    snprintf(addr_dst, add_buf_size, "Test addr1234");
    snprintf(addr_nxt, add_buf_size, "Test address %02d", 34);
    fib_add_entry(42, (uint8_t *)addr_dst, add_buf_size - 1, 0x1234,
                  (uint8_t *)addr_nxt, add_buf_size - 1, 0x34, 100000);

    /* exact match */
    snprintf(addr_lookup, add_buf_size, "Test addr123");
    int ret = fib_get_next_hop(&iface_id,
                               (uint8_t *)addr_nxt, &add_buf_size, &next_hop_flags,
                               (uint8_t *)addr_lookup, add_buf_size - 1, 0x123);

    TEST_ASSERT_EQUAL_INT(0, ret);

    char addr_expect_01[] = "Test address 23";
    ret = strncmp(addr_expect_01, addr_nxt, add_buf_size - 1);
    TEST_ASSERT_EQUAL_INT(0, ret);

    /* prefix match */
    add_buf_size = 16;
    memset(addr_nxt, 0, add_buf_size);
    memset(addr_lookup, 0, add_buf_size);

    /* cppcheck: addr_lookup is only passed but not required to be read,
    *            since we test prefix matching
    */
    /* cppcheck-suppress redundantCopy */
    snprintf(addr_lookup, add_buf_size, "Test addr124");
    ret = fib_get_next_hop(&iface_id,
                           (uint8_t *)addr_nxt, &add_buf_size, &next_hop_flags,
                           (uint8_t *)addr_lookup, add_buf_size - 1, 0x124);

    TEST_ASSERT_EQUAL_INT(0, ret);

    add_buf_size = 16;
    char addr_expect_02[] = "Test address 12";
    ret = strncmp(addr_expect_02, addr_nxt, add_buf_size);
    TEST_ASSERT_EQUAL_INT(0, ret);

#if (TEST_FIB_SHOW_OUTPUT == 1)
    fib_print_fib_table();
    puts("");
    universal_address_print_table();
    puts("");
#endif
    fib_deinit();
}

static void test_fib_15_get_lifetime(void)
{
    timex_t lifetime, now;
    kernel_pid_t iface_id = 1;
    char addr_dst[] = "Test address151";
    char addr_nxt[] = "Test address152";
    size_t add_buf_size = 16;
    uint32_t addr_dst_flags = 0x77777777;
    uint32_t addr_nxt_flags = 0x77777777;

    TEST_ASSERT_EQUAL_INT(0, fib_add_entry(iface_id, (uint8_t *)addr_dst, add_buf_size - 1,
                          addr_dst_flags, (uint8_t *)addr_nxt, add_buf_size - 1,
                          addr_nxt_flags, 1000));

    TEST_ASSERT_EQUAL_INT(0, fib_devel_get_lifetime(&lifetime, (uint8_t *)addr_dst, add_buf_size - 1));

    /* assuming some ms passed during these operations... */
    vtimer_now(&now);
    timex_t cmp_lifetime = timex_add(now, timex_set(0, 900000));
    timex_t cmp_max_lifetime = timex_add(now, timex_set(1,1));
    TEST_ASSERT_EQUAL_INT(1, timex_cmp(lifetime, cmp_lifetime));
    /* make sure lifetime hasn't grown magically either */
    TEST_ASSERT_EQUAL_INT(-1, timex_cmp(lifetime, cmp_max_lifetime));
    fib_deinit();
}

Test *tests_fib_tests(void)
{
    fib_init();
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_fib_01_fill_unique_entries),
                        new_TestFixture(test_fib_02_fill_multiple_entries),
                        new_TestFixture(test_fib_03_removing_all_entries),
                        new_TestFixture(test_fib_04_remove_lower_half),
                        new_TestFixture(test_fib_05_remove_upper_half),
                        new_TestFixture(test_fib_06_remove_one_entry),
                        new_TestFixture(test_fib_07_remove_one_entry_multiple_times),
                        new_TestFixture(test_fib_08_remove_unknown),
                        new_TestFixture(test_fib_09_update_entry),
                        new_TestFixture(test_fib_10_add_exceed),
                        new_TestFixture(test_fib_11_get_next_hop_success),
                        new_TestFixture(test_fib_12_get_next_hop_fail),
                        new_TestFixture(test_fib_13_get_next_hop_fail_on_buffer_size),
                        new_TestFixture(test_fib_14_exact_and_prefix_match),
                        new_TestFixture(test_fib_15_get_lifetime),
    };

    EMB_UNIT_TESTCALLER(fib_tests, NULL, NULL, fixtures);

    return (Test *)&fib_tests;
}

void tests_fib(void)
{
    TESTS_RUN(tests_fib_tests());
}
