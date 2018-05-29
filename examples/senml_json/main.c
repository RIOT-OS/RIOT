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
 * @brief       SenML example application
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
    printf("Link:\t\t%s\n", record->link ? record->link : "NULL");
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
    /* decode a SenML document in JSON format */

    char input[] = "[{\"bver\":5, \"bn\":\"urn:dev:mac:0024befffe804ff1\", \"bt\":1468341362,"
                   "\"n\":\"voltage\", \"u\":\"V\", \"v\":230.05, \"t\":1, \"ut\":1},"
                   "{\"n\":\"current\", \"u\":\"A\", \"v\":1.65, \"t\":1, \"ut\":1},"
                   "{\"n\":\"bindump\", \"l\": \"[{\\\"href\\\":\\\"humidity\\\","
                   "\\\"foo\\\":\\\"bar\\\"}]\", \"vd\":\"U2VuTUwgaXMgc28gY29vbCEK\"}]";

    senml_base_info_t base_info_dec;
    senml_record_t    records_dec[3];
    senml_pack_t      pack_dec;

    memset(&base_info_dec, 0, sizeof(senml_base_info_t));
    memset(records_dec, 0, sizeof(records_dec));

    pack_dec.base_info = &base_info_dec;
    pack_dec.records   = records_dec;
    pack_dec.num       = 3;

    int rc = senml_decode_json_s(input, &pack_dec);

    if (rc == 0) {
        senml_print_pack(&pack_dec);
    }
    else {
        printf("decoding senml document failed\n");
        return -1;
    }

    /* decode binary data from record */

    size_t len = 128;

    unsigned char data_value[len];

    rc = base64_decode((unsigned char *)records_dec[2].value.d, strlen(records_dec[2].value.d),
                       data_value, &len);

    data_value[len] = '\0';

    if (rc == 0) {
        printf("%s\n", data_value);
    }
    else {
        printf("decoding data failed\n");
        return -1;
    }

    /* encode a senml_pack_t to JSON */

    senml_base_info_t base_info_enc;
    senml_record_t    records_enc[3];
    senml_pack_t      pack_enc;

    memset(&base_info_enc, 0, sizeof(senml_base_info_t));
    memset(records_enc, 0, sizeof(records_enc));

    pack_enc.base_info = &base_info_enc;
    pack_enc.records   = records_enc;
    pack_enc.num       = 3;

    base_info_enc.version    = 5;
    base_info_enc.base_name  = "urn:dev:mac:0024befffe804ff2;";
    base_info_enc.base_time  = 1468342153.341;
    base_info_enc.base_unit  = "Cel";
    base_info_enc.base_value = 21.0;

    records_enc[0].name        = "temperature";
    records_enc[0].time        = -5;
    records_enc[0].update_time = 5;
    records_enc[0].value_type  = SENML_TYPE_FLOAT;
    records_enc[0].value.f     = 0.5;

    records_enc[1].name        = "temperature";
    records_enc[1].time        = -10;
    records_enc[1].update_time = 5;
    records_enc[1].value_type  = SENML_TYPE_FLOAT;
    records_enc[1].value.f     = 0.7;

    records_enc[2].name        = "humidity";
    records_enc[2].unit        = "%RH";
    records_enc[2].link        = "[{\\\"href\\\":\\\"humidity\\\",\\\"foo\\\":\\\"bar\\\"}]";
    records_enc[2].time        = -10;
    records_enc[2].update_time = 10;
    records_enc[2].value_type  = SENML_TYPE_FLOAT;
    records_enc[2].value.f     = 64.2;

    char output[512];

    rc = senml_encode_json_s(&pack_enc, output, 512);

    if (rc == 0) {
        printf("%s\n", output);
    }
    else {
        printf("encoding data failed\n");
        return -1;
    }

    return 0;
}
