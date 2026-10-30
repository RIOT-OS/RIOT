/*
 * SPDX-FileCopyrightText: 2012-2023 Bashlinux and python-escpos
 * SPDX-FileCopyrightText: 2026 Christopher Büchse
 * SPDX-License-Identifier: MIT
 */

/**
 * @ingroup     drivers_epson_escpos
 * @{
 *
 * @file
 * @brief       Transport independent core of the ESC/POS driver
 *
 * Port of `escpos/escpos.py` from python-escpos.
 *
 * @author      Christopher Büchse <christopher.buechse@tuhh.de>
 *
 * @}
 */

#include <assert.h>
#include <errno.h>
#include <string.h>

#include "epson_escpos.h"
#include "epson_escpos_constants.h"

#define ENABLE_DEBUG 0
#include "debug.h"

const epson_escpos_profile_t epson_escpos_profile_default = {
    .name = "default",
    .num_fonts = 2,
    .columns = { 42, 56 },
    .full_cut = true,
    .part_cut = true,
};

const epson_escpos_profile_t epson_escpos_profile_tm_t88iv = {
    .name = "TM-T88IV",
    .num_fonts = 2,
    .columns = { 42, 56 },
    .full_cut = true,
    .part_cut = true,
};

static const uint8_t _density[] = ESCPOS_DENSITY_VALUES;

void epson_escpos_init(epson_escpos_t *dev,
                       const epson_escpos_transport_t *transport, void *ctx,
                       const epson_escpos_profile_t *profile)
{
    assert(dev && transport && transport->open && transport->close &&
           transport->write);

    memset(dev, 0, sizeof(*dev));
    dev->transport = transport;
    dev->transport_ctx = ctx;
    dev->profile = profile ? profile : &epson_escpos_profile_default;
    dev->font = -1;
}

int epson_escpos_open(epson_escpos_t *dev)
{
    assert(dev);

    if (dev->is_open) {
        epson_escpos_close(dev);
    }

    int res = dev->transport->open(dev->transport_ctx);
    if (res < 0) {
        DEBUG("[epson_escpos] open failed: %d\n", res);
        return res;
    }
    dev->is_open = true;
    return 0;
}

void epson_escpos_close(epson_escpos_t *dev)
{
    assert(dev);

    if (!dev->is_open) {
        return;
    }
    epson_escpos_flush(dev);
    dev->transport->close(dev->transport_ctx);
    dev->is_open = false;
}

int epson_escpos_flush(epson_escpos_t *dev)
{
    assert(dev);

    if (dev->buf_len == 0) {
        return 0;
    }

    /* like python-escpos, open the connection on first use */
    if (!dev->is_open) {
        int res = epson_escpos_open(dev);
        if (res < 0) {
            return res;
        }
    }

    int res = dev->transport->write(dev->transport_ctx, dev->buf, dev->buf_len);
    dev->buf_len = 0;
    return res;
}

int epson_escpos_raw(epson_escpos_t *dev, const void *data, size_t len)
{
    assert(dev && (data || !len));

    if (len > sizeof(dev->buf) - dev->buf_len) {
        int res = epson_escpos_flush(dev);
        if (res < 0) {
            return res;
        }
    }

    /* data does not fit into the buffer, send it directly */
    if (len > sizeof(dev->buf)) {
        if (!dev->is_open) {
            int res = epson_escpos_open(dev);
            if (res < 0) {
                return res;
            }
        }
        return dev->transport->write(dev->transport_ctx, data, len);
    }

    memcpy(&dev->buf[dev->buf_len], data, len);
    dev->buf_len += len;
    return 0;
}

/* helpers for short commands */
static int _cmd1(epson_escpos_t *dev, uint8_t a)
{
    return epson_escpos_raw(dev, &a, 1);
}

static int _cmd2(epson_escpos_t *dev, uint8_t a, uint8_t b)
{
    const uint8_t cmd[] = { a, b };
    return epson_escpos_raw(dev, cmd, sizeof(cmd));
}

static int _cmd3(epson_escpos_t *dev, uint8_t a, uint8_t b, uint8_t c)
{
    const uint8_t cmd[] = { a, b, c };
    return epson_escpos_raw(dev, cmd, sizeof(cmd));
}

int epson_escpos_text(epson_escpos_t *dev, const char *txt)
{
    assert(txt);
    return epson_escpos_raw(dev, txt, strlen(txt));
}

int epson_escpos_textln(epson_escpos_t *dev, const char *txt)
{
    int res = epson_escpos_text(dev, txt);
    if (res < 0) {
        return res;
    }
    return _cmd1(dev, ESCPOS_CTL_LF);
}

