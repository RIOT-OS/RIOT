/*
 * Copyright (C) 2019 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Shared configuration for NimBLE L2CAP tests
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef NIMBLE_L2CAP_TEST_CONF_H
#define NIMBLE_L2CAP_TEST_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Calculate values for the application specific mbuf pool
 * @{
 */
#define MBUFSIZE_OVHD       (sizeof(struct os_mbuf) + \
                             sizeof(struct os_mbuf_pkthdr))
#define MBUFS_PER_MTU       (APP_MTU / APP_BUF_CHUNKSIZE)
#define MBUFSIZE            (APP_BUF_CHUNKSIZE + MBUFSIZE_OVHD)
#define MBUFCNT             (APP_BUF_NUM * MBUFS_PER_MTU)
/** @} */

/**
 * @name    Default parameters for selected test cases
 * @{
 */
#define PKTSIZE_DEFAULT     (100U)
#define FLOOD_DEFAULT       (50U)
/** @} */

/**
 * @name    Field offsets and type values for test packet payloads
 * @{
 */
#define TYPE_INCTEST        (0x17192123)
#define TYPE_FLOODING       (0x73829384)

#define POS_TYPE            (0U)
#define POS_SEQ             (1U)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* NIMBLE_L2CAP_TEST_CONF_H */
/** @} */
