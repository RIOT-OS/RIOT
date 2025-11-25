/*
 * Copyright 2019 Marian Buschsieweke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_dfplayer
 * @{
 *
 * @file
 * @brief       Header-only functions of the DFPlayer Mini Device driver
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */
#ifndef DFPLAYER_IMPLEMENTATION_H
#define DFPLAYER_IMPLEMENTATION_H

#include <errno.h>

#include "dfplayer.h"
#include "dfplayer_constants.h"
#include "dfplayer_internal.h"
#include "dfplayer_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* This file contains the implementations of the functions whose signature and
 * Doxygen documentation is given in `drivers/include/dfplayer.h`. Doxygen
 * sadly doesn't detect this and creates two entries for every function; with
 * one entry being undocumented. We just hide the implementations here from
 * Doxygen to prevent the duplicate entries
 */
#ifndef DOXYGEN
static inline int dfplayer_next(dfplayer_t *dev)
{
    return dfplayer_step(dev, 1);
}

static inline int dfplayer_prev(dfplayer_t *dev)
{
    return dfplayer_step(dev, -1);
}

static inline int dfplayer_set_volume(dfplayer_t *dev, uint8_t volume)
{
    if (volume > DFPLAYER_MAX_VOLUME) {
        volume = DFPLAYER_MAX_VOLUME;
    }
    return dfplayer_cmd_1param(dev, DFPLAYER_CMD_SET_VOLUME, volume);
}

static inline int dfplayer_set_equalizer(dfplayer_t *dev,
                                         dfplayer_eq_t equalizer)
{
    if ((unsigned)equalizer >= (unsigned)DFPLAYER_EQ_NUMOF) {
        return -EINVAL;
    }

    return dfplayer_cmd_1param(dev, DFPLAYER_CMD_SET_EQUALIZER,
                               (uint8_t)equalizer);
}

static inline int dfplayer_set_source(dfplayer_t *dev, dfplayer_source_t src)
{
    if ((unsigned)src >= (unsigned)DFPLAYER_SOURCE_NUMOF) {
        return -EINVAL;
    }

    return dfplayer_cmd_1param(dev, DFPLAYER_CMD_SET_SOURCE, (uint8_t)src);
}

static inline int dfplayer_enter_standby(dfplayer_t *dev)
{
    return dfplayer_cmd(dev, DFPLAYER_CMD_STANDBY_ENTER);
}

static inline int dfplayer_exit_standby(dfplayer_t *dev)
{
    return dfplayer_cmd(dev, DFPLAYER_CMD_STANDBY_EXIT);
}

static inline int dfplayer_play(dfplayer_t *dev)
{
    return dfplayer_cmd(dev, DFPLAYER_CMD_PLAY);
}

static inline int dfplayer_pause(dfplayer_t *dev)
{
    return dfplayer_cmd(dev, DFPLAYER_CMD_PAUSE);
}

static inline int dfplayer_stop_advert(dfplayer_t *dev)
{
    return dfplayer_cmd(dev, DFPLAYER_CMD_ABORT_ADVERT);
}

static inline int dfplayer_repeat_folder(dfplayer_t *dev, uint8_t folder)
{
    return dfplayer_file_cmd(dev, DFPLAYER_CMD_REPEAT_FOLDER, 0, folder);
}

static inline int dfplayer_shuffle_all(dfplayer_t *dev)
{
    return dfplayer_cmd(dev, DFPLAYER_CMD_RANDOM);
}

static inline int dfplayer_repeat(dfplayer_t *dev, bool repeat)
{
    return dfplayer_cmd_1param(dev, DFPLAYER_CMD_REPEAT, (uint8_t)repeat);
}

static inline int dfplayer_get_volume(dfplayer_t *dev, uint8_t *volume)
{
    if (!volume) {
        return -EINVAL;
    }

    uint16_t tmp;
    int retval = dfplayer_query(dev, &tmp, DFPLAYER_CMD_GET_VOLUME);
    if (retval) {
        return retval;
    }

    *volume = (uint8_t)tmp;
    return 0;
}

static inline int dfplayer_get_equalizer(dfplayer_t *dev,
                                         dfplayer_eq_t *equalizer)
{
    if (!equalizer) {
        return -EINVAL;
    }

    uint16_t tmp;
    int retval = dfplayer_query(dev, &tmp, DFPLAYER_CMD_GET_EQUALIZER);
    if (retval) {
        return retval;
    }

    *equalizer = (dfplayer_eq_t)tmp;
    return 0;
}

static inline int dfplayer_get_mode(dfplayer_t *dev,
                                    dfplayer_mode_t *mode)
{
    if (!mode) {
        return -EINVAL;
    }

    uint16_t tmp;
    int retval = dfplayer_query(dev, &tmp, DFPLAYER_CMD_GET_MODE);
    if (retval) {
        return retval;
    }

    *mode = (dfplayer_mode_t)tmp;
    return 0;
}

static inline int dfplayer_get_version(dfplayer_t *dev, uint16_t *version)
{
    if (!version) {
        return -EINVAL;
    }

    return dfplayer_query(dev, version, DFPLAYER_CMD_GET_VERSION);
}

static inline int dfplayer_files_usb(dfplayer_t *dev, uint16_t *files)
{
    if (!files) {
        return -EINVAL;
    }

    return dfplayer_query(dev, files, DFPLAYER_CMD_FILES_USB);
}

static inline int dfplayer_files_sdcard(dfplayer_t *dev, uint16_t *files)
{
    if (!files) {
        return -EINVAL;
    }

    return dfplayer_query(dev, files, DFPLAYER_CMD_FILES_SDCARD);
}

static inline int dfplayer_files_flash(dfplayer_t *dev, uint16_t *files)
{
    if (!files) {
        return -EINVAL;
    }

    return dfplayer_query(dev, files, DFPLAYER_CMD_FILES_FLASH);
}

static inline int dfplayer_get_fileno_usb(dfplayer_t *dev, uint16_t *fileno)
{
    if (!fileno) {
        return -EINVAL;
    }

    return dfplayer_query(dev, fileno, DFPLAYER_CMD_FILENO_USB);
}

static inline int dfplayer_get_fileno_sdcard(dfplayer_t *dev, uint16_t *fileno)
{
    if (!fileno) {
        return -EINVAL;
    }

    return dfplayer_query(dev, fileno, DFPLAYER_CMD_FILENO_SDCARD);
}

static inline int dfplayer_get_fileno_flash(dfplayer_t *dev, uint16_t *fileno)
{
    if (!fileno) {
        return -EINVAL;
    }

    return dfplayer_query(dev, fileno, DFPLAYER_CMD_FILENO_FLASH);
}

static inline dfplayer_file_t dfplayer_get_played_file(dfplayer_t *dev)
{
    mutex_lock(&dev->mutex);
    dfplayer_file_t res = dev->file;
    mutex_unlock(&dev->mutex);
    return res;
}

static inline int dfplayer_source_set_contains(dfplayer_source_set_t set,
                                               dfplayer_source_t src)
{
    return (set & (0x01 << src)) ? 1 : 0;
}

static inline void dfplayer_source_set_add(dfplayer_source_set_t *set,
                                           dfplayer_source_t src)
{
    *set |= 0x01 << src;
}

static inline void dfplayer_source_set_rm(dfplayer_source_set_t *set,
                                          dfplayer_source_t src)
{
    *set &= ~((dfplayer_source_set_t)(0x01 << src));
}
#endif /* !DOXYGEN */

#ifdef __cplusplus
}
#endif

#endif /* DFPLAYER_IMPLEMENTATION_H */
/** @} */
