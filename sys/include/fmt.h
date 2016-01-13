/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup  sys_fmt string formatting
 * @ingroup   sys
 * @brief     Provides simple string formatting functions
 *
 * @{
 * @file
 * @brief   string formatting API
 * @author  Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef FMT_H_
#define FMT_H_

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
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
 * @param[out]  str  Pointer to string to read from
 * @param[in]   n    Maximum nr of characters to consider
 *
 * @return      nr of digits read
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
 * @brief Print null-terminated string to stdout
 *
 * @param[in]   str  Pointer to string to print
 */
void print_str(const char* str);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* FMT_H_ */
