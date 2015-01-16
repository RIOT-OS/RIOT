/**************************************************************************//**
 * @file
 * @brief Segment LCD Config for the EFM32_G8xx_STK
 * @version 3.20.5
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2014 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#ifndef __SEGMENTLCDCONFIG_H
#define __SEGMENTLCDCONFIG_H

#include "em_lcd.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Range of symbols available on display */
typedef enum
{
  LCD_SYMBOL_GECKO,
  LCD_SYMBOL_ANT,
  LCD_SYMBOL_PAD0,
  LCD_SYMBOL_PAD1,
  LCD_SYMBOL_AM,
  LCD_SYMBOL_PM,
  LCD_SYMBOL_EFM32,
  LCD_SYMBOL_MINUS,
  LCD_SYMBOL_COL3,
  LCD_SYMBOL_COL5,
  LCD_SYMBOL_COL10,
  LCD_SYMBOL_DEGC,
  LCD_SYMBOL_DEGF,
  LCD_SYMBOL_MICROAMP,
  LCD_SYMBOL_MILLIAMP,
  LCD_SYMBOL_DP6,
  LCD_SYMBOL_DP10,
} lcdSymbol;


#define LCD_SYMBOL_GECKO_COM  3
#define LCD_SYMBOL_GECKO_SEG  8
#define LCD_SYMBOL_ANT_COM  3
#define LCD_SYMBOL_ANT_SEG  1
#define LCD_SYMBOL_PAD0_COM  1
#define LCD_SYMBOL_PAD0_SEG  8
#define LCD_SYMBOL_PAD1_COM  2
#define LCD_SYMBOL_PAD1_SEG  8
#define LCD_SYMBOL_AM_COM  0
#define LCD_SYMBOL_AM_SEG  32
#define LCD_SYMBOL_PM_COM  0
#define LCD_SYMBOL_PM_SEG  35
#define LCD_SYMBOL_EFM32_COM  0
#define LCD_SYMBOL_EFM32_SEG  8
#define LCD_SYMBOL_MINUS_COM  0
#define LCD_SYMBOL_MINUS_SEG  9
#define LCD_SYMBOL_COL3_COM  0
#define LCD_SYMBOL_COL3_SEG  16
#define LCD_SYMBOL_COL5_COM  0
#define LCD_SYMBOL_COL5_SEG  24
#define LCD_SYMBOL_COL10_COM  0
#define LCD_SYMBOL_COL10_SEG  39
#define LCD_SYMBOL_DEGC_COM  0
#define LCD_SYMBOL_DEGC_SEG  34
#define LCD_SYMBOL_DEGF_COM  1
#define LCD_SYMBOL_DEGF_SEG  34
#define LCD_SYMBOL_MICROAMP_COM  2
#define LCD_SYMBOL_MICROAMP_SEG  34
#define LCD_SYMBOL_MILLIAMP_COM  3
#define LCD_SYMBOL_MILLIAMP_SEG  34
#define LCD_SYMBOL_DP6_COM  0
#define LCD_SYMBOL_DP6_SEG  21
#define LCD_SYMBOL_DP10_COM  0
#define LCD_SYMBOL_DP10_SEG  37

/* LCD Controller Prescaler (divide by 128) */
/* CLKlcd = 256 Hz */
#define LCD_CMU_CLK_PRE         cmuClkDiv_128
#define LCD_CMU_CLK_DIV         0

#define LCD_BOOST_LEVEL         lcdVBoostLevel0

#define LCD_INIT_DEF \
{ true,\
  lcdMuxQuadruplex,\
  lcdBiasOneThird,\
  lcdWaveLowPower,\
  lcdVLCDSelVDD,\
  lcdConConfVLCD }

#define LCD_NUMBER_OFF() \
do { \
  LCD_SegmentSetLow(0, 0x80000000, 0x00000000);\
  LCD_SegmentSetLow(1, 0x80000000, 0x00000000);\
  LCD_SegmentSetLow(2, 0x80000000, 0x00000000);\
  LCD_SegmentSetLow(3, 0x80000000, 0x00000000);\
  LCD_SegmentSetHigh(0, 0x52, 0x00000000);\
  LCD_SegmentSetHigh(1, 0xFB, 0x00000000);\
  LCD_SegmentSetHigh(2, 0xFB, 0x00000000);\
  LCD_SegmentSetHigh(3, 0xFB, 0x00000000);\
} while (0)

#define LCD_ALPHA_NUMBER_OFF() \
do { \
  LCD_SegmentSetLow(0, 0x7ECCCC00, 0x00000000);\
  LCD_SegmentSetLow(1, 0x7FFFFEFC, 0x00000000);\
  LCD_SegmentSetLow(2, 0x7FFFFEFC, 0x00000000);\
  LCD_SegmentSetLow(3, 0x7FFFFEFC, 0x00000000);\
} while(0)

