/*
 * Copyright (C) 2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_congure_test    CongURE test framework shell commands
 * @ingroup     sys_congure
 * @brief       Shell commands to test a CongURE implementation
 *
 * This module requires an application defined `congure_impl.h` which defines
 * the congure_snd_t extension of the CongURE implementation as
 * @ref congure_test_snd_t and provides a function declaration
 * @ref congure_test_snd_setup() setup said type. E.g.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * typedef congure_reno_snd_t congure_test_snd_t;
 *
 * void congure_test_snd_setup(congure_test_snd_t *c, int id);
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * All constants and initial values can then be set within the application
 * specific definition of @ref congure_test_snd_setup().
 *
 * @{
 *
 * @file
 * @brief   Definitions for the CongURE test framework
 *
 * @author  Martine S Lenders <m.lenders@fu-berlin.de>
 */

#ifndef CONGURE_TEST_H
#define CONGURE_TEST_H

#include "congure_impl.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DOXYGEN
/**
 * @brief   Application-defined type for the CongURE state object under test
 *
 * @extends congure_snd_t
 *
 * @note    Needs to be set within an application-provided `congure_impl.h` to
 *          the state object of the CongURE implementation you want to test.
 */
typedef congure_snd_t congure_test_snd_t;

/**
 * @brief   Setup the application-defined CongURE state object under test
 *
 * @note    Needs to be defined by the application and declare it within
 *          an application-provided `congure_impl.h`
 *
 * @param[in,out]   c   The CongURE state object under test. May not be NULL.
 * @param[in]       id  And application-specific ID that may identify different
 *                      setup parameters, e.g. a set of different constants to
 *                      use when setting up @p c. If not applicable to your
 *                      application just ignore @p id.
 *
 * @retval  0 on success
 * @retval  -1 when @p id is not ignored and is an unknown value to the
 *          application.
 */
int congure_test_snd_setup(void congure_test_snd_t *c, unsigned id);
#endif  /* DOXYGEN */

/**
 * @brief   Pool size for the message list elements for a lost message report
 *
 * @see congure_snd_driver_t::report_msg_lost
 *
 * This defines the maximum number of 3-tuples you can use with
 * @ref congure_test_call_report() when `argv[1] = "msg_lost"`.
 */
#ifndef CONFIG_CONGURE_TEST_LOST_MSG_POOL_SIZE
#define CONFIG_CONGURE_TEST_LOST_MSG_POOL_SIZE  (4U)
#endif

/**
 * @brief   Get the application-defined CongURE state object
 *
 * @note    Needs to be defined by the application
 *
 * @return  The CongURE state object.
 */
congure_test_snd_t *congure_test_get_state(void);

/**
 * @brief   Clears the CongURE state object
 *
 * Every byte in the state object is set to 0.
 *
 * @param[in] argc  Number of @p argv. Needs to be at least 1.
 * @param[in] argv  Command line arguments. No extra arguments are required
 *                  except for the command name in `argv[0]`.
 *
 * Always generates the following JSON object in STDOUT:
 * @code {"success": null} @endcode
 *
 * @return  Always 0.
 */
int congure_test_clear_state(int argc, char **argv);

/**
 * @brief   Setup the CongURE state object
 *
 * Calls application-defined @ref congure_test_snd_setup()
 *
 * @param[in] argc  Number of @p argv. Needs to be at least 1.
 * @param[in] argv  Command line arguments. The command name is expected in
 *                  `argv[0]`. If @p argc > 1, a integer is expected in
 *                  `argv[1]` for the `id` parameter of
 *                  @ref congure_test_snd_setup().
 *
 * This function will generate the following JSON objects in STDOUT:
 * - @code {"success": "0x12345678"} @endcode
 *   On success, with `0x12345678` being replaced with the memory address of the
 *   state object.
 * - @code {"error":"`id` expected to be integer"} @endcode
 *   On error, when `argv[1]` is not parsable to an unsigned integer.
 * - @code {"error":"`id` is invalid"} @endcode
 *   On error, when `argv[1]` is a valid unsigned integer but is an unknown
 *   value to the application.
 *
 * @retval  0 on success.
 * @retval  1 on error. Only can happen if `argv[1]` is provided and an invalid
 *          or unexpected value.
 */
int congure_test_call_setup(int argc, char **argv);

