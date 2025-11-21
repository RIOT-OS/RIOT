/*
 * SPDX-FileCopyrightText: 2015 Martin Landsmann
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#define TEST_FIB_SHOW_OUTPUT (0) /**< set  */

#include <stdio.h> /**< required for snprintf() */
#include <string.h>
#include <errno.h>
#include "embUnit.h"
#include "tests-fib.h"
#include "xtimer.h"

#include "thread.h"
#include "net/fib.h"
#include "universal_address.h"

#define TEST_FIB_TABLE_SIZE (20)
static fib_entry_t _entries[TEST_FIB_TABLE_SIZE];
static fib_table_t test_fib_table = { .data.entries = _entries,
                                      .table_type = FIB_TABLE_TYPE_SH,
                                      .size = TEST_FIB_TABLE_SIZE,
                                      .mtx_access = MUTEX_INIT,
                                      .notify_rp_pos = 0 };

/*
* @brief helper to create FIB test addresses
*/
static void _set_fib_test_addr(char * addr_dst, unsigned add_buf_size, unsigned i) {
    TEST_ASSERT(i < 100 && add_buf_size >= 16);
    snprintf(addr_dst, add_buf_size, "Test address %02u", i);
}

/*
* @brief helper to fill FIB with unique entries
*/
static void _fill_fib_unique(unsigned entries)
{
    size_t add_buf_size = 16;
    char addr_dst[add_buf_size];
    char addr_nxt[add_buf_size];
    uint32_t addr_dst_flags = 0x00777777;
    uint32_t addr_nxt_flags = 0x00777777;

    for (unsigned i = 0; i < entries; ++i) {
        /* construct "addresses" for the FIB */
        _set_fib_test_addr(addr_dst, add_buf_size, i);
        _set_fib_test_addr(addr_nxt, add_buf_size, entries + i);
        /* the terminating \0 is unnecessary here */
        fib_add_entry(&test_fib_table, 42,
                      (uint8_t *)addr_dst, add_buf_size - 1, addr_dst_flags,
                      (uint8_t *)addr_nxt, add_buf_size - 1, addr_nxt_flags,
                      10000);
    }
}

/*
* @brief helper to fill FIB with multiple used entries
* The modulus adjusts the number of reused addresses
*/
static void _fill_fib_multiple(unsigned entries, unsigned modulus)
{
    size_t add_buf_size = 16;
    char addr_dst[add_buf_size];
    char addr_nxt[add_buf_size];
    uint32_t addr_dst_flags = 0x00333333;
    uint32_t addr_nxt_flags = 0x00333333;

    for (unsigned i = 0; i < entries; ++i) {
        /* construct "addresses" for the FIB */
        _set_fib_test_addr(addr_dst, add_buf_size, i);
        _set_fib_test_addr(addr_nxt, add_buf_size, i % modulus);
        fib_add_entry(&test_fib_table, 42,
                      (uint8_t *)addr_dst, add_buf_size - 1, addr_dst_flags,
                      (uint8_t *)addr_nxt, add_buf_size - 1, addr_nxt_flags,
                      10000);
    }
}

/*
* @brief helper to determine the prefix bits
*/
static unsigned _get_prefix_bits_num(char* addr, unsigned addr_len)
{
    /* Get the index of the first trailing `0` */
    int i = 0;
    for (i = addr_len-1; i > 0; --i) {
        if (addr[i] != 0) {
            break;
        }
    }

    /* now we check the bits of the lowest byte */
    uint8_t j = 0;
    for ( ; j < 8; ++j) {
        if ((addr[i] >> j) & 0x01) {
            break;
        }
    }
    return (i << 3) + (8 - j);
}

/*
* @brief filling the FIB with entries
* It is expected to have 20 FIB entries and 40 used universal address entries
*/
static void test_fib_01_fill_unique_entries(void)
{
    _fill_fib_unique(20);
#if (TEST_FIB_SHOW_OUTPUT == 1)
    fib_print_fib_table(&test_fib_table);
    puts("");
    universal_address_print_table();
    puts("");
#endif
    TEST_ASSERT_EQUAL_INT(20, fib_get_num_used_entries(&test_fib_table));
    TEST_ASSERT_EQUAL_INT(40, universal_address_get_num_used_entries());
    fib_deinit(&test_fib_table);
}

/*
 * @brief filling the FIB with reusable entries
 * It is expected to have 20 FIB entries and 20 universal address entries
 */
static void test_fib_02_fill_multiple_entries(void)
{
    unsigned entries = 20;
    _fill_fib_multiple(entries, 11);

#if (TEST_FIB_SHOW_OUTPUT == 1)
    fib_print_fib_table(&test_fib_table);
    puts("");
    universal_address_print_table();
    puts("");
#endif
    TEST_ASSERT_EQUAL_INT(20, fib_get_num_used_entries(&test_fib_table));
    TEST_ASSERT_EQUAL_INT(20, universal_address_get_num_used_entries());
    fib_deinit(&test_fib_table);
}

