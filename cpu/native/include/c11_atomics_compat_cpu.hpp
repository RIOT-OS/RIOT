/**
 * This file was generated using
 * ./dist/tools/generate_c11_atomics_cpp_compat_header/generate_c11_atomics_cpp_compat_header.sh
 * for 32 and 64 bit and merged manually.
 */
#pragma once

#define ATOMIC_BOOL_SIZE                                (1U)
#define ATOMIC_BOOL_SAME_SIZED_TYPE                     uint8_t
#define ATOMIC_CHAR_SIZE                                (1U)
#define ATOMIC_CHAR_SAME_SIZED_TYPE                     uint8_t
#define ATOMIC_SCHAR_SIZE                               (1U)
#define ATOMIC_SCHAR_SAME_SIZED_TYPE                    uint8_t
#define ATOMIC_UCHAR_SIZE                               (1U)
#define ATOMIC_UCHAR_SAME_SIZED_TYPE                    uint8_t
#define ATOMIC_SHORT_SIZE                               (2U)
#define ATOMIC_SHORT_SAME_SIZED_TYPE                    uint16_t
#define ATOMIC_USHORT_SIZE                              (2U)
#define ATOMIC_USHORT_SAME_SIZED_TYPE                   uint16_t
#define ATOMIC_INT_SIZE                                 (4U)
#define ATOMIC_INT_SAME_SIZED_TYPE                      uint32_t
#define ATOMIC_UINT_SIZE                                (4U)
#define ATOMIC_UINT_SAME_SIZED_TYPE                     uint32_t
#ifdef __x86_64__
#define ATOMIC_LONG_SIZE                                (8U)
#define ATOMIC_LONG_SAME_SIZED_TYPE                     uint64_t
#define ATOMIC_ULONG_SIZE                               (8U)
#define ATOMIC_ULONG_SAME_SIZED_TYPE                    uint64_t
#else
#define ATOMIC_LONG_SIZE                                (4U)
#define ATOMIC_LONG_SAME_SIZED_TYPE                     uint32_t
#define ATOMIC_ULONG_SIZE                               (4U)
#define ATOMIC_ULONG_SAME_SIZED_TYPE                    uint32_t
#endif
#define ATOMIC_LLONG_SIZE                               (8U)
#define ATOMIC_LLONG_SAME_SIZED_TYPE                    uint64_t
#define ATOMIC_ULLONG_SIZE                              (8U)
#define ATOMIC_ULLONG_SAME_SIZED_TYPE                   uint64_t
#define ATOMIC_INT_LEAST8_T_SIZE                        (1U)
#define ATOMIC_INT_LEAST8_T_SAME_SIZED_TYPE             uint8_t
#define ATOMIC_UINT_LEAST8_T_SIZE                       (1U)
#define ATOMIC_UINT_LEAST8_T_SAME_SIZED_TYPE            uint8_t
#define ATOMIC_INT_LEAST16_T_SIZE                       (2U)
#define ATOMIC_INT_LEAST16_T_SAME_SIZED_TYPE            uint16_t
#define ATOMIC_UINT_LEAST16_T_SIZE                      (2U)
#define ATOMIC_UINT_LEAST16_T_SAME_SIZED_TYPE           uint16_t
#define ATOMIC_INT_LEAST32_T_SIZE                       (4U)
#define ATOMIC_INT_LEAST32_T_SAME_SIZED_TYPE            uint32_t
#define ATOMIC_UINT_LEAST32_T_SIZE                      (4U)
#define ATOMIC_UINT_LEAST32_T_SAME_SIZED_TYPE           uint32_t
#define ATOMIC_INT_LEAST64_T_SIZE                       (8U)
#define ATOMIC_INT_LEAST64_T_SAME_SIZED_TYPE            uint64_t
#define ATOMIC_UINT_LEAST64_T_SIZE                      (8U)
#define ATOMIC_UINT_LEAST64_T_SAME_SIZED_TYPE           uint64_t
#ifdef __FreeBSD__
#define ATOMIC_INT_FAST8_T_SIZE                         (4U)
#define ATOMIC_INT_FAST8_T_SAME_SIZED_TYPE              uint32_t
#define ATOMIC_UINT_FAST8_T_SIZE                        (4U)
#define ATOMIC_UINT_FAST8_T_SAME_SIZED_TYPE             uint32_t
#else
#define ATOMIC_INT_FAST8_T_SIZE                         (1U)
#define ATOMIC_INT_FAST8_T_SAME_SIZED_TYPE              uint8_t
#define ATOMIC_UINT_FAST8_T_SIZE                        (1U)
#define ATOMIC_UINT_FAST8_T_SAME_SIZED_TYPE             uint8_t
#endif
#if defined(__x86_64__) && defined(__GLIBC__)
#  define ATOMIC_INT_FAST16_T_SIZE                     (8U)
#  define ATOMIC_INT_FAST16_T_SAME_SIZED_TYPE          uint64_t
#  define ATOMIC_UINT_FAST16_T_SIZE                    (8U)
#  define ATOMIC_UINT_FAST16_T_SAME_SIZED_TYPE         uint64_t
#  define ATOMIC_INT_FAST32_T_SIZE                     (8U)
#  define ATOMIC_INT_FAST32_T_SAME_SIZED_TYPE          uint64_t
#  define ATOMIC_UINT_FAST32_T_SIZE                    (8U)
#  define ATOMIC_UINT_FAST32_T_SAME_SIZED_TYPE         uint64_t
#else
#  define ATOMIC_INT_FAST16_T_SIZE                     (4U)
#  define ATOMIC_INT_FAST16_T_SAME_SIZED_TYPE          uint32_t
#  define ATOMIC_UINT_FAST16_T_SIZE                    (4U)
#  define ATOMIC_UINT_FAST16_T_SAME_SIZED_TYPE         uint32_t
#  define ATOMIC_INT_FAST32_T_SIZE                     (4U)
#  define ATOMIC_INT_FAST32_T_SAME_SIZED_TYPE          uint32_t
#  define ATOMIC_UINT_FAST32_T_SIZE                    (4U)
#  define ATOMIC_UINT_FAST32_T_SAME_SIZED_TYPE         uint32_t
#endif
#define ATOMIC_INT_FAST64_T_SIZE                        (8U)
#define ATOMIC_INT_FAST64_T_SAME_SIZED_TYPE             uint64_t
#define ATOMIC_UINT_FAST64_T_SIZE                       (8U)
#define ATOMIC_UINT_FAST64_T_SAME_SIZED_TYPE            uint64_t
#ifdef __x86_64__
#define ATOMIC_INTPTR_T_SIZE                            (8U)
#define ATOMIC_INTPTR_T_SAME_SIZED_TYPE                 uint64_t
#define ATOMIC_UINTPTR_T_SIZE                           (8U)
#define ATOMIC_UINTPTR_T_SAME_SIZED_TYPE                uint64_t
#define ATOMIC_SIZE_T_SIZE                              (8U)
#define ATOMIC_SIZE_T_SAME_SIZED_TYPE                   uint64_t
#else
#define ATOMIC_INTPTR_T_SIZE                            (4U)
#define ATOMIC_INTPTR_T_SAME_SIZED_TYPE                 uint32_t
#define ATOMIC_UINTPTR_T_SIZE                           (4U)
#define ATOMIC_UINTPTR_T_SAME_SIZED_TYPE                uint32_t
#define ATOMIC_SIZE_T_SIZE                              (4U)
#define ATOMIC_SIZE_T_SAME_SIZED_TYPE                   uint32_t
#endif
#define ATOMIC_PTRDIFF_T_SIZE                           (8U)
#define ATOMIC_PTRDIFF_T_SAME_SIZED_TYPE                uint64_t
#define ATOMIC_INTMAX_T_SIZE                            (8U)
#define ATOMIC_INTMAX_T_SAME_SIZED_TYPE                 uint64_t
#define ATOMIC_UINTMAX_T_SIZE                           (8U)
#define ATOMIC_UINTMAX_T_SAME_SIZED_TYPE                uint64_t
