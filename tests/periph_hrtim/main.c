/*
 * Copyright (c) 2020 LAAS-CNRS
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief       Test for low-level HRTIM drivers
 *
 * @author      Hugues Larrive <hugues.larrive@laas.fr>
 *
 * @}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shell.h"
#include "periph/hrtim.h"

static uint8_t initiated;
static uint8_t tu_initiated;

static unsigned _get_dev(const char *dev_str)
{
    unsigned dev = atoi(dev_str);
    if (dev >= HRTIM_NUMOF) {
        printf("Error: device HRTIM_DEV(%u) is unknown\n", dev);
        return UINT_MAX;
    }
    return dev;
}

static int cmd_init(int argc, char** argv)
{
    if (argc != 4) {
        uint32_t f_hrtim = periph_apb_clk(hrtim_config[0].bus) * 2;
        uint32_t max_freq = f_hrtim / 3;
        uint16_t min_freq = f_hrtim / 4 / 0xfffd;
        min_freq++;
        printf("usage: %s <dev> <frequency> <dt>\n", argv[0]);
        printf("\tdev: device by number between 0 and %u\n", HRTIM_NUMOF - 1);
        printf("\tfrequency: desired frequency between %" PRIu16
                " and %" PRIu32 " in Hz\n", min_freq, max_freq);
        puts("\tdt: dead time (ns)");
        return 1;
    }

    unsigned dev = _get_dev(argv[1]);
    if (dev == UINT_MAX) {
        return 1;
    }

    uint16_t period;
    uint32_t freq = atoi(argv[2]);
    period = hrtim_init(HRTIM_DEV(dev), &freq, (uint16_t)atoi(argv[3]));

    if (period != 0) {
        printf("The effective frequency is %" PRIu32 "Hz\n", freq);
        printf("The period is set to %" PRIu16 "\n", period);
        for (unsigned i = 0; i < HRTIM_STU_NUMOF; i++) {
            tu_initiated |= (1 << i);
        }
        initiated |= (1 << dev);
        return 0;
    }

    puts("Error: device is not initiated");
    return 1;
}

static int cmd_pwm_set(int argc, char**argv)
{
    if (argc != 5) {
        printf("usage: %s <dev> <tu> <val> <pha>\n", argv[0]);
        printf("\tdev: device by number between 0 and %d\n", HRTIM_NUMOF - 1);
        printf("\ttu: timing unit by number between 0 and %d\n", HRTIM_STU_NUMOF - 1);
            puts("\t\t0: TIMA");
            puts("\t\t1: TIMB");
            puts("\t\t2: TIMC");
            puts("\t\t3: TIMD");
            puts("\t\t4: TIME");
#if (HRTIM_STU_NUMOF == 6)
            puts("\t\t5: TIMF");
#endif
        puts("\tval: duty cycle");
        puts("\tpha: phase shift\n");
        return 1;
    }

    unsigned dev = _get_dev(argv[1]);
    if (dev == UINT_MAX) {
        return 1;
    }

    if ((initiated & (1 << dev)) == 0) {
        puts("Error: hrtim is not initiated.\n");
        puts("Execute init function first.\n");
        return 1;
    }

    hrtim_tu_t tu = atoi(argv[2]);
    if (tu >= HRTIM_STU_NUMOF) {
        printf("Error: tu %d is unknown.\n", tu);
        return 1;
    }

    if ((tu_initiated & (1 << tu)) == 0) {
        puts("Error: timing unit is not initiated.\n");
        puts("Execute init_tu function first.\n");
        return 1;
    }

    hrtim_pwm_set(HRTIM_DEV(dev), tu, (uint16_t)atoi(argv[3]),
                    (uint16_t)atoi(argv[4]));
    return 0;
}

static int cmd_init_mstr(int argc, char** argv)
{
    if (argc != 3) {
        uint32_t f_hrtim = periph_apb_clk(hrtim_config[0].bus) * 2;
        uint32_t max_freq = f_hrtim / 3;
        uint16_t min_freq = f_hrtim / 4 / 0xfffd;
        min_freq++;
        printf("usage: %s <dev> <frequency>\n", argv[0]);
        printf("\tdev: device by number between 0 and %u\n", HRTIM_NUMOF - 1);
        printf("\tfrequency: desired frequency between %" PRIu16
                " and %" PRIu32 " in Hz\n", min_freq, max_freq);
        return 1;
    }

    unsigned dev = _get_dev(argv[1]);
    if (dev == UINT_MAX) {
        return 1;
    }

    uint32_t freq = atoi(argv[2]);
    uint16_t period;
    period = hrtim_init_mstr(HRTIM_DEV(dev), &freq);

    if (period != 0) {
        printf("The effective frequency is %" PRIu32 "Hz\n", freq);
        printf("The period is set to %" PRIu16 "\n", period);
        initiated |= (1 << dev);
        return 0;
    }

    puts("Error: device is not initiated");
    return 1;
}

static int cmd_init_tu(int argc, char** argv)
{
    if (argc != 5) {
        uint32_t f_hrtim = periph_apb_clk(hrtim_config[0].bus) * 2;
        uint32_t max_freq = f_hrtim / 3;
        uint16_t min_freq = f_hrtim / 4 / 0xfffd;
        min_freq++;
        printf("usage: %s <dev> <tu> <frequency> <dt>\n", argv[0]);
        printf("\tdev: device by number between 0 and %u\n", HRTIM_NUMOF - 1);
        printf("\ttu: timing unit by number between 0 and %d\n",
                HRTIM_STU_NUMOF - 1);
        puts("\t\t0: TIMA");
        puts("\t\t1: TIMB");
        puts("\t\t2: TIMC");
        puts("\t\t3: TIMD");
        puts("\t\t4: TIME");
#if (HRTIM_STU_NUMOF == 6)
        puts("\t\t5: TIMF");
#endif
        printf("\tfrequency: desired frequency between %" PRIu16
                " and %" PRIu32 " in Hz\n", min_freq, max_freq);
        return 1;
    }

    unsigned dev = _get_dev(argv[1]);
    if (dev == UINT_MAX) {
        return 1;
    }

    if ((initiated & (1 << dev)) == 0) {
        puts("Error: hrtim is not initiated.\n");
        puts("Execute init_mstr function first.\n");
        return 1;
    }

    hrtim_tu_t tu = atoi(argv[2]);
    if (tu >= HRTIM_STU_NUMOF) {
        printf("Error: tu %d is unknown.\n", tu);
        return 1;
    }

    uint16_t period;
    uint32_t freq = atoi(argv[3]);
    period = hrtim_init_tu(HRTIM_DEV(dev), tu, &freq);

    if (period != 0) {
        printf("The effective frequency is %" PRIu32 "Hz\n", freq);
        printf("The period is set to %" PRIu16 "\n", period);
        tu_initiated |= (1 << tu);
        return 0;
    }

    puts("Error: device is not initiated");
    return 1;
}

static int cmd_crossbar(int argc, char**argv)
{
    if (argc != 5) {
        printf("usage: %s <dev> <tu> <out> <cb>\n", argv[0]);
        printf("\tdev: device by number between 0 and %d\n", HRTIM_NUMOF - 1);
        printf("\ttu: timing unit by number between 0 and %d\n", HRTIM_STU_NUMOF - 1);
            puts("\t\t 0: TIMA");
            puts("\t\t 1: TIMB");
            puts("\t\t 2: TIMC");
            puts("\t\t 3: TIMD");
            puts("\t\t 4: TIME");
#if (HRTIM_STU_NUMOF == 6)
            puts("\t\t 5: TIMF");
#endif
        puts("\tout: output 1 or 2");
        puts("\t\t 1: OUT1");
        puts("\t\t 2: OUT2");
        puts("\tcb: crossbar");
        puts("\t\t 0: SOFT_TRIG\tSoftware Set/Reset trigger");
        puts("\t\t 1: RESYNC\tTimer x resynchronization");
        puts("\t\t 2: PER\t\tTimer x Period");
        puts("\t\t 3: CMP1\tTimer x Compare 1");
        puts("\t\t 4: CMP2\tTimer x Compare 2");
        puts("\t\t 5: CMP3\tTimer x Compare 3");
        puts("\t\t 6: CMP4\tTimer x Compare 4");
        puts("\t\t 7: MSTPER\tMaster Period");
        puts("\t\t 8: MSTCMP1\tMaster Compare 1");
        puts("\t\t 9: MSTCMP2\tMaster Compare 2");
        puts("\t\t10: MSTCMP3\tMaster Compare 3");
        puts("\t\t11: MSTCMP4\tMaster Compare 4");
        puts("\t\t12: TIMEVNT1\tTimer Event 1");
        puts("\t\t13: TIMEVNT2\tTimer Event 2");
        puts("\t\t14: TIMEVNT3\tTimer Event 3");
        puts("\t\t15: TIMEVNT4\tTimer Event 4");
        puts("\t\t16: TIMEVNT5\tTimer Event 5");
        puts("\t\t17: TIMEVNT6\tTimer Event 6");
        puts("\t\t18: TIMEVNT7\tTimer Event 7");
        puts("\t\t19: TIMEVNT8\tTimer Event 8");
        puts("\t\t20: TIMEVNT9\tTimer Event 9");
        puts("\t\t21: EXTEVNT1\tExternal Event 1");
        puts("\t\t22: EXTEVNT2\tExternal Event 2");
        puts("\t\t23: EXTEVNT3\tExternal Event 3");
        puts("\t\t24: EXTEVNT4\tExternal Event 4");
        puts("\t\t25: EXTEVNT5\tExternal Event 5");
        puts("\t\t26: EXTEVNT6\tExternal Event 6");
        puts("\t\t27: EXTEVNT7\tExternal Event 7");
        puts("\t\t28: EXTEVNT8\tExternal Event 8");
        puts("\t\t29: EXTEVNT9\tExternal Event 9");
        puts("\t\t30: EXTEVNT10\tExternal Event 10");
        puts("\t\t31: UPDATE\tRegisters update (transfer preload to active)\n");
        return 1;
    }

    unsigned dev = _get_dev(argv[1]);
    if (dev == UINT_MAX) {
        return 1;
    }

    hrtim_tu_t tu = atoi(argv[2]);
    if (tu >= HRTIM_STU_NUMOF) {
        printf("Error: tu %d is unknown.\n", tu);
        return 1;
    }

    hrtim_out_t out = atoi(argv[3]);
    if (out < 1 || out > 2) {
        printf("Error: invalid output number: %d.\n", out);
        return 1;
    }

    uint8_t cb_pos = atoi(argv[4]);
    if (cb_pos > 31) {
        printf("Error: invalid crossbar number: %d.\n", cb_pos);
        return 1;
    }
    hrtim_cb_t cb = (1 << cb_pos);

    if (strcmp(argv[0], "set_cb_set") == 0) {
        hrtim_set_cb_set(HRTIM_DEV(dev), tu, out, cb);
    }
    else if (strcmp(argv[0], "set_cb_unset") == 0) {
        hrtim_set_cb_unset(HRTIM_DEV(dev), tu, out, cb);
    }
    else if (strcmp(argv[0], "rst_cb_set") == 0) {
        hrtim_rst_cb_set(HRTIM_DEV(dev), tu, out, cb);
    }
    else if (strcmp(argv[0], "rst_cb_unset") == 0) {
        hrtim_rst_cb_unset(HRTIM_DEV(dev), tu, out, cb);
    }
    return 0;
}

static int cmd_cmpl_pwm_out(int argc, char**argv)
{
    if (argc != 3) {
        printf("usage: %s <dev> <tu>\n", argv[0]);
        printf("\tdev: device by number between 0 and %d\n", HRTIM_NUMOF - 1);
        printf("\ttu: timing unit by number between 0 and %d\n", HRTIM_STU_NUMOF - 1);
            puts("\t\t0: TIMA");
            puts("\t\t1: TIMB");
            puts("\t\t2: TIMC");
            puts("\t\t3: TIMD");
            puts("\t\t4: TIME");
#if (HRTIM_STU_NUMOF == 6)
            puts("\t\t5: TIMF");
#endif
        return 1;
    }

    unsigned dev = _get_dev(argv[1]);
    if (dev == UINT_MAX) {
        return 1;
    }

    hrtim_tu_t tu = atoi(argv[2]);
    if (tu >= HRTIM_STU_NUMOF) {
        printf("Error: tu %d is unknown.\n", tu);
        return 1;
    }
    hrtim_cmpl_pwm_out(HRTIM_DEV(dev), tu);
    return 0;
}

static int cmd_period_set(int argc, char**argv)
{
    if (argc != 4) {
        printf("usage: %s <dev> <tu> <val>\n", argv[0]);
        printf("\tdev: device by number between 0 and %d\n", HRTIM_NUMOF - 1);
        printf("\ttu: timing unit by number between 0 and %d\n", HRTIM_STU_NUMOF - 1);
        puts("\t\t0: TIMA");
        puts("\t\t1: TIMB");
        puts("\t\t2: TIMC");
        puts("\t\t3: TIMD");
        puts("\t\t4: TIME");
#if (HRTIM_STU_NUMOF == 6)
        puts("\t\t5: TIMF");
        puts("\t\t6: MSTR (master timer)");
#else
        puts("\t\t5: MSTR (master timer)");
#endif
        puts("\tval: raw value\n");
        return 1;
    }

    unsigned dev = _get_dev(argv[1]);
    if (dev == UINT_MAX) {
        return 1;
    }

    hrtim_tu_t tu = atoi(argv[2]);
    if (tu > HRTIM_STU_NUMOF) {
        printf("Error: tu %d is unknown.\n", tu);
        return 1;
    }

    hrtim_period_set(HRTIM_DEV(dev), tu, atoi(argv[3]));
    return 0;
}

static int cmd_cmp_set(int argc, char**argv)
{
    if (argc != 5) {
        printf("usage: %s <dev> <tu> <cmp> <val>\n", argv[0]);
        printf("\tdev: device by number between 0 and %d\n", HRTIM_NUMOF - 1);
        printf("\ttu: timing unit by number between 0 and %d\n", HRTIM_STU_NUMOF - 1);
        puts("\t\t0: TIMA");
        puts("\t\t1: TIMB");
        puts("\t\t2: TIMC");
        puts("\t\t3: TIMD");
        puts("\t\t4: TIME");
#if (HRTIM_STU_NUMOF == 6)
        puts("\t\t5: TIMF");
        puts("\t\t6: MSTR (master timer)");
#else
        puts("\t\t5: MSTR (master timer)");
#endif
        puts("\tcmp: comparator from 1 to 4");
        puts("\tval: raw value\n");
        return 1;
    }

    unsigned dev = _get_dev(argv[1]);
    if (dev == UINT_MAX) {
        return 1;
    }

    if ((initiated & (1 << dev)) == 0) {
        puts("Error: hrtim is not initiated.\n");
        puts("Execute init function first.\n");
        return 1;
    }

    hrtim_tu_t tu = atoi(argv[2]);
    if (tu > HRTIM_STU_NUMOF) {
        printf("Error: tu %d is unknown.\n", tu);
        return 1;
    }
    if ((tu_initiated & (1 << tu)) == 0) {
        puts("Error: timing unit is not initiated.\n");
        puts("Execute init_tu function first.\n");
        return 1;
    }

    hrtim_cmp_t cmp = atoi(argv[3]);
    if (cmp < 1 || cmp > 4) {
        printf("Error: cmp %d is unknown.\n", cmp);
        return 1;
    }

    hrtim_cmp_set(HRTIM_DEV(dev), tu, cmp, (uint16_t)atoi(argv[4]));

    return 0;
}

static int cmd_cnt_en(int argc, char**argv)
{
    if (argc != 3) {
        printf("usage: %s <dev> <cen>\n", argv[0]);
        printf("\tdev: device by number between 0 and %d\n", HRTIM_NUMOF - 1);
        puts("\tcen: CEN mask");
        puts("\t\t1: MCEN");
        puts("\t\t2: TACEN");
        puts("\t\t4: TBCEN");
        puts("\t\t8: TCCEN");
        puts("\t\t16: TDCEN");
        puts("\t\t32: TECEN");
#if (HRTIM_STU_NUMOF == 6)
        puts("\t\t64: TFCEN");
#endif
        return 1;
    }

    unsigned dev = _get_dev(argv[1]);
    if (dev == UINT_MAX) {
        return 1;
    }

    hrtim_cen_t cen = (atoi(argv[2]) << HRTIM_MCR_MCEN_Pos);
    hrtim_cnt_en(HRTIM_DEV(dev), cen);
    return 0;
}

static int cmd_cnt_dis(int argc, char**argv)
{
    if (argc != 3) {
        printf("usage: %s <dev> <cen>\n", argv[0]);
        printf("\tdev: device by number between 0 and %d\n", HRTIM_NUMOF - 1);
        puts("\tcen: CEN mask");
        puts("\t\t1: MCEN");
        puts("\t\t2: TACEN");
        puts("\t\t4: TBCEN");
        puts("\t\t8: TCCEN");
        puts("\t\t16: TDCEN");
        puts("\t\t32: TECEN");
#if (HRTIM_STU_NUMOF == 6)
        puts("\t\t64: TFCEN");
#endif
        return 1;
    }

    unsigned dev = _get_dev(argv[1]);
    if (dev == UINT_MAX) {
        return 1;
    }

    hrtim_cen_t cen = (atoi(argv[2]) << HRTIM_MCR_MCEN_Pos);
    hrtim_cnt_dis(HRTIM_DEV(dev), cen);
    return 0;
}

static int cmd_out_en(int argc, char**argv)
{
    if (argc != 4) {
        printf("usage: %s <dev> <tu> <out>\n", argv[0]);
        printf("\tdev: device by number between 0 and %d\n", HRTIM_NUMOF - 1);
        printf("\ttu: timing unit by number between 0 and %d\n", HRTIM_STU_NUMOF - 1);
        puts("\t\t0: TIMA");
        puts("\t\t1: TIMB");
        puts("\t\t2: TIMC");
        puts("\t\t3: TIMD");
        puts("\t\t4: TIME");
#if (HRTIM_STU_NUMOF == 6)
        puts("\t\t5: TIMF");
#endif
        puts("\tout: output(s) to enable");
        puts("\t\t1: OUT1");
        puts("\t\t2: OUT2");
        puts("\t\t3: OUT1 | OUT2 (both)");
        return 1;
    }

    unsigned dev = _get_dev(argv[1]);
    if (dev == UINT_MAX) {
        return 1;
    }

    hrtim_tu_t tu = atoi(argv[2]);
    if (tu >= HRTIM_STU_NUMOF) {
        printf("Error: tu %d is unknown.\n", tu);
        return 1;
    }

    hrtim_out_t out = atoi(argv[3]);
    if (out < 1 || out > 3) {
        printf("Error: out = %d is invalid.\n", out);
        return 1;
    }

    hrtim_out_en(HRTIM_DEV(dev), tu, out);
    return 0;
}

static int cmd_out_dis(int argc, char**argv)
{
    if (argc != 4) {
        printf("usage: %s <dev> <tu> <out>\n", argv[0]);
        printf("\tdev: device by number between 0 and %d\n", HRTIM_NUMOF - 1);
        printf("\ttu: timing unit by number between 0 and %d\n", HRTIM_STU_NUMOF - 1);
        puts("\t\t0: TIMA");
        puts("\t\t1: TIMB");
        puts("\t\t2: TIMC");
        puts("\t\t3: TIMD");
        puts("\t\t4: TIME");
#if (HRTIM_STU_NUMOF == 6)
        puts("\t\t5: TIMF");
#endif
        puts("\tout: output(s) to disable");
        puts("\t\t1: OUT1");
        puts("\t\t2: OUT2");
        puts("\t\t3: OUT1 | OUT2 (both)");
        return 1;
    }

    unsigned dev = _get_dev(argv[1]);
    if (dev == UINT_MAX) {
        return 1;
    }

    hrtim_tu_t tu = atoi(argv[2]);
    if (tu >= HRTIM_STU_NUMOF) {
        printf("Error: tu %d is unknown.\n", tu);
        return 1;
    }

    hrtim_out_t out = atoi(argv[3]);
    if (out < 1 || out > 3) {
        printf("Error: out = %d is invalid.\n", out);
        return 1;
    }

    hrtim_out_dis(HRTIM_DEV(dev), tu, out);
    return 0;
}

static int cmd_pwm_dt(int argc, char**argv)
{
    if (argc != 4) {
        printf("usage: %s <dev> <tu> <ns>\n", argv[0]);
        printf("\tdev: device by number between 0 and %d\n", HRTIM_NUMOF - 1);
        printf("\ttu: timing unit by number between 0 and %d\n", HRTIM_STU_NUMOF - 1);
        puts("\t\t0: TIMA");
        puts("\t\t1: TIMB");
        puts("\t\t2: TIMC");
        puts("\t\t3: TIMD");
        puts("\t\t4: TIME");
#if (HRTIM_STU_NUMOF == 6)
        puts("\t\t5: TIMF");
#endif
        puts("\tns: desired dead-time in nano seconds");
        return 1;
    }

    unsigned dev = _get_dev(argv[1]);
    if (dev == UINT_MAX) {
        return 1;
    }

    hrtim_tu_t tu = atoi(argv[2]);
    if (tu >= HRTIM_STU_NUMOF) {
        printf("Error: tu %d is unknown.\n", tu);
        return 1;
    }

    hrtim_pwm_dt(HRTIM_DEV(dev), tu, atoi(argv[3]));
    return 0;
}

static int cmd_an4539(int argc, char**argv)
{
    if (argc != 2) {
        printf("usage: %s <example>\n", argv[0]);
        puts("\texample: an4539 basic operating principles example");
        puts("\t\t1: Single PWM generation");
        puts("\t\t2: Generating multiple PWMs");
        puts("\t\t3: Generating PWM with other timing units and the "
                "master timer");
        puts("\t\t4: Arbitrary waveform generation");
        return 1;
    }

    uint8_t ex = atoi(argv[1]);
    if (ex < 1 || ex > 4) {
        printf("Error: invalid example number: %d.\n", ex);
        return 1;
    }

    /* reset crossbars */
    hrtim_set_cb_unset(0, TIMA, OUT1, 0xfffffffe);
    hrtim_set_cb_unset(0, TIMA, OUT2, 0xfffffffe);
    hrtim_set_cb_unset(0, TIMD, OUT1, 0xfffffffe);
    hrtim_set_cb_unset(0, TIMD, OUT2, 0xfffffffe);
    hrtim_rst_cb_unset(0, TIMA, OUT1, 0xfffffffe);
    hrtim_rst_cb_unset(0, TIMA, OUT2, 0xfffffffe);
    hrtim_rst_cb_unset(0, TIMD, OUT1, 0xfffffffe);
    hrtim_rst_cb_unset(0, TIMD, OUT2, 0xfffffffe);

    uint32_t freq;
    uint16_t period;
    switch (ex) {
        case 1:
            /* Initialize the master timer and TIMD at 100KHz frequency */
            freq = KHZ(100);
            hrtim_init_mstr(0, &freq);
            period = hrtim_init_tu(0, TIMD, &freq);

            /* Set duty cycle to 50% */
            hrtim_cmp_set(0, TIMD, CMP1xR, period / 2);

            /* TD1 output set on TIMD period and reset on TIMD CMP1 event */
            hrtim_set_cb_set(0, TIMD, OUT1, PER);
            hrtim_rst_cb_set(0, TIMD, OUT1, CMP1);

            /* Start Timer D */
            hrtim_cnt_en(0, (MCEN | TDCEN));

            /* Enable TD1 output */
            hrtim_out_en(0, TIMD, OUT1);
            break;
        case 2:
            /* Initialize the master timer and TIMD at 100KHz frequency */
            freq = 33333;
            hrtim_init_mstr(0, &freq);
            freq = KHZ(100);
            period = hrtim_init_tu(0, TIMD, &freq);

            /* Set CMP1 to 25% and CMP2 to 75% of period */
            hrtim_cmp_set(0, TIMD, CMP1xR, period / 4);
            hrtim_cmp_set(0, TIMD, CMP2xR, period * 3 / 4);

            /* TD1 output set on TIMD period and reset on TIMD CMP1 event*/
            hrtim_set_cb_set(0, TIMD, OUT1, PER);
            hrtim_rst_cb_set(0, TIMD, OUT1, CMP1);
            /* TD2 output set on TIMD CMP2 and reset on TIMD period event*/
            hrtim_set_cb_set(0, TIMD, OUT2, CMP2);
            hrtim_rst_cb_set(0, TIMD, OUT2, PER);

            /* Timer A initialization at 33KHz */
            freq = 33333;
            period = hrtim_init_tu(0, TIMA, &freq);

            /* Set duty cycles to 25% */
            hrtim_cmp_set(0, TIMA, CMP1xR, period / 4);
            hrtim_cmp_set(0, TIMA, CMP2xR, period / 2);
            hrtim_cmp_set(0, TIMA, CMP3xR, period * 3 / 4);

            /* TA1 output set on TIMA period and reset on TIMA CMP1 event*/
            hrtim_set_cb_set(0, TIMA, OUT1, PER);
            hrtim_rst_cb_set(0, TIMA, OUT1, CMP1);
            /* TA2 output set on TIMA CMP2 and reset on TIMA CMP3 event*/
            hrtim_set_cb_set(0, TIMA, OUT2, CMP2);
            hrtim_rst_cb_set(0, TIMA, OUT2, CMP3);

            /* Enable TA1, TA2, TD1 and TD2 outputs */
            hrtim_out_en(0, TIMA, OUT1 | OUT2);
            hrtim_out_en(0, TIMD, OUT1 | OUT2);

            /* Start Master, Timer A and Timer D counters*/
            hrtim_cnt_en(0, (MCEN | TACEN | TDCEN));

            /* Timer A and Timer D counters reset on Master period event */
            hrtim_rst_evt_en(0, TIMA, RST_MSTPER);
            hrtim_rst_evt_en(0, TIMD, RST_MSTPER);
            break;
        case 3:
            /* Initialize the master timer, set period for 100002Hz frequency
             * and duty cycle to 50% */
            freq = KHZ(100) + 2;
            period = hrtim_init_mstr(0, &freq);
            hrtim_cmp_set(0, MSTR, MCMP1R, period / 2);

            /* Initialize timer D, set period for 100kHz frequency
             * and duty cycle to 50% */
            freq = KHZ(100);
            period = hrtim_init_tu(0, 3, &freq);
            hrtim_cmp_set(0, TIMD, CMP1xR, period / 4);

            /* TD1 output set on TIMD period and reset on TIMD CMP1 event*/
            hrtim_set_cb_set(0, TIMD, OUT1, PER);
            hrtim_rst_cb_set(0, TIMD, OUT1, CMP1);

            /* TD2 output set on master timer period and reset on master CMP1 event*/
            hrtim_set_cb_set(0, TIMD, OUT2, MSTPER);
            hrtim_rst_cb_set(0, TIMD, OUT2, MSTCMP1);

            /* Enable TD1 and TD2 outputs */
            hrtim_out_en(0, TIMD, OUT1 | OUT2);

            /* Start Master Timer and Timer D counters*/
            hrtim_cnt_en(0, MCEN | TDCEN);
            break;
        case 4:
            /* Initialize the master timer and TIMD at 100KHz frequency */
            freq = KHZ(100);
            hrtim_init_mstr(0, &freq);
            period = hrtim_init_tu(0, TIMD, &freq);

            /* Set edge timings */
            hrtim_cmp_set(0, TIMD, CMP1xR, period / 4);
            hrtim_cmp_set(0, TIMD, CMP2xR, period * 3 / 8);
            hrtim_cmp_set(0, TIMD, CMP3xR, period / 2);

            /* TD1 toggles on TIMD period, CMP1 and CMP2 event */
            hrtim_set_cb_set(0, TIMD, OUT1, PER | CMP1 | CMP2);
            hrtim_rst_cb_set(0, TIMD, OUT1, PER | CMP1 | CMP2);

            /* TD2 output set on TIMD PER and CMP2 and reset on TIMD CMP1
             * and CMP3 event */
            hrtim_set_cb_set(0, TIMD, OUT2, PER | CMP2);
            hrtim_rst_cb_set(0, TIMD, OUT2, CMP1 | CMP3);

            /* Enable TD1 and TD2 outputs */
            hrtim_out_en(0, TIMD, OUT1 | OUT2);

            /* Start Timer D */
            hrtim_cnt_en(0, TDCEN);
            break;
    }

    return 0;
}


