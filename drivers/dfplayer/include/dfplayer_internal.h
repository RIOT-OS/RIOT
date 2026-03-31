/*
 * SPDX-FileCopyrightText: 2019 Marian Buschsieweke
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_dfplayer
 * @{
 *
 * @file
 * @brief       Internal functions of DFPlayer Mini Device driver
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#include <stdint.h>

#include "dfplayer_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Exchange a frame with the DFPlayer Mini
 *
 * @param   dev         Device descriptor of the DFPlayer to control
 * @param   resp        On success the response info is stored if `resp != NULL`
 * @param   cmd         Command to send
 * @param   p1          First parameter to send
 * @param   p2          Second parameter to send
 *
 * @retval  0           Success
 * @retval  -EINVAL     Called with invalid argument
 * @retval  -EAGAIN     Device responded with error busy
 * @retval  -EIO        Communication error
 * @retval  -ETIMEDOUT  Reply from the DFPlayer timed out
 *
 * The frame format of the DFPlayer Mini is this:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  0                   1
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |     Start     |    Version    |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |     Length    | Command/Code  |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |    Feedback   |  Parameter 1  |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |  Parameter 2  |   FCS (MSB)   |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |   FCS (LSB)   |    Stop       |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * These are the values of the fields when **sending** a frame to the DFPlayer
 *
 * | Field          | Value                                                         |
 * |:-------------- |:------------------------------------------------------------- |
 * | Start          | `0x7e`                                                        |
 * | Version        | `0xff`                                                        |
 * | Length         | Length of the data (always 6)                                 |
 * | Command/Code   | Command to send                                               |
 * | Feedback       | `0x01` if device should acknowledge command, `0x00` otherwise |
 * | Parameter 1    | First parameter of the command, or `0x00`                     |
 * | Parameter 2    | Second parameter of the command, or `0x00`                    |
 * | FCS            | Frame check sequence, `0 - sum(data)`                         |
 * | Stop           | `0xef`                                                        |
 *
 * These are the values of the fields when **receiving** a frame from the DFPlayer
 *
 * | Field          | Value                                                         |
 * |:-------------- |:------------------------------------------------------------- |
 * | Start          | `0x7e`                                                        |
 * | Version        | `0xff`                                                        |
 * | Length         | Length of the data (always 6)                                 |
 * | Command/Code   | Response code  (0x41 = success, 0x40 = error, 0x3* = event)   |
 * | Feedback       | `0x00`                                                        |
 * | Parameter 1    | Additional info (most significant byte)                       |
 * | Parameter 2    | Additional info (least significant byte)                      |
 * | FCS            | Frame check sequence, `0 - sum(data)`                         |
 * | Stop           | `0xef`                                                        |
 *
 */
int dfplayer_transceive(dfplayer_t *dev, uint16_t *resp,
                        uint8_t cmd, uint8_t p1, uint8_t p2);

/**
 * @brief   Send a command selecting a playback file
 *
 * @param   dev         Device descriptor of the DFPlayer to control
 * @param   cmd         Command starting playback of a file
 * @param   p1          First parameter to send
 * @param   p2          Second parameter to send
 *
 * @retval  0           Success
 * @retval  -EINVAL     Called with invalid argument
 * @retval  -EAGAIN     Device responded with error busy
 * @retval  -EIO        Communication error
 * @retval  -ETIMEDOUT  Reply from the DFPlayer timed out
 * @retval  -ENOENT     No such file found
 *
 * @warning In contrast to all other functions in this header, this command
 *          does not lock and unlock the mutex in `dev->mutex` internally, but
 *          leaves this to the caller. This is required to do the booking of
 *          the currently played file consistent with the device state, even if
 *          multiple threads control the same DFPlayer.
 */
int dfplayer_file_cmd(dfplayer_t *dev, uint8_t cmd, uint8_t p1, uint8_t p2);

/**
 * @brief   Reset the DFPlayer Mini MP3 player
 *
 * @param   dev         Device to reset
 *
 * @retval  0           Success
 * @retval  -EINVAL     Called with invalid argument
 * @retval  -EAGAIN     Device responded with error busy
 * @retval  -EIO        Communication error
 * @retval  -ETIMEDOUT  Reply from the DFPlayer timed out
 */
int dfplayer_reset(dfplayer_t *dev);

/**
 * @brief   UART-ISR handler of the DFPLayer driver
 *
 * @param   dev         DFPlayer device descriptor for which a byte was received
 * @param   data        The byte received over UART
 */
void dfplayer_uart_rx_cb(void *dev, uint8_t data);

/**
 * @brief   Send a command with two parameters to the DFPlayer Mini
 *
 * @param   dev         Device descriptor of the DFPlayer to control
 * @param   cmd         Command to send
 * @param   p1          First parameter to send
 * @param   p2          Second parameter to send
 *
 * @retval  0           Success
 * @retval  -EAGAIN     Device responded with error busy
 * @retval  -EIO        Communication error
 * @retval  -ETIMEDOUT  Reply from the DFPlayer timed out
 */
static inline int dfplayer_cmd_2param(dfplayer_t *dev, uint8_t cmd,
                                      uint8_t p1, uint8_t p2)
{
    return dfplayer_transceive(dev, NULL, cmd, p1, p2);
}

/**
 * @brief   Send a command with one parameter to the DFPlayer Mini
 *
 * @param   dev         Device descriptor of the DFPlayer to control
 * @param   cmd         Command to send
 * @param   param       The parameter to send
 *
 * @retval  0           Success
 * @retval  -EAGAIN     Device responded with error busy
 * @retval  -EIO        Communication error
 * @retval  -ETIMEDOUT  Reply from the DFPlayer timed out
 */
static inline int dfplayer_cmd_1param(dfplayer_t *dev, uint8_t cmd, uint8_t param)
{
    return dfplayer_transceive(dev, NULL, cmd, 0, param);
}

/**
 * @brief   Send a command without parameters to the DFPlayer Mini
 *
 * @param   dev         Device descriptor of the DFPlayer to control
 * @param   cmd         Command to send
 *
 * @retval  0           Success
 * @retval  -EAGAIN     Device responded with error busy
 * @retval  -EIO        Communication error
 * @retval  -ETIMEDOUT  Reply from the DFPlayer timed out
 */
static inline int dfplayer_cmd(dfplayer_t *dev, uint8_t cmd)
{
    return dfplayer_transceive(dev, NULL, cmd, 0, 0);
}

/**
 * @brief   Send a query and receive the response
 *
 * @param   dev         Device descriptor of the DFPlayer to control
 * @param   resp        The response code will be stored here
 * @param   cmd         Query-command to send
 *
 * @retval  0           Success
 * @retval  -EAGAIN     Device responded with error busy
 * @retval  -EIO        Communication error
 * @retval  -ETIMEDOUT  Reply from the DFPlayer timed out
 */
static inline int dfplayer_query(dfplayer_t *dev, uint16_t *resp, uint8_t cmd)
{
    return dfplayer_transceive(dev, resp, cmd, 0, 0);
}

#ifdef __cplusplus
}
#endif

/** @} */