/*
* @brief filling the FIB with entries and removing all entries
* It is expected to have 0 FIB entries and 0 universal address entries after remove
*/
static void test_fib_03_removing_all_entries(void)
{
    size_t add_buf_size = 16;
    char addr_dst[add_buf_size];

    unsigned entries = 20;
    _fill_fib_unique(entries);

    TEST_ASSERT_EQUAL_INT(20, fib_get_num_used_entries(&test_fib_table));
    TEST_ASSERT_EQUAL_INT(40, universal_address_get_num_used_entries());

    for (unsigned i = 0; i < entries; ++i) {
        /* construct "addresses" to remove */
        _set_fib_test_addr(addr_dst, add_buf_size, i);
        fib_remove_entry(&test_fib_table, (uint8_t *)addr_dst, add_buf_size - 1);
    }

    TEST_ASSERT_EQUAL_INT(0, fib_get_num_used_entries(&test_fib_table));
    TEST_ASSERT_EQUAL_INT(0, universal_address_get_num_used_entries());

#if (TEST_FIB_SHOW_OUTPUT == 1)
    fib_print_fib_table(&test_fib_table);
    puts("");
    universal_address_print_table();
    puts("");
#endif

    fib_deinit(&test_fib_table);
}

/*
* @brief filling the FIB with entries and removing the lower 1/2 entries (0..9)
* It is expected to have 10 FIB entries and 19 used universal address entries after remove
*/
static void test_fib_04_remove_lower_half(void)
{
    size_t add_buf_size = 16;
    char addr_dst[add_buf_size];

    unsigned entries = 20;
    _fill_fib_multiple(entries, 11);

    TEST_ASSERT_EQUAL_INT(20, fib_get_num_used_entries(&test_fib_table));
    TEST_ASSERT_EQUAL_INT(20, universal_address_get_num_used_entries());

    for (unsigned i = 0; i < entries / 2; ++i) {
        /* construct "addresses" to remove */
        _set_fib_test_addr(addr_dst, add_buf_size, i);
        fib_remove_entry(&test_fib_table, (uint8_t *)addr_dst, add_buf_size - 1);
    }

#if (TEST_FIB_SHOW_OUTPUT == 1)
    fib_print_fib_table(&test_fib_table);
    puts("");
    universal_address_print_table();
    puts("");
#endif
    TEST_ASSERT_EQUAL_INT(10, fib_get_num_used_entries(&test_fib_table));
    TEST_ASSERT_EQUAL_INT(19, universal_address_get_num_used_entries());
    fib_deinit(&test_fib_table);
}

/*
* @brief filling the FIB with entries and removing the upper 1/2 entries (10..19)
* It is expected to have 10 FIB entries and 10 universal address entries after remove
*/
static void test_fib_05_remove_upper_half(void)
{
    size_t add_buf_size = 16;
    char addr_dst[add_buf_size];

    unsigned entries = 20;
    _fill_fib_multiple(entries, 11);

    TEST_ASSERT_EQUAL_INT(20, fib_get_num_used_entries(&test_fib_table));
    TEST_ASSERT_EQUAL_INT(20, universal_address_get_num_used_entries());

    for (unsigned i = 0; i < entries / 2; ++i) {
        /* construct "addresses" to remove */
        _set_fib_test_addr(addr_dst, add_buf_size, (entries / 2) + i);
        fib_remove_entry(&test_fib_table, (uint8_t *)addr_dst, add_buf_size - 1);
    }

#if (TEST_FIB_SHOW_OUTPUT == 1)
    fib_print_fib_table(&test_fib_table);
    puts("");
    universal_address_print_table();
    puts("");
#endif
    TEST_ASSERT_EQUAL_INT(10, fib_get_num_used_entries(&test_fib_table));

    TEST_ASSERT_EQUAL_INT(10, universal_address_get_num_used_entries());
    fib_deinit(&test_fib_table);
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

    unsigned entries = 20;
    _fill_fib_multiple(entries, 11);

    TEST_ASSERT_EQUAL_INT(20, fib_get_num_used_entries(&test_fib_table));
    TEST_ASSERT_EQUAL_INT(20, universal_address_get_num_used_entries());

    fib_remove_entry(&test_fib_table, (uint8_t *)addr_dst, add_buf_size - 1);

#if (TEST_FIB_SHOW_OUTPUT == 1)
    fib_print_fib_table(&test_fib_table);
    puts("");
    universal_address_print_table();
    puts("");
#endif
    TEST_ASSERT_EQUAL_INT(19, fib_get_num_used_entries(&test_fib_table));
    TEST_ASSERT_EQUAL_INT(20, universal_address_get_num_used_entries());
    fib_deinit(&test_fib_table);
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

    unsigned entries = 20;
    _fill_fib_multiple(entries, 11);

    TEST_ASSERT_EQUAL_INT(20, fib_get_num_used_entries(&test_fib_table));
    TEST_ASSERT_EQUAL_INT(20, universal_address_get_num_used_entries());

    fib_remove_entry(&test_fib_table, (uint8_t *)addr_dst, add_buf_size - 1);
    fib_remove_entry(&test_fib_table, (uint8_t *)addr_dst, add_buf_size - 1);
    fib_remove_entry(&test_fib_table, (uint8_t *)addr_dst, add_buf_size - 1);
    fib_remove_entry(&test_fib_table, (uint8_t *)addr_dst, add_buf_size - 1);

#if (TEST_FIB_SHOW_OUTPUT == 1)
    fib_print_fib_table(&test_fib_table);
    puts("");
    universal_address_print_table();
    puts("");
#endif
    TEST_ASSERT_EQUAL_INT(19, fib_get_num_used_entries(&test_fib_table));
    TEST_ASSERT_EQUAL_INT(19, universal_address_get_num_used_entries());
    fib_deinit(&test_fib_table);
}

