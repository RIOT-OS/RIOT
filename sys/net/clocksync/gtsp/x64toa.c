#include <string.h>
#include "x64toa.h"

static void x64toa(unsigned long long val, char *buf, unsigned radix,
        int is_neg)
{
    char *p;
    char *firstdig;
    char temp;
    unsigned digval;
    p = buf;
    *p = 0, p[1] = '\0', p;
    if (val == 0 || radix < 2 || radix > 32 || (radix & 1))
        return;
    if (is_neg)
        *p++ = '-', val = (unsigned long long) (-(long long) val);
    firstdig = p;
    if (radix-- == 10)
        do
        { // optimized for fixed division
            digval = (unsigned) (val % 10);
            val /= 10;
            *p++ = (char) (digval + '0');
        } while (val > 0);
    else
        do
        {
            temp = radix;
            digval = (unsigned) (val & radix);
            while (temp)
                val >>= 1, temp >>= 1;
            *p++ = digval > 9 ? (char) (digval + 'W') : (char) (digval + '0');
        } while (val > 0);
    *p-- = '\0';

    do
    { // reverse string
        temp = *p;
        *p = *firstdig;
        *firstdig = temp;
        --p;
        ++firstdig;
    } while (firstdig < p);
}

//----------------------------------------------------------------------------
char*
_i64toa(long long val, char *buf, int radix)
{
    x64toa((unsigned long long) val, buf, radix, (radix == 10 && val < 0));
    return buf;
}

//----------------------------------------------------------------------------
char*
_ui64toa(unsigned long long val, char *buf, int radix)
{
    x64toa(val, buf, radix, 0);
    return buf;
}

char* l2s(long long v, int sign, char *buf)
{
    char r, s;
    r = sign >> 8;
    s = sign;
    if (!r)
        r = 10;
    if (r != 10 || (s && v >= 0))
        s = 0;
    if (r < 8)
        r = 0;
    x64toa(v, buf, r, s);
    if (strlen(buf) == 0)
    {
        buf[0] = '0';
        buf[1] = '\0';
    }
    return buf;
}
