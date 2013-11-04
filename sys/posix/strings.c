#include <strings.h>
#include <ctype.h>

int strncasecmp(const char *s1, const char *s2, size_t n)
{
    while (n-- && tolower(*s1) == tolower(*s2)) {
        if (!n && !*s1) {
            break;
        }

        s1++;
        s2++;
    }

    return (tolower(*s1) - tolower(*s2));
}