/*
* @brief filling the FIB with entries and removing an unknown entry
* It is expected to have 20 FIB entries and 20 universal address entries after removing
*/
static void test_fib_08_remove_unknown(void)
{
    size_t add_buf_size = 16; /* includes space for terminating \0 */
    char addr_dst[] = "Test address 99";

    unsigned entries = 20;
    _fill_fib_multiple(entries, 11);

    TEST_ASSERT_EQUAL_INT(20, fib_get_num_used_entries(&test_fib_table));
    TEST_ASSERT_EQUAL_INT(20, universal_address_get_num_used_entries());

    fib_remove_entry(&test_fib_table, (uint8_t *)addr_dst, add_buf_size - 1);
    fib_remove_entry(&test_fib_table, (uint8_t *)addr_dst, add_buf_size - 1);
    fib_remove_entry(&test_fib_table, (uint8_t *)addr_dst, add_buf_size - 1);

#if (TEST_FIB_SHOW_OUTPUT == 1)
    fib_print_fib_table(&test_fib_table);
    puts("");
    universal_address_print_table();
    puts("");
#endif
    TEST_ASSERT_EQUAL_INT(20, fib_get_num_used_entries(&test_fib_table));
    TEST_ASSERT_EQUAL_INT(20, universal_address_get_num_used_entries());
    fib_deinit(&test_fib_table);
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

    unsigned entries = 20;
    _fill_fib_multiple(entries, 11);

    TEST_ASSERT_EQUAL_INT(20, fib_get_num_used_entries(&test_fib_table));
    TEST_ASSERT_EQUAL_INT(20, universal_address_get_num_used_entries());

    fib_update_entry(&test_fib_table, (uint8_t *)addr_dst13,
                     add_buf_size - 1, (uint8_t *)addr_nxt2, add_buf_size - 1,
                     0x99, 9999);
    fib_update_entry(&test_fib_table, (uint8_t *)addr_dst07,
                     add_buf_size - 1, (uint8_t *)addr_nxt77, add_buf_size - 1,
                     0x77, 7777);

#if (TEST_FIB_SHOW_OUTPUT == 1)
    fib_print_fib_table(&test_fib_table);
    puts("");
    universal_address_print_table();
    puts("");
#endif
    fib_deinit(&test_fib_table);
}

