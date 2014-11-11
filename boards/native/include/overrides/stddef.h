#ifndef RIOT__NATIVE__STDDEF_H__
#define RIOT__NATIVE__STDDEF_H__

#ifdef __cplusplus
#   define NULL    0
#else
#   define NULL    ((void *) 0)
#   define wchar_t __WCHAR_TYPE__
#endif

typedef __PTRDIFF_TYPE__ ptrdiff_t;
typedef __SIZE_TYPE__    size_t;

#define offsetof(st, m) __builtin_offsetof(st, m)

#endif /* ifndef RIOT__NATIVE__STDDEF_H__ */
