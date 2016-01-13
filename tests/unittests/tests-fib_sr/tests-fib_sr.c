/*
 * Copyright (C) 2015 Martin Landsmann
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdio.h> /**< required for snprintf() */
#include <string.h>
#include <errno.h>
#include "embUnit.h"
#include "tests-fib_sr.h"

#include "thread.h"
#include "net/fib.h"
#include "universal_address.h"

/**
 * @brief maximum number of source routes
 */
#define TEST_MAX_FIB_SR (20)

/**
 * @brief maximum number of entries in all source routes
 */
#define TEST_MAX_FIB_SR_ENTRIES (TEST_MAX_FIB_SR * 20)

/**
 * @brief the FIB source route headers
 */
static fib_sr_t _sr_headers[TEST_MAX_FIB_SR];

/**
 * @brief the FIB source route table entries pool
 */
static fib_sr_entry_t _sr_datapool[TEST_MAX_FIB_SR_ENTRIES];

/**
 * @brief the FIB source route table "glue container"
 */
static fib_sr_meta_t _entries_sr = { .headers = _sr_headers,
                                     .entry_pool = _sr_datapool,
                                     .entry_pool_size = TEST_MAX_FIB_SR_ENTRIES };

/**
 * @brief the FIB source route table
 * @note is initialized in the entry function of this unittest
 */
static fib_table_t test_fib_sr_table;

/*
 * @brief helper function to create source routes.
 *        The enrties are constructed with the given prefix and numbers
 *        counted from the given count borders (from and to).
 * @param[in] pre prefix string to use
 * @param[in] from starting index to count from
 * @param[in] to last index to count to
 * @param[in] sr the source route to append the created entries
 * @param[in] buf_size the size in bytes for each address to be created
 */
static int _create_sr(const char *pre, size_t from, size_t to, fib_sr_t *sr, size_t buf_size)
{
    char addr_nxt[buf_size];

    strncpy(addr_nxt, pre, strlen(pre));

    for (size_t i = from; i < to; ++i) {
        int res;
        snprintf(&(addr_nxt[strlen(pre)]), buf_size, "%d", (int)i);
        /* append a  hop */
        res = fib_sr_entry_append(&test_fib_sr_table, sr, (uint8_t *)&addr_nxt, buf_size);
        if (res != 0) {
            return res;
        }
    }
    return 0;
}


/*
 * @brief create a new empty source route and check the parameters
 * It is expected to have a new source route with the given parameters
 */
static void test_fib_sr_01_create_empty_sr(void)
{
    fib_sr_t *local_sourceroutes[1];

    TEST_ASSERT_EQUAL_INT(0, fib_sr_create(&test_fib_sr_table, &local_sourceroutes[0],
                                           42, 0x0, 10000));

    kernel_pid_t sr_iface_id;
    uint32_t sr_flags;
    uint32_t sr_lifetime;
    TEST_ASSERT_EQUAL_INT(0, fib_sr_read_head(&test_fib_sr_table, local_sourceroutes[0],
                                              &sr_iface_id, &sr_flags,
                                              &sr_lifetime)
                          );

    TEST_ASSERT_EQUAL_INT(42, sr_iface_id);
    TEST_ASSERT_EQUAL_INT(0x0, sr_flags);

    /* the lifetime should decrease
     * Since the FIB uses timex under the hood which has a finer grained resolution
     * testing the lifetime requires to active wait a bit, say 5 ms.
     * Uncomment the following two lines if you want to test the temporal behaviour
     * @note this may fail since unittests are currently not thread-friendly
     */
    //xtimer_usleep(5 * MS_IN_USEC);
    //TEST_ASSERT(sr_lifetime<10000);

    fib_deinit(&test_fib_sr_table);
}

/*
 * @brief create a new empty source route, change its parameters
 * and check the change
 * It is expected to have a new source route with the given adjusted parameters
 */
