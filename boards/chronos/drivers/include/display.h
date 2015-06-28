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
 * *************************************************************************************************
 * Basic display functions.
 * ************************************************************************************************/

#ifndef DISPLAY_H_
#define DISPLAY_H_

#ifdef __cplusplus
extern "C" {
#endif

#define CLOCK_24HR              (0)
#define CLOCK_AM_PM             (1)
#define CLOCK_DISPLAY_SELECT    (2)


/* *************************************************************************************************
 * Global Variable section */

/* Set of display flags */
typedef union {
    struct {
        /* Line1 + Line2 + Icons*/
        uint16_t full_update            : 1;    /* 1 = Redraw all content */
        uint16_t partial_update         : 1;    /* 1 = Update changes */

        /* Line only */
        uint16_t line1_full_update      : 1;    /* 1 = Redraw Line1 content */
        uint16_t line2_full_update      : 1;    /* 1 = Redraw Line2 content */

        /* Logic module data update flags */
        uint16_t update_time            : 1;    /* 1 = Time was updated */
        uint16_t update_stopwatch       : 1;    /* 1 = Stopwatch was updated */
        uint16_t update_temperature     : 1;    /* 1 = Temperature was updated */
        uint16_t update_battery_voltage : 1;    /* 1 = Battery voltage was updated */
        uint16_t update_date            : 1;    /* 1 = Date was updated */
        uint16_t update_alarm           : 1;    /* 1 = Alarm time was updated */
        uint16_t update_acceleration    : 1;    /* 1 = Acceleration data was updated */
    } flag;
    uint16_t all_flags;            /* Shortcut to all display flags (for reset) */
} s_display_flags_t;

extern volatile s_display_flags_t display;

/* Constants defined in library */
extern const uint8_t lcd_font[];
extern const uint8_t *segments_lcdmem[];
extern const uint8_t segments_bitmask[];
extern const uint8_t itoa_conversion_table[][3];

/* ************************************************************************************************* */
/* Defines section */

/* Display function modes */
#define DISPLAY_LINE_UPDATE_FULL        (BIT0)
#define DISPLAY_LINE_UPDATE_PARTIAL     (BIT1)
#define DISPLAY_LINE_CLEAR              (BIT2)

/* Definitions for line view style */
#define DISPLAY_DEFAULT_VIEW            (0u)
#define DISPLAY_ALTERNATIVE_VIEW        (1u)
#define DISPLAY_ALTERNATIVE2_VIEW       (2u)

/* Definitions for line access */
#define LINE1                           (1u)
#define LINE2                           (2u)

/* LCD display modes */
#define SEG_OFF                 (0u)
#define SEG_ON                  (1u)
#define SEG_ON_BLINK_ON         (2u)
#define SEG_ON_BLINK_OFF        (3u)
#define SEG_OFF_BLINK_OFF       (4u)

/* 7-segment character bit assignments */
#define SEG_A                   (BIT4)
#define SEG_B                   (BIT5)
#define SEG_C                   (BIT6)
#define SEG_D                   (BIT7)
#define SEG_E                   (BIT2)
#define SEG_F                   (BIT0)
#define SEG_G                   (BIT1)

/* ------------------------------------------
 * LCD symbols for easier access
 *
 * xxx_SEG_xxx      = Seven-segment character (sequence 5-4-3-2-1-0)
 * xxx_SYMB_xxx     = Display symbol, e.g. "AM" for ante meridiem
 * xxx_UNIT_xxx     = Display unit, e.g. "km/h" for kilometers per hour
 * xxx_ICON_xxx     = Display icon, e.g. heart to indicate reception of heart rate data
 * xxx_L1_xxx       = Item is part of Line1 information
 * xxx_L2_xxx       = Item is part of Line2 information
 */

//* Symbols for Line1 */
#define LCD_SYMB_AM                 0
#define LCD_SYMB_PM                 1
#define LCD_SYMB_ARROW_UP           2
#define LCD_SYMB_ARROW_DOWN         3
#define LCD_SYMB_PERCENT            4

/* Symbols for Line2 */
#define LCD_SYMB_TOTAL              5
#define LCD_SYMB_AVERAGE            6
#define LCD_SYMB_MAX                7
#define LCD_SYMB_BATTERY            8

/* Units for Line1 */
#define LCD_UNIT_L1_FT              9
#define LCD_UNIT_L1_K               10
#define LCD_UNIT_L1_M               11
#define LCD_UNIT_L1_I               12
#define LCD_UNIT_L1_PER_S           13
#define LCD_UNIT_L1_PER_H           14
#define LCD_UNIT_L1_DEGREE          15

/* Units for Line2 */
#define LCD_UNIT_L2_KCAL            16
#define LCD_UNIT_L2_KM              17
#define LCD_UNIT_L2_MI              18

/* Icons */
#define LCD_ICON_HEART              19
#define LCD_ICON_STOPWATCH          20
#define LCD_ICON_RECORD             21
#define LCD_ICON_ALARM              22
#define LCD_ICON_BEEPER1            23
#define LCD_ICON_BEEPER2            24
#define LCD_ICON_BEEPER3            25

/* Line1 7-segments */
#define LCD_SEG_L1_3                26
#define LCD_SEG_L1_2                27
#define LCD_SEG_L1_1                28
#define LCD_SEG_L1_0                29
#define LCD_SEG_L1_COL              30
#define LCD_SEG_L1_DP1              31
#define LCD_SEG_L1_DP0              32

/* Line2 7-segments */
#define LCD_SEG_L2_5                33
#define LCD_SEG_L2_4                34
#define LCD_SEG_L2_3                35
#define LCD_SEG_L2_2                36
#define LCD_SEG_L2_1                37
#define LCD_SEG_L2_0                38
#define LCD_SEG_L2_COL1             39
#define LCD_SEG_L2_COL0             40
#define LCD_SEG_L2_DP               41

/* Line1 7-segment arrays */
#define LCD_SEG_L1_3_0              70
#define LCD_SEG_L1_2_0              71
#define LCD_SEG_L1_1_0              72
#define LCD_SEG_L1_3_1              73
#define LCD_SEG_L1_3_2              74

/* Line2 7-segment arrays */
#define LCD_SEG_L2_5_0              90
#define LCD_SEG_L2_4_0              91
#define LCD_SEG_L2_3_0              92
#define LCD_SEG_L2_2_0              93
#define LCD_SEG_L2_1_0              94
#define LCD_SEG_L2_5_2              95
#define LCD_SEG_L2_3_2              96
#define LCD_SEG_L2_5_4              97
#define LCD_SEG_L2_4_2              98

/* LCD controller memory map */
#define LCD_MEM_1                   ((uint8_t*)0x0A20)
#define LCD_MEM_2                   ((uint8_t*)0x0A21)
#define LCD_MEM_3                   ((uint8_t*)0x0A22)
#define LCD_MEM_4                   ((uint8_t*)0x0A23)
#define LCD_MEM_5                   ((uint8_t*)0x0A24)
#define LCD_MEM_6                   ((uint8_t*)0x0A25)
#define LCD_MEM_7                   ((uint8_t*)0x0A26)
#define LCD_MEM_8                   ((uint8_t*)0x0A27)
#define LCD_MEM_9                   ((uint8_t*)0x0A28)
#define LCD_MEM_10                  ((uint8_t*)0x0A29)
#define LCD_MEM_11                  ((uint8_t*)0x0A2A)
#define LCD_MEM_12                  ((uint8_t*)0x0A2B)

/* Memory assignment */
#define LCD_SEG_L1_0_MEM            (LCD_MEM_6)
#define LCD_SEG_L1_1_MEM            (LCD_MEM_4)
#define LCD_SEG_L1_2_MEM            (LCD_MEM_3)
#define LCD_SEG_L1_3_MEM            (LCD_MEM_2)
#define LCD_SEG_L1_COL_MEM          (LCD_MEM_1)
#define LCD_SEG_L1_DP1_MEM          (LCD_MEM_1)
#define LCD_SEG_L1_DP0_MEM          (LCD_MEM_5)
#define LCD_SEG_L2_0_MEM            (LCD_MEM_8)
#define LCD_SEG_L2_1_MEM            (LCD_MEM_9)
#define LCD_SEG_L2_2_MEM            (LCD_MEM_10)
#define LCD_SEG_L2_3_MEM            (LCD_MEM_11)
#define LCD_SEG_L2_4_MEM            (LCD_MEM_12)
#define LCD_SEG_L2_5_MEM            (LCD_MEM_12)
#define LCD_SEG_L2_COL1_MEM         (LCD_MEM_1)
#define LCD_SEG_L2_COL0_MEM         (LCD_MEM_5)
#define LCD_SEG_L2_DP_MEM           (LCD_MEM_9)
#define LCD_SYMB_AM_MEM             (LCD_MEM_1)
#define LCD_SYMB_PM_MEM             (LCD_MEM_1)
#define LCD_SYMB_ARROW_UP_MEM       (LCD_MEM_1)
#define LCD_SYMB_ARROW_DOWN_MEM     (LCD_MEM_1)
#define LCD_SYMB_PERCENT_MEM        (LCD_MEM_5)
#define LCD_SYMB_TOTAL_MEM          (LCD_MEM_11)
#define LCD_SYMB_AVERAGE_MEM        (LCD_MEM_10)
#define LCD_SYMB_MAX_MEM            (LCD_MEM_8)
#define LCD_SYMB_BATTERY_MEM        (LCD_MEM_7)
#define LCD_UNIT_L1_FT_MEM          (LCD_MEM_5)
#define LCD_UNIT_L1_K_MEM           (LCD_MEM_5)
#define LCD_UNIT_L1_M_MEM           (LCD_MEM_7)
#define LCD_UNIT_L1_I_MEM           (LCD_MEM_7)
#define LCD_UNIT_L1_PER_S_MEM       (LCD_MEM_5)
#define LCD_UNIT_L1_PER_H_MEM       (LCD_MEM_7)
#define LCD_UNIT_L1_DEGREE_MEM      (LCD_MEM_5)
#define LCD_UNIT_L2_KCAL_MEM        (LCD_MEM_7)
#define LCD_UNIT_L2_KM_MEM          (LCD_MEM_7)
#define LCD_UNIT_L2_MI_MEM          (LCD_MEM_7)
#define LCD_ICON_HEART_MEM          (LCD_MEM_2)
#define LCD_ICON_STOPWATCH_MEM      (LCD_MEM_3)
#define LCD_ICON_RECORD_MEM         (LCD_MEM_1)
#define LCD_ICON_ALARM_MEM          (LCD_MEM_4)
#define LCD_ICON_BEEPER1_MEM        (LCD_MEM_5)
#define LCD_ICON_BEEPER2_MEM        (LCD_MEM_6)
#define LCD_ICON_BEEPER3_MEM        (LCD_MEM_7)

/* Bit masks for write access */
#define LCD_SEG_L1_0_MASK           (BIT2+BIT1+BIT0+BIT7+BIT6+BIT5+BIT4)
#define LCD_SEG_L1_1_MASK           (BIT2+BIT1+BIT0+BIT7+BIT6+BIT5+BIT4)
#define LCD_SEG_L1_2_MASK           (BIT2+BIT1+BIT0+BIT7+BIT6+BIT5+BIT4)
#define LCD_SEG_L1_3_MASK           (BIT2+BIT1+BIT0+BIT7+BIT6+BIT5+BIT4)
#define LCD_SEG_L1_COL_MASK         (BIT5)
#define LCD_SEG_L1_DP1_MASK         (BIT6)
#define LCD_SEG_L1_DP0_MASK         (BIT2)
#define LCD_SEG_L2_0_MASK           (BIT3+BIT2+BIT1+BIT0+BIT6+BIT5+BIT4)
#define LCD_SEG_L2_1_MASK           (BIT3+BIT2+BIT1+BIT0+BIT6+BIT5+BIT4)
#define LCD_SEG_L2_2_MASK           (BIT3+BIT2+BIT1+BIT0+BIT6+BIT5+BIT4)
#define LCD_SEG_L2_3_MASK           (BIT3+BIT2+BIT1+BIT0+BIT6+BIT5+BIT4)
#define LCD_SEG_L2_4_MASK           (BIT3+BIT2+BIT1+BIT0+BIT6+BIT5+BIT4)
#define LCD_SEG_L2_5_MASK           (BIT7)
#define LCD_SEG_L2_COL1_MASK        (BIT4)
#define LCD_SEG_L2_COL0_MASK        (BIT0)
#define LCD_SEG_L2_DP_MASK          (BIT7)
#define LCD_SYMB_AM_MASK            (BIT1+BIT0)
#define LCD_SYMB_PM_MASK            (BIT0)
#define LCD_SYMB_ARROW_UP_MASK      (BIT2)
#define LCD_SYMB_ARROW_DOWN_MASK    (BIT3)
#define LCD_SYMB_PERCENT_MASK       (BIT4)
#define LCD_SYMB_TOTAL_MASK         (BIT7)
#define LCD_SYMB_AVERAGE_MASK       (BIT7)
#define LCD_SYMB_MAX_MASK           (BIT7)
#define LCD_SYMB_BATTERY_MASK       (BIT7)
#define LCD_UNIT_L1_FT_MASK         (BIT5)
#define LCD_UNIT_L1_K_MASK          (BIT6)
#define LCD_UNIT_L1_M_MASK          (BIT1)
#define LCD_UNIT_L1_I_MASK          (BIT0)
#define LCD_UNIT_L1_PER_S_MASK      (BIT7)
#define LCD_UNIT_L1_PER_H_MASK      (BIT2)
#define LCD_UNIT_L1_DEGREE_MASK     (BIT1)
#define LCD_UNIT_L2_KCAL_MASK       (BIT4)
#define LCD_UNIT_L2_KM_MASK         (BIT5)
#define LCD_UNIT_L2_MI_MASK         (BIT6)
#define LCD_ICON_HEART_MASK         (BIT3)
#define LCD_ICON_STOPWATCH_MASK     (BIT3)
#define LCD_ICON_RECORD_MASK        (BIT7)
#define LCD_ICON_ALARM_MASK         (BIT3)
#define LCD_ICON_BEEPER1_MASK       (BIT3)
#define LCD_ICON_BEEPER2_MASK       (BIT3)
#define LCD_ICON_BEEPER3_MASK       (BIT3)


/* *************************************************************************************************
 * API section
 */

/* Physical LCD memory write */
/* *************************************************************************************************
 * @fn          write_segment
 * @brief       Write to one or multiple LCD segments
 * @param       lcdmem      Pointer to LCD byte memory
 *              bits        Segments to address
 *              bitmask     Bitmask for particular display item
 *              mode        On, off or blink segments
 * @return
 * ************************************************************************************************/
void write_lcd_mem(uint8_t *lcdmem, uint8_t bits, uint8_t bitmask, uint8_t state);

/* Display init / clear */
/* *************************************************************************************************
 * @fn          lcd_init
 * @brief       Erase LCD memory. Init LCD peripheral.
 * @param       none
 * @return      none
 * ************************************************************************************************/
void lcd_init(void);

/* *************************************************************************************************
 * @fn          clear_display
 * @brief       Erase LINE1 and LINE2 segments. Keep icons.
 * @param       none
 * @return      none
 * ************************************************************************************************/
void clear_display(void);

/* *************************************************************************************************
 * @fn          clear_display_all
 * @brief       Erase LINE1 and LINE2 segments. Clear also function-specific content.
 * @param       none
 * @return      none
 * ************************************************************************************************/
void clear_display_all(void);

/* *************************************************************************************************
 * @fn          clear_line
 * @brief       Erase segments of a given line.
 * @param       uint8_t line    LINE1, LINE2
 * @return      none
 * ************************************************************************************************/
void clear_line(uint8_t line);

/* Blinking function */
/* *************************************************************************************************
 * @fn          start_blink
 * @brief       Start blinking.
 * @param       none
 * @return      none
 * ************************************************************************************************/
void start_blink(void);

/* *************************************************************************************************
 * @fn          stop_blink
 * @brief       Stop blinking.
 * @param       none
 * @return      none
 * ************************************************************************************************/
void stop_blink(void);

/* *************************************************************************************************
 * @fn          stop_blink
 * @brief       Clear blinking memory.
 * @param       none
 * @return      none
 * ************************************************************************************************/
void clear_blink_mem(void);

/* *************************************************************************************************
 * @fn          set_blink_rate
 * @brief       Set blink rate register bits.
 * @param       none
 * @return      none
 * ************************************************************************************************/
void set_blink_rate(uint8_t bits);

/* Character / symbol draw functions */
/* *************************************************************************************************
 * @fn          display_char
 * @brief       Write to 7-segment characters.
 * @param       uint8_t segment     A valid LCD segment
 *              uint8_t chr         Character to display
 *              uint8_t mode        SEG_ON, SEG_OFF, SEG_BLINK
 * @return      none
 * ************************************************************************************************/
void display_char(uint8_t segment, char chr, uint8_t mode);

/* *************************************************************************************************
 * @FN          display_chars
 * @brief       Write to consecutive 7-segment characters.
 * @param       uint8_t segments    LCD segment array
 *              uint8_t * str       Pointer to a string
 *              uint8_t mode        SEG_ON, SEG_OFF, SEG_BLINK
 * @return      none
 * ************************************************************************************************/
void display_chars(uint8_t segments, char *str, uint8_t mode);

/* *************************************************************************************************
 * @fn          display_symbol
 * @brief       Switch symbol on or off on LCD.
 * @param       uint8_t symbol      A valid LCD symbol (index 0..42)
 *              uint8_t state       SEG_ON, SEG_OFF, SEG_BLINK
 * @return      none
 * ************************************************************************************************/
void display_symbol(uint8_t symbol, uint8_t mode);

/* Set_value display functions */
/* *************************************************************************************************
 * @fn          display_value1
 * @brief       Generic decimal display routine. Used exclusively by set_value function.
 * @param       uint8_t segments        LCD segments where value is displayed
 *              uint32_t value          Integer value to be displayed
 *              uint8_t digits          Number of digits to convert
 *              uint8_t blanks          Number of leadings blanks in itoa result string
 * @return      none
 * ************************************************************************************************/
void display_value1(uint8_t segments, uint32_t value, uint8_t digits, uint8_t blanks, uint8_t disp_mode);

/* Integer to string conversion  */
/* *************************************************************************************************
 * @fn          itoa
 * @brief       Generic integer to array routine. Converts integer n to string.
 *              Default conversion result has leading zeros, e.g. "00123"
 *              Option to convert leading '0' into whitespace (blanks)
 * @param       uint32_t n          integer to convert
 *              uint8_t digits      number of digits
 *              uint8_t blanks      fill up result string with number of whitespaces instead of leading zeros
 * @return      uint8_t             string
 * ************************************************************************************************/
char *itoa(uint32_t n, uint8_t digits, uint8_t blanks);

/* Segment index helper function */
/* *************************************************************************************************
 * @fn          switch_seg
 * @brief       Returns index of 7-segment character. Required for display routines that can draw
 *              information on both lines.
 * @param       uint8_t line        LINE1, LINE2
 *              uint8_t index1      Index of LINE1
 *              uint8_t index2      Index of LINE2
 * @return      uint8
 * ************************************************************************************************/
uint8_t switch_seg(uint8_t line, uint8_t index1, uint8_t index2);

/* *************************************************************************************************
 * @fn          display_all_off
 * @brief       Sets everything of on the display
 * @param       none
 * @return      none
 * ************************************************************************************************/
void display_all_off(void);

#ifdef __cplusplus
}
#endif

#endif /* DISPLAY_ */
