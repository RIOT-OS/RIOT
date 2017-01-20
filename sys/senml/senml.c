/*
 * Copyright (C) 2016 Freie Universität Berlin
 * Copyright (C) 2016 Lennart Dührsen <lennart.duehrsen@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @author Lennart Dührsen <lennart.duehrsen@fu-berlin.de>
 */

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdio.h>

#include "senml.h"
#include "jsmn.h"
#include "cbor.h"

#define ENABLE_DEBUG  (0)
#include "debug.h"

int senml_decode_json_s(char *input, senml_pack_t *pack)
{
    jsmn_parser parser;

    jsmn_init(&parser);

    int num_toks = jsmn_parse(&parser, input, strlen(input), NULL, 65535);

    if (num_toks == JSMN_ERROR_INVAL || num_toks == JSMN_ERROR_PART) {
        DEBUG("ERROR: unable to parse JSON string #1\n");
        return -1;
    }

    jsmntok_t tokens[num_toks];

    jsmn_init(&parser);

    num_toks = jsmn_parse(&parser, input, strlen(input), tokens, num_toks);

    if (num_toks == JSMN_ERROR_INVAL || num_toks == JSMN_ERROR_PART) {
        DEBUG("ERROR: unable to parse JSON string #2\n");
        return -1;
    }

    if (tokens[0].type != JSMN_ARRAY) {
        DEBUG("ERROR: not a valid SenML document\n");
        DEBUG("type is: %d\n", (int)tokens[0].type);
        return -1;
    }

    int array_num = -1;

    jsmntype_t last_type = JSMN_UNDEFINED;

    char *tmp_key, *tmp_val;

    bool found_base_info = false;

    for (size_t i = 0, j = num_toks; j > 0; i++, j--) {
        jsmntok_t *t = &tokens[i];

        if (t->type == JSMN_ARRAY) {
            last_type = JSMN_ARRAY;
            continue;
        }

        if (t->type == JSMN_OBJECT) {
            last_type = JSMN_OBJECT;
            array_num++;

            /* the cast to unsigned int is ok because array_num will be >= 0 at this point */
            if ((unsigned int)array_num >= pack->num) {
                DEBUG("ERROR: number of records exceeds pack->num\n");
                return -1;
            }

            continue;
        }

        if (last_type == JSMN_OBJECT && t->type == JSMN_STRING) {
            tmp_key = &input[t->start];
            input[t->end] = '\0';
            i++;
            j--;
            t = &tokens[i];
            tmp_val = &input[t->start];
            input[t->end] = '\0';

            if (array_num == 0) {
                if (!strcmp(tmp_key, SJ_VERSION)) {
                    pack->base_info->version = (uint8_t)atoi(tmp_val);
                    found_base_info = true;
                }
                else if (!strcmp(tmp_key, SJ_BASE_NAME)) {
                    pack->base_info->base_name = tmp_val;
                    found_base_info = true;
                }
                else if (!strcmp(tmp_key, SJ_BASE_TIME)) {
                    pack->base_info->base_time = atof(tmp_val);
                    found_base_info = true;
                }
                else if (!strcmp(tmp_key, SJ_BASE_UNIT)) {
                    pack->base_info->base_unit = tmp_val;
                    found_base_info = true;
                }
                else if (!strcmp(tmp_key, SJ_BASE_VALUE)) {
                    pack->base_info->base_value = atof(tmp_val);
                    found_base_info = true;
                }
                else if (!strcmp(tmp_key, SJ_BASE_SUM)) {
                    pack->base_info->base_sum = atof(tmp_val);
                    found_base_info = true;
                }
            }

            if (!strcmp(tmp_key, SJ_NAME)) {
                (pack->records)[array_num].name = tmp_val;
            }
            else if (!strcmp(tmp_key, SJ_UNIT)) {
                (pack->records)[array_num].unit = tmp_val;
            }
            else if (!strcmp(tmp_key, SJ_LINK)) {
                (pack->records)[array_num].link = tmp_val;
            }
            else if (!strcmp(tmp_key, SJ_VALUE)) {
                (pack->records)[array_num].value_type = SENML_TYPE_FLOAT;
                (pack->records)[array_num].value.f = atof(tmp_val);
            }
            else if (!strcmp(tmp_key, SJ_STRING_VALUE)) {
                (pack->records)[array_num].value_type = SENML_TYPE_STRING;
                (pack->records)[array_num].value.s = tmp_val;
            }
            else if (!strcmp(tmp_key, SJ_BOOL_VALUE)) {
                (pack->records)[array_num].value_type = SENML_TYPE_BOOL;
                if (!strcmp(tmp_val, "true")) {
                    (pack->records)[array_num].value.b = true;
                }
                else if (!strcmp(tmp_val, "false")) {
                    (pack->records)[array_num].value.b = false;
                }
            }
            else if (!strcmp(tmp_key, SJ_DATA_VALUE)) {
                (pack->records)[array_num].value_type = SENML_TYPE_BINARY;
                (pack->records)[array_num].value.d = tmp_val;
            }
            else if (!strcmp(tmp_key, SJ_VALUE_SUM)) {
                (pack->records)[array_num].value_sum = atof(tmp_val);
            }
            else if (!strcmp(tmp_key, SJ_TIME)) {
                (pack->records)[array_num].time = atof(tmp_val);
            }
            else if (!strcmp(tmp_key, SJ_UPDATE_TIME)) {
                (pack->records)[array_num].update_time = atof(tmp_val);
            }
        }
    }

    if (!found_base_info) {
        pack->base_info = NULL;
    }

    pack->num = (array_num + 1);

    return 0;
}

