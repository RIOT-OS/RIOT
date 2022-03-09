/*
 * Copyright 2019 Marian Buschsieweke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_shell_commands
 * @{
 *
 * @file
 * @brief       Provides a shell command to control a DFPlayer Mini
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "dfplayer.h"
#include "dfplayer_internal.h"
#include "fmt.h"

static const char *_equalizers[] = {
    [DFPLAYER_EQ_NORMAL]    = "normal",
    [DFPLAYER_EQ_POP]       = "pop",
    [DFPLAYER_EQ_ROCK]      = "rock",
    [DFPLAYER_EQ_JAZZ]      = "jazz",
    [DFPLAYER_EQ_CLASSIC]   = "classic",
    [DFPLAYER_EQ_BASE]      = "base",
    [DFPLAYER_EQ_NUMOF]     = NULL
};

static const char *_modes[] = {
    [DFPLAYER_MODE_UNKOWN]      = "unknown",
    [DFPLAYER_MODE_REPEAT_DIR]  = "repeat folder",
    [DFPLAYER_MODE_REPEAT]      = "repeat",
    [DFPLAYER_MODE_RANDOM]      = "random",
    [DFPLAYER_MODE_NORMAL]      = "normal",
    [DFPLAYER_MODE_NUMOF]       = NULL
};

static const char *_states[] = {
    [DFPLAYER_STATE_PLAYING]    = "playing",
    [DFPLAYER_STATE_PAUSED]     = "paused",
    [DFPLAYER_STATE_STOPPED]    = "stopped",
    [DFPLAYER_STATE_NUMOF]      = NULL
};

static void _print_error(int retval)
{
    print_str("Error: ");
#ifdef CONFIG_DFPLAYER_NO_STRERROR
    print_s32_dec(retval);
#else
    print_str(strerror(-retval));
#endif
    print("\n", 1);
}

static void _print_state(dfplayer_t *dev)
{
    dfplayer_state_t state;
    print_str("State: ");
    int retval = dfplayer_get_state(dev, &state);
    if (retval) {
        _print_error(retval);
    }
    else {
        print_str(_states[state]);
        print("\n", 1);
    }
}

static void _print_tracks(dfplayer_t *dev)
{
    dfplayer_source_set_t srcs = dfplayer_get_sources(dev);
    print_str("Selected file numbers (in file system):\n");

    if (dfplayer_source_set_contains(srcs, DFPLAYER_SOURCE_USB)) {
        uint16_t fileno;
        int retval = dfplayer_get_fileno_usb(dev, &fileno);
        print_str("  - USB: ");
        if (retval) {
            _print_error(retval);
        }
        else {
            print_u32_dec(fileno);
            print("\n", 1);
        }
    }

    if (dfplayer_source_set_contains(srcs, DFPLAYER_SOURCE_SDCARD)) {
        uint16_t fileno;
        int retval = dfplayer_get_fileno_sdcard(dev, &fileno);
        print_str("  - SD card: ");
        if (retval) {
            _print_error(retval);
        }
        else {
            print_u32_dec(fileno);
            print("\n", 1);
        }
    }

    if (dfplayer_source_set_contains(srcs, DFPLAYER_SOURCE_FLASH)) {
        uint16_t fileno;
        int retval = dfplayer_get_fileno_sdcard(dev, &fileno);
        print_str("  - NOR flash: ");
        if (retval) {
            _print_error(retval);
        }
        else {
            print_u32_dec(fileno);
            print("\n", 1);
        }
    }

    print_str("Current track: ");
    dfplayer_file_t file = dfplayer_get_played_file(dev);
    if (file.scheme == DFPLAYER_SCHEME_FOLDER_FILE) {
        print_u32_dec(file.folder);
        print_str("/");
        print_u32_dec(file.file);
    }
    else {
        print_u32_dec(file.number);
    }

    print("\n", 1);
}

static void _print_volume(dfplayer_t *dev)
{
    uint8_t volume = 0;
    int retval = dfplayer_get_volume(dev, &volume);
    print_str("Volume: ");
    if (retval) {
        _print_error(retval);
    }
    else {
        print_u32_dec(volume);
        print("\n", 1);
    }
}

static void _print_equalizer(dfplayer_t *dev)
{
    dfplayer_eq_t equalizer = 0;
    int retval = dfplayer_get_equalizer(dev, &equalizer);
    print_str("Equalizer: ");
    if (retval) {
        _print_error(retval);
    }
    else {
        if ((unsigned)equalizer >= (unsigned)DFPLAYER_EQ_NUMOF) {
            print_str("Invalid response!?!\n");
        }
        else {
            print_str(_equalizers[equalizer]);
            print("\n", 1);
        }
    }
}

static void _print_mode(dfplayer_t *dev)
{
    dfplayer_mode_t mode;
    int retval = dfplayer_get_mode(dev, &mode);
    if (retval) {
        _print_error(retval);
    }
    else {
        print_str("Playback mode: ");
        print_str(_modes[mode]);
        print("\n", 1);
    }
}

static void _print_files(dfplayer_t *dev)
{
    dfplayer_source_set_t srcs = dfplayer_get_sources(dev);
    uint16_t files;
    int retval;

    print_str("USB: ");
    if (dfplayer_source_set_contains(srcs, DFPLAYER_SOURCE_USB)) {
        retval = dfplayer_files_usb(dev, &files);
        if (retval) {
            _print_error(retval);
        }
        else {
            print_u32_dec(files);
            print("\n", 1);
        }
    }
    else {
        print_str("Not present\n");
    }

    print_str("SD card: ");
    if (dfplayer_source_set_contains(srcs, DFPLAYER_SOURCE_SDCARD)) {
        retval = dfplayer_files_sdcard(dev, &files);
        if (retval) {
            _print_error(retval);
        }
        else {
            print_u32_dec(files);
            print("\n", 1);
        }
    }
    else {
        print_str("Not present\n");
    }

    print_str("Flash: ");
    if (dfplayer_source_set_contains(srcs, DFPLAYER_SOURCE_FLASH)) {
        retval = dfplayer_files_flash(dev, &files);
        if (retval) {
            _print_error(retval);
        }
        else {
            print_u32_dec(files);
            print("\n", 1);
        }
    }
    else {
        print_str("Not present\n");
    }
}

static void _print_status(dfplayer_t *dev)
{
    uint16_t version;
    int retval;

    print_str("Software version: ");
    retval = dfplayer_get_version(dev, &version);
    if (retval) {
        _print_error(retval);
    }
    else {
        print_u32_dec(version);
        print("\n", 1);
    }

    _print_state(dev);
    _print_volume(dev);
    _print_tracks(dev);
    _print_mode(dev);
    _print_files(dev);
}

static void _print_usage(const char *progname)
{
    print_str("Usage: ");
    print_str(progname);
    print_str(" [-d <DEV_NUM>] [CMD [PARAMS]]\n");
}

static void _print_help(const char *progname)
{
    _print_usage(progname);
    print_str("Run ");
    print_str(progname);
    print_str(" without command to query the current status\n");
    print_str(
        "\n"
        "Commands:\n"
        "    play                Start playback\n"
        "    pause               Pause playing\n"
        "    next                Start playing next song\n"
        "    prev                Start playing previous song\n"
        "    track               Print the currently played track\n"
        "    track <VAL>         Select track with number <VAL>\n"
        "    files               Print the number of files present\n"
        "    file <DIR> <FILE>   Play file number <FILE> in folder <DIR>\n"
        "                        (E.g. \"09/042.mp3\" is selected with \""
    );
    print_str(progname);
    print_str(" file 9 42\")\n");
    print_str(
        "    volume              Print current volume\n"
        "    volume <VAL>        Set volume to <VAL> (0 - 30)\n"
        "    source <VAL>        Set source to <VAL> (USB, SD, FLASH)\n"
        "    equalizer           Print current equalizer setting\n"
        "    equalizer <VAL>     Set equalizer to <VAL>\n"
        "    mode                Print the current playback mode\n"
        "    repeat <1 / 0>      Enable (1) or disable (0) repeat\n"
        "    repeat-folder <VAL> Play and repeat folder number <VAL>\n"
        "    state               Print the DFPlayer's state\n"
        "    shuffle             Play all files at random\n"
        "    mp3 <VAL>           Play file number <VAL> from \"MP3\" folder\n"
        "    advert <VAL>        Play file number <VAL> from \"ADVERT\" folder\n"
        "                        (Only works during playback, previous playback\n"
        "                        is resumed afterwards)\n"
        "    stop-advert         During advert, resume previous playback now\n"
        "    cmd <CMD> [[P1] P2] Run command <CMD> with parameters P1 and P2\n"
    );
}

int _sc_dfplayer(int argc, char **argv)
{
    unsigned dev_num = 0;
    int pos = 1;
    dfplayer_t *dev;

    if ((argc > 1) && (!strcmp("-d", argv[1]))) {
        if (argc < 3) {
            print_str("Missing device number for parameter \"-d\"\n\n");
            _print_usage(argv[0]);
            return 1;
        }
        dev_num = atoi(argv[2]);
        pos = 3;
    }

    dev = dfplayer_get(dev_num);

    if (!dev) {
        print_str("Error: No DFPlayer Mini device with number ");
        print_u32_dec(dev_num);
        print("\n", 1);
        return 1;
    }

    if (argc == pos) {
        _print_status(dev);
        return 0;
    }

    if (!strcmp("play", argv[pos])) {
        int retval = dfplayer_play(dev);
        if (retval) {
            _print_error(retval);
            return 1;
        }
        return 0;
    }

    if (!strcmp("pause", argv[pos])) {
        int retval = dfplayer_pause(dev);
        if (retval) {
            _print_error(retval);
            return 1;
        }
        return 0;
    }

    if (!strcmp("next", argv[pos])) {
        int retval = dfplayer_next(dev);
        if (retval) {
            _print_error(retval);
            return 1;
        }
        return 0;
    }

    if (!strcmp("prev", argv[pos])) {
        int retval = dfplayer_prev(dev);
        if (retval) {
            _print_error(retval);
            return 1;
        }
        return 0;
    }

    if (!strcmp("track", argv[pos])) {
        _print_tracks(dev);
        return 0;
    }

    if (!strcmp("files", argv[pos])) {
        if ((pos + 1) != argc) {
            print_str("Error: Expected no parameter for argument \"files\"\n");
            return 1;
        }

        _print_files(dev);
        return 0;
    }

    if (!strcmp("file", argv[pos])) {
        if (pos + 2 >= argc) {
            print_str("Error: Missing folder / file number\n\n");
            _print_help(argv[0]);
            return 1;
        }

        uint8_t folder = atoi(argv[++pos]);
        uint8_t file = atoi(argv[++pos]);
        int retval = dfplayer_play_file(dev, folder, file);
        if (retval) {
            _print_error(retval);
            return 1;
        }
        return 0;
    }

    if (!strcmp("volume", argv[pos])) {
        if (argc == ++pos) {
            _print_volume(dev);
        }
        else {
            uint8_t volume = atoi(argv[pos]);
            int retval = dfplayer_set_volume(dev, volume);
            if (retval) {
                _print_error(retval);
                return -1;
            }
        }
        return 0;
    }

    if (!strcmp("source", argv[pos])) {
        if (argc < ++pos) {
            print_str("Error: Missing argument for command \"source\"\n\n");
            _print_help(argv[0]);
            return 1;
        }

        dfplayer_source_t src;
        if (!strcmp("usb", argv[pos]) || !strcmp("USB", argv[pos])) {
            src = DFPLAYER_SOURCE_USB;
        }
        else if (!strcmp("sd", argv[pos]) || !strcmp("SD", argv[pos])) {
            src = DFPLAYER_SOURCE_SDCARD;
        }
        else if (!strcmp("flash", argv[pos]) || !strcmp("FLASH", argv[pos])) {
            src = DFPLAYER_SOURCE_FLASH;
        }
        else {
            print_str("Error: Unknown source type. (Valid: USB, SD, FLASH)\n");
            return 1;
        }

        dfplayer_source_set_t srcs = dfplayer_get_sources(dev);

        if (!dfplayer_source_set_contains(srcs, src)) {
            print_str("Error: Specified playback source not connected\n");
            return 1;
        }

        int retval = dfplayer_set_source(dev, src);
        if (retval) {
            _print_error(retval);
            return 1;
        }
        return 0;
    }

    if (!strcmp("equalizer", argv[pos])) {
        if (++pos == argc) {
            _print_equalizer(dev);
        }
        else {
            dfplayer_eq_t eq = DFPLAYER_EQ_NUMOF;
            for (unsigned i = 0; _equalizers[i] != NULL; i++) {
                if (!strcmp(_equalizers[i], argv[pos])) {
                    eq = (dfplayer_eq_t)i;
                    break;
                }
            }

            if (eq == DFPLAYER_EQ_NUMOF) {
                print_str("Error: Unknown equalizer\n");
                return 1;
            }

            int retval = dfplayer_set_equalizer(dev, eq);
            if (retval) {
                _print_error(retval);
                return 1;
            }
        }
        return 0;
    }

    if (!strcmp("mode", argv[pos])) {
        _print_mode(dev);
        return 0;
    }

    if (!strcmp("repeat", argv[pos])) {
        if (++pos == argc) {
            print_str("Error: Missing parameter for command \"repeat\"\n");
            return 1;
        }
        else {
            bool repeat = (atoi(argv[pos])) ? true : false;
            int retval = dfplayer_repeat(dev, repeat);
            if (retval) {
                _print_error(retval);
                return 1;
            }
        }
        return 0;
    }

    if (!strcmp("repeat-folder", argv[pos])) {
        if (++pos == argc) {
            print_str("Error: Missing argument for command \"repeat\"\n");
            return 1;
        }
        uint8_t folder = atoi(argv[pos]);
        int retval = dfplayer_repeat_folder(dev, folder);
        if (retval) {
            _print_error(retval);
            return 1;
        }

        return 0;
    }

    if (!strcmp("state", argv[pos])) {
        _print_state(dev);
        return 0;
    }

    if (!strcmp("shuffle", argv[pos])) {
        int retval = dfplayer_shuffle_all(dev);
        if (retval) {
            _print_error(retval);
            return 1;
        }
        return 0;
    }

    if (!strcmp("mp3", argv[pos])) {
        if (++pos == argc) {
            print_str("Error: Missing parameter for command \"mp3\"\n");
            return 1;
        }
        uint16_t file = atoi(argv[pos]);
        int retval = dfplayer_play_from_mp3(dev, file);
        if (retval) {
            _print_error(retval);
            return 1;
        }

        return 0;
    }

    if (!strcmp("advert", argv[pos])) {
        if (++pos == argc) {
            print_str("Error: Missing parameter for command \"advert\"\n");
            return 1;
        }
        uint16_t file = atoi(argv[pos]);
        int retval = dfplayer_play_from_advert(dev, file);
        if (retval) {
            _print_error(retval);
            return 1;
        }

        return 0;
    }

    if (!strcmp("stop-advert", argv[pos])) {
        int retval = dfplayer_stop_advert(dev);
        if (retval) {
            _print_error(retval);
            return 1;
        }
        return 0;
    }

    if (!strcmp("cmd", argv[pos])) {
        uint8_t cmd, p1, p2;
        uint16_t resp;
        p1 = p2 = 0;

        if (++pos == argc) {
            print_str("Error: Missing <CMD> for command \"cmd\"\n");
        }

        cmd = (uint8_t)scn_u32_hex(argv[pos], 2);
        if (++pos < argc) {
            p2 = (uint8_t)scn_u32_hex(argv[pos], 2);
        }

        if (++pos < argc) {
            p1 = p2;
            p2 = (uint8_t)scn_u32_hex(argv[pos], 2);
        }

        print_str("Sending cmd = 0x");
        print_u32_hex(cmd);
        print_str(" with params = 0x");
        print_u32_hex(p1);
        print_str(", 0x");
        print_u32_hex(p2);
        print_str(": ");
        int retval = dfplayer_transceive(dev, &resp, cmd, p1, p2);
        if (retval) {
            _print_error(retval);
            return 1;
        }
        else {
            print_str("Response = 0x");
            print_u32_hex(resp);
            print("\n", 1);
        }
        return 0;
    }

    if (strcmp("-h", argv[pos]) && strcmp("--help", argv[pos])) {
        print_str("Failed to parse command \"");
        print_str(argv[pos]);
        print_str("\"\n");
    }

    _print_help(argv[0]);
    return 1;
}
