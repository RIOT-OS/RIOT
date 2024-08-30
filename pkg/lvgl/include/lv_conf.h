/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_lvgl
 * @brief       LVGL configuration macros
 * @{
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef LV_CONF_H
#define LV_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "kernel_defines.h"
#include "lvgl_riot_conf.h"

/*====================
   COLOR SETTINGS
 *====================*/

/* Color depth:
 * - 1:  1 byte per pixel
 * - 8:  RGB233
 * - 16: RGB565
 * - 32: ARGB8888
 */
#ifndef LV_COLOR_DEPTH
#define LV_COLOR_DEPTH     16
#endif

/* Swap the 2 bytes of RGB565 color.
 * Useful if the display has a 8 bit interface (e.g. SPI)*/
#ifndef LV_COLOR_16_SWAP
#if IS_USED(MODULE_LCD)
#define LV_COLOR_16_SWAP   1
#else
#define LV_COLOR_16_SWAP   0
#endif
#endif

/*Enable more complex drawing routines to manage screens transparency.
 *Can be used if the UI is above another layer, e.g. an OSD menu or video player.
 *Requires `LV_COLOR_DEPTH = 32` colors and the screen's `bg_opa` should be set to non LV_OPA_COVER value*/
#define LV_COLOR_SCREEN_TRANSP    0

/**Images pixels with this color will not be drawn if they are chroma keyed)*/
#define LV_COLOR_CHROMA_KEY    lv_color_hex(0x00ff00)         /*pure green*/

/*=========================
   MEMORY SETTINGS
 *=========================*/

/*1: use custom malloc/free, 0: use the built-in `lv_mem_alloc()` and `lv_mem_free()`*/
#define LV_MEM_CUSTOM      0
#if LV_MEM_CUSTOM == 0
/*Size of the memory available for `lv_mem_alloc()` in bytes (>= 2kB)*/
#ifndef LV_MEM_SIZE
#  if (__SIZEOF_POINTER__ > 4)
/*64-bit platforms require additional space because a lot of pointers are stored on the lvgl heap.*/
#    if IS_USED(MODULE_LVGL_EXTRA_THEME_DEFAULT_GROW)
#    define LV_MEM_SIZE    (9U * 1024U)          /*[bytes]*/
#    else
#    define LV_MEM_SIZE    (8U * 1024U)          /*[bytes]*/
#    endif
#  else
#    if IS_USED(MODULE_LVGL_EXTRA_THEME_DEFAULT_GROW)
#    define LV_MEM_SIZE    (6U * 1024U)          /*[bytes]*/
#    else
#    define LV_MEM_SIZE    (5U * 1024U)          /*[bytes]*/
#    endif
#  endif
#endif

/*Set an address for the memory pool instead of allocating it as a normal array. Can be in external SRAM too.*/
#  define LV_MEM_ADR          0     /*0: unused*/
#else       /*LV_MEM_CUSTOM*/
#  define LV_MEM_CUSTOM_INCLUDE <stdlib.h>   /*Header for the dynamic memory function*/
#  define LV_MEM_CUSTOM_ALLOC     malloc
#  define LV_MEM_CUSTOM_FREE      free
#endif     /*LV_MEM_CUSTOM*/

/** Number of the intermediate memory buffer used during rendering and other internal processing mechanisms.
 *You will see an error log message if there wasn't enough buffers. */
#define LV_MEM_BUF_MAX_NUM      16

/*Use the standard `memcpy` and `memset` instead of LVGL's own functions. (Might or might not be faster).*/
#define LV_MEMCPY_MEMSET_STD    1

/*====================
   HAL SETTINGS
 *====================*/

/*Default display refresh period. LVG will redraw changed ares with this period time*/
#define LV_DISP_DEF_REFR_PERIOD     30      /*[ms]*/

/*Input device read period in milliseconds*/
#define LV_INDEV_DEF_READ_PERIOD    30      /*[ms]*/

/** Default Dot Per Inch. Used to initialize default sizes such as widgets sized, style paddings.
 *(Not so important, you can adjust it to modify default sizes and spaces)*/
#define LV_DPI_DEF                  130     /*[px/inch]*/

/*=======================
 * FEATURE CONFIGURATION
 *=======================*/

/*-------------
 * Drawing
 *-----------*/

/** Enable complex draw engine.
 *Required to draw shadow, gradient, rounded corners, circles, arc, skew lines, image transformations or any masks*/
#define LV_DRAW_COMPLEX 1
#if LV_DRAW_COMPLEX != 0

/** Allow buffering some shadow calculation.
 *LV_SHADOW_CACHE_SIZE is the max. shadow size to buffer, where shadow size is `shadow_width + radius`
 *Caching has LV_SHADOW_CACHE_SIZE^2 RAM cost*/
#define LV_SHADOW_CACHE_SIZE    0
#endif /*LV_DRAW_COMPLEX*/

/*Default image cache size. Image caching keeps the images opened.
 *If only the built-in image formats are used there is no real advantage of caching. (I.e. if no new image decoder is added)
 *With complex image decoders (e.g. PNG or JPG) caching can save the continuous open/decode of images.
 *However the opened images might consume additional RAM.
 *0: to disable caching*/
#define LV_IMG_CACHE_DEF_SIZE       0

/** Number of stops allowed per gradient. Increase this to allow more stops.
 *This adds (sizeof(lv_color_t) + 1) bytes per additional stop*/
#define LV_GRADIENT_MAX_STOPS       2

/**Default gradient buffer size.
 *When LVGL calculates the gradient "maps" it can save them into a cache to avoid calculating them again.
 *LV_GRAD_CACHE_DEF_SIZE sets the size of this cache in bytes.
 *If the cache is too small the map will be allocated only while it's required for the drawing.
 *0 mean no caching.*/
#define LV_GRAD_CACHE_DEF_SIZE      0

/** Maximum buffer size to allocate for rotation. Only used if software rotation is enabled in the display driver.*/
#define LV_DISP_ROT_MAX_BUF         (10*1024)
/*-------------
 * GPU
 *-----------*/

/*Use STM32's DMA2D (aka Chrom Art) GPU*/
#define LV_USE_GPU_STM32_DMA2D  0
#if LV_USE_GPU_STM32_DMA2D
/*Must be defined to include path of CMSIS header of target processor
e.g. "stm32f769xx.h" or "stm32f429xx.h"*/
#define LV_GPU_DMA2D_CMSIS_INCLUDE
#endif

/** Use NXP's PXP GPU iMX RTxxx platforms*/
#define LV_USE_GPU_NXP_PXP      0
#if LV_USE_GPU_NXP_PXP
/*1: Add default bare metal and FreeRTOS interrupt handling routines for PXP (lv_gpu_nxp_pxp_osa.c)
 *   and call lv_gpu_nxp_pxp_init() automatically during lv_init(). Note that symbol SDK_OS_FREE_RTOS
 *   has to be defined in order to use FreeRTOS OSA, otherwise bare-metal implementation is selected.
 *0: lv_gpu_nxp_pxp_init() has to be called manually before lv_init()
 */
#define LV_USE_GPU_NXP_PXP_AUTO_INIT 0
#endif

/** Use NXP's VG-Lite GPU iMX RTxxx platforms*/
#define LV_USE_GPU_NXP_VG_LITE   0

/*-------------
 * Logging
 *-----------*/

/*Enable the log module*/
#define LV_USE_LOG      0
#if LV_USE_LOG

/*How important log should be added:
 *LV_LOG_LEVEL_TRACE       A lot of logs to give detailed information
 *LV_LOG_LEVEL_INFO        Log important events
 *LV_LOG_LEVEL_WARN        Log if something unwanted happened but didn't cause a problem
 *LV_LOG_LEVEL_ERROR       Only critical issue, when the system may fail
 *LV_LOG_LEVEL_USER        Only logs added by the user
 *LV_LOG_LEVEL_NONE        Do not log anything*/
#  define LV_LOG_LEVEL    LV_LOG_LEVEL_WARN

/*1: Print the log with 'printf';
 *0: User need to register a callback with `lv_log_register_print_cb()`*/
#  define LV_LOG_PRINTF   0

/*Enable/disable LV_LOG_TRACE in modules that produces a huge number of logs*/
#  define LV_LOG_TRACE_MEM            1
#  define LV_LOG_TRACE_TIMER          1
#  define LV_LOG_TRACE_INDEV          1
#  define LV_LOG_TRACE_DISP_REFR      1
#  define LV_LOG_TRACE_EVENT          1
#  define LV_LOG_TRACE_OBJ_CREATE     1
#  define LV_LOG_TRACE_LAYOUT         1
#  define LV_LOG_TRACE_ANIM           1

#endif  /*LV_USE_LOG*/

/*-------------
 * Asserts
 *-----------*/

/*Enable asserts if an operation is failed or an invalid data is found.
 *If LV_USE_LOG is enabled an error message will be printed on failure*/
#define LV_USE_ASSERT_NULL          0   /**< Check if the parameter is NULL. (Very fast, recommended)*/
#define LV_USE_ASSERT_MALLOC        0   /**< Checks is the memory is successfully allocated or no. (Very fast, recommended)*/
#define LV_USE_ASSERT_STYLE         0   /**< Check if the styles are properly initialized. (Very fast, recommended)*/
#define LV_USE_ASSERT_MEM_INTEGRITY 0   /**< Check the integrity of `lv_mem` after critical operations. (Slow)*/
#define LV_USE_ASSERT_OBJ           0   /**< Check the object's type and existence (e.g. not deleted). (Slow)*/

/** Add a custom handler when assert happens e.g. to restart the MCU*/
#define LV_ASSERT_HANDLER_INCLUDE   <stdint.h>
#define LV_ASSERT_HANDLER   while(1);   /**< Halt by default*/

/*-------------
 * Others
 *-----------*/

/*1: Show CPU usage and FPS count in the right bottom corner*/
#define LV_USE_PERF_MONITOR     0

/** 1: Show the used memory and the memory fragmentation  in the left bottom corner
 * Requires LV_MEM_CUSTOM = 0*/
#define LV_USE_MEM_MONITOR      0

/** 1: Draw random colored rectangles over the redrawn areas*/
#define LV_USE_REFR_DEBUG       0

/** Change the built in (v)snprintf functions*/
#define LV_SPRINTF_CUSTOM   0
#if LV_SPRINTF_CUSTOM
#  define LV_SPRINTF_INCLUDE <stdio.h>
#  define lv_snprintf     snprintf
#  define lv_vsnprintf    vsnprintf
#else   /*LV_SPRINTF_CUSTOM*/
#  define LV_SPRINTF_USE_FLOAT 0  /**< Don't use float with sprintf */
#endif  /*LV_SPRINTF_CUSTOM*/

#define LV_USE_USER_DATA      1

/*Garbage Collector settings
 *Used if lvgl is binded to higher level language and the memory is managed by that language*/
#define LV_ENABLE_GC 0
#if LV_ENABLE_GC != 0
#  define LV_GC_INCLUDE "gc.h"                           /*Include Garbage Collector related things*/
#endif /*LV_ENABLE_GC*/

/*=====================
 *  COMPILER SETTINGS
 *====================*/

/*For big endian systems set to 1*/
#define LV_BIG_ENDIAN_SYSTEM    0

/*Define a custom attribute to `lv_tick_inc` function*/
#define LV_ATTRIBUTE_TICK_INC

/** Define a custom attribute to `lv_timer_handler` function*/
#define LV_ATTRIBUTE_TIMER_HANDLER

/*Define a custom attribute to `lv_disp_flush_ready` function*/
#define LV_ATTRIBUTE_FLUSH_READY

/*Required alignment size for buffers*/
#define LV_ATTRIBUTE_MEM_ALIGN_SIZE 1

/*Attribute to mark large constant arrays for example font's bitmaps*/
#define LV_ATTRIBUTE_LARGE_CONST

/** Compiler prefix for a big array declaration in RAM*/
#define LV_ATTRIBUTE_LARGE_RAM_ARRAY

/*Place performance critical functions into a faster memory (e.g RAM)*/
#define LV_ATTRIBUTE_FAST_MEM

/*Prefix variables that are used in GPU accelerated operations, often these need to be placed in RAM sections that are DMA accessible*/
#define LV_ATTRIBUTE_DMA

/*Export integer constant to binding. This macro is used with constants in the form of LV_<CONST> that
 *should also appear on LVGL binding API such as Micropython.*/
#define LV_EXPORT_CONST_INT(int_value) struct _silence_gcc_warning /*The default value just prevents GCC warning*/

/** Extend the default -32k..32k coordinate range to -4M..4M by using int32_t for coordinates instead of int16_t*/
#define LV_USE_LARGE_COORD  0

/*==================
 *   FONT USAGE
 *===================*/

/** @name Montserrat fonts with ASCII range and some symbols using bpp = 4
 *https://fonts.google.com/specimen/Montserrat
 * @{
 */
#define LV_FONT_MONTSERRAT_8     0  /**< Montserrat font size 8 */
#define LV_FONT_MONTSERRAT_10    0  /**< Montserrat font size 10 */
#define LV_FONT_MONTSERRAT_12    1  /**< Montserrat font size 12 */
#define LV_FONT_MONTSERRAT_14    0  /**< Montserrat font size 14 */
#define LV_FONT_MONTSERRAT_16    0  /**< Montserrat font size 16 */
#define LV_FONT_MONTSERRAT_18    0  /**< Montserrat font size 18 */
#define LV_FONT_MONTSERRAT_20    0  /**< Montserrat font size 20 */
#define LV_FONT_MONTSERRAT_22    0  /**< Montserrat font size 22 */
#define LV_FONT_MONTSERRAT_24    0  /**< Montserrat font size 24 */
#define LV_FONT_MONTSERRAT_26    0  /**< Montserrat font size 26 */
#define LV_FONT_MONTSERRAT_28    0  /**< Montserrat font size 28 */
#define LV_FONT_MONTSERRAT_30    0  /**< Montserrat font size 30 */
#define LV_FONT_MONTSERRAT_32    0  /**< Montserrat font size 32 */
#define LV_FONT_MONTSERRAT_34    0  /**< Montserrat font size 34 */
#define LV_FONT_MONTSERRAT_36    0  /**< Montserrat font size 36 */
#define LV_FONT_MONTSERRAT_38    0  /**< Montserrat font size 38 */
#define LV_FONT_MONTSERRAT_40    0  /**< Montserrat font size 40 */
#define LV_FONT_MONTSERRAT_42    0  /**< Montserrat font size 42 */
#define LV_FONT_MONTSERRAT_44    0  /**< Montserrat font size 44 */
#define LV_FONT_MONTSERRAT_46    0  /**< Montserrat font size 46 */
#define LV_FONT_MONTSERRAT_48    0  /**< Montserrat font size 48 */
/** @} */

/*Demonstrate special features*/
#define LV_FONT_MONTSERRAT_12_SUBPX      0
#define LV_FONT_MONTSERRAT_28_COMPRESSED 0  /*bpp = 3*/
#define LV_FONT_DEJAVU_16_PERSIAN_HEBREW 0  /*Hebrew, Arabic, Perisan letters and all their forms*/
#define LV_FONT_SIMSUN_16_CJK            0  /*1000 most common CJK radicals*/

/** @name Pixel perfect monospace fonts
 * @{
 */
#define LV_FONT_UNSCII_8        0
#define LV_FONT_UNSCII_16       0
/** @} */

/*Optionally declare custom fonts here.
 *You can use these fonts as default font too and they will be available globally.
 *E.g. #define LV_FONT_CUSTOM_DECLARE   LV_FONT_DECLARE(my_font_1) LV_FONT_DECLARE(my_font_2)*/
#define LV_FONT_CUSTOM_DECLARE

/*Always set a default font*/
#define LV_FONT_DEFAULT &lv_font_montserrat_12

/*Enable handling large font and/or fonts with a lot of characters.
 *The limit depends on the font size, font face and bpp.
 *Compiler error will be triggered if a font needs it.*/
#define LV_FONT_FMT_TXT_LARGE   0

/*Enables/disables support for compressed fonts.*/
#define LV_USE_FONT_COMPRESSED  0

/*Enable subpixel rendering*/
#define LV_USE_FONT_SUBPX       0
#if LV_USE_FONT_SUBPX
/*Set the pixel order of the display. Physical order of RGB channels. Doesn't matter with "normal" fonts.*/
#define LV_FONT_SUBPX_BGR       0  /*0: RGB; 1:BGR order*/
#endif

/*=================
 *  TEXT SETTINGS
 *=================*/

/**
 * Select a character encoding for strings.
 * Your IDE or editor should have the same character encoding
 * - LV_TXT_ENC_UTF8
 * - LV_TXT_ENC_ASCII
 */
#define LV_TXT_ENC LV_TXT_ENC_ASCII

 /*Can break (wrap) texts on these chars*/
#define LV_TXT_BREAK_CHARS                  " ,.;:-_"

/*If a word is at least this long, will break wherever "prettiest"
 *To disable, set to a value <= 0*/
#define LV_TXT_LINE_BREAK_LONG_LEN          0

/*Minimum number of characters in a long word to put on a line before a break.
 *Depends on LV_TXT_LINE_BREAK_LONG_LEN.*/
#define LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN  3

/*Minimum number of characters in a long word to put on a line after a break.
 *Depends on LV_TXT_LINE_BREAK_LONG_LEN.*/
#define LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN 3

/*The control character to use for signalling text recoloring.*/
#define LV_TXT_COLOR_CMD "#"

/*Support bidirectional texts. Allows mixing Left-to-Right and Right-to-Left texts.
 *The direction will be processed according to the Unicode Bidirectioanl Algorithm:
 *https://www.w3.org/International/articles/inline-bidi-markup/uba-basics*/
#define LV_USE_BIDI         0
#if LV_USE_BIDI
/*Set the default direction. Supported values:
 *`LV_BASE_DIR_LTR` Left-to-Right
 *`LV_BASE_DIR_RTL` Right-to-Left
 *`LV_BASE_DIR_AUTO` detect texts base direction*/
#define LV_BIDI_BASE_DIR_DEF  LV_BASE_DIR_AUTO
#endif

/*Enable Arabic/Persian processing
 *In these languages characters should be replaced with an other form based on their position in the text*/
#define LV_USE_ARABIC_PERSIAN_CHARS 0

/*==================
 *  WIDGET USAGE
 *================*/

/*Documentation of the widgets: https://docs.lvgl.io/latest/en/html/widgets/index.html*/

#define LV_USE_ARC          IS_USED(MODULE_LVGL_WIDGET_ARC)

#define LV_USE_ANIMIMG	    IS_USED(MODULE_LVGL_EXTRA_WIDGET_ANIMIMG)

#define LV_USE_BAR          IS_USED(MODULE_LVGL_WIDGET_BAR)

#define LV_USE_BTN          IS_USED(MODULE_LVGL_WIDGET_BTN)

#define LV_USE_BTNMATRIX    IS_USED(MODULE_LVGL_WIDGET_BTNMATRIX)

#define LV_USE_CANVAS       IS_USED(MODULE_LVGL_WIDGET_CANVAS)

#define LV_USE_CHECKBOX     IS_USED(MODULE_LVGL_WIDGET_CHECKBOX)

#define LV_USE_DROPDOWN     IS_USED(MODULE_LVGL_WIDGET_DROPDOWN)   /*Requires: lv_label*/

#define LV_USE_IMG          IS_USED(MODULE_LVGL_WIDGET_IMG)   /*Requires: lv_label*/

#define LV_USE_LABEL        IS_USED(MODULE_LVGL_WIDGET_LABEL)
#if LV_USE_LABEL
#ifndef LV_LABEL_TEXT_SELECTION
#define LV_LABEL_TEXT_SELECTION   1   /**< Enable selecting text of the label*/
#endif
#ifndef LV_LABEL_LONG_TXT_HINT
#define LV_LABEL_LONG_TXT_HINT    1   /**< Store some extra info in labels to speed up drawing of very long texts*/
#endif
#endif

#define LV_USE_LINE         IS_USED(MODULE_LVGL_WIDGET_LINE)

#define LV_USE_ROLLER       IS_USED(MODULE_LVGL_WIDGET_ROLLER)   /*Requires: lv_label*/
#if LV_USE_ROLLER
#  define LV_ROLLER_INF_PAGES       7   /*Number of extra "pages" when the roller is infinite*/
#endif

#define LV_USE_SLIDER       IS_USED(MODULE_LVGL_WIDGET_SLIDER)   /*Requires: lv_bar*/

#define LV_USE_SWITCH       IS_USED(MODULE_LVGL_WIDGET_SWITCH)

#define LV_USE_TEXTAREA     IS_USED(MODULE_LVGL_WIDGET_TEXTAREA)     /*Requires: lv_label*/
#if LV_USE_TEXTAREA != 0
#  define LV_TEXTAREA_DEF_PWD_SHOW_TIME     1500    /*ms*/
#endif

#define LV_USE_TABLE        IS_USED(MODULE_LVGL_WIDGET_TABLE)

/*==================
 * EXTRA COMPONENTS
 *==================*/

/*-----------
 * Widgets
 *----------*/

#define LV_USE_CALENDAR     IS_USED(MODULE_LVGL_EXTRA_WIDGET_CALENDAR)
#if LV_USE_CALENDAR
# define LV_CALENDAR_WEEK_STARTS_MONDAY 0
# if LV_CALENDAR_WEEK_STARTS_MONDAY
#  define LV_CALENDAR_DEFAULT_DAY_NAMES {"Mo", "Tu", "We", "Th", "Fr", "Sa", "Su"}
# else
#  define LV_CALENDAR_DEFAULT_DAY_NAMES {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"}
# endif

# define LV_CALENDAR_DEFAULT_MONTH_NAMES {"January", "February", "March",  "April", "May",  "June", "July", "August", "September", "October", "November", "December"}
# define LV_USE_CALENDAR_HEADER_ARROW       1
# define LV_USE_CALENDAR_HEADER_DROPDOWN    1
#endif  /*LV_USE_CALENDAR*/

#define LV_USE_CHART        IS_USED(MODULE_LVGL_EXTRA_WIDGET_CHART)

#define LV_USE_COLORWHEEL   IS_USED(MODULE_LVGL_EXTRA_WIDGET_COLORWHEEL)

#define LV_USE_IMGBTN       IS_USED(MODULE_LVGL_EXTRA_WIDGET_IMGBTN)

#define LV_USE_KEYBOARD     IS_USED(MODULE_LVGL_EXTRA_WIDGET_KEYBOARD)

#define LV_USE_LED          IS_USED(MODULE_LVGL_EXTRA_WIDGET_LED)

#define LV_USE_LIST         IS_USED(MODULE_LVGL_EXTRA_WIDGET_LIST)

#define LV_USE_MENU         IS_USED(MODULE_LVGL_EXTRA_WIDGET_MENU)

#define LV_USE_METER        IS_USED(MODULE_LVGL_EXTRA_WIDGET_METER)

#define LV_USE_MSGBOX       IS_USED(MODULE_LVGL_EXTRA_WIDGET_MSGBOX)

#define LV_USE_SPINBOX      IS_USED(MODULE_LVGL_EXTRA_WIDGET_SPINBOX)

#define LV_USE_SPINNER      IS_USED(MODULE_LVGL_EXTRA_WIDGET_SPINNER)

#define LV_USE_TABVIEW      IS_USED(MODULE_LVGL_EXTRA_WIDGET_TABVIEW)

#define LV_USE_TILEVIEW     IS_USED(MODULE_LVGL_EXTRA_WIDGET_TILEVIEW)

#define LV_USE_WIN          IS_USED(MODULE_LVGL_EXTRA_WIDGET_WIN)

#define LV_USE_SPAN         IS_USED(MODULE_LVGL_EXTRA_WIDGET_SPAN)
#if LV_USE_SPAN
/*A line text can contain maximum num of span descriptor */
#  define LV_SPAN_SNIPPET_STACK_SIZE   64
#endif

/*-----------
 * Themes
 *----------*/
/*A simple, impressive and very complete theme*/
#define LV_USE_THEME_DEFAULT      IS_USED(MODULE_LVGL_EXTRA_THEME_DEFAULT)

/*0: Light mode; 1: Dark mode*/
#define LV_THEME_DEFAULT_DARK     IS_USED(MODULE_LVGL_EXTRA_THEME_DEFAULT_DARK)

/*1: Enable grow on press*/
#define LV_THEME_DEFAULT_GROW     IS_USED(MODULE_LVGL_EXTRA_THEME_DEFAULT_GROW)

/*Default transition time in [ms]*/
# define LV_THEME_DEFAULT_TRANSITON_TIME    80

/*An very simple them that is a good starting point for a custom theme*/
#define LV_USE_THEME_BASIC        IS_USED(MODULE_LVGL_EXTRA_THEME_BASIC)

/*A theme designed for monochrome displays*/
#define LV_USE_THEME_MONO         IS_USED(MODULE_LVGL_EXTRA_THEME_MONO)

/*-----------
 * Layouts
 *----------*/

/*A layout similar to Flexbox in CSS.*/
#define LV_USE_FLEX               IS_USED(MODULE_LVGL_EXTRA_LAYOUT_FLEX)

/*A layout similar to Grid in CSS.*/
#define LV_USE_GRID               IS_USED(MODULE_LVGL_EXTRA_LAYOUT_GRID)

#ifdef __cplusplus
}
#endif

#endif /* LV_CONF_H */
/** @} */
