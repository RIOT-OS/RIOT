/* *************************************************************************************************
 *
 *  Copyright (C) 2009 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 *    Redistribution and use in source and binary forms, with or without
 *    modification, are permitted provided that the following conditions
 *    are met:
 *
 *      Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *      Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the
 *      distribution.
 *
 *      Neither the name of Texas Instruments Incorporated nor the names of
 *      its contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *    A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ************************************************************************************************/

/* Basic display functions. */
/* ************************************************************************************************* */

/**
 * @file
 * @brief       eZ430-chronos display driver character tables
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 */

/* ************************************************************************************************* */
/* Include section */
#include <stdint.h>
#include <cc430f6137.h>

#include "display.h"

/* ************************************************************************************************* */
/* Global Variable section */

/* Table with memory bit assignment for digits "0" to "9" and characters "A" to "Z" */
/*   A */
/* F   B */
/*   G */
/* E   C */
/*   D */
const uint8_t lcd_font[] = {
    SEG_A + SEG_B + SEG_C + SEG_D + SEG_E + SEG_F,          /* Displays "0" */
    SEG_B + SEG_C,                                          /* Displays "1" */
    SEG_A + SEG_B +         SEG_D + SEG_E +         SEG_G,  /* Displays "2" */
    SEG_A + SEG_B + SEG_C + SEG_D +                 SEG_G,  /* Displays "3" */
            SEG_B + SEG_C +                 SEG_F + SEG_G,  /* Displays "4" */
    SEG_A +         SEG_C + SEG_D +         SEG_F + SEG_G,  /* Displays "5" */
    SEG_A +         SEG_C + SEG_D + SEG_E + SEG_F + SEG_G,  /* Displays "6" */
    SEG_A + SEG_B + SEG_C,                                  /* Displays "7" */
    SEG_A + SEG_B + SEG_C + SEG_D + SEG_E + SEG_F + SEG_G,  /* Displays "8" */
    SEG_A + SEG_B + SEG_C + SEG_D +         SEG_F + SEG_G,  /* Displays "9" */
    0,                                                      /* Displays " " (:) */
    0,                                                      /* Displays " " (;) */
    SEG_A +                                 SEG_F + SEG_G,  /* Displays "<" as high c */
    SEG_D +                                         SEG_G,  /* Displays "=" */
    0,                                                      /* Displays " " (>) */
    SEG_A + SEG_B +                 SEG_E +         SEG_G,  /* Displays "?" */
    0,                                                      /* Displays " " (@) */
    SEG_A + SEG_B + SEG_C +         SEG_E + SEG_F + SEG_G,  /* Displays "A" */
                    SEG_C + SEG_D + SEG_E + SEG_F + SEG_G,  /* Displays "b" */
                            SEG_D + SEG_E +         SEG_G,  /* Displays "c" */
            SEG_B + SEG_C + SEG_D + SEG_E +         SEG_G,  /* Displays "d" */
    SEG_A +                 SEG_D + SEG_E + SEG_F + SEG_G,  /* Displays "E" */
    SEG_A +                         SEG_E + SEG_F + SEG_G,  /* Displays "f" */
    SEG_A + SEG_B + SEG_C + SEG_D +         SEG_F + SEG_G,  /* Displays "g" same as 9 */
    SEG_C +                         SEG_E + SEG_F + SEG_G,  /* Displays "h" */
                                    SEG_E,                  /* Displays "i" */
    SEG_A + SEG_B + SEG_C + SEG_D,                          /* Displays "J" */
                            SEG_D +         SEG_F + SEG_G,  /* Displays "k" */
                            SEG_D + SEG_E + SEG_F,          /* Displays "L" */
    SEG_A + SEG_B + SEG_C +         SEG_E + SEG_F,          /* Displays "M" */
                    SEG_C +         SEG_E +         SEG_G,  /* Displays "n" */
                    SEG_C + SEG_D + SEG_E +         SEG_G,  /* Displays "o" */
    SEG_A + SEG_B +                 SEG_E + SEG_F + SEG_G,  /* Displays "P" */
    SEG_A + SEG_B + SEG_C +                 SEG_F + SEG_G,  /* Displays "q" */
                                    SEG_E +         SEG_G,  /* Displays "r" */
    SEG_A +         SEG_C + SEG_D +         SEG_F + SEG_G,  /* Displays "S" same as 5 */
                            SEG_D + SEG_E + SEG_F + SEG_G,  /* Displays "t" */
                    SEG_C + SEG_D + SEG_E,                  /* Displays "u" */
                    SEG_C + SEG_D + SEG_E,                  /* Displays "v" same as u */
            SEG_B + SEG_C + SEG_D + SEG_E + SEG_F + SEG_G,  /* Displays "W" */
            SEG_B + SEG_C +         SEG_E + SEG_F + SEG_G,  /* Displays "X" as H */
            SEG_B + SEG_C + SEG_D +         SEG_F + SEG_G,  /* Displays "Y" */
    SEG_A + SEG_B +         SEG_D + SEG_E +         SEG_G,  /* Displays "Z" same as 2 */
};