#define LCD_ALL_SEGMENTS_OFF() \
do { \
  LCD_SegmentSetLow(0, 0xFFFFFFFF, 0x00000000);\
  LCD_SegmentSetLow(1, 0xFFFFFFFF, 0x00000000);\
  LCD_SegmentSetLow(2, 0xFFFFFFFF, 0x00000000);\
  LCD_SegmentSetLow(3, 0xFFFFFFFF, 0x00000000);\
  LCD_SegmentSetHigh(0, 0xFF, 0x00000000);\
  LCD_SegmentSetHigh(1, 0xFF, 0x00000000);\
  LCD_SegmentSetHigh(2, 0xFF, 0x00000000);\
  LCD_SegmentSetHigh(3, 0xFF, 0x00000000);\
} while(0)

#define LCD_ALL_SEGMENTS_ON() \
do { \
  LCD_SegmentSetLow(0, 0xFFFFFFFF, 0xFFFFFFFF);\
  LCD_SegmentSetLow(1, 0xFFFFFFFF, 0xFFFFFFFF);\
  LCD_SegmentSetLow(2, 0xFFFFFFFF, 0xFFFFFFFF);\
  LCD_SegmentSetLow(3, 0xFFFFFFFF, 0xFFFFFFFF);\
  LCD_SegmentSetHigh(0, 0xFF, 0xFFFFFFFF);\
  LCD_SegmentSetHigh(1, 0xFF, 0xFFFFFFFF);\
  LCD_SegmentSetHigh(2, 0xFF, 0xFFFFFFFF);\
  LCD_SegmentSetHigh(3, 0xFF, 0xFFFFFFFF);\
} while(0)

#define LCD_SEGMENTS_ENABLE() \
do { \
  LCD_SegmentRangeEnable(lcdSegmentAll, true);\
} while(0)

#define LCD_DISPLAY_ENABLE() \
do { \
  ;\
} while(0)