static void test_fib_sr_02_change_sr_parameters(void)
{
    fib_sr_t *local_sourceroutes[1];

    TEST_ASSERT_EQUAL_INT(0, fib_sr_create(&test_fib_sr_table, &local_sourceroutes[0],
                                           42, 0x0, 10000));

    /* Case 01 - re-set all parameters */
    kernel_pid_t sr_iface_id = 43;
    uint32_t sr_flags = 0x99;
    uint32_t sr_lifetime = 20000;
    TEST_ASSERT_EQUAL_INT(0, fib_sr_set(&test_fib_sr_table, local_sourceroutes[0],
                                        &sr_iface_id, &sr_flags, &sr_lifetime));
    TEST_ASSERT_EQUAL_INT(0, fib_sr_read_head(&test_fib_sr_table, local_sourceroutes[0],
                                              &sr_iface_id, &sr_flags,
                                              &sr_lifetime)
                          );

    TEST_ASSERT_EQUAL_INT(43, sr_iface_id);
    TEST_ASSERT_EQUAL_INT(0x99, sr_flags);

    /* the lifetime should be slightly below 20000
     * Since the FIB uses timex under the hood which has a finer grained resolution
     * testing the lifetime requires to active wait a bit, say 5 ms.
     * Uncomment the following three lines if you want to test the temporal behaviour
     * @note this may fail since unittests are currently not thread-friendly
     */
    //xtimer_usleep(5 * MS_IN_USEC);
    //TEST_ASSERT(sr_lifetime>10000);
    //TEST_ASSERT(sr_lifetime<20000);

    fib_deinit(&test_fib_sr_table);
}

/*
 * @brief check errors on reading sr parameters and trying to access an expired sr
 */
static void test_fib_sr_03_read_sr_parameters(void)
{
    fib_sr_t *local_sourceroutes[1];

    TEST_ASSERT_EQUAL_INT(0, fib_sr_create(&test_fib_sr_table, &local_sourceroutes[0],
                                           42, 0x0, 10000));

    kernel_pid_t sr_iface_id;
    uint32_t sr_flags;
    uint32_t sr_lifetime;
    TEST_ASSERT_EQUAL_INT(-EFAULT, fib_sr_read_head(&test_fib_sr_table, NULL,
                                                    &sr_iface_id, &sr_flags,
                                                    &sr_lifetime)
                          );
    TEST_ASSERT_EQUAL_INT(-EFAULT, fib_sr_read_head(&test_fib_sr_table, local_sourceroutes[0],
                                                    NULL, &sr_flags,
                                                    &sr_lifetime)
                          );
    /* we set the lifetime to 1 ms and expect it to expire */
    sr_lifetime = 1;
    TEST_ASSERT_EQUAL_INT(0, fib_sr_set(&test_fib_sr_table, local_sourceroutes[0],
                                        &sr_iface_id, &sr_flags, &sr_lifetime)
                          );
    /*
     * Since the FIB uses timex under the hood which has a finer grained resolution
     * testing the lifetime requires to active wait a bit, say 1 ms.
     * Uncomment the following lines if you want to test the temporal behaviour
     * @note this may fail since unittests are currently not thread-friendly
     */
    /*
       xtimer_usleep(1 * MS_IN_USEC);
       TEST_ASSERT_EQUAL_INT(-ENOENT, fib_sr_read_head(&test_fib_sr_table, local_sourceroutes[0],
                                                    &sr_iface_id, &sr_flags,
                                                    &sr_lifetime)
                          );
     */

    fib_deinit(&test_fib_sr_table);
}

/*
 * @brief create a new empty source route and delete it
 * It is expected to have no source route
 */
static void test_fib_sr_04_create_and_delete_sr(void)
{
    fib_sr_t *local_sourceroutes[1];

    TEST_ASSERT_EQUAL_INT(0, fib_sr_create(&test_fib_sr_table, &local_sourceroutes[0],
                                           42, 0x0, 10000));

    /* test wrong parameter */
    TEST_ASSERT_EQUAL_INT(-EFAULT, fib_sr_delete(&test_fib_sr_table, NULL));


    TEST_ASSERT_EQUAL_INT(0, fib_sr_delete(&test_fib_sr_table, local_sourceroutes[0]));

    kernel_pid_t sr_iface_id;
    uint32_t sr_flags;
    uint32_t sr_lifetime;
    TEST_ASSERT_EQUAL_INT(-ENOENT, fib_sr_read_head(&test_fib_sr_table, local_sourceroutes[0],
                                                    &sr_iface_id, &sr_flags,
                                                    &sr_lifetime)
                          );

    fib_deinit(&test_fib_sr_table);
}

/*
 * @brief create a new empty source route and append two hops on its path
 */
