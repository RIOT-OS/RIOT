/*
 * SPDX-FileCopyrightText: 2019 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     pkg_lvgl
 * @brief       LVGL configuration macros
 *
 * Only the options that differ from the upstream defaults (see
 * `lv_conf_template.h` in the LVGL sources) or that are driven by RIOT modules
 * are defined here. All other options keep the values set by
 * `lvgl/include/lvgl/config/lv_conf_internal.h`.
 *
 * Most options can be overridden from the application using CFLAGS.
 * @{
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "kernel_defines.h"
#include "lvgl_riot_conf.h"

/** LVGL package requires this definition to confirm that the configuration has been included. */
#define LV_CONF_H   1

/*====================
 * MEMORY SETTINGS
 *====================*/

/** Use the LVGL built-in memory allocator (TLSF) */
#define LV_USE_STDLIB_MALLOC    LV_STDLIB_BUILTIN

/** Use the LVGL built-in string functions */
#define LV_USE_STDLIB_STRING    LV_STDLIB_BUILTIN

/** Use the LVGL built-in sprintf implementation */
#define LV_USE_STDLIB_SPRINTF   LV_STDLIB_BUILTIN

/** Size of the memory pool used by `lv_malloc()` in bytes (>= 2kB)
 *
 * The draw tasks of a frame, with the copies of the points of the lines, are
 * also allocated from this pool: with 10 KiB, the lines of a chart were not
 * drawn on a 32-bit platform. */
#ifndef LV_MEM_SIZE
#  if (__SIZEOF_POINTER__ > 4)
/* 64-bit platforms require additional space because a lot of pointers are stored on the lvgl heap. */
#    if IS_USED(MODULE_LVGL_THEME_DEFAULT_GROW)
#      define LV_MEM_SIZE    (25U * 1024U)
#    else
#      define LV_MEM_SIZE    (24U * 1024U)
#    endif
#  else
#    if IS_USED(MODULE_LVGL_THEME_DEFAULT_GROW)
#      define LV_MEM_SIZE    (17U * 1024U)
#    else
#      define LV_MEM_SIZE    (16U * 1024U)
#    endif
#  endif
#endif

/*====================
 * OPERATING SYSTEM
 *====================*/

/** LVGL is only used from a single RIOT thread, no OS abstraction is needed */
#define LV_USE_OS               LV_OS_NONE

/*========================
 * RENDERING SETTINGS
 *========================*/

/** Default color format of the displays.
 *
 * Monochrome displays are switched to LV_COLOR_FORMAT_I1 at runtime.
 */
#define LV_COLOR_FORMAT_DEFAULT LV_COLOR_FORMAT_RGB565

/** Pixel formats supported by the software renderer.
 *
 * Only the formats used by the RIOT display drivers are enabled by default to
 * save ROM. Enable the other formats when drawing images using them.
 */
#ifndef LV_DRAW_SW_SUPPORT_RGB565_SWAPPED
/** Support of the RGB565 swapped pixel format */
#  define LV_DRAW_SW_SUPPORT_RGB565_SWAPPED         0
#endif
#ifndef LV_DRAW_SW_SUPPORT_RGB888
/** Support of the RGB888 pixel format */
#  define LV_DRAW_SW_SUPPORT_RGB888                 0
#endif
#ifndef LV_DRAW_SW_SUPPORT_XRGB8888
/** Support of the XRGB8888 pixel format */
#  define LV_DRAW_SW_SUPPORT_XRGB8888               IS_USED(MODULE_LVGL_SDL)
#endif
#ifndef LV_DRAW_SW_SUPPORT_ARGB8888
/** Support of the ARGB8888 pixel format */
#  define LV_DRAW_SW_SUPPORT_ARGB8888               IS_USED(MODULE_LVGL_SDL)
#endif
#ifndef LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED
/** Support of the ARGB8888 premultiplied pixel format */
#  define LV_DRAW_SW_SUPPORT_ARGB8888_PREMULTIPLIED 0
#endif
#ifndef LV_DRAW_SW_SUPPORT_L8
/** Support of the L8 pixel format */
#  define LV_DRAW_SW_SUPPORT_L8                     0
#endif
#ifndef LV_DRAW_SW_SUPPORT_AL88
/** Support of the AL88 pixel format */
#  define LV_DRAW_SW_SUPPORT_AL88                   0
#endif
#ifndef LV_DRAW_SW_SUPPORT_A8
/** Support of the A8 pixel format */
#  define LV_DRAW_SW_SUPPORT_A8                     0
#endif
#ifndef LV_DRAW_SW_SUPPORT_I1
/** Support of the I1 (monochrome) pixel format */
#  define LV_DRAW_SW_SUPPORT_I1                     (IS_USED(MODULE_U8G2_DISP_DEV) || \
                                                   IS_USED(MODULE_EPD_BW_SPI))
