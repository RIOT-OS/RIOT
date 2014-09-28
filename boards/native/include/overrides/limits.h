#ifndef RIOT__NATIVE__LIMITS_H__
#define RIOT__NATIVE__LIMITS_H__

#define _POSIX_SSIZE_MAX UINT_MAX
#define CHAR_BIT         8
#define CHAR_MAX         SCHAR_MAX
#define CHAR_MIN         SCHAR_MIN
#define INT_MAX          0x7ffffff
#define LONG_BIT         WORD_BIT
#define LONG_MAX         0x7ffffffl
#define MB_LEN_MAX       1
#define SCHAR_MAX        0x7f
#define SHRT_MAX         0x7fff
#define SSIZE_MAX        _POSIX_SSIZE_MAX
#define UCHAR_MAX        0xff
#define UINT_MAX         0xffffffffu
#define ULONG_MAX        0xfffffffful
#define USHRT_MAX        0xffffu
#define WORD_BIT         (CHAR_BIT * 4)
#define INT_MIN          (-0x80000000)
#define LONG_MIN         (-0x80000000l)
#define SCHAR_MIN        (-0x80)
#define SHRT_MIN         (-0x8000)
#define LLONG_MIN        (-0x8000000000000000ll)
#define LLONG_MAX        (-0x7fffffffffffffffll)
#define ULLONG_MAX       (-0xffffffffffffffffull)

#endif /* ifndef RIOT__NATIVE__LIMITS_H__ */
