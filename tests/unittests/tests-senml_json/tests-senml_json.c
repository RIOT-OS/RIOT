/*
 * Copyright (C) 2016 Lennart Dührsen <lennart.duehrsen@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 */

#include "embUnit/embUnit.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <float.h>
#include <math.h>

#include "senml.h"

#include "tests-senml_json.h"

static char senml_json_empty_doc[]   = "[]";

static char senml_json_nobaseinfo[]  = "[{\"n\":\"urn:dev:ow:10e2073a01080063\",\"u\":\"Cel\","
                                "\"t\":1.276020076e+09,\"v\":23.5},"
                                "{\"n\":\"urn:dev:ow:10e2073a01080063\",\"u\":\"Cel\","
                                "\"t\":1.276020091e+09,\"v\":23.6}]";

static char senml_json_empty_elem[]  = "[{},{\"n\":\"urn:dev:ow:10e2073a01080063\",\"u\":\"Cel\","
                                "\"t\":1.276020076e+09,\"v\":23.5},"
                                "{\"n\":\"urn:dev:ow:10e2073a01080063\",\"u\":\"Cel\","
                                "\"t\":1.276020091e+09,\"v\":23.6}]";

static char senml_json_baseinfo[]    = "[{\"bn\":\"urn:dev:ow:10e2073a01080063\",\"u\":\"Cel\","
                                "\"t\":1.276020076e+09,\"v\":23.5},{\"u\":\"Cel\","
                                "\"t\":1.276020091e+09,\"v\":23.6}]";

static char senml_json_updatetime[]  = "[{\"n\":\"John\",\"u\":\"V\",\"v\":120.398,\"t\":123456789,"
                                "\"ut\":5}]";

static char senml_json_stringvalue[] = "[{\"n\":\"Jackie\",\"vs\":\"what a nice string\","
                                "\"t\":123456790}]";

static char senml_json_basevalues[]  = "[{\"bver\":5,\"bn\":\"riot_device\",\"bt\":123456000,"
                                "\"bu\":\"Cel\",\"bv\":-273.15,\"bs\":89.12,\"n\":\"Hans\","
                                "\"vs\":\"what a nice string\",\"t\":123456790}]";

static char senml_json_datavalue[]   = "[{\"bver\":5,\"bn\":\"riot_device\",\"bt\":123456000,"
                                "\"bu\":\"Cel\",\"bv\":-54.0001,\"n\":\"Kim\",\"t\":-10,"
                                "\"ut\":12,\"vd\":\"U2VuTUwgaXMgc28gY29vbCEK\"}]";

static char senml_json_valuesum[]    = "[{\"n\":\"Donald\",\"u\":\"W\",\"s\":1240.812,\"t\":987654321}]";

static char senml_json_boolvalue[]   = "[{\"n\":\"Daisy\",\"vb\":true}]";

static char senml_json_link[]        = "[{\"n\":\"Link\",\"l\":\"[{\\\"href\\\":\\\"derp\\\","
                                "\\\"foo\\\":\\\"bar\\\"}]\",\"vb\":true}]";

static char expected_with_baseinfo[] = "[{\"bver\":5,\"bn\":\"urn:dev:mac:0024befffe804ff2;\","
                                "\"bt\":1468342153.341000,\"bu\":\"Cel\",\"bv\":21.000000,"
                                "\"bs\":8237823.412000,"
                                "\"n\":\"temperature\",\"t\":-5.000000,\"ut\":5.000000,"
                                "\"v\":0.500000},{\"n\":\"temperature\",\"t\":-10.000000,"
                                "\"ut\":5.000000,\"v\":0.700000}]";

