/*
 * SPDX-FileCopyrightText: 2012-2023 Bashlinux and python-escpos
 * SPDX-FileCopyrightText: 2026 Christopher Büchse
 * SPDX-License-Identifier: MIT
 */

#pragma once

/**
 * @ingroup     drivers_epson_escpos
 * @{
 *
 * @file
 * @brief       ESC/POS command constants
 *
 * Port of `escpos/constants.py` from python-escpos.
 *
 * @author      Christopher Büchse <christopher.buechse@tuhh.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Control characters
 * @{
 */
#define ESCPOS_NUL              (0x00)  /**< null */
#define ESCPOS_EOT              (0x04)  /**< end of transmission */
#define ESCPOS_ENQ              (0x05)  /**< enquiry */
#define ESCPOS_DLE              (0x10)  /**< data link escape */
#define ESCPOS_DC4              (0x14)  /**< device control 4 */
#define ESCPOS_CAN              (0x18)  /**< cancel */
#define ESCPOS_ESC              (0x1b)  /**< escape */
#define ESCPOS_FS               (0x1c)  /**< field separator */
#define ESCPOS_GS               (0x1d)  /**< group separator */
/** @} */

/**
 * @name    Feed control sequences
 * @{
 */
#define ESCPOS_CTL_LF           (0x0a)  /**< print and line feed */
#define ESCPOS_CTL_FF           (0x0c)  /**< form feed */
#define ESCPOS_CTL_CR           (0x0d)  /**< carriage return */
#define ESCPOS_CTL_HT           (0x09)  /**< horizontal tab */
#define ESCPOS_CTL_VT           (0x0b)  /**< vertical tab */
#define ESCPOS_CMD_SET_HT       'D'     /**< ESC D: set horizontal tab positions */
/** @} */

/**
 * @name    Commands (second byte after ESC, GS, ...)
 * @{
 */
#define ESCPOS_CMD_HW_INIT      '@'     /**< ESC @: initialize printer */
#define ESCPOS_CMD_HW_SELECT    '='     /**< ESC = 1: select printer */
#define ESCPOS_CMD_HW_RESET     '?'     /**< ESC ? LF NUL: reset hardware */
#define ESCPOS_CMD_CASH_DRAWER  'p'     /**< ESC p m t1 t2: drawer kick */
#define ESCPOS_CMD_CUT          'V'     /**< GS V m: cut paper */
#define ESCPOS_CMD_BUZZER       'B'     /**< ESC B n t: buzzer */
#define ESCPOS_CMD_PANEL_BUTTON 'c'     /**< ESC c 5 n: panel buttons */
#define ESCPOS_CMD_PRINT_MODE   '!'     /**< ESC ! n: select print mode */
#define ESCPOS_CMD_CHAR_SIZE    '!'     /**< GS ! n: select character size */
#define ESCPOS_CMD_BOLD         'E'     /**< ESC E n: emphasized mode */
#define ESCPOS_CMD_UNDERLINE    '-'     /**< ESC - n: underline mode */
#define ESCPOS_CMD_FONT         'M'     /**< ESC M n: select font */
#define ESCPOS_CMD_ALIGN        'a'     /**< ESC a n: justification */
#define ESCPOS_CMD_INVERT       'B'     /**< GS B n: reverse printing */
#define ESCPOS_CMD_FLIP         '{'     /**< ESC { n: upside-down printing */
#define ESCPOS_CMD_DENSITY      '|'     /**< GS | n: print density */
#define ESCPOS_CMD_SMOOTH       'b'     /**< GS b n: smoothing */
#define ESCPOS_CMD_CODEPAGE     't'     /**< ESC t n: select code page */
#define ESCPOS_CMD_FEED         'd'     /**< ESC d n: print and feed n lines */
#define ESCPOS_CMD_SPACING_RESET '2'    /**< ESC 2: default line spacing */
#define ESCPOS_CMD_SPACING_60   'A'     /**< ESC A n: spacing n/60 inch */
#define ESCPOS_CMD_SPACING_180  '3'     /**< ESC 3 n: spacing n/180 inch */
#define ESCPOS_CMD_SPACING_360  '+'     /**< ESC + n: spacing n/360 inch */
/** @} */

/**
 * @name    Paper cut modes (GS V m)
 * @{
 */
#define ESCPOS_CUT_FULL         (0x00)  /**< full cut */
#define ESCPOS_CUT_PART         (0x01)  /**< partial cut */
#define ESCPOS_CUT_FEED_FULL    (65)    /**< feed to cut position, full cut */
#define ESCPOS_CUT_FEED_PART    (66)    /**< feed to cut position, partial cut */
/** @} */

/**
 * @name    Print mode bits (ESC ! n)
 * @{
 */
#define ESCPOS_MODE_NORMAL      (0x00)  /**< normal text */
#define ESCPOS_MODE_2H          (0x10)  /**< double height */
#define ESCPOS_MODE_2W          (0x20)  /**< double width */
#define ESCPOS_MODE_2X          (0x30)  /**< double width and height */
/** @} */

/**
 * @name    Print density values (GS | n), index 0-8 = -50% ... +50%
 * @{
 */
#define ESCPOS_DENSITY_VALUES   { 0x00, 0x01, 0x02, 0x03, 0x04, \
                                  0x08, 0x07, 0x06, 0x05 }
/** @} */

/**
 * @name    Real time status (DLE EOT n)
 * @{
 */
#define ESCPOS_CMD_RT_STATUS    ESCPOS_EOT /**< DLE EOT n */
#define ESCPOS_RT_STATUS_ONLINE (0x01)     /**< printer status */
#define ESCPOS_RT_STATUS_PAPER  (0x04)     /**< paper roll sensor status */
#define ESCPOS_RT_MASK_ONLINE   (18)       /**< online */
#define ESCPOS_RT_MASK_OFFLINE  (26)       /**< offline */
#define ESCPOS_RT_MASK_PAPER    (18)       /**< paper adequate */
#define ESCPOS_RT_MASK_LOWPAPER (30)       /**< paper near end */
#define ESCPOS_RT_MASK_NOPAPER  (114)      /**< paper end */
/** @} */

/**
 * @name    Print speed (GS ( K 2 0 50 n)
 * @{
 */
#define ESCPOS_SET_PRINT_SPEED  { ESCPOS_GS, '(', 'K', 0x02, 0x00, 0x32 }
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
