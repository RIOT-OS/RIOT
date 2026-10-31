/*
 * SPDX-FileCopyrightText: 2026 Christopher Büchse
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     drivers_epson_escpos
 * @{
 *
 * @file
 * @brief       Printer Configuration Profiles
 *
 * @author      Christopher Büchse <christopher.buechse@tuhh.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

const epson_escpos_profile_t epson_escpos_profile_default = {
    .name = "default",
    .num_fonts = 2,
    .columns = { 42, 56 },
    .full_cut = true,
    .part_cut = true,
};

const epson_escpos_profile_t epson_escpos_profile_tm_t88iii = {
    .name = "TM-T88III",
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

#ifdef __cplusplus
}
#endif

/** @} */