static char expected_no_baseinfo[]   = "[{\"n\":\"float_measure\",\"u\":\"m\",\"t\":1480778196.431000,"
                                "\"ut\":10.000000,\"v\":42.195000},{\"n\":\"with_sum\","
                                "\"u\":\"m\",\"t\":1480778906.541000,\"ut\":10.000000,"
                                "\"s\":1423576.331000,\"v\":39.190000},{\"n\":\"string_measure\","
                                "\"t\":1480778388.934000,\"ut\":30.000000,\"vs\":\"hello i am a "
                                "string value\"},{\"n\":\"bool_measure\",\"t\":1480778500.871000,"
                                "\"ut\":1.000000,\"vb\":false},{\"n\":\"data_measure\","
                                "\"l\":\"[{\\\"href\\\":\\\"derp\\\",\\\"foo\\\":"
                                "\\\"bar\\\"}]\",\"t\":1480778629.230000,\"ut\":120.000000,"
                                "\"vd\":\"VGhpcyBpcyBiaW5hcnkgdmFsdWUK\"}]";


static bool double_almost_equal(float a, float b)
{
    float diff = fabs(a - b);

    a = fabs(a);
    b = fabs(b);

    float largest = (b > a) ? b : a;

    if (diff <= largest * FLT_EPSILON) {
        return true;
    }

    return false;
}

static void test_senml_json_decode_empty(void)
{
    senml_base_info_t base_info;
    senml_record_t    records[1];
    senml_pack_t      pack;

    memset(&base_info, 0, sizeof(senml_base_info_t));
    memset(records, 0, sizeof(records));

    pack.base_info = &base_info;
    pack.records   = records;
    pack.num       = 1;

    int rc = senml_decode_json_s(senml_json_empty_doc, &pack);

    TEST_ASSERT(rc == 0);

    TEST_ASSERT_EQUAL_INT(0, base_info.version);
    TEST_ASSERT_NULL(base_info.base_name);
    TEST_ASSERT(double_almost_equal(0.0, base_info.base_time));
    TEST_ASSERT_NULL(base_info.base_unit);
    TEST_ASSERT(double_almost_equal(0.0, base_info.base_value));
    TEST_ASSERT(double_almost_equal(0.0, base_info.base_sum));

    TEST_ASSERT_NULL(records[0].name);
    TEST_ASSERT_NULL(records[0].unit);
    TEST_ASSERT(double_almost_equal(0.0, records[0].time));
    TEST_ASSERT(double_almost_equal(0.0, records[0].update_time));
    TEST_ASSERT(double_almost_equal(0.0, records[0].value_sum));
    TEST_ASSERT_EQUAL_INT(SENML_TYPE_UNDEF, records[0].value_type);
}

static void test_senml_json_decode_nobaseinfo(void)
{
    senml_base_info_t base_info;
    senml_record_t    records[2];
    senml_pack_t      pack;

    memset(&base_info, 0, sizeof(senml_base_info_t));
    memset(records, 0, sizeof(records));

    pack.base_info = &base_info;
    pack.records   = records;
    pack.num       = 2;

    int rc = senml_decode_json_s(senml_json_nobaseinfo, &pack);

    TEST_ASSERT(rc == 0);

    TEST_ASSERT_EQUAL_STRING("urn:dev:ow:10e2073a01080063", records[0].name);
    TEST_ASSERT_EQUAL_STRING("Cel", records[0].unit);
    TEST_ASSERT(double_almost_equal(1276020076.0, records[0].time));
    TEST_ASSERT(double_almost_equal(0.0, records[0].update_time));
    TEST_ASSERT(double_almost_equal(0.0, records[0].value_sum));
    TEST_ASSERT_EQUAL_INT(SENML_TYPE_FLOAT, records[0].value_type);
    TEST_ASSERT(double_almost_equal(23.5, records[0].value.f));

    TEST_ASSERT_EQUAL_STRING("urn:dev:ow:10e2073a01080063", records[1].name);
    TEST_ASSERT_EQUAL_STRING("Cel", records[1].unit);
    TEST_ASSERT(double_almost_equal(1276020091.0, records[1].time));
    TEST_ASSERT(double_almost_equal(0.0, records[1].update_time));
    TEST_ASSERT(double_almost_equal(0.0, records[1].value_sum));
    TEST_ASSERT_EQUAL_INT(SENML_TYPE_FLOAT, records[1].value_type);
    TEST_ASSERT(double_almost_equal(23.6, records[1].value.f));
}