/* Table with memory address for each display element  */
const uint8_t *segments_lcdmem[] = {
    LCD_SYMB_AM_MEM,
    LCD_SYMB_PM_MEM,
    LCD_SYMB_ARROW_UP_MEM,
    LCD_SYMB_ARROW_DOWN_MEM,
    LCD_SYMB_PERCENT_MEM,
    LCD_SYMB_TOTAL_MEM,
    LCD_SYMB_AVERAGE_MEM,
    LCD_SYMB_MAX_MEM,
    LCD_SYMB_BATTERY_MEM,
    LCD_UNIT_L1_FT_MEM,
    LCD_UNIT_L1_K_MEM,
    LCD_UNIT_L1_M_MEM,
    LCD_UNIT_L1_I_MEM,
    LCD_UNIT_L1_PER_S_MEM,
    LCD_UNIT_L1_PER_H_MEM,
    LCD_UNIT_L1_DEGREE_MEM,
    LCD_UNIT_L2_KCAL_MEM,
    LCD_UNIT_L2_KM_MEM,
    LCD_UNIT_L2_MI_MEM,
    LCD_ICON_HEART_MEM,
    LCD_ICON_STOPWATCH_MEM,
    LCD_ICON_RECORD_MEM,
    LCD_ICON_ALARM_MEM,
    LCD_ICON_BEEPER1_MEM,
    LCD_ICON_BEEPER2_MEM,
    LCD_ICON_BEEPER3_MEM,
    LCD_SEG_L1_3_MEM,
    LCD_SEG_L1_2_MEM,
    LCD_SEG_L1_1_MEM,
    LCD_SEG_L1_0_MEM,
    LCD_SEG_L1_COL_MEM,
    LCD_SEG_L1_DP1_MEM,
    LCD_SEG_L1_DP0_MEM,
    LCD_SEG_L2_5_MEM,
    LCD_SEG_L2_4_MEM,
    LCD_SEG_L2_3_MEM,
    LCD_SEG_L2_2_MEM,
    LCD_SEG_L2_1_MEM,
    LCD_SEG_L2_0_MEM,
    LCD_SEG_L2_COL1_MEM,
    LCD_SEG_L2_COL0_MEM,
    LCD_SEG_L2_DP_MEM,
};

/* Table with bit mask for each display element  */
const uint8_t segments_bitmask[] = {
    LCD_SYMB_AM_MASK,
    LCD_SYMB_PM_MASK,
    LCD_SYMB_ARROW_UP_MASK,
    LCD_SYMB_ARROW_DOWN_MASK,
    LCD_SYMB_PERCENT_MASK,
    LCD_SYMB_TOTAL_MASK,
    LCD_SYMB_AVERAGE_MASK,
    LCD_SYMB_MAX_MASK,
    LCD_SYMB_BATTERY_MASK,
    LCD_UNIT_L1_FT_MASK,
    LCD_UNIT_L1_K_MASK,
    LCD_UNIT_L1_M_MASK,
    LCD_UNIT_L1_I_MASK,
    LCD_UNIT_L1_PER_S_MASK,
    LCD_UNIT_L1_PER_H_MASK,
    LCD_UNIT_L1_DEGREE_MASK,
    LCD_UNIT_L2_KCAL_MASK,
    LCD_UNIT_L2_KM_MASK,
    LCD_UNIT_L2_MI_MASK,
    LCD_ICON_HEART_MASK,
    LCD_ICON_STOPWATCH_MASK,
    LCD_ICON_RECORD_MASK,
    LCD_ICON_ALARM_MASK,
    LCD_ICON_BEEPER1_MASK,
    LCD_ICON_BEEPER2_MASK,
    LCD_ICON_BEEPER3_MASK,
    LCD_SEG_L1_3_MASK,
    LCD_SEG_L1_2_MASK,
    LCD_SEG_L1_1_MASK,
    LCD_SEG_L1_0_MASK,
    LCD_SEG_L1_COL_MASK,
    LCD_SEG_L1_DP1_MASK,
    LCD_SEG_L1_DP0_MASK,
    LCD_SEG_L2_5_MASK,
    LCD_SEG_L2_4_MASK,
    LCD_SEG_L2_3_MASK,
    LCD_SEG_L2_2_MASK,
    LCD_SEG_L2_1_MASK,
    LCD_SEG_L2_0_MASK,
    LCD_SEG_L2_COL1_MASK,
    LCD_SEG_L2_COL0_MASK,
    LCD_SEG_L2_DP_MASK,
};

/* Quick integer to array conversion table for most common integer values
 * discarding this would save aprox. 600 bytes codespace but increase cpu time
 * for displaying values */
const uint8_t itoa_conversion_table[][3] = {
    "000", "001", "002", "003", "004", "005", "006", "007", "008", "009", "010", "011", "012", "013", "014", "015",
    "016", "017", "018", "019", "020", "021", "022", "023", "024", "025", "026", "027", "028", "029", "030", "031",
    "032", "033", "034", "035", "036", "037", "038", "039", "040", "041", "042", "043", "044", "045", "046", "047",
    "048", "049", "050", "051", "052", "053", "054", "055", "056", "057", "058", "059", "060", "061", "062", "063",
    "064", "065", "066", "067", "068", "069", "070", "071", "072", "073", "074", "075", "076", "077", "078", "079",
    "080", "081", "082", "083", "084", "085", "086", "087", "088", "089", "090", "091", "092", "093", "094", "095",
    "096", "097", "098", "099", "100", "101", "102", "103", "104", "105", "106", "107", "108", "109", "110", "111",
    "112", "113", "114", "115", "116", "117", "118", "119", "120", "121", "122", "123", "124", "125", "126", "127",
    "128", "129", "130", "131", "132", "133", "134", "135", "136", "137", "138", "139", "140", "141", "142", "143",
    "144", "145", "146", "147", "148", "149", "150", "151", "152", "153", "154", "155", "156", "157", "158", "159",
    "160", "161", "162", "163", "164", "165", "166", "167", "168", "169", "170", "171", "172", "173", "174", "175",
    "176", "177", "178", "179", "180",
};
