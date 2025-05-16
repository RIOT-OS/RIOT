/*
 * Copyright 2019 Marian Buschsieweke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    drivers_dfplayer DFPlayer Mini MP3 Player
 * @ingroup     drivers_multimedia
 * @brief       Driver for the DFPlayer Mini MP3 Player
 *
 * The DFPlayer Mini is a super cheap MP3 Player that can be controlled via
 * UART. It can play files from SD card, USB storage devices and NOR flash.
 * It has an integrated 3W amplifier that can be used to build a standalone
 * mono speaker. Alternatively a stereo output can be used that can be
 * directly connected to headphones, but needs a dedicated amplifier for
 * connecting to passive speakers.
 *
 * ![DFPlayer pinout](https://camo.githubusercontent.com/fd6ed047e8e3ced124b32051ddfff2df9e8afe47/68747470733a2f2f63646e2e73686f706966792e636f6d2f732f66696c65732f312f313530392f313633382f66696c65732f4d50335f706c617965725f6d6f64756c655f70696e6f75745f6469616772616d2e706e67)
 *
 * File System Setup
 * =================
 *
 * The SD card or USB storage devices needs to be formatted as FAT32 and the
 * MP3 files have to be stored using one of the following naming schemes, if
 * selecting the files efficiently is required:
 *
 * Numbered File Inside Numbered Folder Naming Scheme
 * --------------------------------------------------
 *
 * The MP3 files have to be named with three leading decimal digits (e.g.
 * `001.mp3` or `042 - foo bar.mp3`) and must be placed in in folders named with
 * two decimal digits (e.g. `01` or `99`). The folder name numbers must be in
 * the range 1-99 and the file name numbers in the range 1-255. Playback of
 * these files can be started using the function @ref dfplayer_play_file .
 *
 * Examples of valid paths:
 *
 * - `01/001.mp3`
 * - `19/249 - Nothing Else Matters.mp3`
 *
 * Examples of ***INVALID*** paths:
 *
 * - `01 - Yngwie Malmsteen/042 - Leonardo.mp3`
 *     - The folder name must only consist of two digits, additional chars are
 *       not allowed in the folder name
 * - `9/42.mp3`
 *     - Leading zeros must be added, e.g. `09/042.mp3` would be valid
 * - `99/359.mp3`
 *     - Folder number out of range (1-255 is valid)
 *
 * Numbered File Inside MP3 Folder Naming Scheme
 * ---------------------------------------------
 *
 * The MP3 files have to be named with four decimal digits and must be placed in
 * the folder `"MP3"`. Subsequent characters after the four digits are ignored.
 * Valid names are e.g. `MP3/0001.mp3` or `MP3/0042 - My favorite song.mp3`. The
 * file numbers 1-9999 are valid. Playback of these files can be started using
 * the function @ref dfplayer_play_from_mp3 .
 *
 * Advertisements
 * --------------
 *
 * MP3 files placed in the folder `"ADVERT"` named with four decimal digits
 * (e.g. `"0001.mp3"` or `"1337.mp3"`) can be played as advertisements: The
 * function @ref dfplayer_play_from_advert can be used to start their playback,
 * but only while a non-advertisement is currently playing. After the
 * advertisement the normal playback is resumed. This feature has been
 * implemented in the hope it is used for features like audible user feedback
 * to controls, rather than playing advertisements.
 *
 * Combining Naming Schemes
 * ------------------------
 *
 * All of the above naming schemes can be used on the same storage device.
 * However, you still have to use to the functions intended to be used with
 * the naming scheme of a specific file in order to be able to play it.
 *
 * Track Numbers
 * =============
 *
 * @warning     Track numbers are a bogus unit in the DFPlayer
 *
 * Track numbers in the dfplayer revert to the number of the file in the file
 * system. Without preparing the medium played at a very low level, it will be
 * hard to map track numbers to their corresponding songs. If you started
 * playback using @ref dfplayer_play_file or @ref dfplayer_play_from_mp3, you
 * can use @ref dfplayer_get_played_file to get the currently played filed
 * according to the used naming scheme (see section above).
 *
 * Continuous Playback
 * ===================
 *
 * The are two options for achieving continuous playback with the DFPlayer Mini:
 * The first is to use @ref dfplayer_shuffle_all or @ref dfplayer_repeat_folder
 * to schedule shuffled playback of all files or repeat playback of a folder,
 * respectively. But keep in mind that any playback command will switch back to
 * normal playback mode.
 *
 * The second option is to schedule playback of the next file right after the
 * playback of a file has completed. This can be implemented most conveniently
 * by using the callback functions (see @ref dfplayer_set_callbacks). But
 * beware: The callbacks are called in interrupt context and the API of this
 * driver ***MUST ONLY*** be used in thread context.
 *
 * Known Hardware Bugs
 * ===================
 *
 * Device Bugs Not Handled by the Driver
 * -------------------------------------
 *
 * On some versions of the DFPlayer, any UART communication with the device
 * during playback results in audible and unpleasant glitches. The only solution
 * is to not communicate with the device during playback, as the DFPlayer seems
 * to pause playback for some milliseconds after receiving a command. If the
 * busy pin of the DFPlayer is connected to your board and configured, the
 * function @ref dfplayer_get_state will read the busy pin to detect if the
 * DFPlayer is currently playing. Onlyif according to the busy pin the device is
 * not currently playing, the UART interface is used to detect whether the
 * playback is paused or stopped. Thus, @ref dfplayer_get_state can be used
 * without fearing to cause audio glitches, provided the busy pin is connected
 * and configured. You can use this to poll the state of the DPlayer and issue
 * the commands once playback has completed, or use the callbacks (see
 * @ref dfplayer_set_callbacks) to get notified when playback has completed.
 *
 * When playback of a file from the `"ADVERT"` folder is started while the
 * device not playing, or already playing a file from the `"ADVERT"` folder,
 * the command fails to execute and the DFPlayer stops all playback (if
 * currently playing).
 *
 * Device Bugs Handled by the Driver
 * ---------------------------------
 *
 * (These bugs are transparently handled by the driver, so users of the driver
 * can safely skip reading this section.)
 *
 * When playing a file from the MP3 folder or from the ADVERT folder, the
 * DFPlayer acknowledges the command before checking if the file actually
 * exists. If the file does not exist, a second message to indicate the failure
 * is send, but no second message is send on success. If the second message is
 * not received, the driver has to figure out by other means if this is a
 * communication error or indicates that the playback has started. If the
 * busy pin of the DFPlayer is connected and configured, it will be used to
 * verify that the command succeeded. Otherwise, the driver will query the
 * status of the device via UART to confirm that playback started, which can
 * cause audible glitches on some revisions. For those revisions, the busy pin
 * should be really used.
 *
 * When the DFPlayer completes playback of a file, the next command received
 * is not acknowledged, if it is a control command. Query commands are not
 * affected. The driver works around this bug by querying the volume prior to
 * issuing a control command directly after playback completed.
 *
 * Some versions of the DFPlayer are not able to handle commands send at high
 * frequency. A delay of 100ms is inserted after every communication with the
 * device to counter this.
 *
 * Some versions of the DFPlayer have a high chance of ignoring commands. The
 * driver tries up to @ref DFPLAYER_RETRIES (`5` be default) times the command
 * before giving up. For that reason, non-idempotent commands supported by the
 * DFPlayer (e.g. increase and decrease volume) are not exposed by the driver,
 * as retrying could result in them being executed more than once. There are
 * idempotent commands that can achieve the same (e.g. the set volume command
 * instead of increase or decrease volume commands) for every non-idempotent
 * command, so this doesn't restrict the feature set of the driver.
 *
 * @{
 *
 * @file
 * @brief       DFPlayer Mini Device Driver
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#include <stdbool.h>
#include <stdint.h>

#include "dfplayer_params.h"
#include "dfplayer_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   The number of milliseconds to wait after receiving playback of file
 *          has completed before starting playback of the next file
 */
