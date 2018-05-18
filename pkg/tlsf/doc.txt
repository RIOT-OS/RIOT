/**
 * @defgroup pkg_tlsf	Two-Level Segregated Fit memory allocator
 * @ingroup  pkg
 * @brief    TLSF is a general purpose dynamic memory allocator specifically
 *            designed to meet real-time requirements:
 * @see      http://www.gii.upv.es/tlsf/
 * @see      https://github.com/mattconte/tlsf
 *
 * TLSF provides an implementation of malloc/realloc/free/etc with the following
 * characteristics:
 *
 *  - O(1) Performance
 *  - Works on a user supplied block of memory instead of a global heap.
 *  - Efficient both in terms of memory overhead and processor time.
 *  - Low fragmentation.
 *
 * Additionally, a contrib package @see pkg_tlsf_malloc implements a global heap
 * allocator with the standard malloc/free functions.
 *
 *  @todo    The tlsf code uses printf to report errors. This is not OK.
 */
