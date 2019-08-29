/*
 * Copyright 2019 Marian Buschsieweke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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
 * The MP3 files have to be named with three decimal digits (e.g. `"001.mp3"` or
 * `"042"`) and must be placed in in folders named with two decimal digits
 * (e.g. `"01"` or `"99"`). The folder name numbers must be in the range 1-99
 * and the file name numbers in the range 1-255. (E.g. `"09/099.mp3"` would be
 * a valid path.) Playback of these files can be started using the function
 * @ref dfplayer_play_file
 *
 * Numbered File Inside MP3 Folder Naming Scheme
 * ---------------------------------------------
 *
 * The MP3 files have to be named with four decimal digits (e.g. `"0001.mp3"` or
 * `"1337.mp3") and must be placed in the folder `"MP3"`. The file numbers
 * 1-9999 are valid. Playback of these files can be started using the function
 * @ref dfplayer_play_from_mp3
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
 * Bugs Not Handled by the Driver
 * ------------------------------
 *
 * Any UART communication with the DFPlayer during playback results in
 * audible and unpleasant glitches. The only solution is to not communicate with
 * the device during playback, as the DFPlayer seems to pause playback for some
 * milliseconds after receiving a command. If the busy pin of the DFPlayer is
 * connected to your board and configured, the function @ref dfplayer_get_state
 * will read the busy pin to detect if the DFPlayer is currently playing. Only
 * if according to the busy pin the device is not currently playing, the UART
 * interface is used to detect whether the playback is paused or stopped. Thus,
 * @ref dfplayer_get_state can be used without fearing to cause audio glitches,
 * provided the busy pin is connected and configured. You can use this to
 * poll the state of the DPlayer and issue the commands once playback has
 * completed, or use the callbacks (see @ref dfplayer_set_callbacks) to get
 * notified when playback has completed.
 *
 * Bugs Handled by the Driver
 * --------------------------
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
 * verify that the command succeeded. Otherwise, the driver will report success
 * without having verified that playback succeeded. (Querying the DFPlayer's
 * state over UART would be possible, but would result in audible glitches.)
 *
 * When the DFPlayer completes playback of a file, the next command received
 * is not acknowledged, if it is a control command. Query commands are not
 * affected. The driver works around this bug by querying the volume prior to
 * issuing a control command directly after playback completed.
 *
 * Some versions of the DFPlayer are not able to handle commands send at high
 * frequency. A delay of 100ms is inserted after sending a command, unless you
 * have a DFPlayer version known to not be affected by this bug.
 *
 * @{
 *
 * @file
 * @brief       DFPlayer Mini Device Driver
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef DFPLAYER_H
#define DFPLAYER_H

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

extern dfplayer_t dfplayer_devs[DFPLAYER_NUM];

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
    if (num < DFPLAYER_NUM) {
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
 * @retval  -EINVAL     Invalid paramters
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
 * @param   userdata    Argument to pass to the callback functions
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
                           dfplayer_cb_src_t cb_src, void *userdata);

/**
 * @brief   Start playing the next song in the filesystem
 *
 * @param   dev         Device descriptor of the DFPlayer to control
 *
 * @retval  0           Success
 * @retval  -EINVAL     Called with invalid parameter
 * @retval  -EIO        Communication with the DFPlayer Mini failed
 * @retval  -EAGAIN     DFPlayer responded with error "Device busy"
 * @retval  -ETIMEDOUT  Response of the DFPlayer timed out
 *
 * @warning This will select the next file in the file system, which might
 *          differ from what logically would be the next file.
 * @warning This function messes up with the bookkeeping used by
 *          @ref dfplayer_next, @ref dfplayer_prev, and @ref dfplayer_step
 * @see     dfplayer_next
 */
static inline int dfplayer_next_in_fat(dfplayer_t *dev);

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
 * @brief   Start playing the previous song in the filesystem
 *
 * @param   dev         Device descriptor of the DFPlayer to control
 *
 * @retval  0           Success
 * @retval  -EINVAL     Called with invalid parameter
 * @retval  -EIO        Communication with the DFPlayer Mini failed
 * @retval  -EAGAIN     DFPlayer responded with error "Device busy"
 * @retval  -ETIMEDOUT  Response of the DFPlayer timed out
 *
 * @warning This will select the previous file in the file system, which might
 *          differ from what logically would be the next file.
 * @warning This function messes up with the bookkeeping used by
 *          @ref dfplayer_next, @ref dfplayer_prev, and @ref dfplayer_step
 * @see     dfplayer_prev
 */
static inline int dfplayer_prev_in_fat(dfplayer_t *dev);

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
 * @brief   Increase the volume
 *
 * @param   dev         Device descriptor of the DFPlayer to control
 *
 * @retval  0           Success
 * @retval  -EINVAL     Called with invalid parameters
 * @retval  -EIO        Communication with the DFPlayer Mini failed
 * @retval  -EAGAIN     DFPlayer responded with error "Device busy"
 * @retval  -ETIMEDOUT  Response of the DFPlayer timed out
 */
static inline int dfplayer_volume_increase(dfplayer_t *dev);

