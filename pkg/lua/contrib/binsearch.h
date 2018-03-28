/*
 * Copyright (C) 2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @internal
 * @{
 * @file
 *
 * @brief   Generic binary search for tables containing strings.
 * @author  Juan Carrano <j.carrano@fu-berlin.de>
 *
 * It is often the case that one has an array of structs, where one of the
 * members of the struct is a string pointer containing a key that must be
 * searched. If the array is sorted by this key and of known length, a binary
 * search can be performed.
 *
 * To make the code generic we must reinterpret the structure array
 * as an array of pointers to string with a stride (separation in bytes between
 * elements) and offset (position of the first element relative to the start of
 * the array) given by the struct definition.
 *
 * For example, given the following struct and array definitions and assuming
 * a 32 bit platform with strict aligment:
 *  struct s1 {
 *      int a;      // Takes up 4 bytes
 *      char *name; // Takes up 4 bytes
 *      char m;     // Takes up 1 byte
 *  };
 *  struct s1 my_table[] = {......};
 *
 * Then each element of my_table will be aligned to 12 bytes. The address of the
 * "name" field of the first elements will be 4 bytes more than the address of
 * "my_table". With this two numbers we can compute the address of the i-th
 * "name" field as:
 *      [address of my_table] + offset + i*stride
 * Where stride=12 bytes and offset = 4 bytes.
 */

#ifndef BINSEARCH_H
#define BINSEARCH_H

#include <stdint.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Produce a compiler error if x is not an lvalue.
 */
#define _ENSURE_LVALUE(x) ((void)sizeof(&(x)))

/**
 * UNSAFE MACRO: Difference in bytes between the addresses of two consecutive
 * array elements.
 */
#define _ARRAY_STRIDE(arr) ((size_t)((uint8_t *)((arr) + 1) - (uint8_t *)(arr)))

/**
 * UNSAFE MACRO: Offset in bytes from the start of the array to member "member"
 * of the first element.
 */
#define _ARRAY_MEMBER_OFFS(arr, member) \
    ((size_t)((uint8_t *)(&((arr)->member)) - (uint8_t *)(arr)))

/**
 * Find the index of the array element that contains "str" in
 *     member "member".
 *
 * A compile-time error will be raised if arr is not an lvalue. This ensures the
 * macro is safe.
 *
 * @return      Index of the array element containing the string.
 * @return      (-ENOENT) if it is not found.
 */
#define BINSEARCH_STR(arr, nmemb, member, str, n) \
    (_ENSURE_LVALUE(arr), \
     (binsearch_str((arr), _ARRAY_MEMBER_OFFS(arr, member), _ARRAY_STRIDE(arr), \
                    (nmemb), (str), (n))) \
    )

/**
 * Find a pointer of the array element that contains "str" in
 *     member "member".
 *
 * @return      Address of the element containing the string (as a void pointer).
 * @return      Null if it is not found.
 */
#define BINSEARCH_STR_P(arr, nmemb, member, str, n) \
    (_ENSURE_LVALUE(arr), \
     (binsearch_str_p((arr), _ARRAY_MEMBER_OFFS(arr, member), _ARRAY_STRIDE(arr), \
                      (nmemb), (str), (n))) \
    )

/**
 * Search for an array element containing a string.
 *
 * This does NOT check for NULL pointers, though start can be NULL of the size
 * (nmemb) is zero.
 *
 * @param   start   Pointer to start of array. The array must be ordered
 *                  according to the search string.
 * @param   offset  Offset of member containing string within structure. This
 *                  can be determined using offsetof.
 * @param   stride  Difference in bytes between the addresses of two consecutive
 *                  array elements.
 * @param   nmemb   Number of elements in the array.
 * @param   str     String that will be compared against.
 * @param   n       Compare up to n characters (see strncmp())
 *
 * @return      Index of the array element containing the string.
 * @return      (-ENOENT) if it is not found.
 */
int binsearch_str(const void *start, size_t offset, size_t stride, size_t nmemb,
                  const char *str, size_t n);

/**
 * Like binsearch_str but returns the pointer to the element.
 *
 * @return      Address of the element containing the string.
 * @return      Null if it is not found.
 */
const void *binsearch_str_p(const void *start, size_t offset, size_t stride,
                            size_t nmemb, const char *str, size_t n);


#ifdef __cplusplus
}
#endif

/** @} */

#endif /* BINSEARCH_H */
