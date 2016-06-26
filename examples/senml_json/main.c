/*
 * Copyright (C) 2016 Freie Universität Berlin
 * Copyright (C) 2016 Lennart Dührsen <lennart.duehrsen@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       SenML test application
 *
 * @author      Lennart Dührsen <lennart.duehrsen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>
#include <string.h>

#include "base64.h"
#include "senml.h"


static void senml_print_base_info(const senml_base_info_t *base_info)
{
    printf("Version:\t%u\n", base_info->version);
    printf("Base Name:\t%s\n", base_info->base_name ? base_info->base_name : "NULL");
    printf("Base Time:\t%f\n", base_info->base_time);
    printf("Base Unit:\t%s\n", base_info->base_unit ? base_info->base_unit : "NULL");
    printf("Base Value:\t%f\n", base_info->base_value);
    printf("\n");
}


static void senml_print_record(const senml_record_t *record)
{
    printf("Name:\t\t%s\n", record->name ? record->name : "NULL");
    printf("Unit:\t\t%s\n", record->unit ? record->unit : "NULL");
    printf("Time:\t\t%f\n", record->time);
    printf("Update Time:\t%f\n", record->update_time);

    if (record->value_type == SENML_TYPE_FLOAT) {
        printf("Float Value:\t%f\n", record->value.f);
    }
    else if (record->value_type == SENML_TYPE_STRING) {
        printf("String Value:\t%s\n", record->value.s);
    }
    else if (record->value_type == SENML_TYPE_BOOL) {
        printf("Boolean Value:\t%s\n", record->value.b ? "true" : "false");
    }
    else if (record->value_type == SENML_TYPE_BINARY) {
        printf("Data Value:\t%s\n", record->value.d);
    }

    printf("\n");
}


static void senml_print_pack(const senml_pack_t *pack)
{
    if (pack->base_info) {
        senml_print_base_info(pack->base_info);
    }

    for (size_t i = 0; i < pack->num; i++) {
        senml_print_record(&(pack->records[i]));
    }
}


int main(void)
{
    // decode a SenML document in JSON format

    senml_base_info_t base_info;
    senml_record_t    records[5];
    senml_pack_t      pack;

    memset(&base_info, 0, sizeof(senml_base_info_t));
    memset(records, 0, sizeof(records));

    pack.base_info = &base_info;
    pack.records   = records;
    pack.num       = 5;

    char input[] = "[{\"bn\":\"urn:dev:ow:10e2073a01080063\",\"bt\":1276020076.001,\"bu\":\"A\","
                   "\"bver\":5, \"bv\":999.888,"
                   "\"n\":\"voltage\",\"u\":\"V\",\"v\":120.1,\"ut\":12},"
                   "{\"ut\":8,\"n\":\"current\",\"u\":\"Cel\",\"t\":-5,\"v\":1.2},"
                   "{\"vb\":true, \"n\":\"fitzgerald\",\"ut\":24},"
                   "{\"n\":\"john\",\"s\":9217864},"
                   "{\"n\":\"name\",\"t\":-5,\"ut\":10,\"vd\":\"U2VuTUwgaXMgc28gY29vbCEK\"}]";

    int rc = senml_decode_json_s(input, &pack);

    if (rc == 0) {
        senml_print_pack(&pack);
    }
    else {
        printf("decoding senml document failed\n");
        return -1;
    }

    // decode binary data from record

    size_t len = 128;

    unsigned char data_value[len];

    rc = base64_decode((unsigned char *)records[4].value.d, strlen(records[4].value.d),
                       data_value, &len);

    data_value[len] = '\0';

    if (rc == 0) {
        printf("%s\n", data_value);
    }
    else {
        printf("decoding data failed\n");
        return -1;
    }

    // encode a senml_pack_t to JSON

    char json[512];

    rc = senml_encode_json_s(&pack, json, 512);

    if (rc == 0) {
        printf("%s\n", json);
    }
    else {
        printf("encoding data failed\n");
        return -1;
    }

    return 0;
}