#define DFPLAYER_WAIT_MS    (100U)

#ifdef MODULE_AUTO_INIT_MULTIMEDIA

extern dfplayer_t dfplayer_devs[DFPLAYER_NUMOF];

/**
 * @brief   Get an auto-initialized DFPlayer Mini device descriptor by number
 * @param   num     Number of the DFPlayer Mini device descriptor to get
 * @return  The DFPlayer Mini device descriptor at index @p num
 * @retval  `NULL`  @p num is out of range
 *
 * A value of `0` for @p num is used to retrieve the first device descriptor.
 */
static inline dfplayer_t * dfplayer_get(unsigned num)
{
    if (num < DFPLAYER_NUMOF) {
        return &dfplayer_devs[num];
    }

    return NULL;
}

#endif /* MODULE_AUTO_INIT_MULTIMEDIA */

/**
 * @brief   Initialize a DFPlayer Mini device descriptor
 *
 * @param   dev         Device descriptor to initialized
 * @param   params      Connection parameters to initialize with
 *
 * @retval  0           Success
 * @retval  -EINVAL     Invalid parameters
 * @retval  -EIO        Failed to initialize UART interface / GPIO pin
 * @retval  -ETIMEDOUT  Response of the DFPlayer timed out
 */
int dfplayer_init(dfplayer_t *dev, const dfplayer_params_t *params);