/*
* @brief filling the FIB with entries and adding an additional one (not fitting)
* It is expected to have 20 FIB entries and to receive FPC_ERROR on adding an
* additional one
*/
static void test_fib_10_add_exceed(void)
{
    size_t add_buf_size = 16; /* includes space for terminating \0 */
    char addr_dst[] = "Test address 98";
    char addr_nxt[] = "Test address 99";

    unsigned entries = 20;
    _fill_fib_unique(entries);

    TEST_ASSERT_EQUAL_INT(20, fib_get_num_used_entries(&test_fib_table));
    TEST_ASSERT_EQUAL_INT(40, universal_address_get_num_used_entries());

    int ret = fib_add_entry(&test_fib_table, 42, (uint8_t *)addr_dst,
                            add_buf_size - 1, 0x98, (uint8_t *)addr_nxt,
                            add_buf_size - 1, 0x99, 9999);

    TEST_ASSERT_EQUAL_INT(-ENOMEM, ret);
    TEST_ASSERT_EQUAL_INT(20, fib_get_num_used_entries(&test_fib_table));
    TEST_ASSERT_EQUAL_INT(40, universal_address_get_num_used_entries());

#if (TEST_FIB_SHOW_OUTPUT == 1)
    fib_print_fib_table(&test_fib_table);
    puts("");
    universal_address_print_table();
    puts("");
#endif
    fib_deinit(&test_fib_table);
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

    unsigned entries = 20;
    _fill_fib_multiple(entries, 11);

    TEST_ASSERT_EQUAL_INT(20, fib_get_num_used_entries(&test_fib_table));
    TEST_ASSERT_EQUAL_INT(20, universal_address_get_num_used_entries());
    int ret = fib_get_next_hop(&test_fib_table, &iface_id,
                               (uint8_t *)addr_nxt, &add_buf_size, &next_hop_flags,
                               (uint8_t *)addr_dst, add_buf_size - 1, 0x13);

    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(20, fib_get_num_used_entries(&test_fib_table));
    TEST_ASSERT_EQUAL_INT(20, universal_address_get_num_used_entries());

    ret = strncmp(addr_expect, addr_nxt, add_buf_size);
    TEST_ASSERT_EQUAL_INT(0, ret);

#if (TEST_FIB_SHOW_OUTPUT == 1)
    fib_print_fib_table(&test_fib_table);
    puts("");
    universal_address_print_table();
    puts("");
#endif
    fib_deinit(&test_fib_table);
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

    unsigned entries = 20;
    _fill_fib_multiple(entries, 11);
    TEST_ASSERT_EQUAL_INT(20, fib_get_num_used_entries(&test_fib_table));
    TEST_ASSERT_EQUAL_INT(20, universal_address_get_num_used_entries());
    int ret = fib_get_next_hop(&test_fib_table, &iface_id, (uint8_t *)addr_nxt,
                               &add_buf_size, &next_hop_flags,
                               (uint8_t *)addr_dst, add_buf_size - 1, 0x99);

    TEST_ASSERT_EQUAL_INT(-EHOSTUNREACH, ret);

#if (TEST_FIB_SHOW_OUTPUT == 1)
    fib_print_fib_table(&test_fib_table);
    puts("");
    universal_address_print_table();
    puts("");
#endif
    fib_deinit(&test_fib_table);
}

/*
* @brief get next hop for known destination but insufficient size for the output
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

    unsigned entries = 20;
    _fill_fib_multiple(entries, 11);
    TEST_ASSERT_EQUAL_INT(20, fib_get_num_used_entries(&test_fib_table));
    TEST_ASSERT_EQUAL_INT(20, universal_address_get_num_used_entries());

    int ret = fib_get_next_hop(&test_fib_table, &iface_id, (uint8_t *)addr_nxt,
                               &add_buf_size_nxt, &next_hop_flags,
                               (uint8_t *)addr_dst, add_buf_size - 1, 0x13);

    TEST_ASSERT_EQUAL_INT(-ENOBUFS, ret);
    TEST_ASSERT_EQUAL_INT(add_buf_size_nxt, add_buf_size - 1);

#if (TEST_FIB_SHOW_OUTPUT == 1)
    fib_print_fib_table(&test_fib_table);
    puts("");
    universal_address_print_table();
    puts("");
#endif
    fib_deinit(&test_fib_table);
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

    /* get the prefix in bits */
    uint32_t prefix_len = _get_prefix_bits_num(addr_dst, strlen(addr_dst));

    fib_add_entry(&test_fib_table, 42, (uint8_t *)addr_dst,
                  add_buf_size - 1, ((prefix_len << FIB_FLAG_NET_PREFIX_SHIFT) | 0x12),
                  (uint8_t *)addr_nxt, add_buf_size - 1,
                  0x12, 100000);

    snprintf(addr_dst, add_buf_size, "Test addr123");
    snprintf(addr_nxt, add_buf_size, "Test address %02d", 23);
    prefix_len = _get_prefix_bits_num(addr_dst, strlen(addr_dst));
    fib_add_entry(&test_fib_table, 42, (uint8_t *)addr_dst,
                  add_buf_size - 1, ((prefix_len << FIB_FLAG_NET_PREFIX_SHIFT) | 0x123),
                  (uint8_t *)addr_nxt, add_buf_size - 1,
                  0x23, 100000);

    snprintf(addr_dst, add_buf_size, "Test addr1234");
    snprintf(addr_nxt, add_buf_size, "Test address %02d", 34);
    prefix_len = _get_prefix_bits_num(addr_dst, strlen(addr_dst));
    fib_add_entry(&test_fib_table, 42, (uint8_t *)addr_dst,
                  add_buf_size - 1, ((prefix_len << FIB_FLAG_NET_PREFIX_SHIFT) | 0x1234),
                  (uint8_t *)addr_nxt, add_buf_size - 1,
                  0x34, 100000);

    memset(addr_lookup, 0, add_buf_size);
    /* exact match */
    snprintf(addr_lookup, add_buf_size, "Test addr123");
    int ret = fib_get_next_hop(&test_fib_table, &iface_id,
                               (uint8_t *)addr_nxt, &add_buf_size, &next_hop_flags,
                               (uint8_t *)addr_lookup, add_buf_size - 1, 0x123);

    TEST_ASSERT_EQUAL_INT(0, ret);
    add_buf_size = 16;

    char addr_expect_01[] = "Test address 23";
    ret = strncmp(addr_expect_01, addr_nxt, add_buf_size - 1);
    TEST_ASSERT_EQUAL_INT(0, ret);

    /* prefix match */
    add_buf_size = 16;
    memset(addr_nxt, 0, add_buf_size);
    memset(addr_lookup, 0, add_buf_size);

    /* cppcheck-suppress redundantCopy
     * (reason: addr_lookup is only passed but not required to be read,
     *  since we test prefix matching) */
    snprintf(addr_lookup, add_buf_size, "Test addr124");
    ret = fib_get_next_hop(&test_fib_table, &iface_id,
                           (uint8_t *)addr_nxt, &add_buf_size, &next_hop_flags,
                           (uint8_t *)addr_lookup, add_buf_size - 1, 0x124);

    TEST_ASSERT_EQUAL_INT(0, ret);

    add_buf_size = 16;
    char addr_expect_02[] = "Test address 12";
    ret = strncmp(addr_expect_02, addr_nxt, add_buf_size);
    TEST_ASSERT_EQUAL_INT(0, ret);

