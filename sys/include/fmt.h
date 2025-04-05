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
 * @note The fmt functions expect their `out` parameter to hold the entire output.
 *       This *MUST* be ensured by the caller.
 *
 * @note Each fmt function will not write anything to `out` if it is `NULL`, but
 *       still return the number of characters that would have been written.
 *       This can be used to ensure the `out` buffer is large enough.
 *
 * @note The print functions in this library do not buffer any output.
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

#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef FMT_USE_MEMMOVE
#define FMT_USE_MEMMOVE (1) /**< use memmove() or internal implementation */
#endif

/**
 * @brief   Test if the given character is a numerical digit (regex `[0-9]`)
 *
 * @param[in] c     Character to test
 *
 * @return  true if @p c is a digit, false otherwise
 */
static inline int fmt_is_digit(char c)
{
    return (c >= '0' && c <= '9');
}

/**
 * @brief   Test if the given character is an uppercase letter (regex `[A-Z]`)
 *
 * @param[in] c     Character to test
 *
 * @return  true if @p c is an uppercase letter, false otherwise
 */
static inline int fmt_is_upper(char c)
{
    return (c >= 'A' && c <= 'Z');
}

/**
 * @brief   Test if the given string is a number (regex `[0-9]+`)
 *
 * @param[in] str   String to test, **must be `\0` terminated**
 *
 * @return  true if @p str solely contains decimal digits, false otherwise
 */
int fmt_is_number(const char *str);

/**
 * @brief Format a byte value as hex
 *
 * E.g., converts byte value 0 to the string 00, 255 to the string FF.
 *
 * Will write two characters to @p out.
 * If @p out is NULL, will only return the number of characters that would have
 * been written.
 *
 * @param[out] out  Pointer to output buffer, or NULL
 * @param[in]  byte Byte value to convert
 *
 * @return     2
 */
size_t fmt_byte_hex(char *out, uint8_t byte);

/**
 * @brief Formats a sequence of bytes as hex characters
 *
 * Will write 2*n characters to @p out.
 * If @p out is NULL, will only return the number of characters that would have
 * been written.
 *
 * @param[out] out  Pointer to output buffer, or NULL
 * @param[in]  ptr  Pointer to bytes to convert
 * @param[in]  n    Number of bytes to convert
 *
 * @return     2*n
 */
size_t fmt_bytes_hex(char *out, const uint8_t *ptr, size_t n);

/**
 * @brief Formats a sequence of bytes as hex characters, starting with the last byte
 *
 * Will write 2*n characters to @p out.
 * If @p out is NULL, will only return the number of characters that would have
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
 * @brief Converts a sequence of two hex characters to a byte
 *
 * The hex characters sequence must contain valid hexadecimal characters
 * otherwise the result is undefined.
 *
 * @param[in]  hex  Pointer to input buffer
 * @returns    byte based on hex string
 */
uint8_t fmt_hex_byte(const char *hex);

/**
 * @brief Converts a sequence of hex characters to an array of bytes
 *
 * The sequence of hex characters must have an even length:
 * 2 hex character => 1 byte. If the sequence of hex has an odd length, this
 * function returns 0 and does not write to @p out.
 *
 * The hex characters sequence must contain valid hexadecimal characters
 * otherwise the result in @p out is undefined.
 *
 * If @p out is NULL, will only return the number of bytes that would have
 * been written.
 *
 * @param[out] out  Pointer to converted bytes, or NULL
 * @param[in]  hex  Pointer to input buffer
 * @returns    strlen(hex) / 2 when length of @p hex was even
 * @returns    0 otherwise
 */
size_t fmt_hex_bytes(uint8_t *out, const char *hex);

/**
 * @brief   Convert a uint16 value to hex string.
 *
 * Will write 4 characters to @p out.
 * If @p out is NULL, will only return the number of characters that would have
 * been written.
 *
 * @param[out]  out  Pointer to output buffer, or NULL
 * @param[in]   val  Value to convert
 *
 * @return      4
 */
size_t fmt_u16_hex(char *out, uint16_t val);

