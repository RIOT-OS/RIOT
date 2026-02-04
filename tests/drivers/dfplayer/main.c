/*
 * SPDX-FileCopyrightText: 2019 Marian Buschsieweke
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief   Test application of the DFPlayer Mini driver
 *
 * @author  Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <stdio.h>

#include "dfplayer.h"
#include "event.h"
#include "event/thread.h"
#include "fmt.h"
#include "shell.h"
#include "xtimer.h"

static const char *_sources[] = {
    [DFPLAYER_SOURCE_USB]       = "USB",
    [DFPLAYER_SOURCE_SDCARD]    = "SD card",
    [DFPLAYER_SOURCE_FLASH]     = "Flash",
    [DFPLAYER_SOURCE_NUMOF]     = NULL,
};

typedef struct {
    event_t event;
    dfplayer_source_t src;
    uint16_t track;
} playback_done_event_t;

typedef struct {
    event_t event;
    dfplayer_source_set_t sources;
} media_changed_event_t;

static void playback_done_handler(event_t *_data)
{
    playback_done_event_t *data = (playback_done_event_t *)_data;

    const char *source = _sources[data->src];
    if (!source) {
        source = "unknown/error";
    }

    print_str("Playback of track ");
    print_u32_dec(data->track);
    print_str(" on device ");
    print_str(source);
    print_str(" has completed\n");

    dfplayer_t *dev = dfplayer_get(0);
    dfplayer_file_t track = dfplayer_get_played_file(dev);

    char num[8];
    size_t len;
    switch (track.scheme) {
        default:
            return;
        case DFPLAYER_SCHEME_FOLDER_FILE:
            len = fmt_u16_dec(num, track.folder);
            for (size_t i = len; i < 2; i++) {
                print_str("0");
            }
            print(num, len);
            print_str("/");
            len = fmt_u16_dec(num, track.file);
            for (size_t i = len; i < 3; i++) {
                print_str("0");
            }
            print(num, len);
            break;
        case DFPLAYER_SCHEME_MP3_FILE:
            len = fmt_u16_dec(num, track.number);
            for (size_t i = len; i < 4; i++) {
                print_str("0");
            }
            print(num, len);
            break;
    }
    print_str(".mp3: Playback completed\n");
    xtimer_msleep(DFPLAYER_TIMEOUT_MS);
    dfplayer_next(dev);
}

static void _cb_done(dfplayer_t *dev, dfplayer_source_t src, uint16_t track)
{
    (void)dev;
    static playback_done_event_t event = {
        .event = { .handler = playback_done_handler }
    };

    event.src = src;
    event.track = track;
    event_post(EVENT_PRIO_LOWEST, &event.event);
}

static void media_changed_handler(event_t *_data)
{
    media_changed_event_t *data = (media_changed_event_t *)_data;
    print_str("List of available playback sources changed. New list:\n");

    print_str("USB: ");
    if (dfplayer_source_set_contains(data->sources, DFPLAYER_SOURCE_USB)) {
        print_str("Attached\n");
    }
    else {
        print_str("-\n");
    }

    print_str("SD card: ");
    if (dfplayer_source_set_contains(data->sources, DFPLAYER_SOURCE_SDCARD)) {
        print_str("Attached\n");
    }
    else {
        print_str("-\n");
    }

    print_str("Flash: ");
    if (dfplayer_source_set_contains(data->sources, DFPLAYER_SOURCE_FLASH)) {
        print_str("Attached\n");
    }
    else {
        print_str("-\n");
    }
}

static void _cb_src(dfplayer_t *dev, dfplayer_source_set_t srcs)
{
    (void)dev;
    static media_changed_event_t event = {
        .event = { .handler = media_changed_handler }
    };

    event.sources = srcs;
    event_post(EVENT_PRIO_LOWEST, &event.event);
}

int main(void)
{
    print_str(
        "DFPlayer Mini Driver Test\n"
        "=========================\n"
        "\n"
        "Experiment with the shell command \"dfplayer\"\n\n"
    );

    if (dfplayer_set_callbacks(dfplayer_get(0), _cb_done, _cb_src)) {
        print_str("Error: Failed to register callbacks\n");
    }

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