static void test_fib_sr_05_create_sr_and_append_hop(void)
{
    fib_sr_t *local_sourceroutes[1];
    size_t add_buf_size = 16;
    char addr_nxt[add_buf_size];

    snprintf(addr_nxt, add_buf_size, "Some address X1");

    TEST_ASSERT_EQUAL_INT(0, fib_sr_create(&test_fib_sr_table, &local_sourceroutes[0],
                                           42, 0x0, 10000));

    /* append a first hop */
    TEST_ASSERT_EQUAL_INT(0, fib_sr_entry_append(&test_fib_sr_table, local_sourceroutes[0],
                                                 (uint8_t *)&addr_nxt,
                                                 add_buf_size)
                          );

    snprintf(addr_nxt, add_buf_size, "Some address X2");
    /* append a hop behind the first hop */
    TEST_ASSERT_EQUAL_INT(0, fib_sr_entry_append(&test_fib_sr_table, local_sourceroutes[0],
                                                 (uint8_t *)&addr_nxt,
                                                 add_buf_size)
                          );

    /* the destination should be now X2 */
    char addr_out[add_buf_size];
    TEST_ASSERT_EQUAL_INT(0, fib_sr_read_destination(&test_fib_sr_table, local_sourceroutes[0],
                                                     (uint8_t *)&addr_out,
                                                     &add_buf_size)
                          );

    TEST_ASSERT_EQUAL_INT(0, memcmp(addr_nxt, addr_out, add_buf_size));

    /* try to append the address twice */
    TEST_ASSERT_EQUAL_INT(-EINVAL, fib_sr_entry_append(&test_fib_sr_table, local_sourceroutes[0],
                                                       (uint8_t *)&addr_nxt,
                                                       add_buf_size)
                          );

    /* remove the source route */
    TEST_ASSERT_EQUAL_INT(0, fib_sr_delete(&test_fib_sr_table, local_sourceroutes[0]));

    fib_deinit(&test_fib_sr_table);
}

/*
 * @brief create a new source route with a number of hops on its path
 *        Search entry and add a new entry after the found one
 */
static void test_fib_sr_06_create_sr_with_hops(void)
{
    fib_sr_t *local_sourceroutes[1];
    size_t add_buf_size = 16;
    char addr_nxt[add_buf_size];

    TEST_ASSERT_EQUAL_INT(0, fib_sr_create(&test_fib_sr_table, &local_sourceroutes[0],
                                           42, 0x0, 10000));

    TEST_ASSERT_EQUAL_INT(0, _create_sr("Some address X", 0, 10, local_sourceroutes[0], 16));

    /* now we search for a specific entry, e.g. X6 */
    fib_sr_entry_t *sr_path_entry[1];
    snprintf(addr_nxt, add_buf_size, "Some address X6");

    TEST_ASSERT_EQUAL_INT(0, fib_sr_search(&test_fib_sr_table, local_sourceroutes[0],
                                           (uint8_t *)&addr_nxt, add_buf_size,
                                           &sr_path_entry[0])
                          );

    /* and move on to the next entry (just because we can) */
    TEST_ASSERT_EQUAL_INT(0, fib_sr_next(&test_fib_sr_table, local_sourceroutes[0],
                                         &sr_path_entry[0]));

    /* now we add a new entry behind sr_path_entry[0] and drop the remaining route */
    snprintf(addr_nxt, add_buf_size, "Some address XY");
    TEST_ASSERT_EQUAL_INT(0, fib_sr_entry_add(&test_fib_sr_table, local_sourceroutes[0],
                                              sr_path_entry[0],
                                              (uint8_t *)&addr_nxt,
                                              add_buf_size, false)
                          );

    /* so entries X8 and X9 should be gone */
    snprintf(addr_nxt, add_buf_size, "Some address X8");
    TEST_ASSERT_EQUAL_INT(-EHOSTUNREACH, fib_sr_search(&test_fib_sr_table, local_sourceroutes[0],
                                                       (uint8_t *)&addr_nxt, add_buf_size,
                                                       &sr_path_entry[0])
                          );
    snprintf(addr_nxt, add_buf_size, "Some address X9");
    TEST_ASSERT_EQUAL_INT(-EHOSTUNREACH, fib_sr_search(&test_fib_sr_table, local_sourceroutes[0],
                                                       (uint8_t *)&addr_nxt, add_buf_size,
                                                       &sr_path_entry[0])
                          );

    /* now we go to a earlier entry and add a new entry not dropping the remaining ones */
    snprintf(addr_nxt, add_buf_size, "Some address X2");

    TEST_ASSERT_EQUAL_INT(0, fib_sr_search(&test_fib_sr_table, local_sourceroutes[0],
                                           (uint8_t *)&addr_nxt, add_buf_size,
                                           &sr_path_entry[0])
                          );

    snprintf(addr_nxt, add_buf_size, "Some address XZ");
    TEST_ASSERT_EQUAL_INT(0, fib_sr_entry_add(&test_fib_sr_table, local_sourceroutes[0],
                                              sr_path_entry[0],
                                              (uint8_t *)&addr_nxt,
                                              add_buf_size, true)
                          );

    /*
     * @note uncomment the following line to print the source route.
     * expected X0, .., X2, XZ, X3, .., X7, XY
     */
    /* fib_print_sr( local_sourceroutes[0]); */

    /* remove the source route */
    TEST_ASSERT_EQUAL_INT(0, fib_sr_delete(&test_fib_sr_table, local_sourceroutes[0]));

    fib_deinit(&test_fib_sr_table);
}


