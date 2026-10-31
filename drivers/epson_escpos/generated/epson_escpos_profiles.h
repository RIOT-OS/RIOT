/*
 * SPDX-FileCopyrightText: escpos-printer-db contributors
 * SPDX-FileCopyrightText: 2026 RIOT Contributors
 * SPDX-License-Identifier: CC-BY-4.0
 */

#pragma once

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
 * Generated from the
 * [ESC/POS printer database](https://github.com/receipt-print-hq/escpos-printer-db).
 */

#include "epson_escpos.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Features of a printer profile
 * @{
 */
#define EPSON_ESCPOS_FEATURE_BARCODE_A        (1U << 0)  /**< barcodeA */
#define EPSON_ESCPOS_FEATURE_BARCODE_B        (1U << 1)  /**< barcodeB */
#define EPSON_ESCPOS_FEATURE_BIT_IMAGE_RASTER (1U << 2)  /**< bitImageRaster */
#define EPSON_ESCPOS_FEATURE_BIT_IMAGE_COLUMN (1U << 3)  /**< bitImageColumn */
#define EPSON_ESCPOS_FEATURE_GRAPHICS         (1U << 4)  /**< graphics */
#define EPSON_ESCPOS_FEATURE_STAR_COMMANDS    (1U << 5)  /**< starCommands */
#define EPSON_ESCPOS_FEATURE_QR_CODE          (1U << 6)  /**< qrCode */
#define EPSON_ESCPOS_FEATURE_PDF417_CODE      (1U << 7)  /**< pdf417Code */
#define EPSON_ESCPOS_FEATURE_HIGH_DENSITY     (1U << 8)  /**< highDensity */
#define EPSON_ESCPOS_FEATURE_PULSE_STANDARD   (1U << 9)  /**< pulseStandard */
#define EPSON_ESCPOS_FEATURE_PULSE_BEL        (1U << 10) /**< pulseBel */
#define EPSON_ESCPOS_FEATURE_PAPER_FULL_CUT   (1U << 11) /**< paperFullCut */
#define EPSON_ESCPOS_FEATURE_PAPER_PART_CUT   (1U << 12) /**< paperPartCut */
/** @} */

/**
 * @brief   Profile of the Epson TM-T88III
 */
extern const epson_escpos_profile_t epson_escpos_profile_tm_t88iii;

/**
 * @brief   Profile of the Epson TM-T88IV
 */
extern const epson_escpos_profile_t epson_escpos_profile_tm_t88iv;

#ifdef __cplusplus
}
#endif

/** @} */