static void test_senml_json_decode_empty_elem(void)
{
    senml_base_info_t base_info;
    senml_record_t    records[3];
    senml_pack_t      pack;

    memset(&base_info, 0, sizeof(senml_base_info_t));
    memset(records, 0, sizeof(records));

    pack.base_info = &base_info;
    pack.records   = records;
    pack.num       = 3;

    int rc = senml_decode_json_s(senml_json_empty_elem, &pack);

    TEST_ASSERT(rc == 0);

    TEST_ASSERT_NULL(records[0].name);
    TEST_ASSERT_NULL(records[0].unit);
    TEST_ASSERT(double_almost_equal(0.0, records[0].time));
    TEST_ASSERT(double_almost_equal(0.0, records[0].update_time));
    TEST_ASSERT(double_almost_equal(0.0, records[0].value_sum));
    TEST_ASSERT_EQUAL_INT(SENML_TYPE_UNDEF, records[0].value_type);

    TEST_ASSERT_EQUAL_STRING("urn:dev:ow:10e2073a01080063", records[1].name);
    TEST_ASSERT_EQUAL_STRING("Cel", records[1].unit);
    TEST_ASSERT(double_almost_equal(1276020076.0, records[1].time));
    TEST_ASSERT(double_almost_equal(0.0, records[1].update_time));
    TEST_ASSERT(double_almost_equal(0.0, records[1].value_sum));
    TEST_ASSERT_EQUAL_INT(SENML_TYPE_FLOAT, records[1].value_type);
    TEST_ASSERT(double_almost_equal(23.5, records[1].value.f));

    TEST_ASSERT_EQUAL_STRING("urn:dev:ow:10e2073a01080063", records[2].name);
    TEST_ASSERT_EQUAL_STRING("Cel", records[2].unit);
    TEST_ASSERT(double_almost_equal(1276020091.0, records[2].time));
    TEST_ASSERT(double_almost_equal(0.0, records[2].update_time));
    TEST_ASSERT(double_almost_equal(0.0, records[2].value_sum));
    TEST_ASSERT_EQUAL_INT(SENML_TYPE_FLOAT, records[2].value_type);
    TEST_ASSERT(double_almost_equal(23.6, records[2].value.f));
}

static void test_senml_json_decode_baseinfo(void)
{
    senml_base_info_t base_info;
    senml_record_t    records[2];
    senml_pack_t      pack;

    memset(&base_info, 0, sizeof(senml_base_info_t));
    memset(records, 0, sizeof(records));

    pack.base_info = &base_info;
    pack.records   = records;
    pack.num       = 2;

    int rc = senml_decode_json_s(senml_json_baseinfo, &pack);

    TEST_ASSERT(rc == 0);

    TEST_ASSERT_EQUAL_INT(0, base_info.version);
    TEST_ASSERT_EQUAL_STRING("urn:dev:ow:10e2073a01080063", base_info.base_name);
    TEST_ASSERT(double_almost_equal(0.0, base_info.base_time));
    TEST_ASSERT_NULL(base_info.base_unit);
    TEST_ASSERT(double_almost_equal(0.0, base_info.base_value));
    TEST_ASSERT(double_almost_equal(0.0, base_info.base_sum));

    TEST_ASSERT_NULL(records[0].name);
    TEST_ASSERT_EQUAL_STRING("Cel", records[0].unit);
    TEST_ASSERT(double_almost_equal(1276020076.0, records[0].time));
    TEST_ASSERT(double_almost_equal(0.0, records[0].update_time));
    TEST_ASSERT(double_almost_equal(0.0, records[0].value_sum));
    TEST_ASSERT_EQUAL_INT(SENML_TYPE_FLOAT, records[0].value_type);
    TEST_ASSERT(double_almost_equal(23.5, records[0].value.f));

    TEST_ASSERT_NULL(records[1].name);
    TEST_ASSERT_EQUAL_STRING("Cel", records[1].unit);
    TEST_ASSERT(double_almost_equal(1276020091.0, records[1].time));
    TEST_ASSERT(double_almost_equal(0.0, records[1].update_time));
    TEST_ASSERT(double_almost_equal(0.0, records[1].value_sum));
    TEST_ASSERT_EQUAL_INT(SENML_TYPE_FLOAT, records[1].value_type);
    TEST_ASSERT(double_almost_equal(23.6, records[1].value.f));
}

