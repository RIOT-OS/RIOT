#include <strings.h>
#include <ctype.h>

int strncasecmp(const char *s1, const char *s2, size_t n)
{
    while (n-- && tolower((unsigned char) *s1) == tolower((unsigned char) *s2)) {
        if (!n && !*s1) {
            break;
        }

        s1++;
        s2++;
    }

    return (tolower((unsigned char) *s1) - tolower((unsigned char) *s2));
}
