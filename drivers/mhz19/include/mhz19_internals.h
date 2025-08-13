/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     drivers_mhz19
 * @{
 * @file
 * @brief       Internal addresses, registers, constants for the MH-Z19 CO2
 *              sensor
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    MH-Z19 Baud rate
 *
 * Fixed at 9600 by design.
 */
#define MHZ19_UART_BAUDRATE 9600

/**
 * @name    MH-Z19 Buffer size
 *
 * A transmission from the MH-Z19 is 9 bytes long:
 * 1 Start byte
 * 1 Sensor number
 * 2 bytes data
 * 4 bytes padding
 * 1 byte checksum
 *
 * The start byte is not stored because it is not used in the checksum
 * calculation.
 */
#define MHZ19_BUF_SIZE      8

/**
 * @name    MH-Z19 Timeout in milliseconds
 *
 * 20 ms gives a decent margin on top of the UART transmission time. The
 * datasheet does not specify any timings beside the UART baud rate.
 *
 * A single byte takes 10 bits effectively: a start bit, 8 bits data
 * and stop bit. 9 bytes are transmitted, thus 10 bits * 9 / 9600bps = 9.3 ms.
 */
#define MHZ19_TIMEOUT_READ  20

/***
 * @name MH-Z19 Command timeout in milliseconds
 *
 * While undocumented, it seems that some commands take some time. Without
 * an additional delay, the first sensor read will fail with a timeout value,
 * or return an arbitrary value.
 */
#define MHZ19_TIMEOUT_CMD   (MHZ19_TIMEOUT_READ * 5)

/**
 * @name    MH-Z19 transmission constants
 * @{
 */
#define MHZ19_READ_START                0xff    /**< Start bytes */
#define MHZ19_READ_SENSOR_NUM           0x01    /**< Sensor number */
/** @} */

/**
 * @name    MH-Z19 commands
 * @{
 */
#define MHZ19_CMD_AUTO_CALIBRATION      0x79    /**< Auto calibration command */
#define MHZ19_CMD_GAS_CONCENTRATION     0x86    /**< Gas concentration command */
#define MHZ19_CMD_CALIBRATE_ZERO        0x87    /**< Zero calibration command */
#define MHZ19_CMD_CALIBRATE_SPAN        0x88    /**< Span calibration command */
/** @} */

/**
 * @name    MH-Z19 transmission data positions
 * @{
 */
#define MHZ19_RX_POS_PPM_HIGH           1   /**< Measurement high byte */
#define MHZ19_RX_POS_PPM_LOW            2   /**< Measurement low byte */
#define MHZ19_RX_POS_CHECKSUM           7   /**< Checksum position */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