#if (TEST_FIB_SHOW_OUTPUT == 1)
    fib_print_fib_table(&test_fib_table);
    puts("");
    universal_address_print_table();
    puts("");
#endif
    fib_deinit(&test_fib_table);
}

static void test_fib_15_get_lifetime(void)
{
    uint64_t lifetime, now;
    kernel_pid_t iface_id = 1;
    char addr_dst[] = "Test address151";
    char addr_nxt[] = "Test address152";
    size_t add_buf_size = 16;
    uint32_t addr_dst_flags = 0x77777777;
    uint32_t addr_nxt_flags = 0x77777777;

    TEST_ASSERT_EQUAL_INT(0, fib_add_entry(&test_fib_table,
                          iface_id, (uint8_t *)addr_dst, add_buf_size - 1,
                          addr_dst_flags, (uint8_t *)addr_nxt, add_buf_size - 1,
                          addr_nxt_flags, 1000));

    TEST_ASSERT_EQUAL_INT(0, fib_devel_get_lifetime(&test_fib_table, &lifetime,
                                                    (uint8_t *)addr_dst,
                                                    add_buf_size - 1));

    /* assuming some ms passed during these operations... */
    now = xtimer_now_usec64();
    uint64_t cmp_lifetime = now + 900000lU;
    uint64_t cmp_max_lifetime = now + 1100000lU;

    TEST_ASSERT_EQUAL_INT(1, (lifetime > cmp_lifetime));
    /* make sure lifetime hasn't grown magically either */
    TEST_ASSERT_EQUAL_INT(1, (lifetime < cmp_max_lifetime));

    fib_deinit(&test_fib_table);
}