static void test_senml_json_decode_updatetime(void)
{
    senml_record_t    records[1];
    senml_pack_t      pack;

    memset(records, 0, sizeof(records));

    pack.base_info = NULL;
    pack.records   = records;
    pack.num       = 1;

    int rc = senml_decode_json_s(senml_json_updatetime, &pack);

    TEST_ASSERT(rc == 0);

    TEST_ASSERT_EQUAL_STRING("John", records[0].name);
    TEST_ASSERT_EQUAL_STRING("V", records[0].unit);
    TEST_ASSERT(double_almost_equal(123456789.0, records[0].time));
    TEST_ASSERT(double_almost_equal(5.0, records[0].update_time));
    TEST_ASSERT(double_almost_equal(0.0, records[0].value_sum));
    TEST_ASSERT_EQUAL_INT(SENML_TYPE_FLOAT, records[0].value_type);
    TEST_ASSERT(double_almost_equal(120.398, records[0].value.f));
}

static void test_senml_json_decode_stringvalue(void)
{
    senml_record_t    records[1];
    senml_pack_t      pack;

    memset(records, 0, sizeof(records));

    pack.base_info = NULL;
    pack.records   = records;
    pack.num       = 1;

    int rc = senml_decode_json_s(senml_json_stringvalue, &pack);

    TEST_ASSERT(rc == 0);

    TEST_ASSERT_EQUAL_STRING("Jackie", records[0].name);
    TEST_ASSERT_NULL(records[0].unit);
    TEST_ASSERT(double_almost_equal(123456790.0, records[0].time));
    TEST_ASSERT(double_almost_equal(0.0, records[0].update_time));
    TEST_ASSERT(double_almost_equal(0.0, records[0].value_sum));
    TEST_ASSERT_EQUAL_INT(SENML_TYPE_STRING, records[0].value_type);
    TEST_ASSERT_EQUAL_STRING("what a nice string", records[0].value.s);
}

static void test_senml_json_decode_basevalues(void)
{
    senml_base_info_t base_info;
    senml_record_t    records[1];
    senml_pack_t      pack;

    memset(&base_info, 0, sizeof(senml_base_info_t));
    memset(records, 0, sizeof(records));

    pack.base_info = &base_info;
    pack.records   = records;
    pack.num       = 1;

    int rc = senml_decode_json_s(senml_json_basevalues, &pack);

    TEST_ASSERT(rc == 0);

    TEST_ASSERT_EQUAL_INT(5, base_info.version);
    TEST_ASSERT_EQUAL_STRING("riot_device", base_info.base_name);
    TEST_ASSERT(double_almost_equal(123456000.0, base_info.base_time));
    TEST_ASSERT_EQUAL_STRING("Cel", base_info.base_unit);
    TEST_ASSERT(double_almost_equal(-273.15, base_info.base_value));
    TEST_ASSERT(double_almost_equal(89.12, base_info.base_sum));

    TEST_ASSERT_EQUAL_STRING("Hans", records[0].name);
    TEST_ASSERT_NULL(records[0].unit);
    TEST_ASSERT(double_almost_equal(123456790.0, records[0].time));
    TEST_ASSERT(double_almost_equal(0.0, records[0].update_time));
    TEST_ASSERT(double_almost_equal(0.0, records[0].value_sum));
    TEST_ASSERT_EQUAL_INT(SENML_TYPE_STRING, records[0].value_type);
    TEST_ASSERT_EQUAL_STRING("what a nice string", records[0].value.s);
}

