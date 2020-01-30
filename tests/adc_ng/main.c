/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
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
 * @brief       Test application for ADC NG drivers
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <stdlib.h>

#include "adc_ng.h"
#include "adc_ng_util.h"
#include "fmt.h"
#include "periph/adc.h"
#include "shell.h"

static int cmd_init(int argc, char **argv);
static int cmd_off(int argc, char **argv);
static int cmd_single(int argc, char **argv);
static int cmd_quick(int argc, char **argv);
static int cmd_volt(int argc, char **argv);
static int cmd_vcc(int argc, char **argv);
static int cmd_ntc(int argc, char **argv);
static int cmd_entropy(int argc, char **argv);

static const shell_command_t cmds[] = {
    { "init", "adc_ng_init(adc, chan, res, ref)", cmd_init },
    { "off", "adc_ng_off(adc)", cmd_off },
    { "single", "adc_ng_single(adc)", cmd_single },
    { "quick", "adc_ng_quick(adc, chan)", cmd_quick },
    { "volt", "adc_ng_voltage(adc)", cmd_volt },
    { "vcc", "adc_ng_vcc(adc)", cmd_vcc },
    { "ntc", "adc_ng_internal_ntc(adc)", cmd_ntc },
    { "entropy", "adc_ng_entropy(adc, num_bytes)", cmd_entropy },
    { NULL, NULL, NULL }
};

static uint8_t entropy_buf[32];

static int is_adc_invalid(uint8_t adc)
{
    if (adc >= XFA_LEN(adc_ng_backend_t, adc_ng_backends)) {
        print_str("ADC #");
        print_u32_dec(adc);
        print_str(" is out of range\n");
        return 1;
    }

    return 0;
}

static int cmd_init(int argc, char **argv)
{
    if (argc != 5) {
        print_str("Usage: ");
        print_str(argv[0]);
        print_str(" <ADC> <CHANNEL> <RESOLUTION> <REF_VOLTAGE_MV>\n");
        return -1;
    }

    uint8_t adc = atoi(argv[1]);
    if (is_adc_invalid(adc)) return -1;
    uint8_t chan = atoi(argv[2]);
    uint8_t res = atoi(argv[3]);
    int16_t ref = atoi(argv[4]);

    print_str("adc_ng_init(");
    print_u32_dec(adc);
    print_str(", ");
    print_u32_dec(chan);
    print_str(", ");
    print_u32_dec(res);
    print_str(", &");
    print_u32_dec(ref);
    print_str(")\n");

    int retval = adc_ng_init(adc, chan, res, &ref);
    if (!retval) {
        print_str("Chosen reference voltage: ");
        char buf[16];
        print(buf, fmt_s32_dfp(buf, ref, -3));
        print_str(" V \n");
    }
    else {
        print_str("adc_ng_init() failed with ");
        print_s32_dec(retval);
        print_str("\n");
    }

    return 0;
}

static int cmd_off(int argc, char **argv)
{
    if (argc != 2) {
        print_str("Usage: ");
        print_str(argv[0]);
        print_str(" <ADC>\n");
        return -1;
    }

    uint8_t adc = atoi(argv[1]);
    if (is_adc_invalid(adc)) return -1;
    adc_ng_off(adc);
    print_str("adc_ng_off(");
    print_u32_dec(adc);
    print_str(")\n");

    return 0;
}

static int cmd_single(int argc, char **argv)
{
    if (argc != 2) {
        print_str("Usage: ");
        print_str(argv[0]);
        print_str(" <ADC>\n");
        return -1;
    }

    uint8_t adc = atoi(argv[1]);
    if (is_adc_invalid(adc)) return -1;
    int32_t sample;
    print_str("adc_ng_single(");
    print_u32_dec(adc);
    print_str("): ");
    int retval = adc_ng_single(adc, &sample);
    if (!retval) {
        print_str("Success\nSample: ");
        print_s32_dec(sample);
        print_str("\n");
    }
    else {
        print_str("Failed (");
        print_s32_dec(retval);
        print_str(")\n");
    }

    return 0;
}

static int cmd_quick(int argc, char **argv)
{
    if (argc != 3) {
        print_str("Usage: ");
        print_str(argv[0]);
        print_str(" <ADC> <CHANNEL>\n");
        return -1;
    }

    uint8_t adc = atoi(argv[1]);
    if (is_adc_invalid(adc)) return -1;
    uint8_t chan = atoi(argv[2]);

    print_str("adc_ng_quick(");
    print_u32_dec(adc);
    print_str(", ");
    print_u32_dec(chan);
    print_str(", &sample)\n");

    int32_t sample;
    int retval = adc_ng_quick(adc, chan, &sample);
    if (retval) {
        print_str("adc_ng_quick() failed with ");
        print_s32_dec(retval);
        print_str("\n");
        return -1;
    }
    print_str("sample = ");
    print_s32_dec(sample);
    print_str("\n");

    return 0;
}

static int cmd_volt(int argc, char **argv)
{
    if (argc != 2) {
        print_str("Usage: ");
        print_str(argv[0]);
        print_str(" <ADC>\n");
        return -1;
    }

    uint8_t adc = atoi(argv[1]);
    if (is_adc_invalid(adc)) return -1;

    print_str("adc_ng_voltage(");
    print_u32_dec(adc);
    print_str(", &value)\n");

    int16_t value;
    int retval = adc_ng_voltage(adc, &value);
    if (retval) {
        print_str("adc_ng_voltage() failed with ");
        print_s32_dec(retval);
        print_str("\n");
        return -1;
    }
    char buf[16];
    print_str("value = ");
    print(buf, fmt_s32_dfp(buf, value, -3));
    print_str(" V \n");

    return 0;
}

