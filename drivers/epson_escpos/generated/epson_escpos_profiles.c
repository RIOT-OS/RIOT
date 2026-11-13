/*
 * SPDX-FileCopyrightText: escpos-printer-db contributors
 * SPDX-FileCopyrightText: 2026 RIOT Contributors
 * SPDX-License-Identifier: CC-BY-4.0
 */

/* WARNING: This file has been auto-generated from the YAML files in
 *          contrib/. Do not edit this by hand, but update the profiles.
 *          Finally, run `./gen_profiles.py` in drivers/epson_escpos. */

/**
 * @ingroup     drivers_epson_escpos
 * @{
 *
 * @file
 * @brief       Printer profiles of the Epson ESC/POS driver
 *
 * @}
 */

#include "container.h"
#include "epson_escpos_profiles.h"

static const uint8_t _tm_t20ii_columns[] = { 48, 64 };

static const epson_escpos_codepage_t _tm_t20ii_codepages[] = {
    { 0, "CP437" },
    { 1, "CP932" },
    { 2, "CP850" },
    { 3, "CP860" },
    { 4, "CP863" },
    { 5, "CP865" },
    { 11, "CP851" },
    { 12, "CP853" },
    { 13, "CP857" },
    { 14, "CP737" },
    { 15, "ISO_8859-7" },
    { 16, "CP1252" },
    { 17, "CP866" },
    { 18, "CP852" },
    { 19, "CP858" },
    { 30, "TCVN-3-1" },
    { 31, "TCVN-3-2" },
    { 32, "CP720" },
    { 33, "CP775" },
    { 34, "CP855" },
    { 35, "CP861" },
    { 36, "CP862" },
    { 37, "CP864" },
    { 38, "CP869" },
    { 39, "ISO_8859-2" },
    { 40, "ISO_8859-15" },
    { 41, "CP1098" },
    { 44, "CP1125" },
    { 45, "CP1250" },
    { 46, "CP1251" },
    { 47, "CP1253" },
    { 48, "CP1254" },
    { 49, "CP1255" },
    { 50, "CP1256" },
    { 51, "CP1257" },
    { 52, "CP1258" },
    { 53, "RK1048" },
    { 255, NULL },
};

const epson_escpos_profile_t epson_escpos_profile_tm_t20ii = {
    .name = "TM-T20II",
    .vendor = "Epson",
    .features = EPSON_ESCPOS_FEATURE_BARCODE_A |
                EPSON_ESCPOS_FEATURE_BARCODE_B |
                EPSON_ESCPOS_FEATURE_BIT_IMAGE_RASTER |
                EPSON_ESCPOS_FEATURE_BIT_IMAGE_COLUMN |
                EPSON_ESCPOS_FEATURE_GRAPHICS |
                EPSON_ESCPOS_FEATURE_QR_CODE |
                EPSON_ESCPOS_FEATURE_PDF417_CODE |
                EPSON_ESCPOS_FEATURE_HIGH_DENSITY |
                EPSON_ESCPOS_FEATURE_PULSE_STANDARD |
                EPSON_ESCPOS_FEATURE_PAPER_FULL_CUT |
                EPSON_ESCPOS_FEATURE_PAPER_PART_CUT,
    .num_fonts = ARRAY_SIZE(_tm_t20ii_columns),
    .columns = _tm_t20ii_columns,
    .width_mm = 72,
    .width_px = 576,
    .dpi = 203,
    .num_codepages = ARRAY_SIZE(_tm_t20ii_codepages),
    .codepages = _tm_t20ii_codepages,
};

static const uint8_t _tm_t88iii_columns[] = { 42, 56 };

static const epson_escpos_codepage_t _tm_t88iii_codepages[] = {
    { 0, "CP437" },
    { 1, "CP932" },
    { 2, "CP850" },
    { 3, "CP860" },
    { 4, "CP863" },
    { 5, "CP865" },
    { 6, "CP1251" },
    { 16, "CP1252" },
    { 17, "CP866" },
    { 18, "CP862" },
    { 19, "CP858" },
    { 255, NULL },
};

const epson_escpos_profile_t epson_escpos_profile_tm_t88iii = {
    .name = "TM-T88III",
    .vendor = "Epson",
    .features = EPSON_ESCPOS_FEATURE_BARCODE_A |
                EPSON_ESCPOS_FEATURE_BARCODE_B |
                EPSON_ESCPOS_FEATURE_BIT_IMAGE_RASTER |
                EPSON_ESCPOS_FEATURE_BIT_IMAGE_COLUMN |
                EPSON_ESCPOS_FEATURE_GRAPHICS |
                EPSON_ESCPOS_FEATURE_QR_CODE |
                EPSON_ESCPOS_FEATURE_PDF417_CODE |
                EPSON_ESCPOS_FEATURE_HIGH_DENSITY |
                EPSON_ESCPOS_FEATURE_PULSE_STANDARD |
                EPSON_ESCPOS_FEATURE_PAPER_FULL_CUT |
                EPSON_ESCPOS_FEATURE_PAPER_PART_CUT,
    .num_fonts = ARRAY_SIZE(_tm_t88iii_columns),
    .columns = _tm_t88iii_columns,
    .width_mm = 80,
    .width_px = 512,
    .dpi = 180,
    .num_codepages = ARRAY_SIZE(_tm_t88iii_codepages),
    .codepages = _tm_t88iii_codepages,
};

static const uint8_t _tm_t88iv_columns[] = { 42, 56 };

static const epson_escpos_codepage_t _tm_t88iv_codepages[] = {
    { 0, "CP437" },
    { 1, "CP932" },
    { 2, "CP850" },
    { 3, "CP860" },
    { 4, "CP863" },
    { 5, "CP865" },
    { 16, "CP1252" },
    { 17, "CP866" },
    { 18, "CP852" },
    { 19, "CP858" },
    { 255, NULL },
};

const epson_escpos_profile_t epson_escpos_profile_tm_t88iv = {
    .name = "TM-T88IV",
    .vendor = "Epson",
    .features = EPSON_ESCPOS_FEATURE_BARCODE_A |
                EPSON_ESCPOS_FEATURE_BARCODE_B |
                EPSON_ESCPOS_FEATURE_BIT_IMAGE_RASTER |
                EPSON_ESCPOS_FEATURE_BIT_IMAGE_COLUMN |
                EPSON_ESCPOS_FEATURE_GRAPHICS |
                EPSON_ESCPOS_FEATURE_QR_CODE |
                EPSON_ESCPOS_FEATURE_PDF417_CODE |
                EPSON_ESCPOS_FEATURE_HIGH_DENSITY |
                EPSON_ESCPOS_FEATURE_PULSE_STANDARD |
                EPSON_ESCPOS_FEATURE_PAPER_FULL_CUT |
                EPSON_ESCPOS_FEATURE_PAPER_PART_CUT,
    .num_fonts = ARRAY_SIZE(_tm_t88iv_columns),
    .columns = _tm_t88iv_columns,
    .width_mm = 80,
    .width_px = 512,
    .dpi = 180,
    .num_codepages = ARRAY_SIZE(_tm_t88iv_codepages),
    .codepages = _tm_t88iv_codepages,
};