/**
 * @brief   Get the set of available playback sources of the given DFPlayer
 *
 * @param   dev         Device descriptor of the DFPlayer Mini check
 *
 * @return  The set of playback sources currently available
 * @retval  0           If @p dev is `NULL` or no source is available
 */
dfplayer_source_set_t dfplayer_get_sources(dfplayer_t *dev);

/**
 * @brief   Set/clear the event callbacks of the DFPlayer Mini
 *
 * @param   dev         Device descriptor of the DFPlayer to update
 * @param   cb_done     Function to call when playback of a file/track completed
 * @param   cb_src      Function to call when a source was inserted/ejected
 *
 * @retval  0           Success
 * @retval  -EINVAL     Called with invalid parameter
 *
 * Calling with `NULL` for @p cb_done and/or for @p cb_src clears the
 * corresponding callback functions.
 *
 * @warning The callbacks are called from interrupt context
 */
int dfplayer_set_callbacks(dfplayer_t *dev, dfplayer_cb_done_t cb_done,
                           dfplayer_cb_src_t cb_src);

/**
 * @brief   Start playing the next song in the currently used naming scheme
 *
 * @param   dev         Device descriptor of the DFPlayer to control
 *
 * @retval  0           Success
 * @retval  -EINVAL     Called with invalid parameter
 * @retval  -EIO        Communication with the DFPlayer Mini failed
 * @retval  -EAGAIN     DFPlayer responded with error "Device busy"
 * @retval  -ETIMEDOUT  Response of the DFPlayer timed out
 * @retval  -ERANGE     The next song is out of range (depending to the
 *                      currently used naming scheme)
 * @retval  -ENOENT     No such file
 *
 * If previously @ref dfplayer_play_file was used to start playback of
 * file "42/113.mp3", this call will try to start "42/114.mp3". If previously
 * @ref dfplayer_play_from_mp3 was used to start playback of file
 * "MP3/1337.mp3", this call will try to start "MP3/1338.mp3".
 */
static inline int dfplayer_next(dfplayer_t *dev);

/**
 * @brief   Start playing the previous song in the currently used naming scheme
 *
 * @param   dev         Device descriptor of the DFPlayer to control
 *
 * @retval  0           Success
 * @retval  -EINVAL     Called with invalid parameter
 * @retval  -EIO        Communication with the DFPlayer Mini failed
 * @retval  -EAGAIN     DFPlayer responded with error "Device busy"
 * @retval  -ETIMEDOUT  Response of the DFPlayer timed out
 * @retval  -ERANGE     The next song is out of range (depending to the
 *                      currently used naming scheme)
 * @retval  -ENOENT     No such file
 *
 * If previously @ref dfplayer_play_file was used to start playback of
 * file "42/113.mp3", this call will try to start "42/112.mp3". If previously
 * @ref dfplayer_play_from_mp3 was used to start playback of file
 * "MP3/1337.mp3", this call will try to start "MP3/1336.mp3".
 */