/*
 * @brief create a new source route with a number of hops on its path,
 *        and delete several entries
 */
static void test_fib_sr_07_create_sr_with_hops_and_delete_one(void)
{
    fib_sr_t *local_sourceroutes[1];
    size_t add_buf_size = 16;
    char addr_nxt[add_buf_size];

    TEST_ASSERT_EQUAL_INT(0, fib_sr_create(&test_fib_sr_table, &local_sourceroutes[0],
                                           42, 0x0, 10000));

    TEST_ASSERT_EQUAL_INT(0, _create_sr("Some address X", 0, 10, local_sourceroutes[0], 16));

    snprintf(addr_nxt, add_buf_size, "Some address X6");
    /* we delete X6 keeping the remaining entries */
    TEST_ASSERT_EQUAL_INT(0, fib_sr_entry_delete(&test_fib_sr_table, local_sourceroutes[0],
                                                 (uint8_t *)&addr_nxt, add_buf_size,
                                                 true)
                          );

    snprintf(addr_nxt, add_buf_size, "Some address X9");
    /* now the destination */
    TEST_ASSERT_EQUAL_INT(0, fib_sr_entry_delete(&test_fib_sr_table, local_sourceroutes[0],
                                                 (uint8_t *)&addr_nxt, add_buf_size,
                                                 true)
                          );

    snprintf(addr_nxt, add_buf_size, "Some address X0");
    /* and now the first hop on the path */
    TEST_ASSERT_EQUAL_INT(0, fib_sr_entry_delete(&test_fib_sr_table, local_sourceroutes[0],
                                                 (uint8_t *)&addr_nxt, add_buf_size,
                                                 true)
                          );

    /*
     * @note uncomment the following line to print the source route.
     * expected X1, .., X5, X7, .., X8
     */
    /* fib_print_sr( local_sourceroutes[0]); */

    /* remove the source route */
    TEST_ASSERT_EQUAL_INT(0, fib_sr_delete(&test_fib_sr_table, local_sourceroutes[0]));

    fib_deinit(&test_fib_sr_table);
}

/*
 * @brief create a new source route with a number of hops on its path,
 *        and overwrite the address of one entry
 */
static void test_fib_sr_08_create_sr_with_hops_and_overwrite_one(void)
{
    fib_sr_t *local_sourceroutes[1];
    size_t add_buf_size = 16;
    char addr_nxt[add_buf_size];
    char addr_exc[add_buf_size];

    TEST_ASSERT_EQUAL_INT(0, fib_sr_create(&test_fib_sr_table, &local_sourceroutes[0],
                                           42, 0x0, 10000));

    TEST_ASSERT_EQUAL_INT(0, _create_sr("Some address X", 0, 10, local_sourceroutes[0], 16));

    /* now we search for a specific entry, e.g. X6 */
    snprintf(addr_nxt, add_buf_size, "Some address X6");
    snprintf(addr_exc, add_buf_size, "Some address XY");

    TEST_ASSERT_EQUAL_INT(0, fib_sr_entry_overwrite(&test_fib_sr_table, local_sourceroutes[0],
                                                    (uint8_t *)&addr_nxt, add_buf_size,
                                                    (uint8_t *)&addr_exc, add_buf_size)
                          );
    /*
     * @note uncomment the following line to print the source route.
     * expected X0, .., X5, XY, X7, .., X9
     */
    /* fib_print_sr( local_sourceroutes[0]); */

    /* remove the source route */
    TEST_ASSERT_EQUAL_INT(0, fib_sr_delete(&test_fib_sr_table, local_sourceroutes[0]));

    fib_deinit(&test_fib_sr_table);
}

