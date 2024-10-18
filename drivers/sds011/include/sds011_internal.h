/*
 * Copyright (C) 2018 HAW-Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

 /**
  * @ingroup     drivers_sds011
  * @{
  *
  * @file
  * @brief       Internal constants etc. for the SDS011 laser dust sensor
  *
  * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
  */
#ifndef SDS011_INTERNAL_H
#define SDS011_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief SDS011 baud rate
 */
#define SDS011_UART_BAUDRATE             (9600U)

 /**
  * @name SDS011 frame lengths
  * @{
  */
#define SDS011_FRAME_SEND_LEN            (19U)
#define SDS011_FRAME_RECV_LEN            (10U)
/** @} */

/**
 * @name SDS011 command values
 * @{
 */
#define SDS011_CMD_DB1_SET_DR_MODE       (2U)
#define SDS011_CMD_DB1_QUERY_DATA        (4U)
#define SDS011_CMD_DB1_SET_DEV_ID        (5U)
#define SDS011_CMD_DB1_SET_SLEEP_WORK    (6U)
#define SDS011_CMD_DB1_CHECK_FIRMWARE    (7U)
#define SDS011_CMD_DB1_SET_WORK_PERIOD   (8U)
/** @} */

/**
 * @name SDS011 command option values
 * @{
 */
#define SDS011_CMD_OPT_QUERY             (0U)
#define SDS011_CMD_OPT_SET               (1U)
#define SDS011_CMD_OPT_REPORT_ACTIVE     (0U)
#define SDS011_CMD_OPT_REPORT_QUERY      (1U)
#define SDS011_CMD_OPT_SLEEP             (0U)
#define SDS011_CMD_OPT_WORK              (1U)
/** @} */

/**
 * @name SDS011 frame constants
 * @{
 */
#define SDS011_CMDID_QUERY               (0xB4)
#define SDS011_RCMDID_REPLY              (0xC5)
#define SDS011_RCMDID_DATA               (0xC0)
#define SDS011_FRAME_TAIL                (0xAB)
#define SDS011_FRAME_HEAD                (0xAA)
#define SDS011_FRAME_CSUM_MSK            (0xFF)
/** @} */

/**
 * @name SDS011 frame value indexes
 * @{
 */
#define SDS011_FRAME_HEAD_IDX            (0U)
#define SDS011_CMDID_IDX                 (1U)
#define SDS011_DB1_IDX                   (2U)
#define SDS011_DB2_IDX                   (3U)
#define SDS011_DB3_IDX                   (4U)
#define SDS011_DB4_IDX                   (5U)
#define SDS011_DB5_IDX                   (6U)
#define SDS011_DB6_IDX                   (7U)
#define SDS011_DEVID1_IDX                (15U)
#define SDS011_DEVID2_IDX                (16U)
#define SDS011_FRAME_SEND_TAIL_IDX       (SDS011_FRAME_SEND_LEN - 1)
#define SDS011_FRAME_RECV_TAIL_IDX       (SDS011_FRAME_RECV_LEN - 1)
#define SDS011_FRAME_SEND_CSUM_IDX       (SDS011_FRAME_SEND_LEN - 2)
#define SDS011_FRAME_RECV_CSUM_IDX       (SDS011_FRAME_RECV_LEN - 2)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* SDS011_INTERNAL_H */
