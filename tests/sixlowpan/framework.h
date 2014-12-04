/*
 * Copyright (C) 2014 Martin Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file        framework.h
 * @brief       Definition of prototypes for 6LoWPAN functionality tests
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */


#ifndef __FRAMEWORK_H_
#define __FRAMEWORK_H_

#include <stdio.h>
#include <stdint.h>

/**
 * @brief   @ref msg_t type to transfer sixlowpan_test_exp_rcv_t types.
 */
#define SIXLOWPAN_TEST_MSG_TYPE_EXP         (0x4521)

/**
 * @brief   Test status code to inform set setting an option
 *          lead to an error.
 */
#define SIXLOWPAN_TEST_SET_OPTION_ERROR     (0xfffa)

/**
 * @brief   Test status code to inform set setting the header compression status
 *          lead to an error.
 */
#define SIXLOWPAN_TEST_SET_HC_STATUS_ERROR  (0xfffb)

/**
 * @brief   Test status code to inform that no space was left to store all
 *          expected values.
 */
#define SIXLOWPAN_TEST_ENO_EXP_BUF          (0xfffc)

/**
 * @brief   Test status code for test receiver and mac to inform that received
 *          netapi command an expcected command.
 */
#define SIXLOWPAN_TEST_EUNEXPECTED_COMMAND  (0xfffd)

/**
 * @brief   Test status code to inform that the test results where not as
 *          expected
 */
#define SIXLOWPAN_TEST_EWRONG_TEST_RESULT   (0xfffe)

/**
 * @brief   Test status code that the packet buffer is not empty (which it
 *          should be after a test completed successfully)
 */
#define SIXLOWPAN_TEST_EPKTBUF_LEAK         (0xffff)

/**
 * @brief   Type to for test functions
 */
typedef uint32_t (*sixlowpan_test_t)(void);

/**
 * @brief   Type to communicate expected test data to the test mac thread
 */
typedef struct {
    netdev_hlist_t *ulh;/**< Expected headers of upper layers */
    void *dest;         /**< Expected destination address */
    size_t dest_len;    /**< Expected destination address length */
    void *data;         /**< Expected data */
    size_t data_len;    /**< Expected data length */
} sixlowpan_test_exp_snd_t;

/**
 * @brief   Type to communicate expected test data to the test receiver thread
 */
typedef struct {
    void *src;          /**< Expected source address */
    size_t src_len;     /**< Expected source address length */
    void *dest;         /**< Expected destination address */
    size_t dest_len;    /**< Expected destination address length */
    void *data;         /**< Expected data */
    size_t data_len;    /**< Expected data length */
} sixlowpan_test_exp_rcv_t;

/**
 * @brief   PID for sixlowpan in these tests.
 */
extern kernel_pid_t sixlowpan_test_sixlowpan_pid;

/**
 * @brief   PID for mockup L3 receiver in these tests.
 */
extern kernel_pid_t sixlowpan_test_receiver_pid;

/**
 * @brief   PID for mockup MAC layer in these tests.
 */
extern kernel_pid_t sixlowpan_test_mac_pid;

/**
 * @brief   Runs a test
 *
 * @brief[in] exp   expected return value for *test*
 * @brief[in] test  test to run
 * @brief[in] name  screen name for *test*
 *
 * @return  1 on success
 * @return  0 on failure
 */
int sixlowpan_test_run(uint32_t exp, sixlowpan_test_t test, char *name);

/**
 * @brief   Short-hand for sixlowpan_test_run()
 *
 * @detail  Aborts currently running caller on test failure and prints a dot
 *          (or the name of the function + "(success)" if TEXT is defined) on
 *          success
 *
 * @param[in] exp   expected return value for *test*
 * @param[in] test  test to run.
 */
#ifndef TEXT
#define SIXLOWPAN_TEST_RUN(exp, test) \
    if (!sixlowpan_test_run(exp, test, #test)) { \
        return 1; \
    } \
    else { \
        printf("."); \
    }
#else
#define SIXLOWPAN_TEST_RUN(exp, test) \
    printf("%s", #test); \
    if (!sixlowpan_test_run(exp, test, #test)) { \
        return 1; \
    } \
    else { \
        puts(" (success)"); \
    }
#endif

/**
 * @brief   Runs a test in interaction with the mockup test MAC layer
 *
 * @param[in] snd       Array of send commands to send to 6LoWPAN control thread
 * @param[in] num_snd   Number of receive commands in *snd*
 * @param[in] exp       Array of expected results received in test MAC layer
 * @param[in] num_exp   Number of expected results in *exp*
 *
 * @return 0 on success
 * @return test status code or errno >0 on error or failure.
 */
uint32_t sixlowpan_test_send_test(netapi_snd_pkt_t *rcv, int num_snd,
                                  sixlowpan_test_exp_snd_t *exp, int num_exp);

/**
 * @brief   Runs a test in interaction with the mockup test receiver
 *
 * @param[in] rcv       Array of receive commands to send to 6LoWPAN control thread
 * @param[in] num_rcv   Number of receive commands in *rcv*
 * @param[in] exp       Array of expected results received in test receiver
 * @param[in] num_exp   Number of expected results in *exp*
 *
 * @return 0 on success
 * @return test status code or errno >0 on error or failure.
 */
uint32_t sixlowpan_test_receiver_test(netapi_rcv_pkt_t *rcv, int num_rcv,
                                      sixlowpan_test_exp_rcv_t *exp, int num_exp);

/**
 * @brief   Initializes test framework
 */
void sixlowpan_test_init(void);

#endif /* __FRAMEWORK_H_ */
/**
 * @}
 */