/*
 * @brief create a new source route with a number of hops on its path,
 *        search an entry and copy the address to the caller
 */
static void test_fib_sr_09_create_sr_with_hops_and_get_one(void)
{
    fib_sr_t *local_sourceroutes[1];
    size_t add_buf_size = 16;
    char addr_nxt[add_buf_size];
    char addr_out[add_buf_size];

    TEST_ASSERT_EQUAL_INT(0, fib_sr_create(&test_fib_sr_table, &local_sourceroutes[0],
                                           42, 0x0, 10000));

    TEST_ASSERT_EQUAL_INT(0, _create_sr("Some address X", 0, 10, local_sourceroutes[0], 16));

    /* first we have an empty iterator */
    fib_sr_entry_t *sr_path_entry[1];
    sr_path_entry[0] = NULL;

    /* now we move on to a specific hop, i.e. the 6th one (X5) */
    for (size_t i = 0; i < 6; ++i) {
        TEST_ASSERT_EQUAL_INT(0, fib_sr_next(&test_fib_sr_table, local_sourceroutes[0],
                                             &sr_path_entry[0]));
    }

    TEST_ASSERT_EQUAL_INT(0, fib_sr_entry_get_address(&test_fib_sr_table, local_sourceroutes[0],
                                                      sr_path_entry[0],
                                                      (uint8_t *)&addr_out, &add_buf_size)
                          );

    /* we ecpect the X5 address */
    snprintf(addr_nxt, add_buf_size, "Some address X5");
    TEST_ASSERT_EQUAL_INT(0, memcmp(addr_nxt, addr_out, add_buf_size));

    /*
     * @note uncomment the following line to print the source route.
     */
    /* fib_print_sr( local_sourceroutes[0]); */

    /* remove the source route */
    TEST_ASSERT_EQUAL_INT(0, fib_sr_delete(&test_fib_sr_table, local_sourceroutes[0]));

    fib_deinit(&test_fib_sr_table);
}

/*
 * @brief create a new source route with a number of hops on its path,
 *        and get it (also in reverse order)
 */
static void test_fib_sr_10_create_sr_with_hops_and_get_a_route(void)
{
    fib_sr_t *local_sourceroutes[1];
    size_t add_buf_size = 16;
    char addr_nxt[add_buf_size];

    TEST_ASSERT_EQUAL_INT(0, fib_sr_create(&test_fib_sr_table, &local_sourceroutes[0],
                                           42, 0x0, 10000));

    TEST_ASSERT_EQUAL_INT(0, _create_sr("Some address X", 0, 10, local_sourceroutes[0], 16));

    size_t addr_list_elements = 10;
    size_t element_size = 16;
    uint8_t addr_list[ addr_list_elements * element_size ];
    kernel_pid_t sr_iface_id;
    uint32_t sr_flags = 0x0;
    snprintf(addr_nxt, add_buf_size, "Some address X9");

    TEST_ASSERT_EQUAL_INT(0, fib_sr_get_route(&test_fib_sr_table, (uint8_t *)&addr_nxt,
                                              add_buf_size, &sr_iface_id, &sr_flags,
                                              addr_list, &addr_list_elements, &element_size,
                                              false, NULL)
                          );

    /*
     * @note uncomment the following lines to print the source route and the result.
     * expected X0, .., X9
     */
    /*
       fib_print_sr( local_sourceroutes[0]);

       for (size_t i = 0; i < addr_list_elements; ++i) {
        printf("[");
        for (size_t j = 0; j < element_size; j++) {
            printf("%c", addr_list[(i*element_size)+j]);
        }printf("] ");
       }puts("");
     */

    /* now we get the addresses in reverse order */
    TEST_ASSERT_EQUAL_INT(0, fib_sr_get_route(&test_fib_sr_table, (uint8_t *)&addr_nxt,
                                              add_buf_size, &sr_iface_id, &sr_flags,
                                              addr_list, &addr_list_elements, &element_size,
                                              true, NULL)
                          );

    /*
     * @note uncomment the following lines to print the source route and the result.
     * expected X0, .., X9
     */
    /*
       fib_print_sr( local_sourceroutes[0]);

       for (size_t i = 0; i < addr_list_elements; ++i) {
        printf("[");
        for (size_t j = 0; j < element_size; j++) {
            printf("%c", addr_list[(i*element_size)+j]);
        }printf("] ");
       }puts("");
     */

    /* remove the source route */
    TEST_ASSERT_EQUAL_INT(0, fib_sr_delete(&test_fib_sr_table, local_sourceroutes[0]));

    fib_deinit(&test_fib_sr_table);
}