static int cmd_vcc(int argc, char **argv)
{
    if (argc != 2) {
        print_str("Usage: ");
        print_str(argv[0]);
        print_str(" <ADC>\n");
        return -1;
    }

    uint8_t adc = atoi(argv[1]);
    if (is_adc_invalid(adc)) return -1;

    print_str("adc_ng_vcc(");
    print_u32_dec(adc);
    print_str(", &value)\n");

    int16_t value;
    int retval = adc_ng_vcc(adc, &value);
    if (retval) {
        print_str("adc_ng_vcc() failed with ");
        print_s32_dec(retval);
        print_str("\n");
        return -1;
    }
    char buf[16];
    print_str("value = ");
    print(buf, fmt_s32_dfp(buf, value, -3));
    print_str(" V \n");

    return 0;
}

static int cmd_ntc(int argc, char **argv)
{
    if (argc != 2) {
        print_str("Usage: ");
        print_str(argv[0]);
        print_str(" <ADC>\n");
        return -1;
    }

    uint8_t adc = atoi(argv[1]);
    if (is_adc_invalid(adc)) return -1;

    print_str("adc_ng_internal_ntc(");
    print_u32_dec(adc);
    print_str(", &value)\n");

    int16_t value;
    int retval = adc_ng_internal_ntc(adc, &value);
    if (retval == -ENOTSUP) {
        print_str("No NTC connected to this ADC (or configured)\n");
        return -1;
    }
    else if (retval != 0) {
        print_str("adc_ng_internal_ntc() failed with ");
        print_s32_dec(retval);
        print_str("\n");
        return -1;
    }
    char buf[16];
    print_str("value = ");
    print(buf, fmt_s32_dfp(buf, value, -1));
    print_str("°C\n");

    return 0;
}

static int cmd_entropy(int argc, char **argv)
{
    if (argc != 3) {
        print_str("Usage: ");
        print_str(argv[0]);
        print_str(" <ADC> <NUM_BYTES>\n");
        return -1;
    }

    uint8_t adc = atoi(argv[1]);
    if (is_adc_invalid(adc)) return -1;

    size_t size = atoi(argv[2]);

    if (size > sizeof(entropy_buf)) {
        print_str("Given size too big, must be at most ");
        print_u32_dec(sizeof(entropy_buf));
        print_str("\n");
        return -1;
    }

    print_str("adc_ng_entropy(");
    print_u32_dec(adc);
    print_str(", buf, ");
    print_u32_dec(size);
    print_str(")\n");

    int retval = adc_ng_entropy(adc, entropy_buf, size);
    if (retval == -ENOTSUP) {
        print_str("The given ADC doesn't support entropy collection.\n");
        return -1;
    }
    else if (retval) {
        print_str("adc_ng_entropy() failed with ");
        print_s32_dec(retval);
        print_str("\n");
        return -1;
    }

    uint8_t *buf = (uint8_t *)entropy_buf;
    print_str("Output:\n");
    char printbuf[2];
    fmt_byte_hex(printbuf, buf[0]);
    print(printbuf, 2);
    for (size_t i = 1; i < size; i++) {
        print((i % 8) ? " " : "\n", 1);
        fmt_byte_hex(printbuf, buf[i]);
        print(printbuf, 2);
    }
    print_str("\n");

    return 0;
}

static void print_driver(uint8_t adc)
{
    const adc_ng_backend_t be = adc_ng_backends[adc];
    const adc_ng_driver_t *drv = be.driver;
    print_str("Resolutions supported: ");
    uint8_t lowest = adc_ng_min_res(adc);
    print_u32_dec(lowest);
    print_str(" bit");
    for (uint8_t i = lowest + 1; i < 32; i++) {
        if (adc_ng_supports_res(adc, i)) {
            print_str(", ");
            print_u32_dec(i);
            print_str(" bit");
        }
    }

    print_str("\nChannels referring to single ended inputs: 0 - ");
    print_u32_dec(adc_ng_highest_single_ended_channel(adc));

    print_str("\nSupported references: ");
    char buf[16];
    print(buf, fmt_s32_dfp(buf, drv->refs[0], -3));
    print_str(" V");
    for (unsigned i = 1; drv->refs[i] != 0; i++) {
        print_str(", ");
        print(buf, fmt_s32_dfp(buf, drv->refs[i], -3));
        print_str(" V");
    }

    print_str("\nReference selectable as input: ");
    if (drv->ref_input_idx == ADC_NG_NO_SUCH_REF) {
        print_str("None");
    }
    else {
        int32_t ref = drv->refs[drv->ref_input_idx];
        print(buf, fmt_s32_dfp(buf, ref, -3));
        print_str(" V");
    }
}

int main(void)
{
    print_str(
        "RIOT ADC NG driver test\n"
        "=======================\n"
        "\n"
    );

    /* initialize all available ADC lines */
    for (uint8_t i = 0; i < XFA_LEN(adc_ng_backend_t, adc_ng_backends); i++) {
        print_str("Driver #");
        print_u32_dec(i);
        print_str(":\n");
        print_driver(i);
    }

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(cmds, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
