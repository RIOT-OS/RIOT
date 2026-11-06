/*
 * SPDX-FileCopyrightText: 2026 TU Dresden
 * SPDX-FileCopyrightText: 2026 Carl Seifert <carl.seifert@tu-dresden.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup core_macros_ansi_sgr ANSI Select Graphic Rendition (SGR) for styled terminal text
 * @ingroup core_macros
 * @brief Macros to apply color and style to text in supporting terminal emulators
 *
 * @{
 *
 * Use @ref ANSI_STYLED, and @ref ANSI_STYLE to customize text
 * color, color intensities, and text styles, such as bold or underlined text.
 *
 * ```c
 * const char* just_bold1    = ANSI_STYLE(BOLD) "Error!" ANSI_STYLE();
 * const char* just_bold2    = ANSI_STYLED("Error!", BOLD);
 *
 * const char* just_colored1 = ANSI_STYLE(FOREGROUND(RED)) "Error!" ANSI_STYLE();
 * const char* just_colored2 = ANSI_STYLED("Error!", FOREGROUND(RED));
 *
 * const char* fancy1 =
 *   ANSI_STYLE(FOREGROUND_BRIGHT(YELLOW), BACKGROUND(PURPLE), BOLD, UNDERLINED) \
 *   "Hello," \
 *   ANSI_STYLE(BACKGROUND_BRIGHT(PRIMARY), ITALIC) \
 *   " World!" \
 *   ANSI_STYLE();
 *
 * const char* fancy2 =
 *   ANSI_STYLED("Hello,", FOREGROUND_BRIGHT(YELLOW), BACKGROUND(PURPLE), BOLD, UNDERLINED) \
 *   ANSI_STYLED(" World!", BACKGROUND_BRIGHT(PRIMARY), ITALIC);
 * ```
 */

/**
 * @file
 * @brief ANSI SGR definitions
 * @author Carl Seifert <carl.seifert@tu-dresden.de>
 */