/*
 * @brief create a new source route with a number of hops on its path,
 *        and get a partial route (also in reverse order)
 */
static void test_fib_sr_11_create_sr_with_hops_and_get_a_partial_route(void)
{
    fib_sr_t *local_sourceroutes[1];
    size_t add_buf_size = 16;
    char addr_nxt[add_buf_size];

    TEST_ASSERT_EQUAL_INT(0, fib_sr_create(&test_fib_sr_table, &local_sourceroutes[0],
                                           42, 0x0, 10000));

    TEST_ASSERT_EQUAL_INT(0, _create_sr("Some address X", 0, 10, local_sourceroutes[0], 16));

    size_t addr_list_elements = 10;
    size_t element_size = 16;
    uint8_t addr_list[ addr_list_elements * element_size ];
    kernel_pid_t sr_iface_id;
    uint32_t sr_flags = 0x0;
    snprintf(addr_nxt, add_buf_size, "Some address X7");

    TEST_ASSERT_EQUAL_INT(0, fib_sr_get_route(&test_fib_sr_table, (uint8_t *)&addr_nxt,
                                              add_buf_size, &sr_iface_id, &sr_flags,
                                              addr_list, &addr_list_elements, &element_size,
                                              false, NULL)
                          );

    /*
     * @note uncomment the following lines to print the resulting source route.
     * expected X0..X9 where X0..X7 has usecount of 2
     */
    /*
       fib_print_sr( local_sourceroutes[0]);

       for (size_t i = 0; i < addr_list_elements; ++i) {
        printf("[");
        for (size_t j = 0; j < element_size; j++) {
            printf("%c", addr_list[(i*element_size)+j]);
        }printf("] ");
       }puts("");
     */

    /* now we get the addresses in reverse order */
    TEST_ASSERT_EQUAL_INT(0, fib_sr_get_route(&test_fib_sr_table, (uint8_t *)&addr_nxt,
                                              add_buf_size, &sr_iface_id, &sr_flags,
                                              addr_list, &addr_list_elements, &element_size,
                                              true, NULL)
                          );

    /*
     * @note uncomment the following lines to print the resulting source route.
     */
    /*
       for (size_t i = 0; i < addr_list_elements; ++i) {
        printf("[");
        for (size_t j = 0; j < element_size; j++) {
            printf("%c", addr_list[(i*element_size)+j]);
        }printf("] ");
       }puts("");
     */

    /* remove the source route */
    TEST_ASSERT_EQUAL_INT(0, fib_sr_delete(&test_fib_sr_table, local_sourceroutes[0]));

    fib_deinit(&test_fib_sr_table);
}

/*
 * @brief create new source routes and get all routes succesively
 */