/**
 * @brief Convert a uint32 value to hex string.
 *
 * Will write 8 characters to @p out.
 * If @p out is NULL, will only return the number of characters that would have
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
 * Will write 16 characters to @p out.
 * If @p out is NULL, will only return the number of characters that would have
 * been written.
 *
 * @param[out]  out  Pointer to output buffer, or NULL
 * @param[in]   val  Value to convert
 *
 * @return      16
 */
size_t fmt_u64_hex(char *out, uint64_t val);

/**
 * @brief Convert a uint16 value to decimal string.
 *
 * If @p out is NULL, will only return the number of characters that would have
 * been written.
 *
 * @param[out]  out  Pointer to output buffer, or NULL
 * @param[in]   val  Value to convert
 *
 * @return      nr of characters written to (or needed in) @p out
 */
size_t fmt_u16_dec(char *out, uint16_t val);

/**
 * @brief Convert a uint32 value to decimal string.
 *
 * If @p out is NULL, will only return the number of characters that would have
 * been written.
 *
 * @param[out]  out  Pointer to output buffer, or NULL
 * @param[in]   val  Value to convert
 *
 * @return      nr of characters written to (or needed in) @p out
 */
size_t fmt_u32_dec(char *out, uint32_t val);

/**
 * @brief Convert a uint64 value to decimal string.
 *
 * If @p out is NULL, will only return the number of characters that would have
 * been written.
 *
 * @note This adds ~400b of code when used.
 *
 * @param[out]  out  Pointer to output buffer, or NULL
 * @param[in]   val  Value to convert
 *
 * @return      nr of characters written to (or needed in) @p out
 */
size_t fmt_u64_dec(char *out, uint64_t val);

/**
 * @brief Convert a int64 value to decimal string.
 *
 * Will add a leading "-" if @p val is negative.
 *
 * If @p out is NULL, will only return the number of characters that would have
 * been written.
 *
 * @param[out]  out  Pointer to output buffer, or NULL
 * @param[in]   val  Value to convert
 *
 * @return      nr of characters written to (or needed in) @p out
 */
size_t fmt_s64_dec(char *out, int64_t val);

/**
 * @brief Convert a int32 value to decimal string.
 *
 * Will add a leading "-" if @p val is negative.
 *
 * If @p out is NULL, will only return the number of characters that would have
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
 * If @p out is NULL, will only return the number of characters that would have
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
 * See fmt_s32_dfp() for more details
 *
 * @param[out] out          Pointer to the output buffer, or NULL
 * @param[in]  val          Fixed point value
 * @param[in]  scale        Scale value
 *
 * @return      Length of the resulting string
 */
size_t fmt_s16_dfp(char *out, int16_t val, int scale);

/**
 * @brief Convert 32-bit fixed point number to a decimal string
 *
 * This multiplies a 32bit signed number by 10^(scale) before formatting.
 *
 * The resulting string will always be padded with zeros after the decimal point.
 *
 * For example: if @p val is -35648 and @p scale is -2, the resulting
 * string will be "-352.48"( -35648*10^-2). The same value for @p val with
 * @p scale of 2 will result in "-3524800" (-35648*10^2).
 *
 * @param[out] out          Pointer to the output buffer, or NULL
 * @param[in]  val          Fixed point value
 * @param[in]  scale        Scale value
 *
 * @return      Length of the resulting string
 */
size_t fmt_s32_dfp(char *out, int32_t val, int scale);

/**
 * @brief Format float to string
 *
 * Converts float value @p f to string
 *
 * If @p out is NULL, will only return the number of characters that would have
 * been written.
 *
 * @attention This function is using floating point math.
 *            It pulls in about 2.4k bytes of code on ARM Cortex-M platforms.
 *
 * @pre -2^32 < f < 2^32
 * @pre precision < 8 (TENMAP_SIZE)
 *
 * @param[out]  out         string to write to (or NULL)
 * @param[in]   f           float value to convert
 * @param[in]   precision   number of digits after decimal point
 *
 * @returns     nr of characters the function did or would write to out
 */
size_t fmt_float(char *out, float f, unsigned precision);

/**
 * @brief   Copy @p in char to string (without terminating '\0')
 *
 * If @p out is NULL, will only return the number of characters that would have
 * been written.
 *
 * @param[out]  out     string to write to (or NULL)
 * @param[in]   c       char value to append
 *
 * @return      nr of characters the function did or would write to out
 */