static inline int dfplayer_prev(dfplayer_t *dev);

/**
 * @brief   Step forward/backward following the currently used naming scheme
 *
 * @param   dev         Device descriptor of the DFPlayer to control
 * @param   step        Steps to take (negative for previous files)
 *
 * @retval  0           Success
 * @retval  -EINVAL     Called with invalid parameter
 * @retval  -EIO        Communication with the DFPlayer Mini failed
 * @retval  -EAGAIN     DFPlayer responded with error "Device busy"
 * @retval  -ETIMEDOUT  Response of the DFPlayer timed out
 * @retval  -ERANGE     The next song is out of range (depending to the
 *                      currently used naming scheme)
 * @retval  -ENOENT     No such file
 *
 * Calling with a value of `1` for @p step is equivalent to @ref dfplayer_next,
 * calling with a value of `-1` for @p step is equivalent to @ref dfplayer_prev.
 * A value of `0` will restart the currently played song
 */
int dfplayer_step(dfplayer_t *dev, int step);

/**
 * @brief   Sets the volume to the given value
 *
 * @param   dev         Device descriptor of the DFPlayer to control
 * @param   volume      Volume to set (max 30)
 *
 * @retval  0           Success
 * @retval  -EINVAL     Called with invalid parameters
 * @retval  -EIO        Communication with the DFPlayer Mini failed
 * @retval  -EAGAIN     DFPlayer responded with error "Device busy"
 * @retval  -ETIMEDOUT  Response of the DFPlayer timed out
 *
 * If @p volume is greater than the maximum allowed volume of 30, 30 will be
 * send instead.
 */
static inline int dfplayer_set_volume(dfplayer_t *dev, uint8_t volume);

/**
 * @brief   Apply the given equalizer setting
 *
 * @param   dev         Device descriptor of the DFPlayer to control
 * @param   equalizer   The equalizer setting to apply
 *
 * @retval  0           Success
 * @retval  -EINVAL     Called with invalid parameters
 * @retval  -EIO        Communication with the DFPlayer Mini failed
 * @retval  -EAGAIN     DFPlayer responded with error "Device busy"
 * @retval  -ETIMEDOUT  Response of the DFPlayer timed out
 */
static inline int dfplayer_set_equalizer(dfplayer_t *dev,
                                         dfplayer_eq_t equalizer);

/**
 * @brief   Apply the given source
 *
 * @param   dev         Device descriptor of the DFPlayer to control
 * @param   src         The source to use for playback
 *
 * @retval  0           Success
 * @retval  -EINVAL     Called with invalid parameters
 * @retval  -EIO        Communication with the DFPlayer Mini failed
 * @retval  -EAGAIN     DFPlayer responded with error "Device busy"
 * @retval  -ETIMEDOUT  Response of the DFPlayer timed out
 */
static inline int dfplayer_set_source(dfplayer_t *dev, dfplayer_source_t src);

/**
 * @brief   Enter standby mode
 *
 * @param   dev         Device descriptor of the DFPlayer to control
 *
 * @retval  0           Success
 * @retval  -EINVAL     Called with invalid parameters
 * @retval  -EIO        Communication with the DFPlayer Mini failed
 * @retval  -EAGAIN     DFPlayer responded with error "Device busy"
 * @retval  -ETIMEDOUT  Response of the DFPlayer timed out
 */
static inline int dfplayer_enter_standby(dfplayer_t *dev);