/*
* @brief testing prefix with bits
*/
static void test_fib_16_prefix_match(void)
{
    size_t add_buf_size = 16;
    char addr_dst[add_buf_size];
    char addr_nxt[add_buf_size];
    char addr_lookup[add_buf_size];
    kernel_pid_t iface_id = KERNEL_PID_UNDEF;
    uint32_t next_hop_flags = 0;

    memset(addr_dst, 0, add_buf_size);
    memset(addr_nxt, 0, add_buf_size);
    memset(addr_lookup, 0, add_buf_size);

    snprintf(addr_dst, add_buf_size, "Test address 1X");
    snprintf(addr_nxt, add_buf_size, "Test address 99");
    snprintf(addr_lookup, add_buf_size, "Test address 1X");

    /* now we change the last byte of addr_dst to have defined trailing 0 bits */
    /* test success */
    addr_dst[14] = (char)0x80;    /* 1000 0000 */
    addr_lookup[14] = (char)0x87; /* 1000 0111 */

    uint32_t prefix_len = _get_prefix_bits_num(addr_dst, strlen(addr_dst));
    fib_add_entry(&test_fib_table, 42, (uint8_t *)addr_dst,
                  add_buf_size - 1, ((prefix_len << FIB_FLAG_NET_PREFIX_SHIFT) | 0x123),
                  (uint8_t *)addr_nxt, add_buf_size - 1,
                  0x23, 100000);

    addr_dst[14] = (char)0x3c;    /* 0011 1100 */
    prefix_len = _get_prefix_bits_num(addr_dst, strlen(addr_dst));
    fib_add_entry(&test_fib_table, 42, (uint8_t *)addr_dst,
                  add_buf_size - 1, ((prefix_len << FIB_FLAG_NET_PREFIX_SHIFT) | 0x123),
                  (uint8_t *)addr_nxt, add_buf_size - 1,
                  0x23, 100000);

    memset(addr_nxt, 0, add_buf_size);

    int ret = fib_get_next_hop(&test_fib_table, &iface_id,
                             (uint8_t *)addr_nxt, &add_buf_size, &next_hop_flags,
                             (uint8_t *)addr_lookup, add_buf_size - 1, 0x123);

    TEST_ASSERT_EQUAL_INT(0, ret);

    /* test fail */
    addr_dst[14] = (char)0x3c;    /* 0011 1100 */
    addr_lookup[14] = (char)0x34; /* 0011 0100 */
    addr_lookup[13] += 1;
    add_buf_size = 16;
    prefix_len = _get_prefix_bits_num(addr_dst, strlen(addr_dst));

    fib_add_entry(&test_fib_table, 42, (uint8_t *)addr_dst,
                  add_buf_size - 1, ((prefix_len << FIB_FLAG_NET_PREFIX_SHIFT) | 0x123),
                  (uint8_t *)addr_nxt, add_buf_size -
                  1, 0x23, 100000);

    memset(addr_nxt, 0, add_buf_size);

    ret = fib_get_next_hop(&test_fib_table, &iface_id,
                             (uint8_t *)addr_nxt, &add_buf_size, &next_hop_flags,
                             (uint8_t *)addr_lookup, add_buf_size - 1, 0x123);

    TEST_ASSERT_EQUAL_INT(-EHOSTUNREACH, ret);

    /* test success (again) by adjusting the lsb */
    addr_lookup[14] = (char)0x3e; /* 0011 1110 */
    addr_lookup[13] -= 1;
    add_buf_size = 16;

    memset(addr_nxt, 0, add_buf_size);

    ret = fib_get_next_hop(&test_fib_table, &iface_id,
                             (uint8_t *)addr_nxt, &add_buf_size, &next_hop_flags,
                             (uint8_t *)addr_lookup, add_buf_size - 1, 0x123);

    TEST_ASSERT_EQUAL_INT(0, ret);

#if (TEST_FIB_SHOW_OUTPUT == 1)
    fib_print_fib_table(&test_fib_table);
    puts("");
    universal_address_print_table();
    puts("");
#endif
    fib_deinit(&test_fib_table);
}

/*
* @brief testing receiving an destination address set matching a specific prefix
*/
static void test_fib_17_get_entry_set(void)
{
    /* FIXME: init as enum to fix folding-constant compiler error on macOS */
    enum { addr_buf_size = 16 };
    char addr_dst[addr_buf_size];
    char addr_nxt[addr_buf_size];

    /* fill 20 addresses */
    for (unsigned i = 0; i < 20; ++i) {
        /* construct "addresses" for the FIB */
        _set_fib_test_addr(addr_dst, addr_buf_size, i);
        _set_fib_test_addr(addr_nxt, addr_buf_size, i % 11);
        fib_add_entry(&test_fib_table, 42,
                      (uint8_t *)addr_dst, addr_buf_size - 1, 0x0,
                      (uint8_t *)addr_nxt, addr_buf_size - 1, 0x0, 100000);
    }

    size_t arr_size = 20;
    fib_destination_set_entry_t arr_dst[arr_size];
    char prefix[addr_buf_size];
    memset(prefix, 0, addr_buf_size);
    /* cppcheck-suppress redundantCopy
     * (reason: prefix is set to all 0 before adding an address) */
    snprintf(prefix, addr_buf_size, "Test address 1");

    int ret = fib_get_destination_set(&test_fib_table,
                                      (uint8_t *)prefix, addr_buf_size-1,
                                      &arr_dst[0], &arr_size);
    TEST_ASSERT_EQUAL_INT(0, ret);

    /* we should receive 10 entries 10 to 19 */
    TEST_ASSERT_EQUAL_INT(10, arr_size);
    arr_size = 20;

    memset(prefix, 0, addr_buf_size);
    /* cppcheck-suppress redundantCopy
     * (reason: prefix is set to all 0 before adding an address) */
    snprintf(prefix, addr_buf_size, "Test address 0");

    ret = fib_get_destination_set(&test_fib_table,
                                  (uint8_t *)prefix, addr_buf_size - 1,
                                  &arr_dst[0], &arr_size);
    TEST_ASSERT_EQUAL_INT(0, ret);

    /* we should receive 20 entries 0-19 */
    TEST_ASSERT_EQUAL_INT(20, arr_size);
    arr_size = 20;

    memset(prefix, 0, addr_buf_size);
    /* cppcheck-suppress redundantCopy
     * (reason: prefix is set to all 0 before adding an address) */
    snprintf(prefix, addr_buf_size, "Test address");

    ret = fib_get_destination_set(&test_fib_table,
                                  (uint8_t *)prefix, addr_buf_size - 1,
                                  &arr_dst[0], &arr_size);
    TEST_ASSERT_EQUAL_INT(0, ret);

    /* we should receive 20 entries 0-19 */
    TEST_ASSERT_EQUAL_INT(20, arr_size);

#if (TEST_FIB_SHOW_OUTPUT == 1)
    puts("");
    for (unsigned i = 0; i < arr_size; ++i) {
        for (unsigned j = 0; j < arr_dst[i].dest_size; ++j) {
            printf("%c", (char)arr_dst[i].dest[j]);
        }
        puts("");
    }
#endif

    fib_deinit(&test_fib_table);
}