#include "macros/xtstr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name SGR escape sequence
 * @{
 */

/** @brief ANSI escape prefix */
#define _ANSI_SGR_PREFIX         "\x1b["

/** @brief ANSI color/style suffix */
#define _ANSI_SGR_SUFFIX         "m"

/** @} */

/**
 * @name Color codes
 * @{
 */

/** @brief ANSI color code for primary color (black in light appearance, white in dark) */
#define _ANSI_COLOR_CODE_PRIMARY "0"

/** @brief Alias for @ref _ANSI_COLOR_CODE_PRIMARY */
#define _ANSI_COLOR_CODE_BLACK   _ANSI_COLOR_CODE_PRIMARY

/** @brief ANSI color code for red */
#define _ANSI_COLOR_CODE_RED     "1"

/** @brief ANSI color code for green */
#define _ANSI_COLOR_CODE_GREEN   "2"

/** @brief ANSI color code for yellow */
#define _ANSI_COLOR_CODE_YELLOW  "3"

/** @brief ANSI color code for blue */
#define _ANSI_COLOR_CODE_BLUE    "4"

/** @brief ANSI color code for purple (pink, magenta) */
#define _ANSI_COLOR_CODE_PURPLE  "5"

/** @brief ANSI color code for cyan (light blue) */
#define _ANSI_COLOR_CODE_CYAN    "6"

/** @brief ANSI color code for white (gray) */
#define _ANSI_COLOR_CODE_WHITE   "7"

/** @brief ANSI color code for 24-bit RGB colors */
#define _ANSI_COLOR_CODE_RGB(r, g, b) "8;2;" XTSTR(r) ";" XTSTR(g) ";" XTSTR(b)

/** @brief ANSI color code for 8-bit indexed colors */
#define _ANSI_COLOR_CODE_INDEXED(index) "8;5;" XTSTR(index)

/** @} */

/**
 * @name Color modifiers
 * @{
 */

/** @brief ANSI modifier to make color apply to foreground text */
#define _ANSI_COLOR_MODIFIER_FOREGROUND        "3"

/** @brief ANSI modifier to make color apply to foreground text in high intensity */
#define _ANSI_COLOR_MODIFIER_BRIGHT_FOREGROUND "9"

/** @brief ANSI modifier to make color apply to background */
#define _ANSI_COLOR_MODIFIER_BACKGROUND        "4"

/** @brief ANSI modifier to make color apply to background in high intensity */
#define _ANSI_COLOR_MODIFIER_BRIGHT_BACKGROUND "10"

/** @} */

/**
 * @name Color styles
 * @{
 */

#ifndef DOXYGEN
/* This macro resolves the color suffix (preprocessor token) to the macro, e.g.
 * RED => _ANSI_COLOR_CODE_RED. */
#  define __ANSI_COLOR(color) _ANSI_COLOR_CODE_ ## color
#endif

/** @brief ANSI style for foreground text color */
#define ANSI_STYLE_FOREGROUND(color) \
    _ANSI_COLOR_MODIFIER_FOREGROUND __ANSI_COLOR(color)

/**
 * @brief Alias for @ref ANSI_STYLE_FOREGROUND
 * @experimental
 */
#define ANSI_STYLE_FG(color) ANSI_STYLE_FOREGROUND(color)

/** @brief ANSI style for a brighter foreground text color */
#define ANSI_STYLE_FOREGROUND_BRIGHT(color) \
    _ANSI_COLOR_MODIFIER_BRIGHT_FOREGROUND __ANSI_COLOR(color)

/**
 * @brief Alias for @ref ANSI_STYLE_FOREGROUND_BRIGHT
 * @experimental
 */
#define ANSI_STYLE_FG_BRIGHT(color) ANSI_STYLE_FOREGROUND_BRIGHT(color)

/** @brief ANSI style for foreground text color */
#define ANSI_STYLE_BACKGROUND(color) \
    _ANSI_COLOR_MODIFIER_BACKGROUND __ANSI_COLOR(color)

/**
 * @brief Alias for @ref ANSI_STYLE_BACKGROUND
 * @experimental
 */
#define ANSI_STYLE_BG(color) ANSI_STYLE_BACKGROUND(color)

/** @brief ANSI style for a brighter foreground text color */
#define ANSI_STYLE_BACKGROUND_BRIGHT(color) \
    _ANSI_COLOR_MODIFIER_BRIGHT_BACKGROUND __ANSI_COLOR(color)

/**
 * @brief Alias for @ref ANSI_STYLE_BACKGROUND_BRIGHT
 * @experimental
 */
#define ANSI_STYLE_BG_BRIGHT(color) ANSI_STYLE_BACKGROUND_BRIGHT(color)

/** @} */

/**
 * @name Text styles
 * @{
 */

#ifndef DOXYGEN
#  define ANSI_STYLE_NONE                     "0"
#endif

/**
 * @brief ANSI style for bold text
 *
 * Must not be combined with @ref ANSI_STYLE_DIM
 */
#define ANSI_STYLE_BOLD                       "1"

/**
 * @brief ANSI style for dimmer, thinner, less prominent text
 *
 * Must not be combined with @ref ANSI_STYLE_BOLD
 */
#define ANSI_STYLE_DIM                        "2"

/** @brief ANSI style for italic text */
#define ANSI_STYLE_ITALIC                     "3"
/** @brief ANSI style for underlined text */
#define ANSI_STYLE_UNDERLINED                 "4"

/** @brief ANSI style for slowly blinking text */
#define ANSI_STYLE_BLINK_SLOW                 "5"
/** @brief ANSI style for fast-blinking text */
#define ANSI_STYLE_BLINK_FAST                 "6"

/** @brief ANSI style swapping foreground and background color settings */
#define ANSI_STYLE_NEGATED                    "7"

/** @brief ANSI style for concealed (redacted) text */
#define ANSI_STYLE_CONCEALED                  "8"

/** @brief ANSI style for crossed out text (strikethrough) */
#define ANSI_STYLE_STRIKETHROUGH              "9"

/** @} */

/**
 * @name Formatting macros
 * @{
 */

#ifndef DOXYGEN
/* These macros apply multiple SGR text styles given as suffixes of their respective
 * ANSI_STYLE_... macro, so styling macros at the expansion site are kept short.
 * Example: __ANSI_STYLE_2(BOLD, UNDERLINED) => ";"  ANSI_STYLE_BOLD ";" ANSI_STYLE_UNDERLINED
 * __ANSI_GET_STYLE implements variadic style parameters, so ANSI_STYLE can be given
 * anywhere between zero and 5 style macros. This limit may be increased in the future. */

/* __ANSI_GET_STYLE returns the correct concatenation macro (__ANSI_STYLE_N) based on the
 * number of arguments. */
#  define __ANSI_GET_STYLE(_0, _1, _2, _3, _4, _5, _6, name, ...) name
/* __ANSI_STYLED builds the longer style macro from just the suffix token. */
#  define __ANSI_STYLED(keyword)        ANSI_STYLE_ ## keyword
/* These macros recursively expand 1 to 5 style arguments. */
#  define __ANSI_STYLE_0()                 __ANSI_STYLED(NONE)
#  define __ANSI_STYLE_1(a)                __ANSI_STYLED(a)
#  define __ANSI_STYLE_2(a, b)             __ANSI_STYLE_1(a)             ";" __ANSI_STYLED(b)
#  define __ANSI_STYLE_3(a, b, c)          __ANSI_STYLE_2(a, b)          ";" __ANSI_STYLED(c)
#  define __ANSI_STYLE_4(a, b, c, d)       __ANSI_STYLE_3(a, b, c)       ";" __ANSI_STYLED(d)
#  define __ANSI_STYLE_5(a, b, c, d, e)    __ANSI_STYLE_4(a, b, c, d)    ";" __ANSI_STYLED(e)
#  define __ANSI_STYLE_6(a, b, c, d, e, f) __ANSI_STYLE_5(a, b, c, d, e) ";" __ANSI_STYLED(f)

/* This macro calls __ANSI_GET_STYLE and expands to the concatenated SGR
 * style string (";" .. ";" ...) */
#  define __ANSI_APPLY_STYLES(...) \
    __ANSI_GET_STYLE(dummy, ##__VA_ARGS__, \
        __ANSI_STYLE_6, __ANSI_STYLE_5, __ANSI_STYLE_4, __ANSI_STYLE_3, __ANSI_STYLE_2, \
        __ANSI_STYLE_1, __ANSI_STYLE_0 \
    )(__VA_ARGS__)

#endif

/**
 * @brief Builds string literal for a terminal emulator to apply color and text styles
 *        to subsequent text
 *
 * This macro uses ANSI Select Graphic Rendition (SGR) codes to instruct terminal emulators
 * that support SGR to apply custom styling to subsequent text.
 * You output this macro before any text, e.g., by calling `puts` or `printf` separately,
 * or by using static string literal concatenation in C. For example, this is how you would
 * format "Hello, World!" such that it appears purple, bold, and underlined. You must append
 * @ref ANSI_STYLE_RESET to return to the default terminal text format.
 *
 * ```c
 * const char* message =
 *   ANSI_STYLE(FOREGROUND(PURPLE), BOLD, UNDERLINED) "Hello, World!" ANSI_STYLE();
 * ```
 *
 * You can also override/chain the current format in the string using static string concatenation.
 *
 * ```c
 * const char* message =
 *   ANSI_STYLE(FOREGROUND(PURPLE), BOLD) "styled" ANSI_STYLE(BACKGROUND(YELLOW), ITALIC) \
 *   "styled" ANSI_STYLE_RESET;
 * ```
 *
 * @param ... Variadic style arguments to apply to following text,
 *.           e.g., `FOREGROUND(RED)`, `BOLD`, `UNDERLINED`
 *
 * You may apply multiple styles, currently up to 6, from the following categories.
 *
 * ## Color styling
 * Choose one of the `FOREGROUND(color)`, `BACKGROUND(color)`, or their brighter variants
 * `FOREGROUND_BRIGHT(color)` and `BACKGROUND_BRIGHT(color)` as an argument to this macro,
 * with `color` being one of the following:
 * - `PRIMARY` is black on white/light terminal backgrounds/themes and white in black/dark themes
 * - `RED`
 * - `GREEN`
 * - `YELLOW`
 * - `BLUE`
 * - `CYAN`
 * - `WHITE` may appear gray
 * - `RGB(r, g, b)` where `r`, `g`, and `b` are number literals between 0 and 255
 * - `INDEXED(index)` where `index` is the ANSI SGR color index between 0 and 255
 *
 * You may not use `FOREGROUND_BRIGHT` and `BACKGROUND_BRIGHT` with the `RGB` and `INDEXED` color
 * modes. These macros only make the predefined ANSI colors above appear brighter. You can add
 * up to one foreground style, and up to one background style, i.e., only the normal color
 * style or the `_BRIGHT` variant.
 *
 * ```c
 * ANSI_STYLE(FOREGROUND_BRIGHT(PURPLE), BACKGROUND_BRIGHT(YELLOW))
 * ANSI_STYLE(FOREGROUND_BRIGHT(PURPLE), BACKGROUND(YELLOW))
 * ANSI_STYLE(FOREGROUND(PURPLE), BACKGROUND_BRIGHT(YELLOW))
 * ANSI_STYLE(FOREGROUND(PURPLE), BACKGROUND(YELLOW))
 * ANSI_STYLE(FOREGROUND(PURPLE))
 * ANSI_STYLE(BACKGROUND(YELLOW))
 * ```
 *
 * ## Text styling
 * - `BOLD` makes text appear thicker
 * - `DIM` makes text appear lighter or less intense, i.e., darker
 *
 * The previous two styles are mutually exclusive (according to ANSI SGR). The following
 * may not be supported by every terminal emulator.
 *
 * - `ITALIC`
 * - `UNDERLINED`
 * - `STRIKETHROUGH` makes text appear crossed out
 * - `CONCEALED` hides the following text, but is still selectable
 * - `BLINK_FAST` makes text blink fast
 * - `BLINK_SLOW` makes text blink slowly
 * - `NEGATED` swaps foreground and background colors
 *
 * ```c
 * ANSI_STYLE(BOLD)
 * ANSI_STYLE(BOLD, UNDERLINED)
 * ANSI_STYLE(FOREGROUND_BRIGHT(CYAN), BOLD, UNDERLINED)
 * ```
 *
 * @returns Format string literal
 */
#define ANSI_STYLE(...) \
    _ANSI_SGR_PREFIX \
    __ANSI_APPLY_STYLES(__VA_ARGS__) \
    _ANSI_SGR_SUFFIX

/**
 * @brief Resets text color and style applied previously back to defaults
 *
 * Append this escape sequence after any styles applied using @ref ANSI_STYLE.
 *
 * This is equivalent to `ANSI_STYLE()`
 */
#define ANSI_STYLE_RESET _ANSI_SGR_PREFIX ANSI_STYLE_NONE _ANSI_SGR_SUFFIX

/**
 * @brief Applies ANSI styling but only modifies given text
 *
 * Like @ref ANSI_STYLE, but the the ANSI styling does not need to be reset after an invocation
 * of this macro.
 *
 * @experimental Experimental API, may change in the future.
 *
 * @param text Text to style
 * @param ...  Variadic style arguments to apply to @p text, e.g.,
 *            `FOREGROUND(RED)`, `BOLD`, `UNDERLINED`
 *
 * Example:
 * ```c
 * DEBUG(ANSI_STYLED("fatal error", FG(RED), BOLD) ": programmer error!!");
 * ```
 */
#define ANSI_STYLED(text, ...) \
    ANSI_STYLE(__VA_ARGS__) text ANSI_STYLE_RESET

/** @} */ /* end of section */

#ifdef __cplusplus
}
#endif

/** @} */ /* end of group */