int epson_escpos_ln(epson_escpos_t *dev, unsigned count)
{
    while (count--) {
        int res = _cmd1(dev, ESCPOS_CTL_LF);
        if (res < 0) {
            return res;
        }
    }
    return 0;
}

static int _check_style(const epson_escpos_t *dev,
                        const epson_escpos_style_t *style)
{
    uint16_t f = style->fields;

    if ((f & EPSON_ESCPOS_STYLE_SIZE) &&
        ((style->size > EPSON_ESCPOS_SIZE_CUSTOM) ||
         ((style->size == EPSON_ESCPOS_SIZE_CUSTOM) &&
          ((style->width < 1) || (style->width > 8) ||
           (style->height < 1) || (style->height > 8))))) {
        return -EINVAL;
    }
    if ((f & EPSON_ESCPOS_STYLE_UNDERLINE) &&
        (style->underline > EPSON_ESCPOS_UNDERLINE_2DOT)) {
        return -EINVAL;
    }
    if ((f & EPSON_ESCPOS_STYLE_FONT) &&
        ((unsigned)style->font >= dev->profile->num_fonts)) {
        return -EINVAL;
    }
    if ((f & EPSON_ESCPOS_STYLE_ALIGN) &&
        (style->align > EPSON_ESCPOS_ALIGN_RIGHT)) {
        return -EINVAL;
    }
    if ((f & EPSON_ESCPOS_STYLE_DENSITY) && (style->density > 9)) {
        return -EINVAL;
    }
    return 0;
}

static int _set_size(epson_escpos_t *dev, const epson_escpos_style_t *style)
{
    static const uint8_t modes[] = {
        [EPSON_ESCPOS_SIZE_NORMAL] = ESCPOS_MODE_NORMAL,
        [EPSON_ESCPOS_SIZE_2H] = ESCPOS_MODE_2H,
        [EPSON_ESCPOS_SIZE_2W] = ESCPOS_MODE_2W,
        [EPSON_ESCPOS_SIZE_2X] = ESCPOS_MODE_2X,
    };

    if (style->size == EPSON_ESCPOS_SIZE_CUSTOM) {
        uint8_t size = ((style->width - 1) << 4) | (style->height - 1);
        return _cmd3(dev, ESCPOS_GS, ESCPOS_CMD_CHAR_SIZE, size);
    }

    /* ESC ! also selects font A, so the cached font is no longer valid */
    dev->font = -1;
    return _cmd3(dev, ESCPOS_ESC, ESCPOS_CMD_PRINT_MODE, modes[style->size]);
}

int epson_escpos_set(epson_escpos_t *dev, const epson_escpos_style_t *style)
{
    assert(dev && style);

    int res = _check_style(dev, style);
    if (res < 0) {
        return res;
    }

    uint16_t f = style->fields;

    /* same order as in python-escpos */
    if (f & EPSON_ESCPOS_STYLE_SIZE) {
        res = _set_size(dev, style);
    }
    if ((res == 0) && (f & EPSON_ESCPOS_STYLE_FLIP)) {
        res = _cmd3(dev, ESCPOS_ESC, ESCPOS_CMD_FLIP, style->flip);
    }
    if ((res == 0) && (f & EPSON_ESCPOS_STYLE_SMOOTH)) {
        res = _cmd3(dev, ESCPOS_GS, ESCPOS_CMD_SMOOTH, style->smooth);
    }
    if ((res == 0) && (f & EPSON_ESCPOS_STYLE_BOLD)) {
        res = _cmd3(dev, ESCPOS_ESC, ESCPOS_CMD_BOLD, style->bold);
    }
    if ((res == 0) && (f & EPSON_ESCPOS_STYLE_UNDERLINE)) {
        res = _cmd3(dev, ESCPOS_ESC, ESCPOS_CMD_UNDERLINE, style->underline);
    }
    if ((res == 0) && (f & EPSON_ESCPOS_STYLE_FONT) &&
        (dev->font != (int8_t)style->font)) {
        res = _cmd3(dev, ESCPOS_ESC, ESCPOS_CMD_FONT, style->font);
        dev->font = style->font;
    }
    if ((res == 0) && (f & EPSON_ESCPOS_STYLE_ALIGN)) {
        res = _cmd3(dev, ESCPOS_ESC, ESCPOS_CMD_ALIGN, style->align);
    }
    if ((res == 0) && (f & EPSON_ESCPOS_STYLE_DENSITY) &&
        (style->density != 9)) {
        res = _cmd3(dev, ESCPOS_GS, ESCPOS_CMD_DENSITY,
                    _density[style->density]);
    }
    if ((res == 0) && (f & EPSON_ESCPOS_STYLE_INVERT)) {
        res = _cmd3(dev, ESCPOS_GS, ESCPOS_CMD_INVERT, style->invert);
    }

    return res;
}