/*
* @brief call get next hop with invalid parameters
* It is expected to receive -EINVAL on calling get_next_hop()
*/
static void test_fib_18_get_next_hop_invalid_parameters(void)
{
    size_t add_buf_size = 16; /* includes space for terminating \0 */
    char addr_dst[] = "Test address 13";
    char addr_expect[] = "Test address 02";
    kernel_pid_t iface_id = KERNEL_PID_UNDEF;
    uint32_t next_hop_flags = 0;
    char addr_nxt[add_buf_size];

    unsigned entries = 20;
    _fill_fib_multiple(entries, 11);

    int ret = fib_get_next_hop(&test_fib_table, NULL, NULL,
                               NULL, NULL, NULL, add_buf_size - 1, 0x13);

    TEST_ASSERT_EQUAL_INT(-EINVAL, ret);

    ret = fib_get_next_hop(&test_fib_table, &iface_id,
                           (uint8_t *)addr_nxt, &add_buf_size, &next_hop_flags,
                           (uint8_t *)addr_dst, add_buf_size - 1, 0x13);

    TEST_ASSERT_EQUAL_INT(0, ret);

    ret = strncmp(addr_expect, addr_nxt, add_buf_size);
    TEST_ASSERT_EQUAL_INT(0, ret);

#if (TEST_FIB_SHOW_OUTPUT == 1)
    fib_print_fib_table(&test_fib_table);
    puts("");
    universal_address_print_table();
    puts("");
#endif
    fib_deinit(&test_fib_table);
}

/*
* @brief testing default gateway address
*/
static void test_fib_19_default_gateway(void)
{
    size_t add_buf_size = 16;
    char addr_dst[add_buf_size];
    char addr_nxt_hop[add_buf_size];
    char addr_nxt[add_buf_size];
    char addr_lookup[add_buf_size];
    kernel_pid_t iface_id = KERNEL_PID_UNDEF;
    uint32_t next_hop_flags = 0;

    memset(addr_dst, 0, add_buf_size);
    memset(addr_nxt, 0, add_buf_size);
    memset(addr_nxt_hop, 0, add_buf_size);
    memset(addr_lookup, 0, add_buf_size);

    snprintf(addr_lookup, add_buf_size, "Some address X1");

    /* set the bytes to 0x01..0x10 of the next-hop */
    for (unsigned i = 0; i < add_buf_size; i++) {
        addr_nxt[i] = i+1;
    }

    /* add a default gateway entry */
    fib_add_entry(&test_fib_table, 42, (uint8_t *)addr_dst,
                  add_buf_size, 0x123,
                  (uint8_t *)addr_nxt, add_buf_size, 0x23,
                  100000);

    /* check if it matches all */
    int ret = fib_get_next_hop(&test_fib_table, &iface_id,
                               (uint8_t *)addr_nxt_hop, &add_buf_size,
                               &next_hop_flags, (uint8_t *)addr_lookup,
                               add_buf_size, 0x123);

    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(0, memcmp(addr_nxt, addr_nxt_hop, add_buf_size));

    memset(addr_nxt_hop, 0, add_buf_size);

    /* set the bytes to 0x02..0x11 of the new next-hop for the default gateway */
    for (unsigned i = 0; i < add_buf_size; ++i) {
        addr_nxt[i] = i+2;
    }

    /* change the default gateway entry */
    fib_add_entry(&test_fib_table, 42, (uint8_t *)addr_dst,
                  add_buf_size, 0x123, (uint8_t *)addr_nxt, add_buf_size, 0x24,
                  100000);

    /* and check again if it matches all */
    ret = fib_get_next_hop(&test_fib_table, &iface_id,
                           (uint8_t *)addr_nxt_hop, &add_buf_size, &next_hop_flags,
                           (uint8_t *)addr_lookup, add_buf_size, 0x123);

    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(0, memcmp(addr_nxt, addr_nxt_hop, add_buf_size));

#if (TEST_FIB_SHOW_OUTPUT == 1)
    fib_print_fib_table(&test_fib_table);
    puts("");
    universal_address_print_table();
    puts("");
#endif
    fib_deinit(&test_fib_table);
}