static void test_senml_json_decode_datavalue(void)
{
    senml_base_info_t base_info;
    senml_record_t    records[1];
    senml_pack_t      pack;

    memset(&base_info, 0, sizeof(senml_base_info_t));
    memset(records, 0, sizeof(records));

    pack.base_info = &base_info;
    pack.records   = records;
    pack.num       = 1;

    int rc = senml_decode_json_s(senml_json_datavalue, &pack);

    TEST_ASSERT(rc == 0);

    TEST_ASSERT_EQUAL_INT(5, base_info.version);
    TEST_ASSERT_EQUAL_STRING("riot_device", base_info.base_name);
    TEST_ASSERT(double_almost_equal(123456000.0, base_info.base_time));
    TEST_ASSERT_EQUAL_STRING("Cel", base_info.base_unit);
    TEST_ASSERT(double_almost_equal(-54.0001, base_info.base_value));
    TEST_ASSERT(double_almost_equal(0.0, base_info.base_sum));

    TEST_ASSERT_EQUAL_STRING("Kim", records[0].name);
    TEST_ASSERT_NULL(records[0].unit);
    TEST_ASSERT(double_almost_equal(-10.0, records[0].time));
    TEST_ASSERT(double_almost_equal(12.0, records[0].update_time));
    TEST_ASSERT(double_almost_equal(0.0, records[0].value_sum));
    TEST_ASSERT_EQUAL_INT(SENML_TYPE_BINARY, records[0].value_type);
    TEST_ASSERT_EQUAL_STRING("U2VuTUwgaXMgc28gY29vbCEK", records[0].value.d);
}

static void test_senml_json_decode_valuesum(void)
{
    senml_record_t    records[1];
    senml_pack_t      pack;

    memset(records, 0, sizeof(records));

    pack.base_info = NULL;
    pack.records   = records;
    pack.num       = 1;

    int rc = senml_decode_json_s(senml_json_valuesum, &pack);

    TEST_ASSERT(rc == 0);

    TEST_ASSERT_EQUAL_STRING("Donald", records[0].name);
    TEST_ASSERT_EQUAL_STRING("W", records[0].unit);
    TEST_ASSERT(double_almost_equal(987654321.0, records[0].time));
    TEST_ASSERT(double_almost_equal(0.0, records[0].update_time));
    TEST_ASSERT(double_almost_equal(1240.812, records[0].value_sum));
    TEST_ASSERT_EQUAL_INT(SENML_TYPE_UNDEF, records[0].value_type);
}

static void test_senml_json_decode_boolvalue(void)
{
    senml_record_t    records[1];
    senml_pack_t      pack;

    memset(records, 0, sizeof(records));

    pack.base_info = NULL;
    pack.records   = records;
    pack.num       = 1;

    int rc = senml_decode_json_s(senml_json_boolvalue, &pack);

    TEST_ASSERT(rc == 0);

    TEST_ASSERT_EQUAL_STRING("Daisy", records[0].name);
    TEST_ASSERT_NULL(records[0].unit);
    TEST_ASSERT(double_almost_equal(0.0, records[0].time));
    TEST_ASSERT(double_almost_equal(0.0, records[0].update_time));
    TEST_ASSERT(double_almost_equal(0.0, records[0].value_sum));
    TEST_ASSERT_EQUAL_INT(SENML_TYPE_BOOL, records[0].value_type);
    TEST_ASSERT(records[0].value.b);
}

static void test_senml_json_decode_link(void)
{
    senml_record_t    records[1];
    senml_pack_t      pack;

    memset(records, 0, sizeof(records));

    pack.base_info = NULL;
    pack.records   = records;
    pack.num       = 1;

    int rc = senml_decode_json_s(senml_json_link, &pack);

    TEST_ASSERT(rc == 0);

    TEST_ASSERT_EQUAL_STRING("Link", records[0].name);
    TEST_ASSERT_NULL(records[0].unit);
    TEST_ASSERT_EQUAL_STRING("[{\\\"href\\\":\\\"derp\\\",\\\"foo\\\":\\\"bar\\\"}]",
                             records[0].link);
    TEST_ASSERT(double_almost_equal(0.0, records[0].time));
    TEST_ASSERT(double_almost_equal(0.0, records[0].update_time));
    TEST_ASSERT(double_almost_equal(0.0, records[0].value_sum));
    TEST_ASSERT_EQUAL_INT(SENML_TYPE_BOOL, records[0].value_type);
    TEST_ASSERT(records[0].value.b);
}