int epson_escpos_set_default(epson_escpos_t *dev)
{
    static const epson_escpos_style_t style = EPSON_ESCPOS_STYLE_DEFAULT;
    return epson_escpos_set(dev, &style);
}

int epson_escpos_codepage(epson_escpos_t *dev, uint8_t page)
{
    return _cmd3(dev, ESCPOS_ESC, ESCPOS_CMD_CODEPAGE, page);
}

int epson_escpos_line_spacing(epson_escpos_t *dev, int spacing,
                              unsigned divisor)
{
    uint8_t cmd;

    if (spacing < 0) {
        return _cmd2(dev, ESCPOS_ESC, ESCPOS_CMD_SPACING_RESET);
    }

    switch (divisor) {
    case 60:
        if (spacing > 85) {
            return -EINVAL;
        }
        cmd = ESCPOS_CMD_SPACING_60;
        break;
    case 180:
        cmd = ESCPOS_CMD_SPACING_180;
        break;
    case 360:
        cmd = ESCPOS_CMD_SPACING_360;
        break;
    default:
        return -EINVAL;
    }
    if (spacing > 255) {
        return -EINVAL;
    }

    return _cmd3(dev, ESCPOS_ESC, cmd, spacing);
}

int epson_escpos_print_and_feed(epson_escpos_t *dev, uint8_t n)
{
    return _cmd3(dev, ESCPOS_ESC, ESCPOS_CMD_FEED, n);
}

int epson_escpos_cut(epson_escpos_t *dev, epson_escpos_cut_t mode, bool feed)
{
    bool part;

    /* fall back to the other mode if the requested one is not supported */
    if (mode == EPSON_ESCPOS_CUT_PART) {
        part = dev->profile->part_cut || !dev->profile->full_cut;
    }
    else {
        part = !dev->profile->full_cut && dev->profile->part_cut;
    }

    if (!feed) {
        const uint8_t cmd[] = {
            ESCPOS_GS, ESCPOS_CMD_CUT,
            part ? ESCPOS_CUT_FEED_PART : ESCPOS_CUT_FEED_FULL, 0
        };
        return epson_escpos_raw(dev, cmd, sizeof(cmd));
    }

    int res = epson_escpos_print_and_feed(dev, 6);
    if (res < 0) {
        return res;
    }
    if (!dev->profile->full_cut && !dev->profile->part_cut) {
        return 0;
    }
    return _cmd3(dev, ESCPOS_GS, ESCPOS_CMD_CUT,
                 part ? ESCPOS_CUT_PART : ESCPOS_CUT_FULL);
}

int epson_escpos_hw(epson_escpos_t *dev, epson_escpos_hw_t hw)
{
    switch (hw) {
    case EPSON_ESCPOS_HW_INIT:
        /* ESC @ resets all settings */
        dev->font = -1;
        return _cmd2(dev, ESCPOS_ESC, ESCPOS_CMD_HW_INIT);
    case EPSON_ESCPOS_HW_SELECT:
        return _cmd3(dev, ESCPOS_ESC, ESCPOS_CMD_HW_SELECT, 0x01);
    case EPSON_ESCPOS_HW_RESET: {
        const uint8_t cmd[] = {
            ESCPOS_ESC, ESCPOS_CMD_HW_RESET, ESCPOS_CTL_LF, ESCPOS_NUL
        };
        return epson_escpos_raw(dev, cmd, sizeof(cmd));
    }
    default:
        return -EINVAL;
    }
}

int epson_escpos_control(epson_escpos_t *dev, epson_escpos_ctl_t ctl,
                         unsigned count, unsigned tab_size)
{
    switch (ctl) {
    case EPSON_ESCPOS_CTL_LF:
        return _cmd1(dev, ESCPOS_CTL_LF);
    case EPSON_ESCPOS_CTL_FF:
        return _cmd1(dev, ESCPOS_CTL_FF);
    case EPSON_ESCPOS_CTL_CR:
        return _cmd1(dev, ESCPOS_CTL_CR);
    case EPSON_ESCPOS_CTL_VT:
        return _cmd1(dev, ESCPOS_CTL_VT);
    case EPSON_ESCPOS_CTL_HT:
        break;
    default:
        return -EINVAL;
    }

    if ((count > 32) || (tab_size < 1) || (tab_size > 255) ||
        (count * tab_size >= 256)) {
        return -EINVAL;
    }

    int res = _cmd2(dev, ESCPOS_ESC, ESCPOS_CMD_SET_HT);
    for (unsigned i = 1; (res == 0) && (i < count); i++) {
        res = _cmd1(dev, i * tab_size);
    }
    if (res == 0) {
        res = _cmd1(dev, ESCPOS_NUL);
    }
    return res;
}

