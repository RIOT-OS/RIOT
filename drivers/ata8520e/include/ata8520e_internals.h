/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_ata8520e
 * @brief       Internal addresses, registers, constants for the ATA8520E device
 * @{
 * @file
 * @brief       Internal addresses, registers, constants for the ATA8520E device
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name SPI commands for ATA8520E
 * @{
 */
#define ATA8520E_SYSTEM_RESET                (0x01)
#define ATA8520E_IO_INIT                     (0x02)
#define ATA8520E_IO_WRITE                    (0x03)
#define ATA8520E_IO_READ                     (0x04)
#define ATA8520E_OFF_MODE                    (0x05)
#define ATA8520E_ATMEL_VERSION               (0x06)
#define ATA8520E_WRITE_TX_BUFFER             (0x07)
#define ATA8520E_TEST_MODE                   (0x08)
#define ATA8520E_SIGFOX_VERSION              (0x09)
#define ATA8520E_GET_STATUS                  (0x0A)
#define ATA8520E_SEND_BIT                    (0x0B)
#define ATA8520E_SEND_FRAME                  (0x0D)
#define ATA8520E_SEND_RECEIVE_FRAME          (0x0E)
#define ATA8520E_GET_PAC                     (0x0F)
#define ATA8520E_READ_RX_BUFFER              (0x10)
#define ATA8520E_WRITE_SYS_CONF              (0x11)
#define ATA8520E_GET_ID                      (0x12)
#define ATA8520E_READ_SUP_TEMP               (0x13)
#define ATA8520E_START_MEASUREMENT           (0x14)
#define ATA8520E_TX_TEST_MODE                (0x15)
#define ATA8520E_SEND_CW                     (0x17)
#define ATA8520E_SET_TX_FREQUENCY            (0x1B)
/** @} */

/**
 * @name Constants for ATA8520E
 * @{
 */
#define ATA8520E_ATMEL_SYSTEM_READY_MASK     (0x20)
#define ATA8520E_ATMEL_FRAME_SENT_MASK       (0x10)
#define ATA8520E_ATMEL_PA_MASK               (0x01)
/** @} */

/**
 * @name Sigfox errors codes
 * @{
 */
#define ATA8520E_SIGFOX_NO_ERROR             (0x00)
#define ATA8520E_SIGFOX_TX_LEN_TOO_LONG      (0x30)
#define ATA8520E_SIGFOX_RX_TIMEOUT           (0x3E)
#define ATA8520E_SIGFOX_RX_BIT_TIMEOUT       (0x4E)
/** @} */

/**
 * @name Sigfox2 errors codes
 * @{
 */
#define ATA8520E_SIGFOX2_INIT_ERROR          (0x10)
#define ATA8520E_SIGFOX2_TX_ERROR            (0x18)
#define ATA8520E_SIGFOX2_RF_ERROR            (0x40)
#define ATA8520E_SIGFOX2_DF_WAIT_ERROR       (0x68)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