static void test_senml_json_encode_no_baseinfo(void)
{
    char output[1024];

    senml_record_t    records[5];
    senml_pack_t      pack;

    memset(records, 0, sizeof(records));

    pack.base_info = NULL;
    pack.records   = records;
    pack.num       = 5;

    records[0].name        = "float_measure";
    records[0].unit        = "m";
    records[0].time        = 1480778196.431;
    records[0].update_time = 10.0;
    records[0].value_type  = SENML_TYPE_FLOAT;
    records[0].value.f     = 42.195;

    records[1].name        = "with_sum";
    records[1].unit        = "m";
    records[1].time        = 1480778906.541;
    records[1].update_time = 10.0;
    records[1].value_type  = SENML_TYPE_FLOAT;
    records[1].value.f     = 39.19;
    records[1].value_sum   = 1423576.331;

    records[2].name        = "string_measure";
    records[2].time        = 1480778388.934;
    records[2].update_time = 30.0;
    records[2].value_type  = SENML_TYPE_STRING;
    records[2].value.s     = "hello i am a string value";

    records[3].name        = "bool_measure";
    records[3].time        = 1480778500.871;
    records[3].update_time = 1.0;
    records[3].value_type  = SENML_TYPE_BOOL;
    records[3].value.b     = false;

    records[4].name        = "data_measure";
    records[4].link        = "[{\\\"href\\\":\\\"derp\\\",\\\"foo\\\":\\\"bar\\\"}]";
    records[4].time        = 1480778629.23;
    records[4].update_time = 120.000;
    records[4].value_type  = SENML_TYPE_BINARY;
    records[4].value.d     = "VGhpcyBpcyBiaW5hcnkgdmFsdWUK";

    int rc = senml_encode_json_s(&pack, output, 1024);

    TEST_ASSERT(rc == 0);

    TEST_ASSERT(!strcmp(expected_no_baseinfo, output));
}

static void test_senml_json_encode_with_baseinfo(void)
{
    char output[512];

    senml_base_info_t base_info;
    senml_record_t    records[2];
    senml_pack_t      pack;

    memset(&base_info, 0, sizeof(senml_base_info_t));
    memset(records, 0, sizeof(records));

    pack.base_info = &base_info;
    pack.records   = records;
    pack.num       = 2;

    base_info.version    = 5;
    base_info.base_name  = "urn:dev:mac:0024befffe804ff2;";
    base_info.base_time  = 1468342153.341;
    base_info.base_unit  = "Cel";
    base_info.base_value = 21.0;
    base_info.base_sum   = 8237823.412;

    records[0].name        = "temperature";
    records[0].time        = -5;
    records[0].update_time = 5;
    records[0].value_type  = SENML_TYPE_FLOAT;
    records[0].value.f     = 0.5;

    records[1].name        = "temperature";
    records[1].time        = -10;
    records[1].update_time = 5;
    records[1].value_type  = SENML_TYPE_FLOAT;
    records[1].value.f     = 0.7;

    int rc = senml_encode_json_s(&pack, output, 512);

    TEST_ASSERT(rc == 0);

    TEST_ASSERT(!strcmp(expected_with_baseinfo, output));
}

Test *tests_senml_json_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_senml_json_decode_empty),
        new_TestFixture(test_senml_json_decode_nobaseinfo),
        new_TestFixture(test_senml_json_decode_empty_elem),
        new_TestFixture(test_senml_json_decode_baseinfo),
        new_TestFixture(test_senml_json_decode_updatetime),
        new_TestFixture(test_senml_json_decode_stringvalue),
        new_TestFixture(test_senml_json_decode_basevalues),
        new_TestFixture(test_senml_json_decode_datavalue),
        new_TestFixture(test_senml_json_decode_valuesum),
        new_TestFixture(test_senml_json_decode_boolvalue),
        new_TestFixture(test_senml_json_decode_link),
        new_TestFixture(test_senml_json_encode_with_baseinfo),
        new_TestFixture(test_senml_json_encode_no_baseinfo)
    };

    EMB_UNIT_TESTCALLER(senml_json_tests,
                        NULL, /* no set_up function */
                        NULL, /* no tear_down function */
                        fixtures);

    return (Test *)&senml_json_tests;
}

void tests_senml_json(void)
{
    TESTS_RUN(tests_senml_json_tests());
}

/** @} */