#endif

/*====================
 * CORE SETTINGS
 *====================*/

/** Widget subject observers are not used by RIOT */
#ifndef LV_USE_OBSERVER
/** Not supported */
#  define LV_USE_OBSERVER       0
#endif

/*====================
 * FONT USAGE
 *====================*/

/** Montserrat font size 12 */
#ifndef LV_FONT_MONTSERRAT_12
#  define LV_FONT_MONTSERRAT_12 1
#endif
/** Montserrat font size 14 */
#ifndef LV_FONT_MONTSERRAT_14
#  define LV_FONT_MONTSERRAT_14 0
#endif

/** Always set a default font */
#ifndef LV_FONT_DEFAULT
#  define LV_FONT_DEFAULT       LV_FONT_DEFAULT_MONTSERRAT_12
#endif

/*====================
 * WIDGET USAGE
 *====================*/

/** Not supported */
#define LV_USE_3DTEXTURE        0
/** Enabled with lvgl_widget_animimage */
#define LV_USE_ANIMIMG          IS_USED(MODULE_LVGL_WIDGET_ANIMIMAGE)
/** Enabled with lvgl_widget_arc */
#define LV_USE_ARC              IS_USED(MODULE_LVGL_WIDGET_ARC)
/** Not supported */
#define LV_USE_ARCLABEL         0
/** Enabled with lvgl_widget_bar */
#define LV_USE_BAR              IS_USED(MODULE_LVGL_WIDGET_BAR)
/** Enabled with lvgl_widget_button */
#define LV_USE_BUTTON           IS_USED(MODULE_LVGL_WIDGET_BUTTON)
/** Enabled with lvgl_widget_buttonmatrix */
#define LV_USE_BUTTONMATRIX     IS_USED(MODULE_LVGL_WIDGET_BUTTONMATRIX)
/** Enabled with lvgl_widget_calendar */
#define LV_USE_CALENDAR         IS_USED(MODULE_LVGL_WIDGET_CALENDAR)
/** Enabled with lvgl_widget_canvas */
#define LV_USE_CANVAS           IS_USED(MODULE_LVGL_WIDGET_CANVAS)
/** Enabled with lvgl_widget_chart */
#define LV_USE_CHART            IS_USED(MODULE_LVGL_WIDGET_CHART)
/** Enabled with lvgl_widget_checkbox */
#define LV_USE_CHECKBOX         IS_USED(MODULE_LVGL_WIDGET_CHECKBOX)
/** Enabled with lvgl_widget_dropdown */
#define LV_USE_DROPDOWN         IS_USED(MODULE_LVGL_WIDGET_DROPDOWN)
/** Enabled with lvgl_widget_image */
#define LV_USE_IMAGE            IS_USED(MODULE_LVGL_WIDGET_IMAGE)
/** Enabled with lvgl_widget_imagebutton */
#define LV_USE_IMAGEBUTTON      IS_USED(MODULE_LVGL_WIDGET_IMAGEBUTTON)
/** Enabled with lvgl_widget_keyboard */
#define LV_USE_KEYBOARD         IS_USED(MODULE_LVGL_WIDGET_KEYBOARD)
/** Enabled with lvgl_widget_label */
#define LV_USE_LABEL            IS_USED(MODULE_LVGL_WIDGET_LABEL)
/** Enabled with lvgl_widget_led */
#define LV_USE_LED              IS_USED(MODULE_LVGL_WIDGET_LED)
/** Enabled with lvgl_widget_line */
#define LV_USE_LINE             IS_USED(MODULE_LVGL_WIDGET_LINE)
/** Enabled with lvgl_widget_list */
#define LV_USE_LIST             IS_USED(MODULE_LVGL_WIDGET_LIST)
/** Enabled with lvgl_widget_menu */
#define LV_USE_MENU             IS_USED(MODULE_LVGL_WIDGET_MENU)
/** Enabled with lvgl_widget_msgbox */
#define LV_USE_MSGBOX           IS_USED(MODULE_LVGL_WIDGET_MSGBOX)
/** Enabled with lvgl_widget_roller */
#define LV_USE_ROLLER           IS_USED(MODULE_LVGL_WIDGET_ROLLER)
/** Enabled with lvgl_widget_scale */
#define LV_USE_SCALE            IS_USED(MODULE_LVGL_WIDGET_SCALE)
/** Enabled with lvgl_widget_slider */
#define LV_USE_SLIDER           IS_USED(MODULE_LVGL_WIDGET_SLIDER)
/** Enabled with lvgl_widget_span */
#define LV_USE_SPAN             IS_USED(MODULE_LVGL_WIDGET_SPAN)
/** Enabled with lvgl_widget_spinbox */
#define LV_USE_SPINBOX          IS_USED(MODULE_LVGL_WIDGET_SPINBOX)
/** Enabled with lvgl_widget_spinner */
#define LV_USE_SPINNER          IS_USED(MODULE_LVGL_WIDGET_SPINNER)
/** Enabled with lvgl_widget_switch */
#define LV_USE_SWITCH           IS_USED(MODULE_LVGL_WIDGET_SWITCH)
/** Enabled with lvgl_widget_table */
#define LV_USE_TABLE            IS_USED(MODULE_LVGL_WIDGET_TABLE)
/** Enabled with lvgl_widget_tabview */
#define LV_USE_TABVIEW          IS_USED(MODULE_LVGL_WIDGET_TABVIEW)
/** Enabled with lvgl_widget_textarea */
#define LV_USE_TEXTAREA         IS_USED(MODULE_LVGL_WIDGET_TEXTAREA)
/** Enabled with lvgl_widget_tileview */
#define LV_USE_TILEVIEW         IS_USED(MODULE_LVGL_WIDGET_TILEVIEW)
/** Enabled with lvgl_widget_win */
#define LV_USE_WIN              IS_USED(MODULE_LVGL_WIDGET_WIN)