/**
 * @brief   Exit standby mode
 *
 * @param   dev         Device descriptor of the DFPlayer to control
 *
 * @retval  0           Success
 * @retval  -EINVAL     Called with invalid parameters
 * @retval  -EIO        Communication with the DFPlayer Mini failed
 * @retval  -EAGAIN     DFPlayer responded with error "Device busy"
 * @retval  -ETIMEDOUT  Response of the DFPlayer timed out
 */
static inline int dfplayer_exit_standby(dfplayer_t *dev);

/**
 * @brief   Start/resume playing
 *
 * @param   dev         Device descriptor of the DFPlayer to control
 *
 * @retval  0           Success
 * @retval  -EINVAL     Called with invalid parameter
 * @retval  -EIO        Communication with the DFPlayer Mini failed
 * @retval  -EAGAIN     DFPlayer responded with error "Device busy"
 * @retval  -ETIMEDOUT  Response of the DFPlayer timed out
 */
static inline int dfplayer_play(dfplayer_t *dev);

/**
 * @brief   Pause the playback
 *
 * @param   dev         Device descriptor of the DFPlayer to control
 *
 * @retval  0           Success
 * @retval  -EINVAL     Called with invalid parameter
 * @retval  -EIO        Communication with the DFPlayer Mini failed
 * @retval  -EAGAIN     DFPlayer responded with error "Device busy"
 * @retval  -ETIMEDOUT  Response of the DFPlayer timed out
 */
static inline int dfplayer_pause(dfplayer_t *dev);

/**
 * @brief   Start playing the specified file in the specified folder
 *
 * @param   dev         Device descriptor of the DFPlayer to control
 * @param   folder      Number of the folder
 * @param   file        Number of the file
 *
 * @retval  0           Success
 * @retval  -EINVAL     Called with invalid parameters (see precondition)
 * @retval  -EIO        Communication with the DFPlayer Mini failed
 * @retval  -EAGAIN     DFPlayer responded with error "Device busy"
 * @retval  -ETIMEDOUT  Response of the DFPlayer timed out
 * @retval  -ENOENT     Specified file and/or folder does not exist
 *
 * @pre     `0 < folder <= 100` and `file > 0`
 *
 * E.g. when called with @p folder set to `9` and @p file set to `42`, the
 * file `"09/042.mp3"` is played. Thus, the folder and file names need to
 * follow a specific naming convention in order to be selectable with this
 * function.
 */
int dfplayer_play_file(dfplayer_t *dev, uint8_t folder, uint8_t file);

/**
 * @brief   Start playing the specified number in the MP3 folder
 *
 * @param   dev         Device descriptor of the DFPlayer to control
 * @param   number      Number of the file in the folder `"MP3"` to play
 *
 * @retval  0           Success
 * @retval  -EINVAL     Called with invalid parameters (see precondition)
 * @retval  -EIO        Communication with the DFPlayer Mini failed
 * @retval  -EAGAIN     DFPlayer responded with error "Device busy"
 * @retval  -ETIMEDOUT  Response of the DFPlayer timed out
 * @retval  -ENOENT     Specified number does not exist
 *
 * @pre     `0 < number <= 9999`
 *
 * E.g. when called with @p number set to `42`, the file `"MP3/0042.mp3"` is
 * played. Thus, the folder and file names need to follow a specific naming
 * convention in order to be selectable with this function.
 */
int dfplayer_play_from_mp3(dfplayer_t *dev, uint16_t number);

