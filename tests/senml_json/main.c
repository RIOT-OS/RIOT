/*
 * Copyright (C) 2016 Freie Universität Berlin
 * Copyright (C) 2016 Lennart Dührsen <lennart.duehrsen@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       SenML JSON test application 
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

    if (record->value_sum != 0) {
        printf("Value Sum:\t%f\n", record->value_sum);
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
    int rc;

    char output[512];

    char input_01[] = "[]";

    char input_02[] = "[{\"n\":\"John\", \"u\":\"V\", \"v\":120.398, \"t\":123456789, \"ut\":5}]";

    char input_03[] = "[{\"n\":\"Jackie\", \"vs\":\"what a nice string\", \"t\":123456790}]";

    char input_04[] = "[{\"bver\":5, \"bn\":\"riot_device\", \"bt\":123456000, \"bu\":\"Cel\", \"bv\":-273.15,"
                         "\"n\":\"Hans\", \"vs\":\"what a nice string\", \"t\":123456790}]";

    char input_05[] = "[{\"bver\":5, \"bn\":\"riot_device\", \"bt\":123456000, \"bu\":\"Cel\", \"bv\":-54.0001},"
                      "{\"n\":\"Kim\",\"t\":-10,\"ut\":12,\"vd\":\"U2VuTUwgaXMgc28gY29vbCEK\"}]";

    char input_06[] = "[{\"n\":\"Donald\", \"u\":\"W\", \"s\":1240.812, \"t\":987654321}]";

    char input_07[] = "[{\"n\":\"Daisy\",\"vb\":true}]";

    senml_base_info_t base_info;
    senml_record_t    records[2];
    senml_pack_t      pack;

    memset(&base_info, 0, sizeof(senml_base_info_t));
    memset(records, 0, sizeof(records));

    pack.base_info = &base_info;
    pack.records   = records;
    pack.num       = 2;

    printf("\nINPUT:\t%s\n\n", input_01);

    rc = senml_decode_json_s(input_01, &pack);

    if (rc == 0) {
        senml_print_pack(&pack);
    }
    else {
        printf("ERROR: decoding input_01 failed\n");
        return -1;
    }

    rc = senml_encode_json_s(&pack, output, 512);

    if (rc == 0) {
        printf("%s\n\n", output);
    }
    else {
        printf("ERROR: encoding data based on input_01 failed\n");
        return -1;
    }


    memset(&base_info, 0, sizeof(senml_base_info_t));
    memset(records, 0, sizeof(records));

    pack.base_info = &base_info;
    pack.records   = records;
    pack.num       = 2;

    printf("\nINPUT:\t%s\n\n", input_02);

    rc = senml_decode_json_s(input_02, &pack);

    if (rc == 0) {
        senml_print_pack(&pack);
    }
    else {
        printf("ERROR: decoding input_02 failed\n");
        return -1;
    }

    rc = senml_encode_json_s(&pack, output, 512);

    if (rc == 0) {
        printf("%s\n\n", output);
    }
    else {
        printf("ERROR: encoding data based on input_02 failed\n");
        return -1;
    }


    memset(&base_info, 0, sizeof(senml_base_info_t));
    memset(records, 0, sizeof(records));

    pack.base_info = &base_info;
    pack.records   = records;
    pack.num       = 2;

    printf("\nINPUT:\t%s\n\n", input_03);

    rc = senml_decode_json_s(input_03, &pack);

    if (rc == 0) {
        senml_print_pack(&pack);
    }
    else {
        printf("ERROR: decoding input_03 failed\n");
        return -1;
    }

    rc = senml_encode_json_s(&pack, output, 512);

    if (rc == 0) {
        printf("%s\n\n", output);
    }
    else {
        printf("ERROR: encoding data based on input_03 failed\n");
        return -1;
    }


    memset(&base_info, 0, sizeof(senml_base_info_t));
    memset(records, 0, sizeof(records));

    pack.base_info = &base_info;
    pack.records   = records;
    pack.num       = 2;

    printf("\nINPUT:\t%s\n\n", input_04);

    rc = senml_decode_json_s(input_04, &pack);

    if (rc == 0) {
        senml_print_pack(&pack);
    }
    else {
        printf("ERROR: decoding input_04 failed\n");
        return -1;
    }

    rc = senml_encode_json_s(&pack, output, 512);

    if (rc == 0) {
        printf("%s\n\n", output);
    }
    else {
        printf("ERROR: encoding data based on input_04 failed\n");
        return -1;
    }


    memset(&base_info, 0, sizeof(senml_base_info_t));
    memset(records, 0, sizeof(records));

    pack.base_info = &base_info;
    pack.records   = records;
    pack.num       = 2;

    printf("\nINPUT:\t%s\n\n", input_05);

    rc = senml_decode_json_s(input_05, &pack);

    if (rc == 0) {
        senml_print_pack(&pack);
    }
    else {
        printf("ERROR: decoding input_05 failed\n");
        return -1;
    }

    rc = senml_encode_json_s(&pack, output, 512);

    if (rc == 0) {
        printf("%s\n\n", output);
    }
    else {
        printf("ERROR: encoding data based on input_05 failed\n");
        return -1;
    }


    memset(&base_info, 0, sizeof(senml_base_info_t));
    memset(records, 0, sizeof(records));

    pack.base_info = &base_info;
    pack.records   = records;
    pack.num       = 2;

    printf("\nINPUT:\t%s\n\n", input_06);

    rc = senml_decode_json_s(input_06, &pack);

    if (rc == 0) {
        senml_print_pack(&pack);
    }
    else {
        printf("ERROR: decoding input_06 failed\n");
        return -1;
    }

    rc = senml_encode_json_s(&pack, output, 512);

    if (rc == 0) {
        printf("%s\n\n", output);
    }
    else {
        printf("ERROR: encoding data based on input_06 failed\n");
        return -1;
    }


    memset(&base_info, 0, sizeof(senml_base_info_t));
    memset(records, 0, sizeof(records));

    pack.base_info = &base_info;
    pack.records   = records;
    pack.num       = 2;

    printf("\nINPUT:\t%s\n\n", input_07);

    rc = senml_decode_json_s(input_07, &pack);

    if (rc == 0) {
        senml_print_pack(&pack);
    }
    else {
        printf("ERROR: decoding input_07 failed\n");
        return -1;
    }

    rc = senml_encode_json_s(&pack, output, 512);

    if (rc == 0) {
        printf("%s\n\n", output);
    }
    else {
        printf("ERROR: encoding data based on input_07 failed\n");
        return -1;
    }

    return 0;
}
