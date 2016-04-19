/*
 * Copyright (c) 2010 Serge A. Zaitsev
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * adapted to RIOT by Kaspar Schleiser <kaspar@schleiser.de>
 *
 * changes: - reduced nr of expected tokens to 16, stack would overflow
 *            otherwise with default stacksize on Cortex-M
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jsmn.h"

/*
 * A small example of jsmn parsing when JSON structure is known and number of
 * tokens is predictable.
 */

const char *JSON_STRING =
    "{\"user\": \"johndoe\", \"admin\": false, \"uid\": 1000,\n  "
    "\"groups\": [\"users\", \"wheel\", \"audio\", \"video\"]}";

static int jsoneq(const char *json, jsmntok_t *tok, const char *s)
{
    if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
        strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
        return 0;
    }
    return -1;
}

int main(void)
{
    int i;
    int r;
    jsmn_parser p;
    jsmntok_t t[16]; /* We expect no more than 16 tokens */

    jsmn_init(&p);
    r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING), t, sizeof(t) / sizeof(t[0]));
    if (r < 0) {
        printf("Failed to parse JSON: %d\n", r);
        return 1;
    }

    /* Assume the top-level element is an object */
    if (r < 1 || t[0].type != JSMN_OBJECT) {
        printf("Object expected\n");
        return 1;
    }

    /* Loop over all keys of the root object */
    for (i = 1; i < r; i++) {
        if (jsoneq(JSON_STRING, &t[i], "user") == 0) {
            /* We may use strndup() to fetch string value */
            printf("- User: %.*s\n", t[i + 1].end - t[i + 1].start,
                   JSON_STRING + t[i + 1].start);
            i++;
        }
        else if (jsoneq(JSON_STRING, &t[i], "admin") == 0) {
            /* We may additionally check if the value is either "true" or "false" */
            printf("- Admin: %.*s\n", t[i + 1].end - t[i + 1].start,
                   JSON_STRING + t[i + 1].start);
            i++;
        }
        else if (jsoneq(JSON_STRING, &t[i], "uid") == 0) {
            /* We may want to do strtol() here to get numeric value */
            printf("- UID: %.*s\n", t[i + 1].end - t[i + 1].start,
                   JSON_STRING + t[i + 1].start);
            i++;
        }
        else if (jsoneq(JSON_STRING, &t[i], "groups") == 0) {
            int j;
            printf("- Groups:\n");
            if (t[i + 1].type != JSMN_ARRAY) {
                continue; /* We expect groups to be an array of strings */
            }
            for (j = 0; j < t[i + 1].size; j++) {
                jsmntok_t *g = &t[i + j + 2];
                printf("  * %.*s\n", g->end - g->start, JSON_STRING + g->start);
            }
            i += t[i + 1].size + 1;
        }
        else {
            printf("Unexpected key: %.*s\n", t[i].end - t[i].start,
                   JSON_STRING + t[i].start);
        }
    }
    return 0;
}