/**
 * @brief   Start playing the specified number in the ADVERT folder
 *
 * @param   dev         Device descriptor of the DFPlayer to control
 * @param   number      Number of the number in the folder `"ADVERT"` to play
 *
 * @retval  0           Success
 * @retval  -EINVAL     Called with invalid parameters (see precondition)
 * @retval  -EIO        Communication with the DFPlayer Mini failed
 * @retval  -EAGAIN     DFPlayer responded with error "Device busy"
 * @retval  -ETIMEDOUT  Response of the DFPlayer timed out
 * @retval  -ENOENT     Specified number does not exist
 *
 * @pre     `0 < number <= 9999`
 * @warning The playback is only started when the DFPlayer is currently playing
 *          a non-advert file. The current playback is paused, the advert-file
 *          is played, and the previous playback is resumed afterwards
 * @note    While this feature was obviously added to allow playing
 *          advertisements, this function was provided in the best hope it is
 *          not used this way.
 *
 * E.g. when called with @p number set to `42`, the file `"ADVERT/0042.mp3"` is
 * played. Thus, the folder and file names need to follow a specific naming
 * convention in order to be selectable with this function.
 *
 * The most obvious use (apart for advertisements `:-/`) is to use it for
 * audible feedback to control commands. E.g. when the user changes the volume,
 * a short "boing" sound could be played. That would allow the user to perceive
 * the configured volume, even if currently played song is silent while
 * configuring the volume.
 */
int dfplayer_play_from_advert(dfplayer_t *dev, uint16_t number);

/**
 * @brief   Stop playing a file from the ADVERT folder and resume previous playback
 *
 * @param   dev         Device descriptor of the DFPlayer to control
 *
 * @retval  0           Success
 * @retval  -EINVAL     Called with invalid parameters (see precondition)
 * @retval  -EIO        Communication with the DFPlayer Mini failed
 * @retval  -EAGAIN     DFPlayer responded with error "Device busy"
 * @retval  -ETIMEDOUT  Response of the DFPlayer timed out
 */
static inline int dfplayer_stop_advert(dfplayer_t *dev);

/**
 * @brief   Start playing and repeating the specified folder
 *
 * @param   dev         Device descriptor of the DFPlayer to control
 * @param   folder      Number of the folder to play and repeat
 *
 * @retval  0           Success
 * @retval  -EINVAL     Called with invalid parameters (see precondition)
 * @retval  -EIO        Communication with the DFPlayer Mini failed
 * @retval  -EAGAIN     DFPlayer responded with error "Device busy"
 * @retval  -ETIMEDOUT  Response of the DFPlayer timed out
 * @retval  -ENOENT     Specified file and/or folder does not exist
 *
 * @pre     `0 < folder <= 100` and `file > 0`
 */
static inline int dfplayer_repeat_folder(dfplayer_t *dev, uint8_t folder);

/**
 * @brief   Launch shuffle playback of all files
 *
 * @param   dev         Device descriptor of the DFPlayer to control
 *
 * @retval  0           Success
 * @retval  -EINVAL     Called with invalid parameters (see precondition)
 * @retval  -EIO        Communication with the DFPlayer Mini failed
 * @retval  -EAGAIN     DFPlayer responded with error "Device busy"
 * @retval  -ETIMEDOUT  Response of the DFPlayer timed out
 * @retval  -ENOENT     Specified file and/or folder does not exist
 *
 * @warning Even files in the `"ADVERT"` folder are played
 */
static inline int dfplayer_shuffle_all(dfplayer_t *dev);

/**
 * @brief   Enable or disable repeat playback
 *
 * @param   dev         Device descriptor of the DFPlayer to control
 * @param   repeat      Enable repeat playback?
 *
 * @retval  0           Success
 * @retval  -EINVAL     Called with invalid parameters (see precondition)
 * @retval  -EIO        Communication with the DFPlayer Mini failed
 * @retval  -EAGAIN     DFPlayer responded with error "Device busy"
 * @retval  -ETIMEDOUT  Response of the DFPlayer timed out
 * @retval  -ENOENT     Specified file and/or folder does not exist
 */
static inline int dfplayer_repeat(dfplayer_t *dev, bool repeat);

/**
 * @brief   Query the state of the DFPlayer Mini
 *
 * @param   dev         Device descriptor of the DFPlayer to query
 * @param   state       The state will be stored here
 *
 * @retval  0           Success
 * @retval  -EINVAL     Called with invalid parameters
 * @retval  -EIO        Communication with the DFPlayer Mini failed
 * @retval  -EAGAIN     DFPlayer responded with error "Device busy"
 * @retval  -ETIMEDOUT  Response of the DFPlayer timed out
 *
 * @note    This function will work best when the busy pin is connected and
 *          and configured, as this can avoid UART communication if the device
 *          is currently playing. (Remember that UART communication results
 *          in audible glitches during playback...)
 */
