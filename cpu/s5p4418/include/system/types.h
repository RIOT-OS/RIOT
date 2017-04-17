#ifndef __ARM32_TYPES_H__
#define __ARM32_TYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef signed char             s8_t;
typedef unsigned char           u8_t;

typedef signed short            s16_t;
typedef unsigned short          u16_t;

typedef signed int              s32_t;
typedef unsigned int            u32_t;

typedef signed long long        s64_t;
typedef unsigned long long      u64_t;

typedef signed long long        intmax_t;
typedef unsigned long long      uintmax_t;

typedef signed int              ptrdiff_t;
typedef signed int              intptr_t;
typedef unsigned int            uintptr_t;

typedef unsigned int            size_t;
typedef signed int              ssize_t;

//typedef signed int            off_t;
//typedef signed long long      loff_t;

//typedef   unsigned int        clock_t;
//typedef   signed int          time_t;

typedef signed int              bool_t;

typedef signed int              register_t;
typedef unsigned int            irq_flags_t;

typedef unsigned int            virtual_addr_t;
typedef unsigned int            virtual_size_t;
typedef unsigned int            physical_addr_t;
typedef unsigned int            physical_size_t;

typedef struct {
    volatile long counter;
} atomic_t;

typedef struct {
    volatile long lock;
} spinlock_t;


#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif


#define ARRAY_SIZE(array)   ( sizeof(array) / sizeof((array)[0]) )

/*
 * __ffs - find first bit in word.
 * Undefined if no bit exists, so code should check against 0 first.
 */
static inline __attribute__((always_inline)) unsigned long __ffs(unsigned long word)
{
    return __builtin_ctzl(word);
}

#ifdef __cplusplus
}
#endif

#endif /* __ARM32_TYPES_H__ */
