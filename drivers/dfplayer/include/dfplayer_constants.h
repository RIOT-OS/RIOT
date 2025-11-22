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
 * @brief       Constants used in the DFPlayer Mini Driver
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#include <stdint.h>
#include <periph/gpio.h>
#include <mutex.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Constants used in frames send to the DFPlayer Mini
 * @{
 */
#define DFPLAYER_START              (0x7e)  /**< Start symbol */
#define DFPLAYER_VERSION            (0xff)  /**< Value to use in version field */
#define DFPLAYER_LEN                (0x06)  /**< Length of a frame */
#define DFPLAYER_NO_ACK             (0x00)  /**< No acknowledgement of CMD required */
#define DFPLAYER_ACK                (0x01)  /**< Acknowledgement of CMD required */
#define DFPLAYER_END                (0xef)  /**< End symbol */
/** @} */

/**
 * @name    UART settings of the DFPlayer Mini
 * @{
 */
#define DFPLAYER_BAUD               (9600)              /**< Symbol rate of the DFPlayer mini */
#define DFPLAYER_DATA_BITS          (UART_DATA_BITS_8)  /**< The DFPlayer uses 8 data bits */
#define DFPLAYER_PARITY             (UART_PARITY_NONE)  /**< The DFPlayer does not use a parity bit */
#define DFPLAYER_STOP_BITS          (UART_STOP_BITS_1)  /**< The DFPlayer uses 1 stop bit */
/** @} */

/**
 * @name    Commands supported by the DFPlayer Mini
 * @{
 */
#define DFPLAYER_CMD_NEXT           (0x01)  /**< Start playing the next song */
#define DFPLAYER_CMD_PREV           (0x02)  /**< Start playing the next song */
#define DFPLAYER_CMD_VOLUME_INC     (0x04)  /**< Increase volume */
#define DFPLAYER_CMD_VOLUME_DEC     (0x05)  /**< Decrease volume */
#define DFPLAYER_CMD_SET_VOLUME     (0x06)  /**< Set the volume to the given level */
#define DFPLAYER_CMD_SET_EQUALIZER  (0x07)  /**< Set the equalizer to the given setting */
#define DFPLAYER_CMD_SET_SOURCE     (0x09)  /**< Set the source to play files from */
#define DFPLAYER_CMD_STANDBY_ENTER  (0x0a)  /**< Enter low power mode */
#define DFPLAYER_CMD_STANDBY_EXIT   (0x0b)  /**< Exit low power mode, back to normal mode */
#define DFPLAYER_CMD_RESET          (0x0c)  /**< Reset the DFPlayer Mini */
#define DFPLAYER_CMD_PLAY           (0x0d)  /**< Start playing the selected file */
#define DFPLAYER_CMD_PAUSE          (0x0e)  /**< Pause the playback */
#define DFPLAYER_CMD_FILE           (0x0f)  /**< Play the given file */
#define DFPLAYER_CMD_PLAY_FROM_MP3  (0x12)  /**< Play the given file (1-9999) from the folder `"MP3"` */
#define DFPLAYER_CMD_PLAY_ADVERT    (0x13)  /**< Play the given file (1-9999) from the folder `"ADVERT"`, resume current playback afterwards */
#define DFPLAYER_CMD_ABORT_ADVERT   (0x15)  /**< Play the given file (1-9999) from the folder `"ADVERT"`, resume current playback afterwards */
#define DFPLAYER_CMD_REPEAT_FOLDER  (0x17)  /**< Start repeat-playing the given folder (1-99) */
#define DFPLAYER_CMD_RANDOM         (0x18)  /**< Start playing all files in random order */
#define DFPLAYER_CMD_REPEAT         (0x19)  /**< 0 = repeat currently played file, 1 = stop repeating */
#define DFPLAYER_CMD_GET_STATUS     (0x42)  /**< Retrieve the current status */
#define DFPLAYER_CMD_GET_VOLUME     (0x43)  /**< Retrieve the current volume */
#define DFPLAYER_CMD_GET_EQUALIZER  (0x44)  /**< Retrieve the current equalizer setting */
#define DFPLAYER_CMD_GET_MODE       (0x45)  /**< Retrieve the current playback mode */
#define DFPLAYER_CMD_GET_VERSION    (0x46)  /**< Retrieve the device's software version */
#define DFPLAYER_CMD_FILES_USB      (0x47)  /**< Get the total number of files on USB storage */
#define DFPLAYER_CMD_FILES_SDCARD   (0x48)  /**< Get the total number of files on the SD card */
#define DFPLAYER_CMD_FILES_FLASH    (0x49)  /**< Get the total number of files on NOR flash */
#define DFPLAYER_CMD_FILENO_USB     (0x4b)  /**< Get the currently select file number on the USB storage */
#define DFPLAYER_CMD_FILENO_SDCARD  (0x4c)  /**< Get the currently select file number on the SD-Card */
#define DFPLAYER_CMD_FILENO_FLASH   (0x4d)  /**< Get the currently select file number on the NOR flash */
/** @} */

/**
 * @name    Classes of messages received from the DFPlayer
 * @{
 */
#define DFPLAYER_CLASS_MASK         (0xf0)  /**< Use this mask to get the class from a response code */
#define DFPLAYER_CLASS_NOTIFY       (0x30)  /**< Message is an event notification (unrelated to any command) */
#define DFPLAYER_CLASS_RESPONSE     (0x40)  /**< Message is a response to the most recent command */
/** @} */

/**
 * @name    Notification codes send by the DFPlayer Mini
 * @{
 */
#define DFPLAYER_NOTIFY_INSERT      (0x3a)  /**< A USB storage device or an SD card was inserted */
#define DFPLAYER_NOTIFY_EJECT       (0x3b)  /**< A USB storage device or an SD card was ejected */
#define DFPLAYER_NOTIFY_DONE_USB    (0x3c)  /**< Completed playing the indicated track from USB storage */
#define DFPLAYER_NOTIFY_DONE_SDCARD (0x3d)  /**< Completed playing the indicated track from SD card */
#define DFPLAYER_NOTIFY_DONE_FLASH  (0x3e)  /**< Completed playing the indicated track from flash */
/**
 * @brief   The DFPlayer is ready
 *
 * This notification is send after boot/reset when the DFPlayer Mini has become
 * ready. As additional info the 4 least significant bits indicate which
 * playback sources are available.
 */
#define DFPLAYER_NOTIFY_READY       (0x3f)
/** @} */

/**
 * @name    Bitmasks identifying the playback sources in the ready notification
 * @{
 */
#define DFPLAYER_MASK_USB           (0x01)  /**< USB stick is connected */
#define DFPLAYER_MASK_SDCARD        (0x02)  /**< SD-Card is connected */
#define DFPLAYER_MASK_PC            (0x04)  /**< Unclear, has something to do with debugging */
#define DFPLAYER_MASK_FLASH         (0x08)  /**< NOR flash is connected */
/** @} */

/**
 * @name    Response codes codes send by the DFPlayer Mini
 * @{
 */
#define DFPLAYER_RESPONSE_ERROR     (0x40)  /**< While processing the most recent command an error occurred */
#define DFPLAYER_RESPONSE_OK        (0x41)  /**< Last command succeeded */
/* Beware: Handle every code of class response (0x4*) that does not indicate
 * an error (0x40) as success */
/** @} */

/**
 * @name    Error codes send as parameter of error messages
 * @{
 */
#define DFPLAYER_ERROR_BUSY         (0x00)  /**< Module is busy */
#define DFPLAYER_ERROR_FRAME        (0x01)  /**< Received incomplete frame */
#define DFPLAYER_ERROR_FCS          (0x02)  /**< Frame check sequence of last frame didn't match */
/**
 * @brief   File/folder selected for playback (command 0x06) does not exit
 *
 * Beware: The DFPlayer Mini will acknowledge the command 0x06 first blindly,
 * and send a second acknowledgement when the file exists (0x41), or an error
 * (0x40) if not.
 */
#define DFPLAYER_ERROR_NO_SUCH_FILE (0x06)
/** @} */

/**
 * @name    Device identifiers in insert/eject notifications
 * @{
 */
#define DFPLAYER_DEVICE_USB         (0x01)  /**< A USB storage device was inserted/ejected */
#define DFPLAYER_DEVICE_SDCARD      (0x02)  /**< An SD card was inserted/ejected */
/** @} */

/**
 * @name    Status bitmasks
 *
 * These values have been obtained by reverse engineering.
 *
 * @{
 */
#define DFPLAYER_STATUS_PLAYING     (0x01)  /**< The DFPlayer is currently playing a song */
#define DFPLAYER_STATUS_PAUSE       (0x02)  /**< The DFPlayer is paused */
/** @} */

/**
 * @name    Flags to store info about the driver state
 * @{
 */
/**
 * @brief   The next command will be affected by the no-ACK bug
 *
 * After playback of a file completed, the next command does not get ack'ed by
 * the device. Any subsequent command is however normally ack'ed. Luckily,
 * query commands (0x40 and higher) are not affected. The driver works around
 * this issue by querying the volume prior to sending a control command when
 * this flag is set
 */
#define DFPLAYER_FLAG_NO_ACK_BUG    (0x01)
/** @} */

#define DFPLAYER_BOOTUP_TIME_MS     (3000)  /**< Boot up of the device takes 1.5 to 3 secs */
#define DFPLAYER_TIMEOUT_MS         (100)   /**< Timeout waiting for a replay in milliseconds */
#define DFPLAYER_SEND_DELAY_MS      (100)   /**< Wait 100ms after a cmd to work around hw bug */
#ifndef DFPLAYER_RETRIES
#define DFPLAYER_RETRIES            (5)     /**< How often to retry a command on timeout */
#endif /* DFPLAYER_RETRIES */
#define DFPLAYER_MAX_VOLUME         (30)    /**< Maximum supported volume */
#define DFPLAYER_MAX_FOLDER         (99)    /**< Highest supported folder number */
#define DFPLAYER_MAX_MP3_FILE       (9999)  /**< Highest supported file number in the `"MP3"` folder */
#define DFPLAYER_MAX_ADVERT_FILE    (9999)  /**< Highest supported file number in the `"ADVERT"` folder */
#define DFPLAYER_LOWEST_QUERY       (0x40)  /**< Query commands are 0x40 or higher */

#ifdef __cplusplus
}
#endif
/** @} */
