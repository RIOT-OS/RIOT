/*
 * SPDX-FileCopyrightText: 2010 Serge A. Zaitsev
 * SPDX-License-Identifier: MIT
 */

/*
 * Adapted to RIOT by Kaspar Schleiser <kaspar@schleiser.de>
 *
 * changes: - reduced nr of expected tokens to 16, stack would overflow
 *            otherwise with default stacksize on Cortex-M
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kernel_defines.h"  /* for ARRAY_SIZE macro */
#include "jsmn.h"

/*
 * A small example of jsmn parsing when JSON structure is known and number of
 * tokens is predictable.
 */

const char *JSON_STRING =
    "{\"user\": \"johndoe\", \"admin\": false, \"uid\": 1000,\n  "
    "\"groups\": [\"users\", \"wheel\", \"audio\", \"video\"]}";

static char printbuf[32];

static int jsoneq(const char *json, jsmntok_t *tok, const char *s)
{
    if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
        strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
        return 0;
    }
    return -1;
}

static inline size_t min(size_t a, size_t b)
{
    return a <= b ? a : b;
}

static void print_string(char *prefix, const char *buf, size_t len)
{
    len = min(len, sizeof(printbuf) - 1);
    strncpy(printbuf, buf, len);
    printbuf[len] = 0;
    printf("%s%s\n", prefix, printbuf);
}

int main(void)
{
    int i;
    int r;
    jsmn_parser p;
    jsmntok_t t[16]; /* We expect no more than 16 tokens */

    jsmn_init(&p);
    r = jsmn_parse(&p, JSON_STRING, strlen(JSON_STRING), t, ARRAY_SIZE(t));
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
            print_string("- User: ", JSON_STRING + t[i + 1].start, t[i + 1].end - t[i + 1].start);
            i++;
        }
        else if (jsoneq(JSON_STRING, &t[i], "admin") == 0) {
            /* We may additionally check if the value is either "true" or
            "false" */
            print_string("- Admin: ", JSON_STRING + t[i + 1].start, t[i + 1].end - t[i + 1].start);
            i++;
        }
        else if (jsoneq(JSON_STRING, &t[i], "uid") == 0) {
            /* We may want to do strtol() here to get numeric value */
            print_string("- UID: ", JSON_STRING + t[i + 1].start, t[i + 1].end - t[i + 1].start);
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
                print_string("  * ", JSON_STRING + g->start, g->end - g->start);
            }
            i += t[i + 1].size + 1;
        }
        else {
            print_string("Unexpected key: ", JSON_STRING + t[i].start, t[i].end - t[i].start);
        }
    }
    return 0;
}