int dfplayer_get_state(dfplayer_t *dev, dfplayer_state_t *state);

/**
 * @brief   Query the current volume of the DFPlayer Mini
 *
 * @param   dev         Device descriptor of the DFPlayer to query
 * @param   volume      The volume will be stored here
 *
 * @retval  0           Success
 * @retval  -EINVAL     Called with invalid parameters
 * @retval  -EIO        Communication with the DFPlayer Mini failed
 * @retval  -EAGAIN     DFPlayer responded with error "Device busy"
 * @retval  -ETIMEDOUT  Response of the DFPlayer timed out
 */
static inline int dfplayer_get_volume(dfplayer_t *dev, uint8_t *volume);

/**
 * @brief   Query the current equalizer setting of the DFPlayer Mini
 *
 * @param   dev         Device descriptor of the DFPlayer to query
 * @param   equalizer   The equalizer setting will be stored here
 *
 * @retval  0           Success
 * @retval  -EINVAL     Called with invalid parameters
 * @retval  -EIO        Communication with the DFPlayer Mini failed
 * @retval  -EAGAIN     DFPlayer responded with error "Device busy"
 * @retval  -ETIMEDOUT  Response of the DFPlayer timed out
 */
static inline int dfplayer_get_equalizer(dfplayer_t *dev,
                                         dfplayer_eq_t *equalizer);

/**
 * @brief   Query the current playback mode of the DFPlayer Mini
 *
 * @param   dev         Device descriptor of the DFPlayer to query
 * @param   mode        The playback mode will be stored here
 *
 * @retval  0           Success
 * @retval  -EINVAL     Called with invalid parameters
 * @retval  -EIO        Communication with the DFPlayer Mini failed
 * @retval  -EAGAIN     DFPlayer responded with error "Device busy"
 * @retval  -ETIMEDOUT  Response of the DFPlayer timed out
 */
static inline int dfplayer_get_mode(dfplayer_t *dev,
                                    dfplayer_mode_t *mode);

/**
 * @brief   Query the software version running on the DFPlayer Mini
 *
 * @param   dev         Device descriptor of the DFPlayer to query
 * @param   version     The software version will be stored here
 *
 * @retval  0           Success
 * @retval  -EINVAL     Called with invalid parameters
 * @retval  -EIO        Communication with the DFPlayer Mini failed
 * @retval  -EAGAIN     DFPlayer responded with error "Device busy"
 * @retval  -ETIMEDOUT  Response of the DFPlayer timed out
 */
static inline int dfplayer_get_version(dfplayer_t *dev, uint16_t *version);

/**
 * @brief   Query the number of files on the USB storage device
 *
 * @param   dev         Device descriptor of the DFPlayer to query
 * @param   files       The number of files will be stored here
 *
 * @retval  0           Success
 * @retval  -EINVAL     Called with invalid parameters
 * @retval  -EIO        Communication with the DFPlayer Mini failed
 * @retval  -EAGAIN     DFPlayer responded with error "Device busy"
 * @retval  -ETIMEDOUT  Response of the DFPlayer timed out
 */
static inline int dfplayer_files_usb(dfplayer_t *dev, uint16_t *files);

/**
 * @brief   Query the number of files on the SD card
 *
 * @param   dev         Device descriptor of the DFPlayer to query
 * @param   files       The number of files will be stored here
 *
 * @retval  0           Success
 * @retval  -EINVAL     Called with invalid parameters
 * @retval  -EIO        Communication with the DFPlayer Mini failed
 * @retval  -EAGAIN     DFPlayer responded with error "Device busy"
 * @retval  -ETIMEDOUT  Response of the DFPlayer timed out
 */
static inline int dfplayer_files_sdcard(dfplayer_t *dev, uint16_t *files);