/*
* @brief testing prefix entry changing
*/
static void test_fib_20_replace_prefix(void)
{
    size_t add_buf_size = 16;
    char addr_dst[add_buf_size];
    char addr_nxt_hop[add_buf_size];
    char addr_nxt[add_buf_size];
    char addr_lookup[add_buf_size];
    kernel_pid_t iface_id = KERNEL_PID_UNDEF;
    uint32_t next_hop_flags = 0;

    memset(addr_dst, 0, add_buf_size);
    memset(addr_nxt, 0, add_buf_size);
    memset(addr_nxt_hop, 0, add_buf_size);
    memset(addr_lookup, 0, add_buf_size);

    /* set the bytes to 0x01..0x10 of the next-hop */
    for (unsigned i = 0; i < add_buf_size; i++) {
        addr_nxt[i] = i+1;
    }

    /* set the bytes to 0x01..0x08 of the destination prefix */
    for (unsigned i = 0; i < add_buf_size/2; i++) {
        addr_dst[i] = i+1;
    }

    /* set the bytes to 0x01..0x0e of the lookup address */
    for (unsigned i = 0; i < 14; i++) {
        addr_lookup[i] = i+1;
    }

    uint32_t prefix_len = _get_prefix_bits_num(addr_dst, strlen(addr_dst));
    /* add a prefix entry */
    fib_add_entry(&test_fib_table, 42, (uint8_t *)addr_dst,
                  add_buf_size, ((prefix_len << FIB_FLAG_NET_PREFIX_SHIFT) | 0x123),
                  (uint8_t *)addr_nxt, add_buf_size, 0x23,
                  100000);

    /* check if it matches */
    int ret = fib_get_next_hop(&test_fib_table, &iface_id,
                               (uint8_t *)addr_nxt_hop, &add_buf_size,
                               &next_hop_flags, (uint8_t *)addr_lookup,
                               add_buf_size, 0x123);

    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(0, memcmp(addr_nxt, addr_nxt_hop, add_buf_size));

    fib_remove_entry(&test_fib_table, (uint8_t *)addr_dst,
                     add_buf_size);

    memset(addr_nxt_hop, 0, add_buf_size);

    /* set the bytes to 0x02..0x11 of the new next-hop */
    for (unsigned i = 0; i < add_buf_size; ++i) {
        addr_nxt[i] = i+2;
    }

    /* set the bytes to 0x01..0x0d of the new destination prefix */
    for (unsigned i = 0; i < 13; i++) {
        addr_dst[i] = i+1;
    }

    prefix_len = _get_prefix_bits_num(addr_dst, strlen(addr_dst));
    /* change the prefix entry */
    fib_add_entry(&test_fib_table, 42, (uint8_t *)addr_dst,
                  add_buf_size, ((prefix_len << FIB_FLAG_NET_PREFIX_SHIFT) | 0x123),
                  (uint8_t *)addr_nxt, add_buf_size, 0x24,
                  100000);

    /* and check again if it matches  */
    ret = fib_get_next_hop(&test_fib_table, &iface_id,
                           (uint8_t *)addr_nxt_hop, &add_buf_size, &next_hop_flags,
                           (uint8_t *)addr_lookup, add_buf_size, 0x123);

    TEST_ASSERT_EQUAL_INT(0, ret);
    TEST_ASSERT_EQUAL_INT(0, memcmp(addr_nxt, addr_nxt_hop, add_buf_size));

#if (TEST_FIB_SHOW_OUTPUT == 1)
    fib_print_fib_table(&test_fib_table);
    puts("");
    universal_address_print_table();
    puts("");
#endif
    fib_deinit(&test_fib_table);
}

Test *tests_fib_tests(void)
{
    fib_init(&test_fib_table);
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
                        new_TestFixture(test_fib_16_prefix_match),
                        new_TestFixture(test_fib_17_get_entry_set),
                        new_TestFixture(test_fib_18_get_next_hop_invalid_parameters),
                        new_TestFixture(test_fib_19_default_gateway),
                        new_TestFixture(test_fib_20_replace_prefix),
    };

    EMB_UNIT_TESTCALLER(fib_tests, NULL, NULL, fixtures);

    return (Test *)&fib_tests;
}

void tests_fib(void)
{
    TESTS_RUN(tests_fib_tests());
}