/**
 * @brief   Decrease the volume
 *
 * @param   dev         Device descriptor of the DFPlayer to control
 *
 * @retval  0           Success
 * @retval  -EINVAL     Called with invalid parameters
 * @retval  -EIO        Communication with the DFPlayer Mini failed
 * @retval  -EAGAIN     DFPlayer responded with error "Device busy"
 * @retval  -ETIMEDOUT  Response of the DFPlayer timed out
 */
static inline int dfplayer_volume_decrease(dfplayer_t *dev);

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
 * @param   src         The source to use for playback
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
 * @param   src         The source to use for playback
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
 * @brief   Start playing the specified track in the MP3 folder
 *
 * @param   dev         Device descriptor of the DFPlayer to control
 * @param   track       Number of the file in the folder `"MP3"` to play
 *
 * @retval  0           Success
 * @retval  -EINVAL     Called with invalid parameters (see precondition)
 * @retval  -EIO        Communication with the DFPlayer Mini failed
 * @retval  -EAGAIN     DFPlayer responded with error "Device busy"
 * @retval  -ETIMEDOUT  Response of the DFPlayer timed out
 * @retval  -ENOENT     Specified track does not exist
 *
 * @pre     `0 < track <= 9999`
 *
 * E.g. when called with @p track set to `42`, the file `"MP3/0042.mp3"` is
 * played. Thus, the folder and file names need to follow a specific naming
 * convention in order to be selectable with this function.
 */
int dfplayer_play_from_mp3(dfplayer_t *dev, uint16_t track);

/**
 * @brief   Start playing the specified track in the ADVERT folder
 *
 * @param   dev         Device descriptor of the DFPlayer to control
 * @param   track       Number of the track in the folder `"ADVERT"` to play
 *
 * @retval  0           Success
 * @retval  -EINVAL     Called with invalid parameters (see precondition)
 * @retval  -EIO        Communication with the DFPlayer Mini failed
 * @retval  -EAGAIN     DFPlayer responded with error "Device busy"
 * @retval  -ETIMEDOUT  Response of the DFPlayer timed out
 * @retval  -ENOENT     Specified track does not exist
 *
 * @pre     `0 < track <= 9999`
 * @warning The playback is only started when the DFPlayer is currently playing
 *          a non-advert file. The current playback is paused, the advert-file
 *          is played, and the previous playback is resumed afterwards
 * @note    While this feature was obviously added to allow playing
 *          advertisements, this function was provided in the best hope it is
 *          not used this way.
 *
 * E.g. when called with @p track set to `42`, the file `"ADVERT/0042.mp3"` is
 * played. Thus, the folder and file names need to follow a specific naming
 * convention in order to be selectable with this function.
 *
 * The most obvious use (apart for advertisements `:-/`) is to use it for
 * audible feedback to control commands. E.g. when the user changes the volume,
 * a short "boing" sound could be played. That would allow the user to perceive
 * the configured volume, even if currently played song is silent while
 * configuring the volume.
 */
int dfplayer_play_from_advert(dfplayer_t *dev, uint16_t track);

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
 * @brief   Query the selected track on the USB storage device
 *
 * @param   dev         Device descriptor of the DFPlayer to query
 * @param   track       The selected track will be stored here
 *
 * @retval  0           Success
 * @retval  -EINVAL     Called with invalid parameters
 * @retval  -EIO        Communication with the DFPlayer Mini failed
 * @retval  -EAGAIN     DFPlayer responded with error "Device busy"
 * @retval  -ETIMEDOUT  Response of the DFPlayer timed out
 */
static inline int dfplayer_get_track_usb(dfplayer_t *dev, uint16_t *track);

/**
 * @brief   Query the selected track on the SD card
 *
 * @param   dev         Device descriptor of the DFPlayer to query
 * @param   track       The selected track will be stored here
 *
 * @retval  0           Success
 * @retval  -EINVAL     Called with invalid parameters
 * @retval  -EIO        Communication with the DFPlayer Mini failed
 * @retval  -EAGAIN     DFPlayer responded with error "Device busy"
 * @retval  -ETIMEDOUT  Response of the DFPlayer timed out
 */
static inline int dfplayer_get_track_sdcard(dfplayer_t *dev, uint16_t *track);

/**
 * @brief   Query the selected track on the NOR flash
 *
 * @param   dev         Device descriptor of the DFPlayer to query
 * @param   track       The selected track will be stored here
 *
 * @retval  0           Success
 * @retval  -EINVAL     Called with invalid parameters
 * @retval  -EIO        Communication with the DFPlayer Mini failed
 * @retval  -EAGAIN     DFPlayer responded with error "Device busy"
 * @retval  -ETIMEDOUT  Response of the DFPlayer timed out
 */
static inline int dfplayer_get_track_flash(dfplayer_t *dev, uint16_t *track);

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
static inline void dfplayer_source_set_add(dfplayer_source_set_t set,
                                           dfplayer_source_t src);

/**
 * @brief   Remove the given source to the given source set
 *
 * @param   set         Set of sources to remove the source from
 * @param   src         Source to remove
 *
 * This function is idempotent
 */
static inline void dfplayer_source_set_rm(dfplayer_source_set_t set,
                                          dfplayer_source_t src);

#ifdef __cplusplus
}
#endif

/* Include implementation of the static inline functions */
#include "dfplayer_implementation.h"

#endif /* DFPLAYER_H */
/** @} */
