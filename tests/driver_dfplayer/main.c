/*
 * Copyright (C) 2019 Marian Buschsieweke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
#include "fmt.h"
#include "shell.h"

static const char *_sources[] = {
    [DFPLAYER_SOURCE_USB]       = "USB",
    [DFPLAYER_SOURCE_SDCARD]    = "SD card",
    [DFPLAYER_SOURCE_FLASH]     = "Flash",
    [DFPLAYER_SOURCE_NUMOF]     = NULL,
};

static void _cb_done(dfplayer_source_t src, uint16_t track, void *data)
{
    (void)data;
    const char *dev = _sources[src];
    if (!dev) {
        dev = "unknown/error";
    }

    print_str("Playback of track ");
    print_u32_dec(track);
    print_str(" on device ");
    print_str(dev);
    print_str(" has completed\n");
}

static void _cb_src(dfplayer_source_set_t srcs, void *data)
{
    (void)data;
    print_str("List of available playback sources changed. New list:\n");

    print_str("USB: ");
    if (dfplayer_source_set_contains(srcs, DFPLAYER_SOURCE_USB)) {
        print_str("Attached\n");
    }
    else {
        print_str("-\n");
    }

    print_str("SD card: ");
    if (dfplayer_source_set_contains(srcs, DFPLAYER_SOURCE_SDCARD)) {
        print_str("Attached\n");
    }
    else {
        print_str("-\n");
    }

    print_str("Flash: ");
    if (dfplayer_source_set_contains(srcs, DFPLAYER_SOURCE_FLASH)) {
        print_str("Attached\n");
    }
    else {
        print_str("-\n");
    }
}

int main(void)
{
    print_str(
        "DFPlayer Mini Driver Test\n"
        "=========================\n"
        "\n"
        "Experiment with the shell command \"dfplayer\"\n\n"
    );

    if (dfplayer_set_callbacks(dfplayer_get(0), _cb_done, _cb_src, NULL)) {
        print_str("Error: Failed to register callbacks\n");
    }

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