/**
 * @brief   Calls `init()` method for CongURE state object.
 *
 * @see congure_snd_driver_t::init()
 *
 * @param[in] argc  Number of @p argv. Needs to be at least 2.
 * @param[in] argv  Command line arguments. `argv[0]` needs to be the command
 *                  name and `argv[1]` needs to be a hexadecimal integer of
 *                  format 0xXXXX, represending a pointer to the object used as
 *                  the `ctx` parameter for `init()`.
 *
 * This function will generate the following JSON objects in STDOUT on error:
 * - @code {"error":"State object not set up"} @endcode
 *   When @ref congure_test_snd_setup() was not called before calling this
 *   command (i.e. the `driver` member of the state object is `NULL`).
 * - @code {"error":"`ctx` argument expected"} @endcode
 *   When @p argc < 2.
 * - @code {"error":"`ctx` expected to be hex"} @endcode
 *   When `argv[1]` is not parseable as a hexadecimal integer.
 *
 * Always generates the following JSON object in STDOUT:
 * @code {"success": null} @endcode
 *
 * @retval  0 on success.
 * @retval  1 on error.
 */
int congure_test_call_init(int argc, char **argv);

/**
 * @brief   Calls `inter_msg_interval()` method for CongURE state object.
 *
 * @see congure_snd_driver_t::inter_msg_interval()
 *
 * @param[in] argc  Number of @p argv. Needs to be at least 1.
 * @param[in] argv  Command line arguments. No extra arguments are required
 *                  except for the command name in `argv[0]`.
 *
 * This function will generate the following JSON objects in STDOUT:
 * - @code {"success":X} @endcode
 *   On success, with `X` being replaced with the return value of
 *   congure_snd_driver_t::inter_msg_interval().
 * - @code {"error":"State object not set up"} @endcode
 *   When @ref congure_test_snd_setup() was not called before calling this
 *   command (i.e. the `driver` member of the state object is `NULL`).
 *
 *
 * @retval  0 on success.
 * @retval  1 on error.
 */
int congure_test_call_inter_msg_interval(int argc, char **argv);

