/*
 * Copyright (C) 2014 Freie Universität Berlin.
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @file    testing.h
 * @brief   Test suite for the 6LoWPAN Border Router.
 *
 *          The tests are managed in the following way:
 *          Packets (ICMPv6 Echo Request) are send onto the sensor node
 *          via the serial interface. The user can then decide what she
 *          wants to measure by calling testing_start() to start the
 *          measuring and testing_stop() to end it.
 *
 * @author  Martin Lenders <mlenders@inf.fu-berlin.de>
 */

#ifndef TESTING_H
#define TESTING_H

#include <stdint.h>

/**
 * @brief   Defines the format to store test results.
 */
struct test_stats {
    uint8_t seq_num;    ///< Sequence number of the send packet.
    /**
     * @brief   Time difference between call of testing_start() and
     *          testing_stop().
     */
    long int time_diff;
};

/**
 * @brief   Starts a test measuring.
 * @param[in]   seq_num Sequence number of the measurement.
 */
void testing_start(uint8_t seq_num);

/**
 * @brief   Stops a test measuring.
 * @param[in]   seq_num Sequence number of the measurement.
 */
void testing_stop(uint8_t seq_num);

/**
 * @brief   Starts the sending of the packets and initialises the
 *          test environment
 * @param[in]   ping_addr           The address the packets should be
 *                                  send to.
 * @param[in]   results_dir_name    Name of the results directory.
 * @param[in]   skeleton_file       Name of the skeleton file, where
 *                                  the template for the results file
 *                                  is. Lines of the formats
 *                                  <tt>"# count=%ld (-c)\n"</tt> and
 *                                  <tt>"# interval=%0.2f (-i)\n"</tt>
 *                                  will be replaced with <tt>\%ld</tt>
 *                                  set to <em>runs_per_test</em> and
 *                                  <tt>\%0.2f</tt> set to
 *                                  <em>interval</em>.
 * @param[in]   runs_per_test       How many packets shall be send in
 *                                  this test.
 * @param[in]   interval            How many seconds shall lay between
 *                                  the sending of each packet.
 */
void start_test(const char *ping_addr,
                const char *results_dir_name, const char *skeleton_file,
                int runs_per_test, float interval);

#endif /* TESTING_H*/
