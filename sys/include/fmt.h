/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_fmt String formatting (fmt)
 * @ingroup     sys
 * @brief       Provides simple string formatting functions
 *
 * The goal of this API is to provide a string formatting interface which has a
 * reduced code size footprint compared to the libc provided stdio.h functionality.
 *
 * This library provides a set of formatting and printing functions for 64 bit
 * integers, even when the C library was built without support for 64 bit
 * formatting (newlib-nano).
 *
 * \note The print functions in this library do not buffer any output.
 * Mixing calls to standard @c printf from stdio.h with the @c print_xxx
 * functions in fmt, especially on the same output line, may cause garbled
 * output.
 *
 * @{
 *
 * @file
 * @brief       String formatting API
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef FMT_H
#define FMT_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef FMT_USE_MEMMOVE
#define FMT_USE_MEMMOVE (1) /**< use memmove() or internal implementation */
#endif

/**
 * @brief Format a byte value as hex
 *
 * E.g., converts byte value 0 to the string 00, 255 to the string FF.
 *
 * Will write two bytes to @p out.
 * If @p out is NULL, will only return the number of bytes that would have
 * been written.
 *
 * @param[out] out  Pointer to output buffer, or NULL
 * @param[in]  byte Byte value to convert
 *
 * @return     2
 */
size_t fmt_byte_hex(char *out, uint8_t byte);

/**
 * @brief Formats a sequence of bytes as hex bytes, starting with the last byte
 *
 * Will write 2*n bytes to @p out.
 * If @p out is NULL, will only return the number of bytes that would have
 * been written.
 *
 * @param[out] out  Pointer to output buffer, or NULL
 * @param[in]  ptr  Pointer to bytes to convert
 * @param[in]  n    Number of bytes to convert
 *
 * @return     2*n
 */
size_t fmt_bytes_hex_reverse(char *out, const uint8_t *ptr, size_t n);

/**
 * @brief Convert a uint32 value to hex string.
 *
 * Will write 8 bytes to @p out.
 * If @p out is NULL, will only return the number of bytes that would have
 * been written.
 *
 * @param[out]  out  Pointer to output buffer, or NULL
 * @param[in]   val  Value to convert
 *
 * @return      8
 */
size_t fmt_u32_hex(char *out, uint32_t val);

/**
 * @brief Convert a uint64 value to hex string.
 *
 * Will write 16 bytes to @p out.
 * If @p out is NULL, will only return the number of bytes that would have
 * been written.
 *
 * @param[out]  out  Pointer to output buffer, or NULL
 * @param[in]   val  Value to convert
 *
 * @return      16
 */
size_t fmt_u64_hex(char *out, uint64_t val);

/**
 * @brief Convert a uint32 value to decimal string.
 *
 * If @p out is NULL, will only return the number of bytes that would have
 * been written.
 *
 * @param[out]  out  Pointer to output buffer, or NULL
 * @param[in]   val  Value to convert
 *
 * @return      nr of digits written to (or needed in) @p out
 */
size_t fmt_u32_dec(char *out, uint32_t val);

/**
 * @brief Convert a uint64 value to decimal string.
 *
 * If @p out is NULL, will only return the number of bytes that would have
 * been written.
 *
 * @note This adds ~400b of code when used.
 *
 * @param[out]  out  Pointer to output buffer, or NULL
 * @param[in]   val  Value to convert
 *
 * @return      nr of digits written to (or needed in) @p out
 */
size_t fmt_u64_dec(char *out, uint64_t val);

/**
 * @brief Convert a uint16 value to decimal string.
 *
 * If @p out is NULL, will only return the number of bytes that would have
 * been written.
 *
 * @param[out]  out  Pointer to output buffer, or NULL
 * @param[in]   val  Value to convert
 *
 * @return      nr of digits written to (or needed in) @p out
 */
size_t fmt_u16_dec(char *out, uint16_t val);

/**
 * @brief Convert a int32 value to decimal string.
 *
 * Will add a leading "-" if @p val is negative.
 *
 * If @p out is NULL, will only return the number of bytes that would have
 * been written.
 *
 * @param[out]  out  Pointer to output buffer, or NULL
 * @param[in]   val  Value to convert
 *
 * @return      nr of characters written to (or needed in) @p out
 */
size_t fmt_s32_dec(char *out, int32_t val);

/**
 * @brief Convert a int16 value to decimal string.
 *
 * Will add a leading "-" if @p val is negative.
 *
 * If @p out is NULL, will only return the number of bytes that would have
 * been written.
 *
 * @param[out]  out  Pointer to output buffer, or NULL
 * @param[in]   val  Value to convert
 *
 * @return      nr of characters written to (or needed in) @p out
 */
size_t fmt_s16_dec(char *out, int16_t val);

/**
 * @brief Convert 16-bit fixed point number to a decimal string
 *
 * The input for this function is a signed 16-bit integer holding the fixed
 * point value as well as an unsigned integer defining the position of the
 * decimal point, so this value defines the number of decimal digits after the
 * decimal point.
 *
 * The resulting string will always be patted with zeros after the decimal point.
 *
 * For example: if @p val is -3548 and @p fp_digits is 2, the resulting string
 * will be "-35.48". For @p val := 12010 and @p fp_digits := 3 the result will
 * be "12.010".
 *
 * Will add a leading "-" if @p val is negative.
 *
 * If @p out is NULL, will only return the number of bytes that would have
 * been written.
 *
 * @param[out] out          Pointer to the output buffer, or NULL
 * @param[in]  val          Fixed point value, MUST be <= 4
 * @param[in]  fp_digits    Number of digits after the decimal point
 *
 * @return      Length of the resulting string
 * @return      0 if @p fp_digits is > 4
 */
size_t fmt_s16_dfp(char *out, int16_t val, unsigned fp_digits);

/**
 * @brief Format float to string
 *
 * Converts float value @p f to string
 *
 * @pre -2^32 < f < 2^32
 *
 * @note This function is using floating point math. It pulls in about 2.4k
 *       bytes of code on ARM Cortex-M platforms.
 *
 * @param[out]  out         string to write to (or NULL)
 * @param[in]   f           float value to convert
 * @param[in]   precision   number of digits after decimal point (<=7)
 *
 * @returns     nr of bytes the function did or would write to out
 */
size_t fmt_float(char *out, float f, unsigned precision);

/**
 * @brief Count characters until '\0' (exclusive) in @p str
 *
 * @param[in]   str  Pointer to string
 *
 * @return      nr of characters in string @p str points to
 */
size_t fmt_strlen(const char *str);

/**
 * @brief Copy null-terminated string (excluding terminating \0)
 *
 * If @p out is NULL, will only return the number of bytes that would have
 * been written.
 *
 * @param[out]  out  Pointer to output buffer, or NULL
 * @param[in]   str  Pointer to null-terminated source string
 *
 * @return      nr of characters written to (or needed in) @p out
 */
size_t fmt_str(char *out, const char *str);

/**
 * @brief Convert digits to uint32
 *
 * Will convert up to @p n digits. Stops at any non-digit or '\0' character.
 *
 * @param[in]   str  Pointer to string to read from
 * @param[in]   n    Maximum nr of characters to consider
 *
 * @return      converted uint32_t value
 */
uint32_t scn_u32_dec(const char *str, size_t n);

/**
 * @brief Print string to stdout
 *
 * Writes @p n bytes from @p s to STDOUT_FILENO.
 *
 * @param[out]  s   Pointer to string to print
 * @param[in]   n   Number of bytes to print
 */
void print(const char* s, size_t n);

/**
 * @brief Print uint32 value to stdout
 *
 * @param[in]   val  Value to print
 */
void print_u32_dec(uint32_t val);

/**
 * @brief Print int32 value to stdout
 *
 * @param[in]   val  Value to print
 */
void print_s32_dec(int32_t val);

/**
 * @brief Print byte value as hex to stdout
 *
 * @param[in]  byte Byte value to print
 */
void print_byte_hex(uint8_t byte);

/**
 * @brief Print uint32 value as hex to stdout
 *
 * @param[in]   val  Value to print
 */
void print_u32_hex(uint32_t val);

/**
 * @brief Print uint64 value as hex to stdout
 *
 * @param[in]   val  Value to print
 */
void print_u64_hex(uint64_t val);

/**
 * @brief Print uint64 value as decimal to stdout
 *
 * @note This uses fmt_u64_dec(), which uses ~400b of code.
 *
 * @param[in]   val  Value to print
 */
void print_u64_dec(uint64_t val);

/**
 * @brief Print float value
 *
 * @pre -2^32 < f < 2^32
 *
 * @param[in]   f           float value to print
 * @param[in]   precision   number of digits after decimal point (<=7)
 */
void print_float(float f, unsigned precision);

/**
 * @brief Print null-terminated string to stdout
 *
 * @note See fmt_float for code size warning!
 *
 * @param[in]   str  Pointer to string to print
 */
void print_str(const char* str);

/**
 * @brief Pad string to the left
 *
 * This function left-pads a given string @p str with @p pad_char.
 *
 * For example, calling
 *
 *     fmt_lpad("abcd", 4, 7, ' ');
 *
 * would result in "   abcd".
 *
 * The function only writes to @p str if str is non-NULL and @p pad_len is < @p
 * in_len.
 *
 * @note Caller must ensure @p str can take pad_len characters!
 *
 * @param[inout]    str         string to pad (or NULL)
 * @param[in]       in_len      length of str
 * @param[in]       pad_len     total length after padding
 * @param[in]       pad_char    char to use as pad char
 *
 * @returns         max(in_len, pad_len)
 */
size_t fmt_lpad(char *str, size_t in_len, size_t pad_len, char pad_char);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* FMT_H */