/*==================
 * THEMES
 *==================*/

/** A simple, impressive and very complete theme */
#define LV_USE_THEME_DEFAULT        IS_USED(MODULE_LVGL_THEME_DEFAULT)
#if LV_USE_THEME_DEFAULT
/** 0: Light mode; 1: Dark mode */
#  define LV_THEME_DEFAULT_DARK     IS_USED(MODULE_LVGL_THEME_DEFAULT_DARK)
/** 1: Enable grow on press */
#  define LV_THEME_DEFAULT_GROW     IS_USED(MODULE_LVGL_THEME_DEFAULT_GROW)
#endif

/** A very simple theme that is a good starting point for a custom theme */
#define LV_USE_THEME_SIMPLE         IS_USED(MODULE_LVGL_THEME_SIMPLE)

/** A theme designed for monochrome displays */
#define LV_USE_THEME_MONO           IS_USED(MODULE_LVGL_THEME_MONO)

/*==================
 * LAYOUTS
 *==================*/

/** A layout similar to Flexbox in CSS */
#define LV_USE_FLEX                 IS_USED(MODULE_LVGL_LAYOUT_FLEX)

/** A layout similar to Grid in CSS */
#define LV_USE_GRID                 IS_USED(MODULE_LVGL_LAYOUT_GRID)

/*==================
 * DRIVERS
 *==================*/

/** SDL2 based display and mouse driver, used on native */
#define LV_USE_SDL                  IS_USED(MODULE_LVGL_SDL)
#if LV_USE_SDL
#  define LV_SDL_INCLUDE_PATH       <SDL2/SDL.h>
#  define LV_SDL_RENDER_MODE        LV_DISPLAY_RENDER_MODE_DIRECT
#  define LV_SDL_BUF_COUNT          1
#  define LV_SDL_ACCELERATED        0
#  define LV_SDL_FULLSCREEN         0
#  define LV_SDL_DIRECT_EXIT        1
#endif

/*==================
 * DEBUGGING
 *==================*/

/** Logging is disabled by default */
#ifndef LV_USE_LOG
/** Not supported */
#  define LV_USE_LOG                0
#endif

/*==================
 * BUILD
 *==================*/

/** Don't build the examples */
#define LV_BUILD_EXAMPLES           0
/** Don't build the demos */
#define LV_BUILD_DEMOS              0

#ifdef __cplusplus
}
#endif

/** @} */
