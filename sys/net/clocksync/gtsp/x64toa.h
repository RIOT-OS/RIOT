/**
 * 64 Bit integer to string conversion functions.
 * Shamelessly plucked from
 * http://www.mikrocontroller.net/attachment/highlight/184262
 * necessary because printf in RIOT / newlib fails to print 64 Bit
 * integers correctly.
 */

#define X64LL_SIGNED 1
#define X64LL_UNSIGNED 0
#define X64LL_HEX 16<<8

char *_i64toa(long long val, char *buf, int radix);

char *_ui64toa(unsigned long long val, char *buf, int radix);

char* l2s(long long v, int sign, char *buf);