int senml_encode_json_s(const senml_pack_t *pack, char *output, size_t len)
{
    if (len < 3) {           /* we need space for at least '[', ']', and '\0' */
        return -1;
    }

    size_t ins_pos = 0;      /* position in output where next character will be inserted */
    size_t buf_len = len;    /* length of space left in output */

    size_t ins_pos_after_bi; /* stores value of ins_pos after inserting base info to check
                                whether all records provided were empty */

    int chars_written = 0;   /* stores return values of snprintf calls: negative value indicates
                                error, value >= buf_len indicates insufficient space in output */

    bool insert_opening_brace = true;  /* is used to check whether '{' has to be inserted after
                                          we the first record is empty but base info is provided */

    output[ins_pos] = '[';
    ins_pos++;
    buf_len--;

    if (pack->base_info) {
        output[ins_pos] = '{';
        ins_pos++;
        buf_len--;

        insert_opening_brace = false;

        if (pack->base_info->version) {
            chars_written = snprintf(&output[ins_pos], buf_len, "\"bver\":%u,",
                                     pack->base_info->version);

            if (chars_written >= (ssize_t)buf_len || chars_written < 0) {
                return -1;
            }

            ins_pos += chars_written;
            buf_len -= chars_written;
        }

        if (pack->base_info->base_name) {
            chars_written = snprintf(&output[ins_pos], buf_len, "\"bn\":\"%s\",",
                                     pack->base_info->base_name);

            if (chars_written >= (ssize_t)buf_len || chars_written < 0) {
                return -1;
            }

            ins_pos += chars_written;
            buf_len -= chars_written;
        }

        if (pack->base_info->base_time != 0) {
            chars_written = snprintf(&output[ins_pos], buf_len, "\"bt\":%f,",
                                     pack->base_info->base_time);

            if (chars_written >= (ssize_t)buf_len || chars_written < 0) {
                return -1;
            }

            ins_pos += chars_written;
            buf_len -= chars_written;
        }

        if (pack->base_info->base_unit) {
            chars_written = snprintf(&output[ins_pos], buf_len, "\"bu\":\"%s\",",
                                     pack->base_info->base_unit);

            if (chars_written >= (ssize_t)buf_len || chars_written < 0) {
                return -1;
            }

            ins_pos += chars_written;
            buf_len -= chars_written;
        }

        if (pack->base_info->base_value != 0) {
            chars_written = snprintf(&output[ins_pos], buf_len, "\"bv\":%f,",
                                     pack->base_info->base_value);

            if (chars_written >= (ssize_t)buf_len || chars_written < 0) {
                return -1;
            }

            ins_pos += chars_written;
            buf_len -= chars_written;
        }

        if (pack->base_info->base_sum != 0) {
            chars_written = snprintf(&output[ins_pos], buf_len, "\"bs\":%f,",
                                     pack->base_info->base_sum);

            if (chars_written >= (ssize_t)buf_len || chars_written < 0) {
                return -1;
            }

            ins_pos += chars_written;
            buf_len -= chars_written;
        }
    }

    ins_pos_after_bi = ins_pos;

    if (pack->num == 0) {
        /* this means no base info and no records */
        if (ins_pos == 1) {
            output[ins_pos] = ']';
            ins_pos++;
            buf_len--;
            output[ins_pos] = '\0';
            ins_pos++;
            buf_len--;
            return 0;
        }
        /* this means there is base info but records, which is illegal */
        else {
            return -1;
        }
    }

    if (buf_len < 4) {   /* we now need space for at least '}', ']', '\0' and perhaps '{' before */
        return -1;
    }

    buf_len -= 1;   /* reserve space for '\0' (trailing comma will be overwritten with ']') */

    for (size_t i = 0; i < pack->num; i++) {
        senml_record_t *curr_record = &(pack->records[i]);

        size_t buf_len_old;

        if (insert_opening_brace) {
            output[ins_pos] = '{';
            ins_pos++;
            buf_len--;
        }

        buf_len_old = buf_len;

        if (curr_record->name) {
            chars_written = snprintf(&output[ins_pos], buf_len, "\"n\":\"%s\",",
                                     curr_record->name);

            if (chars_written >= (ssize_t)buf_len || chars_written < 0) {
                return -1;
            }

            ins_pos += chars_written;
            buf_len -= chars_written;
        }

        if (curr_record->unit) {
            chars_written = snprintf(&output[ins_pos], buf_len, "\"u\":\"%s\",",
                                     curr_record->unit);

            if (chars_written >= (ssize_t)buf_len || chars_written < 0) {
                return -1;
            }

            ins_pos += chars_written;
            buf_len -= chars_written;
        }

        if (curr_record->link) {
            chars_written = snprintf(&output[ins_pos], buf_len, "\"l\":\"%s\",",
                                     curr_record->link);

            if (chars_written >= (ssize_t)buf_len || chars_written < 0) {
                return -1;
            }

            ins_pos += chars_written;
            buf_len -= chars_written;
        }

        if (curr_record->time != 0) {
            chars_written = snprintf(&output[ins_pos], buf_len, "\"t\":%f,",
                                     curr_record->time);

            if (chars_written >= (ssize_t)buf_len || chars_written < 0) {
                return -1;
            }

            ins_pos += chars_written;
            buf_len -= chars_written;
        }

        if (curr_record->update_time != 0) {
            chars_written = snprintf(&output[ins_pos], buf_len, "\"ut\":%f,",
                                     curr_record->update_time);

            if (chars_written >= (ssize_t)buf_len || chars_written < 0) {
                return -1;
            }

            ins_pos += chars_written;
            buf_len -= chars_written;
        }

        if (curr_record->value_sum != 0) {
            chars_written = snprintf(&output[ins_pos], buf_len, "\"s\":%f,",
                                     curr_record->value_sum);

            if (chars_written >= (ssize_t)buf_len || chars_written < 0) {
                return -1;
            }

            ins_pos += chars_written;
            buf_len -= chars_written;
        }

        if (curr_record->value_type != SENML_TYPE_UNDEF) {
            if (curr_record->value_type == SENML_TYPE_FLOAT) {
                chars_written = snprintf(&output[ins_pos], buf_len, "\"v\":%f,",
                                         curr_record->value.f);
            }
            else if (curr_record->value_type == SENML_TYPE_STRING) {
                chars_written = snprintf(&output[ins_pos], buf_len, "\"vs\":\"%s\",",
                                         curr_record->value.s);
            }
            else if (curr_record->value_type == SENML_TYPE_BOOL) {
                chars_written = snprintf(&output[ins_pos], buf_len, "\"vb\":%s,",
                                         curr_record->value.b ? "true" : "false");
            }
            else if (curr_record->value_type == SENML_TYPE_BINARY) {
                chars_written = snprintf(&output[ins_pos], buf_len, "\"vd\":\"%s\",",
                                         curr_record->value.d);
            }

            if (chars_written >= (ssize_t)buf_len || chars_written < 0) {
                return -1;
            }

            ins_pos += chars_written;
            buf_len -= chars_written;
        }

        if (buf_len == buf_len_old) {         /* record was empty, no values inserted: remove '{' */
            if (output[ins_pos - 1] == '{') {
                ins_pos--;
                buf_len++;
            }
            else {
                insert_opening_brace = false;
            }

            continue;
        }

        /* otherwise remove trailing ',', then insert '}' and ',' */

        ins_pos--;
        buf_len++;

        output[ins_pos] = '}';

        ins_pos++;
        buf_len--;

        output[ins_pos] = ',';

        ins_pos++;
        buf_len--;

        insert_opening_brace = true;

        if ((i + 1 < pack->num) && buf_len == 0) {   /* no space left for next record */
            return -1;
        }
    }

    if (ins_pos + 1 == ins_pos_after_bi) {
        DEBUG("base info was provided and number of records is > 0, but all records were empty\n");
        return -1;
    }

    if (ins_pos > 1) {   /* if records provided were all empty, don't remove last character */
        ins_pos--;
    }

    buf_len += 2;

    output[ins_pos] = ']';

    ins_pos++;
    buf_len--;

    output[ins_pos] = '\0';

    ins_pos++;
    buf_len--;

    return 0;
}