/**
 * @brief   Calls one of the `report_*()` methods for CongURE state object.
 *
 * @see congure_snd_driver_t::report_msg_sent()
 * @see congure_snd_driver_t::report_msg_discarded()
 * @see congure_snd_driver_t::report_msg_timeout()
 * @see congure_snd_driver_t::report_msg_lost()
 * @see congure_snd_driver_t::report_msg_acked()
 * @see congure_snd_driver_t::report_ecn_ce()
 *
 * @param[in] argc  Number of @p argv. Needs to be at least 3.
 * @param[in] argv  Command line arguments. `argv[0]` needs to be the command
 *                  name and `argv[1]` needs to one of the following
 *                  sub-commands that may require at least one extra arguments:
 *                  - `msg_sent`: `argv[2]` is expected to be an integer for the
 *                    `msg_size` parameter of
 *                    congure_snd_driver_t::report_msg_sent()
 *                  - `msg_sent`: `argv[2]` is expected to be an integer for the
 *                    `msg_size` parameter of
 *                    congure_snd_driver_t::report_msg_discarded()
 *                  - `msg_timeout`: `argv` is expected to have a number of
 *                    parameters divisible by 3 after `argv[1]` (i.e.
 *                    `(argc - 2) % 3 == 0` must hold). Each group of 3
 *                    `argv[2+i]`, `argv[3+i]`, argv[4+i] (with `i` being the
 *                    offset of the group) represents an element in the `msgs`
 *                    list parameter of
 *                    congure_snd_driver_t::report_msg_timeout():
 *                    - `argv[2+i]` (`msg_send_time`) is expected to be an
 *                      integer for the `send_time` member of
 *                      @ref congure_snd_msg_t,
 *                    - `argv[3+i]` (`msg_size`) is expected to be a an integer
 *                      for the `size` member of @ref congure_snd_msg_t, and
 *                    - `argv[4+i]` (`msg_resends`) is expected to be an integer
 *                      integer for the `resends` member of
 *                      @ref congure_snd_msg_t.
 *                  - `msg_lost`: `argv` is expected to have a number of
 *                    parameters divisible by 3 after `argv[1]` (i.e.
 *                    `(argc - 2) % 3 == 0` must hold. Each group of 3
 *                    `argv[2+i]`, `argv[3+i]`, argv[4+i] (with `i` being the
 *                    offset of the group) represents an element in the `msgs`
 *                    list parameter of
 *                    congure_snd_driver_t::report_msg_lost():
 *                    - `argv[2+i]` (`msg_send_time`) is expected to be an
 *                      integer for the `send_time` member of
 *                      @ref congure_snd_msg_t,
 *                    - `argv[3+i]` (`msg_size`) is expected to be an integer
 *                      for the `size` member of @ref congure_snd_msg_t, and
 *                    - `argv[4+i]` (`msg_resends`) is expected to be a an integer
 *                      integer for the `resends` member of
 *                      @ref congure_snd_msg_t.
 *                  - `msg_acked`: `argc` must be 11. The first three arguments
 *                    after `argv[1]` represent members of the `msg` parameter
 *                    of congure_snd_driver_t::report_msg_acked():
 *                    - `argv[2]` (`msg_send_time`) is expected to be an
 *                      integer for the `send_time` member of
 *                      @ref congure_snd_msg_t,
 *                    - `argv[3]` (`msg_size`) is expected to be an integer
 *                      for the `size` member of @ref congure_snd_msg_t, and
 *                    - `argv[4]` (`msg_resends`) is expected to be an integer
 *                      for the `resends` member of @ref congure_snd_msg_t.
 *
 *                    The `next` member of @ref congure_snd_msg_t will be
 *                    initialized with `NULL`.
 *
 *                    The remaining 6 arguments represent members of the `ack`
 *                    parameter of congure_snd_driver_t::report_msg_acked():
 *                    - `argv[5]` (`ack_recv_time`) is expected to be a an
 *                      integer for the `recv_time` member of
 *                      @ref congure_snd_ack_t,
 *                    - `argv[6]` (`ack_id`) is expected to be a an integer
 *                      for the `ack_id` member of @ref congure_snd_ack_t, and
 *                    - `argv[7]` (`ack_size`) is expected to be a an integer
 *                      integer for the `size` member of @ref congure_snd_ack_t.
 *                    - `argv[8]` (`ack_clean`) is expected to be a an integer
 *                      for the `clean` member of @ref congure_snd_ack_t. If
 *                      `argv[8]` is `"0"`, `clean` will be set to `false` and to
 *                      `true` otherwise.
 *                    - `argv[9]` (`ack_wnd`) is expected to be a 16-bit integer
 *                      for the `wnd` member of @ref congure_snd_ack_t.
 *                    - `argv[10]` (`ack_delay`) is expected to be a 16-bit
 *                      integer for the `delay` member of
 *                      @ref congure_snd_ack_t.
 *                  - `ecn_ce`: `argv[2]` is expected to be an integer for the
 *                    `time` parameter of congure_snd_driver_t::report_ecn_ce()
 *
 * This function will generate the following JSON objects in STDOUT:
 * - @code {"success":null} @endcode
 *   On success
 * - @code {"error":"State object not set up"} @endcode
 *   When @ref congure_test_snd_setup() was not called before calling this
 *   command (i.e. the `driver` member of the state object is `NULL`).
 * - @code {"error":"No report command provided"} @endcode
 *   When @p argc < 2.
 * - @code {"error":"Unknown command `<command>`"} @endcode
 *   When `argv[1] = "<command>"` is not a known sub-command.
 * - @code {"error":"`<arg_name>` argument expected"} @endcode
 *   When `argv[i] = "<arg_name>"` is expected but `argc <= i`.
 * - @code {"error":"`<arg_name>` expected to be integer"} @endcode
 *   When `argv[i] = "<arg_name>"` is expected to be an integer but is not
 *   parseable
 * - @code {"error":"`<arg_name>` expected not 16 bit wide"} @endcode
 *   When `argv[i] = "<arg_name>"` is expected to be an 16-bit unsigned integer
 *   but is is greater than or equal to $2^{16}$
 * - @code {"error":"At least `<arg_num>` arguments <arglist> expecdted"}
 *   @endcode
 *   When `argc` is smaller than expected. `<arg_num>` provides the number of
 *   arguments beyond the sub-command (i.e. `argc` needs at least to be
 *   `<arg_num> + 2`), with the names of the arguments expected provided in
 *   `<arg_list>` as a comma-seperated list of <tt>`<arg_name>`</tt>.
 * - @code {"error":"Number of arguments must be divisible by 3"} @endcode
 *   When `argv[1] == "msg_timeout"` or `argv[1] == "msg_lost"` but
 *   the length of the argument list after `argv[1]` is not divisible by 3 (i.e.
 *   `(argc - 2) % 3 != 0`).
 * - @code {"error":"List element pool depleted"} @endcode
 *   When `argv[1] == "msg_timeout"` or `argv[1] == "msg_lost"` and
 *   `(argc - 2) / 3` >= @ref CONFIG_CONGURE_TEST_LOST_MSG_POOL_SIZE).
 *
 * Provides no output on success.
 *
 * @retval  0 on success.
 * @retval  1 on error.
 */
int congure_test_call_report(int argc, char **argv);

#ifdef __cplusplus
}
#endif

#endif /* CONGURE_TEST_H */
/** @} */