int epson_escpos_panel_buttons(epson_escpos_t *dev, bool enable)
{
    const uint8_t cmd[] = {
        ESCPOS_ESC, ESCPOS_CMD_PANEL_BUTTON, '5', enable ? 0 : 1
    };
    return epson_escpos_raw(dev, cmd, sizeof(cmd));
}

int epson_escpos_cashdraw(epson_escpos_t *dev, unsigned pin,
                          uint8_t on_time, uint8_t off_time)
{
    if ((pin != 2) && (pin != 5)) {
        return -EINVAL;
    }

    const uint8_t cmd[] = {
        ESCPOS_ESC, ESCPOS_CMD_CASH_DRAWER, (pin == 2) ? 0 : 1,
        on_time, off_time
    };
    return epson_escpos_raw(dev, cmd, sizeof(cmd));
}

int epson_escpos_buzzer(epson_escpos_t *dev, unsigned times, unsigned duration)
{
    if ((times < 1) || (times > 9) || (duration < 1) || (duration > 9)) {
        return -EINVAL;
    }
    const uint8_t cmd[] = { ESCPOS_ESC, ESCPOS_CMD_BUZZER, times, duration };
    return epson_escpos_raw(dev, cmd, sizeof(cmd));
}

int epson_escpos_print_speed(epson_escpos_t *dev, unsigned speed)
{
    if ((speed < 1) || (speed > 17)) {
        return -EINVAL;
    }
    const uint8_t cmd[] = ESCPOS_SET_PRINT_SPEED;
    int res = epson_escpos_raw(dev, cmd, sizeof(cmd));
    if (res < 0) {
        return res;
    }
    return _cmd1(dev, speed);
}

/* A status byte has the format 0xx1xx10 */
static bool _check_valid_response(uint8_t resp)
{
    return ((resp & 0x81) == 0x00) && ((resp & 0x12) == 0x12);
}

int epson_escpos_query_status(epson_escpos_t *dev, uint8_t mode,
                              uint8_t *status)
{
    assert(dev && status);

    if (!dev->transport->read) {
        return -ENOTSUP;
    }

    int res = _cmd3(dev, ESCPOS_DLE, ESCPOS_CMD_RT_STATUS, mode);
    if (res == 0) {
        res = epson_escpos_flush(dev);
    }
    if (res < 0) {
        return res;
    }

    uint8_t resp[16];
    ssize_t len = dev->transport->read(dev->transport_ctx, resp, sizeof(resp),
                                       CONFIG_EPSON_ESCPOS_READ_TIMEOUT_MS);
    if (len < 0) {
        DEBUG("[epson_escpos] reading status failed: %d\n", (int)len);
        return len;
    }
    if ((len != 1) || !_check_valid_response(resp[0])) {
        DEBUG("[epson_escpos] invalid status response (%d bytes)\n", (int)len);
        return -EBADMSG;
    }

    *status = resp[0];
    return 0;
}

int epson_escpos_is_online(epson_escpos_t *dev)
{
    uint8_t status;
    int res = epson_escpos_query_status(dev, ESCPOS_RT_STATUS_ONLINE, &status);
    if (res < 0) {
        return res;
    }
    return (status & ESCPOS_RT_MASK_OFFLINE) != ESCPOS_RT_MASK_OFFLINE;
}

int epson_escpos_paper_status(epson_escpos_t *dev)
{
    uint8_t status;
    int res = epson_escpos_query_status(dev, ESCPOS_RT_STATUS_PAPER, &status);
    if (res < 0) {
        return res;
    }
    if ((status & ESCPOS_RT_MASK_NOPAPER) == ESCPOS_RT_MASK_NOPAPER) {
        return EPSON_ESCPOS_PAPER_EMPTY;
    }
    if ((status & ESCPOS_RT_MASK_LOWPAPER) == ESCPOS_RT_MASK_LOWPAPER) {
        return EPSON_ESCPOS_PAPER_NEAR_END;
    }
    return EPSON_ESCPOS_PAPER_OK;
}