static void test_fib_sr_12_get_consecutive_sr(void)
{
    fib_sr_t *local_sourceroutes[3];
    size_t add_buf_size = 16;
    char addr_nxt[add_buf_size];

    /* Create SR1 X0,.., X9,XX  */
    TEST_ASSERT_EQUAL_INT(0, fib_sr_create(&test_fib_sr_table, &local_sourceroutes[0],
                                           42, 0x0, 10000));

    TEST_ASSERT_EQUAL_INT(0, _create_sr("Some address X", 0, 10, local_sourceroutes[0], 16));

    snprintf(addr_nxt, add_buf_size, "Some address XX");

    /* append a the last hop, i.e. the destination */
    TEST_ASSERT_EQUAL_INT(0, fib_sr_entry_append(&test_fib_sr_table, local_sourceroutes[0],
                                                 (uint8_t *)&addr_nxt,
                                                 add_buf_size)
                          );

    /* Create SR1 Y1,.., Y7,XX  */
    TEST_ASSERT_EQUAL_INT(0, fib_sr_create(&test_fib_sr_table, &local_sourceroutes[1],
                                           42, 0x0, 10000));

    TEST_ASSERT_EQUAL_INT(0, _create_sr("Some address Y", 1, 8, local_sourceroutes[1], 16));

    snprintf(addr_nxt, add_buf_size, "Some address XX");
    /* append a the last hop, i.e. the destination */
    TEST_ASSERT_EQUAL_INT(0, fib_sr_entry_append(&test_fib_sr_table, local_sourceroutes[1],
                                                 (uint8_t *)&addr_nxt,
                                                 add_buf_size)
                          );


    /* Create SR1 Z5,.., Z7,XX  */
    TEST_ASSERT_EQUAL_INT(0, fib_sr_create(&test_fib_sr_table, &local_sourceroutes[2],
                                           42, 0x0, 10000));

    TEST_ASSERT_EQUAL_INT(0, _create_sr("Some address Z", 5, 8, local_sourceroutes[2], 16));

    snprintf(addr_nxt, add_buf_size, "Some address XX");
    /* append a the last hop, i.e. the destination */
    TEST_ASSERT_EQUAL_INT(0, fib_sr_entry_append(&test_fib_sr_table, local_sourceroutes[2],
                                                 (uint8_t *)&addr_nxt,
                                                 add_buf_size)
                          );

    fib_sr_t *iter[1];
    iter[0] = NULL;
    size_t addr_list_elements = 11;
    size_t element_size = 16;
    uint8_t addr_list[ addr_list_elements * element_size ];
    kernel_pid_t sr_iface_id;
    uint32_t sr_flags = 0x0;
    snprintf(addr_nxt, add_buf_size, "Some address XX");

    TEST_ASSERT_EQUAL_INT(0, fib_sr_get_route(&test_fib_sr_table, (uint8_t *)&addr_nxt,
                                              add_buf_size, &sr_iface_id, &sr_flags,
                                              addr_list, &addr_list_elements, &element_size,
                                              false, iter)
                          );

    TEST_ASSERT_EQUAL_INT(11, addr_list_elements);

    TEST_ASSERT_EQUAL_INT(0, fib_sr_get_route(&test_fib_sr_table, (uint8_t *)&addr_nxt,
                                              add_buf_size, &sr_iface_id, &sr_flags,
                                              addr_list, &addr_list_elements, &element_size,
                                              false, iter)
                          );

    TEST_ASSERT_EQUAL_INT(8, addr_list_elements);

    TEST_ASSERT_EQUAL_INT(0, fib_sr_get_route(&test_fib_sr_table, (uint8_t *)&addr_nxt,
                                              add_buf_size, &sr_iface_id, &sr_flags,
                                              addr_list, &addr_list_elements, &element_size,
                                              false, iter)
                          );
    TEST_ASSERT_EQUAL_INT(4, addr_list_elements);

    fib_deinit(&test_fib_sr_table);
}

Test *tests_fib_sr_tests(void)
{
    test_fib_sr_table.data.source_routes = &_entries_sr;
    test_fib_sr_table.table_type = FIB_TABLE_TYPE_SR;
    test_fib_sr_table.size = TEST_MAX_FIB_SR;
    mutex_init(&(test_fib_sr_table.mtx_access));
    test_fib_sr_table.notify_rp_pos = 0;

    fib_init(&test_fib_sr_table);
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_fib_sr_01_create_empty_sr),
        new_TestFixture(test_fib_sr_02_change_sr_parameters),
        new_TestFixture(test_fib_sr_03_read_sr_parameters),
        new_TestFixture(test_fib_sr_04_create_and_delete_sr),
        new_TestFixture(test_fib_sr_05_create_sr_and_append_hop),
        new_TestFixture(test_fib_sr_06_create_sr_with_hops),
        new_TestFixture(test_fib_sr_07_create_sr_with_hops_and_delete_one),
        new_TestFixture(test_fib_sr_08_create_sr_with_hops_and_overwrite_one),
        new_TestFixture(test_fib_sr_09_create_sr_with_hops_and_get_one),
        new_TestFixture(test_fib_sr_10_create_sr_with_hops_and_get_a_route),
        new_TestFixture(test_fib_sr_11_create_sr_with_hops_and_get_a_partial_route),
        new_TestFixture(test_fib_sr_12_get_consecutive_sr),
    };

    EMB_UNIT_TESTCALLER(fib_sr_tests, NULL, NULL, fixtures);

    return (Test *)&fib_sr_tests;
}

void tests_fib_sr(void)
{
    TESTS_RUN(tests_fib_sr_tests());
}