static const shell_command_t shell_commands[] = {
    { "init", "initial full hrtimer configuration for complementary pwm",
        cmd_init },
    { "pwm_set", "set pwm duty cycle, dead time and phase shifting",
        cmd_pwm_set },
    { "init_mstr", "initial master timer configuration", cmd_init_mstr },
    { "init_tu", "initial timing unit configuration", cmd_init_tu },
    { "set_cb_set", "set a set crossbar for an output", cmd_crossbar },
    { "set_cb_unset", "unset a set crossbar for an output", cmd_crossbar },
    { "rst_cb_set", "set a reset crossbar for an output", cmd_crossbar },
    { "rst_cb_unset", "unset a reset crossbar for an output", cmd_crossbar },
    { "cmpl_pwm_out", "setup a complementary pwm output", cmd_cmpl_pwm_out },
    { "period_set", "Set a period value", cmd_period_set },
    { "cmp_set", "set a comparator value", cmd_cmp_set },
    { "cnt_en", "enable a timing unit counter", cmd_cnt_en },
    { "cnt_dis", "disable a timing unit counter", cmd_cnt_dis },
    { "out_en", "enable the given timing unit output(s)", cmd_out_en },
    { "out_dis", "disable the given timing unit output(s)", cmd_out_dis },
    { "pwm_dt", "add a dead-time to a complementary pwm output", cmd_pwm_dt },
    { "an4539", "AN4539 Single PWM generation example", cmd_an4539 },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("HRTIM peripheral driver test\n");
    initiated = 0;

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