size_t fmt_char(char *out, char c);

/**
 * @brief Count characters until '\0' (exclusive) in @p str
 *
 * @param[in]   str  Pointer to string
 *
 * @return      nr of characters in string @p str points to
 */
size_t fmt_strlen(const char *str);

/**
 * @brief Count at most @p maxlen characters until '\0' (exclusive) in @p str
 *
 * @param[in]   str     Pointer to string
 * @param[in]   maxlen  Maximum number of chars to count
 *
 * @return      nr of characters in string @p str points to, or @p maxlen if no
 *              null terminator is found within @p maxlen chars
 */
size_t fmt_strnlen(const char *str, size_t maxlen);

/**
 * @brief Copy null-terminated string (excluding terminating \0)
 *
 * If @p out is NULL, will only return the number of characters that would have
 * been written.
 *
 * @param[out]  out  Pointer to output buffer, or NULL
 * @param[in]   str  Pointer to null-terminated source string
 *
 * @return      nr of characters written to (or needed in) @p out
 */
size_t fmt_str(char *out, const char *str);

/**
 * @brief   Copy null-terminated string to a lowercase string (excluding terminating \0)
 *
 * If @p out is NULL, will only return the number of characters that would have
 * been written.
 *
 * @param[out]  out     Pointer to output buffer, or NULL
 * @param[in]   str     Pointer to null-terminated source string
 *
 * @return      nr of characters written to (or needed in) @p out
 */
size_t fmt_to_lower(char *out, const char *str);

/**
 * @brief Convert string of decimal digits to uint32
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
 * @brief Convert string hexadecimal digits to uin32_t
 *
 * Will convert up to @p n digits. Stop at any non-hexadecimal or '\0' character
 *
 * @param[in]   str Pointer to string to read from
 * @param[in]   n   Maximum number of characters to consider
 *
 * @return  converted uint32_t value
 */
uint32_t scn_u32_hex(const char *str, size_t n);

/**
 * @brief   Convert a hex to binary
 *
 * @param[out]  dest        Destination buffer to write to
 * @param[in]   dest_len    Size of @p dest in bytes
 * @param[in]   hex         Hex string to convert
 * @param[in]   hex_len     Length of @p hex in bytes
 *
 * @return  Number of bytes written
 * @retval  -EINVAL     @p hex_len is odd or @p hex contains non-hex chars
 * @retval  -EOVERFLOW  Destination to small
 *
 * @pre     If @p dest_len is > 0, @p dest is not a null pointer
 * @pre     If @p hex_len is > 0, @p hex is not a null pointer
 *
 * Examples
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 * const char *hex = "deadbeef";
 * uint8_t binary[sizeof(hex) / 2];
 * ssize_t len = scn_buf_hex(binary, sizeof(binary), hex, strlen(hex));
 * if (len >= 0) {
 *     make_use_of(binary, len);
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
ssize_t scn_buf_hex(void *dest, size_t dest_len, const char *hex, size_t hex_len);

/**
 * @brief Print string to stdout
 *
 * Writes @p n characters from @p s to STDOUT_FILENO.
 *
 * @param[out]  s   Pointer to string to print
 * @param[in]   n   Number of characters to print
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
 * @brief Print bytes as hex to stdout
 *
 * @param[in]  bytes Byte array to print
 * @param[in]  n     Number of bytes to print
 */
void print_bytes_hex(const void *bytes, size_t n);

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
 * @brief Print int64 value as decimal to stdout
 *
 * @note This uses fmt_s64_dec(), which uses ~400b of code.
 *
 * @param[in]   val  Value to print
 */
void print_s64_dec(uint64_t val);

/**
 * @brief Print float value
 *
 * @note See fmt_float for code size warning!
 *
 * @pre -2^32 < f < 2^32
 * @pre precision < TENMAP_SIZE (== 8)
 *
 * @param[in]   f           float value to print
 * @param[in]   precision   number of digits after decimal point
 */
void print_float(float f, unsigned precision);

/**
 * @brief Print null-terminated string to stdout
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
 * @param[in,out]   str         string to pad (or NULL)
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