#define EFM_DISPLAY_DEF {\
  .Text        = {\
    { /* 1 */\
      .com[0] = 3, .com[1] = 3, .com[2] = 1, .com[3] = 0,\
      .bit[0] = 10, .bit[1] = 12, .bit[2] = 12, .bit[3] = 10,\
      .com[4] = 1, .com[5] = 3, .com[6] = 2, .com[7] = 2,\
      .bit[4] = 9, .bit[5] = 9, .bit[6] = 9, .bit[7] = 10,\
      .com[8] = 2, .com[9] = 3, .com[10] = 2, .com[11] = 0,\
      .bit[8] = 11, .bit[9] = 11, .bit[10] = 12, .bit[11] = 11,\
      .com[12] = 1, .com[13] = 1,\
      .bit[12] = 11, .bit[13] = 10\
    },\
    { /* 2 */\
      .com[0] = 3, .com[1] = 3, .com[2] = 1, .com[3] = 0,\
      .bit[0] = 14, .bit[1] = 16, .bit[2] = 16, .bit[3] = 14,\
      .com[4] = 1, .com[5] = 3, .com[6] = 2, .com[7] = 2,\
      .bit[4] = 13, .bit[5] = 13, .bit[6] = 13, .bit[7] = 14,\
      .com[8] = 2, .com[9] = 3, .com[10] = 2, .com[11] = 0,\
      .bit[8] = 15, .bit[9] = 15, .bit[10] = 16, .bit[11] = 15,\
      .com[12] = 1, .com[13] = 1,\
      .bit[12] = 15, .bit[13] = 14\
    },\
    { /* 3 */\
      .com[0] = 3, .com[1] = 3, .com[2] = 1, .com[3] = 0,\
      .bit[0] = 18, .bit[1] = 20, .bit[2] = 20, .bit[3] = 18,\
      .com[4] = 1, .com[5] = 3, .com[6] = 2, .com[7] = 2,\
      .bit[4] = 17, .bit[5] = 17, .bit[6] = 17, .bit[7] = 18,\
      .com[8] = 2, .com[9] = 3, .com[10] = 2, .com[11] = 0,\
      .bit[8] = 19, .bit[9] = 19, .bit[10] = 20, .bit[11] = 19,\
      .com[12] = 1, .com[13] = 1,\
      .bit[12] = 19, .bit[13] = 18\
    },\
    { /* 4 */\
      .com[0] = 3, .com[1] = 3, .com[2] = 1, .com[3] = 0,\
      .bit[0] = 22, .bit[1] = 24, .bit[2] = 24, .bit[3] = 22,\
      .com[4] = 1, .com[5] = 3, .com[6] = 2, .com[7] = 2,\
      .bit[4] = 21, .bit[5] = 21, .bit[6] = 21, .bit[7] = 22,\
      .com[8] = 2, .com[9] = 3, .com[10] = 2, .com[11] = 0,\
      .bit[8] = 23, .bit[9] = 23, .bit[10] = 24, .bit[11] = 23,\
      .com[12] = 1, .com[13] = 1,\
      .bit[12] = 23, .bit[13] = 22\
    },\
    { /* 5 */\
      .com[0] = 3, .com[1] = 3, .com[2] = 1, .com[3] = 0,\
      .bit[0] = 25, .bit[1] = 6, .bit[2] = 6, .bit[3] = 25,\
      .com[4] = 1, .com[5] = 3, .com[6] = 2, .com[7] = 2,\
      .bit[4] = 7, .bit[5] = 7, .bit[6] = 7, .bit[7] = 25,\
      .com[8] = 2, .com[9] = 3, .com[10] = 2, .com[11] = 0,\
      .bit[8] = 26, .bit[9] = 26, .bit[10] = 6, .bit[11] = 26,\
      .com[12] = 1, .com[13] = 1,\
      .bit[12] = 26, .bit[13] = 25\
    },\
    { /* 6 */\
      .com[0] = 3, .com[1] = 3, .com[2] = 1, .com[3] = 0,\
      .bit[0] = 27, .bit[1] = 04, .bit[2] = 04, .bit[3] = 27,\
      .com[4] = 1, .com[5] = 3, .com[6] = 2, .com[7] = 2,\
      .bit[4] = 5, .bit[5] = 5, .bit[6] = 5, .bit[7] = 27,\
      .com[8] = 2, .com[9] = 3, .com[10] = 2, .com[11] = 0,\
      .bit[8] = 28, .bit[9] = 28, .bit[10] = 4, .bit[11] = 28,\
      .com[12] = 1, .com[13] = 1,\
      .bit[12] = 28, .bit[13] = 27\
    },\
    { /* 7 */\
      .com[0] = 3, .com[1] = 3, .com[2] = 1, .com[3] = 0,\
      .bit[0] = 29, .bit[1] = 2, .bit[2] = 2, .bit[3] = 29,\
      .com[4] = 1, .com[5] = 3, .com[6] = 2, .com[7] = 2,\
      .bit[4] = 03, .bit[5] = 3, .bit[6] = 3, .bit[7] = 29,\
      .com[8] = 2, .com[9] = 3, .com[10] = 2, .com[11] = 0,\
      .bit[8] = 30, .bit[9] = 30, .bit[10] = 2, .bit[11] = 30,\
      .com[12] = 1, .com[13] = 1,\
      .bit[12] = 30, .bit[13] = 29\
    }\
  },\
  .Number      = {\
    {\
      .com[0] = 3, .com[1] = 2, .com[2] = 1, .com[3] = 0,\
      .bit[0] = 31, .bit[1] = 31, .bit[2] = 31, .bit[3] = 31,\
      .com[4] = 1, .com[5] = 3, .com[6] = 2,\
      .bit[4] = 32, .bit[5] = 32, .bit[6] = 32,\
    },\
    {\
      .com[0] = 3, .com[1] = 2, .com[2] = 1, .com[3] = 0,\
      .bit[0] = 33, .bit[1] = 33, .bit[2] = 33, .bit[3] = 33,\
      .com[4] = 1, .com[5] = 3, .com[6] = 2,\
      .bit[4] = 35, .bit[5] = 35, .bit[6] = 35,\
    },\
    {\
      .com[0] = 3, .com[1] = 2, .com[2] = 1, .com[3] = 0,\
      .bit[0] = 36, .bit[1] = 36, .bit[2] = 36, .bit[3] = 36,\
      .com[4] = 1, .com[5] = 3, .com[6] = 2,\
      .bit[4] = 37, .bit[5] = 37, .bit[6] = 37,\
    },\
    {\
      .com[0] = 3, .com[1] = 2, .com[2] = 1, .com[3] = 0,\
      .bit[0] = 38, .bit[1] = 38, .bit[2] = 38, .bit[3] = 38,\
      .com[4] = 1, .com[5] = 3, .com[6] = 2,\
      .bit[4] = 39, .bit[5] = 39, .bit[6] = 39,\
    },\
  },\
  .EMode       = {\
    .com[0] = 1, .bit[0] = 1,\
    .com[1] = 2, .bit[1] = 1,\
    .com[2] = 1, .bit[2] = 0,\
    .com[3] = 2, .bit[3] = 0,\
    .com[4] = 3, .bit[4] = 0,\
  },\
  .ARing       = {\
    .com[0] = 0, .bit[0] = 0,\
    .com[1] = 0, .bit[1] = 1,\
    .com[2] = 0, .bit[2] = 2,\
    .com[3] = 0, .bit[3] = 3,\
    .com[4] = 0, .bit[4] = 4,\
    .com[5] = 0, .bit[5] = 5,\
    .com[6] = 0, .bit[6] = 6,\
    .com[7] = 0, .bit[7] = 7,\
  },\
  .Battery     = {\
    .com[0] = 0, .bit[0] = 12,\
    .com[1] = 0, .bit[1] = 17,\
    .com[2] = 0, .bit[2] = 20,\
    .com[3] = 0, .bit[3] = 13,\
  }\
}

#ifdef __cplusplus
}
#endif

#endif
