#ifndef RIOT__NATIVE__STDDEF_H__
#define RIOT__NATIVE__STDDEF_H__

#define NULL ((void *) 0)

typedef __PTRDIFF_TYPE__ ptrdiff_t;
typedef __WCHAR_TYPE__   wchar_t;
typedef __SIZE_TYPE__    size_t;

#define offsetof(st, m) __builtin_offsetof(st, m)

#endif /* ifndef RIOT__NATIVE__STDDEF_H__ */
