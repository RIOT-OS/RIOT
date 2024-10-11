/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    sys_ptrtag          Helpers for pointer tagging
 * @ingroup     sys
 *
 * # Concept
 *
 * Data structures are often aligned in memory to the word size. On 32 bit
 * machines, this often results in the two least significant bits containing no
 * information (as they have to be zero, due to this alignment). In many cases
 * gaining two bits of information storage in RAM is all you need. In these
 * case, pointer tagging can come in handy.
 *
 * The tricky part is however to get this working portable on all architectures,
 * possibly even on 8 bit machines that have no alignment requirements. This
 * utility provides helpers to enforce alignment requirements for all platforms,
 * so that pointer tagging can be used everywhere.
 *
 * # Usage
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 *  // Use the PTRTAG attribute to ensure that pointers to this structure can
 *  // be tagged, even if structure would not have a suitable alignment
 *  // otherwise
 *  typedef struct PTRTAG {
 *     uint32_t bar;
 * } foo_t;
 *
 *  void isr_callback(void *data) {
 *      // extract pointer
 *      struct foo *ptr = ptrtag_ptr(data);
 *      // extract tag
 *      uint8_t tag = ptrtag_tag(data);
 *      work_on_data(ptr, tag);
 *  }
 *
 *  int main(void) {
 *      foo_t data;
 *      uint8_t tag = 3;
 *      // pack pointer and tag into tagged pointer
 *      void *ptr_and_tag = ptrtag(&data, tag);
 *      init_isr(params, isr_callback, ptr_and_tag);
 *  }
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @{
 *
 * @file
 * @brief       Pointer Tagging Helpers
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef PTRTAG_H
#define PTRTAG_H

#include <assert.h>
#include <inttypes.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Pointers to data marked with this attribute will be tag-able.
 *
 * @details This will ensure a minimum alignment of 4 bytes
 */
#define PTRTAG  __attribute__((aligned(4)))

/**
 * @brief   Create a tagged pointer
 * @param   ptr         Pointer to tag
 * @param   tag         Tag to add
 * @return              Tagged pointer encoding both @p ptr and @p tag
 *
 * @pre     @p ptr points to data marked with @ref PTRTAG
 * @pre     @p tag contains a two bit value (its numeric value 0, 1, 2, or 3)
 *
 * Expect assertions blowing up when the preconditions are not met.
 */
static inline void * ptrtag(void *ptr, uint8_t tag)
{
    uintptr_t tmp = (uintptr_t)ptr;
    /* ensure ptr is aligned to four bytes and tag fits in two bits */
    assert((tag < 4) && !(tmp & 0x3));
    return (void *)(tmp | tag);
}

/**
 * @brief   Extract the original pointer from a tagged pointer
 * @param   tagged_ptr  The tagged pointer to extract the original pointer from
 * @return  The original "un-tagged" pointer encoded in @p tagged_ptr
 */
static inline void * ptrtag_ptr(void *tagged_ptr)
{
    uintptr_t tagged = (uintptr_t)tagged_ptr;
    const uintptr_t mask = 0x3;
    return (void *)(tagged & (~mask));
}

/**
 * @brief   Extract the tag from a tagged pointer
 * @param   tagged_ptr  The tagged pointer to extract the original pointer from
 * @return  The tag encoded into @p tagged_ptr
 */
static inline uint8_t ptrtag_tag(void *tagged_ptr)
{
    uintptr_t tagged = (uintptr_t)tagged_ptr;
    return tagged & 0x3;
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* PTRTAG_H */