/**
 * @brief   Query the number of files on the NOR flash
 *
 * @param   dev         Device descriptor of the DFPlayer to query
 * @param   files       The number of files will be stored here
 *
 * @retval  0           Success
 * @retval  -EINVAL     Called with invalid parameters
 * @retval  -EIO        Communication with the DFPlayer Mini failed
 * @retval  -EAGAIN     DFPlayer responded with error "Device busy"
 * @retval  -ETIMEDOUT  Response of the DFPlayer timed out
 */
static inline int dfplayer_files_flash(dfplayer_t *dev, uint16_t *files);

/**
 * @brief   Query the selected file on the USB storage device
 *
 * @warning The file number refers to the internal file system order
 *
 * @param   dev         Device descriptor of the DFPlayer to query
 * @param   fileno      The selected file will be stored here
 *
 * @retval  0           Success
 * @retval  -EINVAL     Called with invalid parameters
 * @retval  -EIO        Communication with the DFPlayer Mini failed
 * @retval  -EAGAIN     DFPlayer responded with error "Device busy"
 * @retval  -ETIMEDOUT  Response of the DFPlayer timed out
 */
static inline int dfplayer_get_fileno_usb(dfplayer_t *dev, uint16_t *fileno);

/**
 * @brief   Query the selected file on the SD card
 *
 * @warning The file number refers to the internal file system order
 *
 * @param   dev         Device descriptor of the DFPlayer to query
 * @param   fileno      The selected file will be stored here
 *
 * @retval  0           Success
 * @retval  -EINVAL     Called with invalid parameters
 * @retval  -EIO        Communication with the DFPlayer Mini failed
 * @retval  -EAGAIN     DFPlayer responded with error "Device busy"
 * @retval  -ETIMEDOUT  Response of the DFPlayer timed out
 */
static inline int dfplayer_get_fileno_sdcard(dfplayer_t *dev, uint16_t *fileno);

/**
 * @brief   Query the selected file on the NOR flash
 *
 * @warning The file number refers to the internal file system order
 *
 * @param   dev         Device descriptor of the DFPlayer to query
 * @param   fileno      The selected file will be stored here
 *
 * @retval  0           Success
 * @retval  -EINVAL     Called with invalid parameters
 * @retval  -EIO        Communication with the DFPlayer Mini failed
 * @retval  -EAGAIN     DFPlayer responded with error "Device busy"
 * @retval  -ETIMEDOUT  Response of the DFPlayer timed out
 */
static inline int dfplayer_get_fileno_flash(dfplayer_t *dev, uint16_t *fileno);

/**
 * @brief   Get the currently played file and the used naming scheme
 *
 * @param   dev         Device descriptor of the DFPlayer to query
 *
 * @return  The currently played file and the used naming scheme
 */
static inline dfplayer_file_t dfplayer_get_played_file(dfplayer_t *dev);

/**
 * @brief   Check if the given source set contains the given source
 *
 * @param   set         Set of sources to check
 * @param   src         Source to check for
 *
 * @retval  0           @p src is ***NOT*** part of @p set
 * @retval  1           @p src ***IS*** part of @p set
 */
static inline int dfplayer_source_set_contains(dfplayer_source_set_t set,
                                               dfplayer_source_t src);

/**
 * @brief   Add the given source to the given source set
 *
 * @param   set         Set of sources to add source to
 * @param   src         Source to add
 *
 * This function is idempotent
 */
static inline void dfplayer_source_set_add(dfplayer_source_set_t *set,
                                           dfplayer_source_t src);

/**
 * @brief   Remove the given source to the given source set
 *
 * @param   set         Set of sources to remove the source from
 * @param   src         Source to remove
 *
 * This function is idempotent
 */
static inline void dfplayer_source_set_rm(dfplayer_source_set_t *set,
                                          dfplayer_source_t src);

#ifdef __cplusplus
}
#endif

/* Include implementation of the static inline functions */
#include "dfplayer_implementation.h"

/** @} */
