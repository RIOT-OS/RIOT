/*
 * Copyright (C) 2020 ML!PA Consulting GmbH
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
 * @brief       Test application for LIS2DH12 accelerometer driver
 *
 * @author      Jan Mohr <jan.mohr@ml-pa.com>
 *
 * @}
 */

#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "xtimer.h"
#include "fmt.h"
#include "thread.h"
#include "shell.h"

#include "lis2dh12.h"
#include "lis2dh12_params.h"
#include "lis2dh12_registers.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define REFERENCE_DEFAULT 10 /* LSB according to SCALE */

#define THOLD_SHOCK_MILLIG_DEFAULT 1500
#define NUM_DATA_SHOCK_DETECT 7  /* detect shock in NUM last FIFO samples */

/* device specific */
#define NUM_AXES 3
#define NUM_FIFO_VALUES 32

/* axis define for click */
#define X_CLICK 1
#define Y_CLICK 2
#define Z_CLICK 3
#define DCLICK_DEFAULT 40   /* default threshold for double click */

#ifdef MODULE_LIS2DH12_INT
static kernel_pid_t lis2dh12_process;
#endif /* MODULE_LIS2DH12_INT */

int __attribute__((weak)) shell_lis2dh12_cmd(int argc, char** argv);

static const shell_command_t shell_commands[] = {
                { "lis", "Command with multiple subcommands.", shell_lis2dh12_cmd },
                { NULL, NULL, NULL },
};

char lis2dh12_process_stack[THREAD_STACKSIZE_MAIN];

/* setting the double click order */
static LIS2DH12_CLICK_SRC_t click_src_reg;
static lis2dh12_click_t click_cfg = {
    .enable_DOUBLE = true,
    .enable_X_CLICK = true,
    .enable_Y_CLICK = true,
    .enable_Z_CLICK = true,
    .noINT_latency = true,
    .CLICK_thold = DCLICK_DEFAULT,
    .TIME_limit = 4,    /* 4 ODR cycles -> 40ms */
    .TIME_latency = 16, /* 16 ODR cycles -> 160ms */
    .TIME_window = 10,  /* 10 ODR cycles -> 100ms */
};

/* allocate device descriptor */
static lis2dh12_t dev;

#ifdef MODULE_LIS2DH12_INT
/* Interrupt lines */
static uint8_t line1 = 1;
static uint8_t line2 = 2;

/* Interrupt params */
static lis2dh12_int_params_t params_int1 = {0};
static lis2dh12_int_params_t params_int2 = {0};

/* Interrupt source register */
static uint8_t int1_src;
#endif /* MODULE_LIS2DH12_INT */

/* FIFO data memory */
static lis2dh12_fifo_data_t data_fifo[NUM_FIFO_VALUES];
/* FIFO configuration */
static lis2dh12_fifo_t fifo_cfg = {
    .FIFO_set_INT2 = false,
    .FIFO_watermark = 10,
    .FIFO_mode = LIS2DH12_FIFO_MODE_STREAMtoFIFO,
};

/* Memory to print current data */
static char str_out[3][8];

/* current lis acceleration data */
static int16_t data_lis[3];

/* highpass configuration */
lis2dh12_highpass_t highpass_cfg = {
    .Highpass_mode = LIS2DH12_HP_MODE_REFERENCE,
    .Highpass_freq = LIS2DH12_HP_FREQ_DIV100,
    .CLICK_enable = false,
    .INT1_enable = false,
    .INT2_enable = false,
    .DATA_OUT_enable = false,
};

/* reference data */
static uint8_t reference_value;

/* shock threshold */
static int16_t shock_thold;

#ifdef MODULE_LIS2DH12_INT
/* previous values */
static int16_t old_data_lis[3];
static uint8_t int1_src_old;

/* lis2dh12 interrupt callback function. */
static void lis2dh12_int_cb(void* l) {

    /* disable IRQ until lis_process is done */
    gpio_irq_disable(dev.p->int1_pin);
    gpio_irq_disable(dev.p->int2_pin);

    /* reset click source */
    lis2dh12_read_click_src(&dev, &click_src_reg);
    DEBUG("[INT]: CLICK_SRC 0x%x\n", click_src_reg.reg);

    uint8_t line = *(uint8_t*)l;
    printf("Info: INT_line: %d\n", line);

    lis2dh12_read_reference(&dev, &reference_value);
    DEBUG("[INT]: REF: 0x%x\n", reference_value);

    lis2dh12_read_int_src(&dev, &int1_src, 1);
    DEBUG("[INT]: INT_SRC 0x%x\n", int1_src);
    DEBUG("[INT]: INT_SRC - IA %d; ZH %d; ZL %d; YH %d; YL %d; XH %d; XL %d.\n",
            int1_src & LIS2DH12_INT_SRC_IA,
            int1_src & LIS2DH12_INT_SRC_ZH, int1_src & LIS2DH12_INT_SRC_ZL,
            int1_src & LIS2DH12_INT_SRC_YH, int1_src & LIS2DH12_INT_SRC_YL,
            int1_src & LIS2DH12_INT_SRC_XH, int1_src & LIS2DH12_INT_SRC_XL);

    thread_wakeup(lis2dh12_process);
}
#endif /* MODULE_LIS2DH12_INT */

void lis2dh12_test_init(void) {

    if (IS_USED(MODULE_LIS2DH12_SPI)) {
        puts("using SPI mode, for I2C mode select the lis2dh12_i2c module");
    } else {
        puts("using I2C mode, for SPI mode select the lis2dh12_spi module");
    }

    /* init lis */
    if (lis2dh12_init(&dev, &lis2dh12_params[0]) == LIS2DH12_OK) {
        puts("lis2dh12 [Initialized]");
    }
    else {
        puts("lis2dh12 [Failed]");
    }

    /* change LIS settings */
    lis2dh12_set_powermode(&dev, LIS2DH12_POWER_LOW);
    lis2dh12_set_datarate(&dev, LIS2DH12_RATE_100HZ);
    lis2dh12_set_scale(&dev, LIS2DH12_SCALE_4G);

#ifdef MODULE_LIS2DH12_INT
    /* set interrupt pins */
    gpio_t pin1 = dev.p->int1_pin;
    gpio_t pin2 = dev.p->int2_pin;

    /* set Interrupt params */
    if (gpio_is_valid(pin1)) {
        /* enables interrupt on all axes above the threshold value */
        params_int1.int_config = LIS2DH12_INT_CFG_XHIE
                               | LIS2DH12_INT_CFG_YHIE
                               | LIS2DH12_INT_CFG_ZHIE;
        params_int1.int_duration = 1;
        params_int1.cb = lis2dh12_int_cb;
        params_int1.arg = &line1;
    }
    if (gpio_is_valid(pin2)) {
        /* enables interrupt on Y-axis below the threshold value */
        params_int2.int_config = LIS2DH12_INT_CFG_YLIE;
        params_int2.int_duration = 1;
        params_int2.cb = lis2dh12_int_cb;
        params_int2.arg = &line2;
    }

    if (gpio_init_int(pin1, GPIO_IN, GPIO_RISING, lis2dh12_int_cb, &line1)) {
        DEBUG("[lis_init]: INT1 failed\n");
    }
    else {
        DEBUG("[lis_init]: INT1 done\n");
    }

    if (gpio_init_int(pin2, GPIO_IN, GPIO_RISING, lis2dh12_int_cb, &line2)) {
        DEBUG("[lis_init]: INT2 failed\n");
    }
    else {
        DEBUG("[lis_init]: INT2 done\n");
    }
#endif /* MODULE_LIS2DH12_INT */

    /* enable FIFO */
    lis2dh12_set_fifo(&dev, &fifo_cfg);

    /* enable click detection */
    lis2dh12_set_click(&dev, &click_cfg);

    /* set default shock value */
    shock_thold = THOLD_SHOCK_MILLIG_DEFAULT;

    /* read registers to reset device */
    lis2dh12_read_click_src(&dev, &click_src_reg);
    lis2dh12_read_reference(&dev, &reference_value);
#ifdef MODULE_LIS2DH12_INT
    lis2dh12_read_int_src(&dev, &int1_src, 1);
#endif /* MODULE_LIS2DH12_INT */
}

#ifdef MODULE_LIS2DH12_INT
void* lis2dh12_test_process(void* arg) {
    (void) arg;
    while (1) {
        /* start processing */
        DEBUG("[Process]: start process\n");

        /* read FIFO_src before getting data */
        LIS2DH12_FIFO_SRC_REG_t fifo_src;
        lis2dh12_read_fifo_src(&dev, &fifo_src);
        DEBUG("[Process]: FIFO SRC 0x%x\n", fifo_src.reg);
        DEBUG("[Process]: WTM %x, OVRN %d, EMPTY %d, FSS %d\n", fifo_src.bit.WTM,
                                fifo_src.bit.OVRN_FIFO, fifo_src.bit.EMPTY, fifo_src.bit.FSS);

        /* get fifo data */
        uint8_t number_read = lis2dh12_read_fifo_data(&dev, data_fifo, NUM_FIFO_VALUES);

        /* read FIFO_src after getting data */
        lis2dh12_read_fifo_src(&dev, &fifo_src);
        DEBUG("[Process]: FIFO SRC 0x%x\n", fifo_src.reg);
        DEBUG("[Process]: WTM %x, OVRN %d, EMPTY %d, FSS %d\n", fifo_src.bit.WTM,
                                fifo_src.bit.OVRN_FIFO, fifo_src.bit.EMPTY, fifo_src.bit.FSS);

        /* display FIFO data */
        if (ENABLE_DEBUG) {
            for (int i = 0; i < number_read; i++){
                printf("[Process]: X[%2d]  %d\n", i, data_fifo[i].X_AXIS);
                printf("[Process]: Y[%2d]  %d\n", i, data_fifo[i].Y_AXIS);
                printf("[Process]: Z[%2d]  %d\n", i, data_fifo[i].Z_AXIS);
            }
        }

        /* After the Interrupt the FIFO needs to be enabled again. */
        lis2dh12_restart_fifo(&dev);

        /* check if shock occurred*/
        uint16_t max_data_X = 0;
        uint16_t max_data_Y = 0;
        uint16_t max_data_Z = 0;

        bool X_shock_pos = false;
        bool Y_shock_pos = false;
        bool Z_shock_pos = false;

        for (uint8_t entry = NUM_FIFO_VALUES - NUM_DATA_SHOCK_DETECT; entry < NUM_FIFO_VALUES;
                entry++) {
            uint16_t abs_X = data_fifo[entry].X_AXIS >= 0 ? data_fifo[entry].X_AXIS :
                                                            -1*data_fifo[entry].X_AXIS;
            uint16_t abs_Y = data_fifo[entry].Y_AXIS >= 0 ? data_fifo[entry].Y_AXIS :
                                                            -1*data_fifo[entry].Y_AXIS;
            uint16_t abs_Z = data_fifo[entry].Z_AXIS >= 0 ? data_fifo[entry].Z_AXIS :
                                                            -1*data_fifo[entry].Z_AXIS;

            /* check X shock direction */
            if (max_data_X <= abs_X) {
                max_data_X = abs_X;
                X_shock_pos = (data_fifo[entry].X_AXIS >= 0);
            }
            /* check Y shock direction */
            if (max_data_Y <= abs_Y) {
                max_data_Y = abs_Y;
                Y_shock_pos = (data_fifo[entry].Y_AXIS >= 0);
            }
            /* check Z shock direction */
            if (max_data_Z <= abs_Z) {
                max_data_Z = abs_Z;
                Z_shock_pos = (data_fifo[entry].Z_AXIS >= 0);
            }
        }

        DEBUG("[Process]: oldX %d, oldY %d, oldZ %d\n", old_data_lis[0], old_data_lis[1],
                    old_data_lis[2]);
        DEBUG("[Process]: maxX %d, maxY %d, maxZ %d\n", max_data_X, max_data_Y, max_data_Z);

        /* X shock */
        int16_t diff_value = max_data_X - old_data_lis[0];
        if (diff_value >= shock_thold) {
            if (X_shock_pos) {
                puts("positive X shock detected.");
            }
            else {
                puts("negative X shock detected.");
            }
        }
        /* Y shock */
        diff_value = max_data_Y - old_data_lis[1];
        if (diff_value >= shock_thold) {
            if (Y_shock_pos) {
                puts("positive Y shock detected.");
            }
            else {
                puts("negative Y shock detected.");
            }
        }
        /* Z shock */
        diff_value = max_data_Z - old_data_lis[2];
        if (diff_value >= shock_thold) {
            if (Z_shock_pos) {
                puts("positive Z shock detected.");
            }
            else {
                puts("negative Z shock detected.");
            }
        }

        /* check for roll */
        /* roll conditions
         *
         * only 180°, changes can be detected with 6D reg in INT1_SRC
         * change in ZH and ZL -> X-roll (device flipped from top to bottom)
         * change in YH and YL -> Z-roll
         * change in XH and XL -> Y-roll
         */
        DEBUG("[Process]: OLD - IA %d; ZH %d; ZL %d; YH %d; YL %d; XH %d; XL %d.\n",
                    int1_src_old & LIS2DH12_INT_SRC_IA,
                    int1_src_old & LIS2DH12_INT_SRC_ZH, int1_src_old & LIS2DH12_INT_SRC_ZL,
                    int1_src_old & LIS2DH12_INT_SRC_YH, int1_src_old & LIS2DH12_INT_SRC_YL,
                    int1_src_old & LIS2DH12_INT_SRC_XH, int1_src_old & LIS2DH12_INT_SRC_XL);

        DEBUG("[Process]: NEW - IA %d; ZH %d; ZL %d; YH %d; YL %d; XH %d; XL %d.\n",
                    int1_src & LIS2DH12_INT_SRC_IA,
                    int1_src & LIS2DH12_INT_SRC_ZH, int1_src & LIS2DH12_INT_SRC_ZL,
                    int1_src & LIS2DH12_INT_SRC_YH, int1_src & LIS2DH12_INT_SRC_YL,
                    int1_src & LIS2DH12_INT_SRC_XH, int1_src & LIS2DH12_INT_SRC_XL);

        if (((int1_src_old & LIS2DH12_INT_SRC_ZH) != (int1_src & LIS2DH12_INT_SRC_ZH))
                && ((int1_src_old & LIS2DH12_INT_SRC_ZL) != (int1_src & LIS2DH12_INT_SRC_ZL))) {
            printf("X roll detected.\n");
        }
        if (((int1_src_old & LIS2DH12_INT_SRC_XH) != (int1_src & LIS2DH12_INT_SRC_XH))
                && ((int1_src_old & LIS2DH12_INT_SRC_YL) != (int1_src & LIS2DH12_INT_SRC_YL))) {
            printf("Z roll detected.\n");
        }
        if (((int1_src_old & LIS2DH12_INT_SRC_YH) != (int1_src & LIS2DH12_INT_SRC_YH))
                && ((int1_src_old & LIS2DH12_INT_SRC_XL) != (int1_src & LIS2DH12_INT_SRC_XL))) {
            printf("Y roll detected.\n");
        }

        int1_src_old = int1_src;

        /* check click order */
        /*
         * idea is to do a sequence of double clicks, to enable the device
         *
         */
        /* click_src read during interrupt callback */
        DEBUG("[Process]: clickSRC 0x%x\n", click_src_reg.reg);

        if (click_src_reg.bit.IA && click_src_reg.bit.DClick) {
            /* X-Double */
            if (click_src_reg.bit.X_AXIS && !click_src_reg.bit.Y_AXIS
                && !click_src_reg.bit.Z_AXIS) {
                int8_t sign = click_src_reg.bit.Sign ? -1 : 1;
                printf("got X-DCLICK, sign %d\n", sign);
            }
            /* Y-Double */
            if (!click_src_reg.bit.X_AXIS && click_src_reg.bit.Y_AXIS
                && !click_src_reg.bit.Z_AXIS) {
                int8_t sign = click_src_reg.bit.Sign ? -1 : 1;
                printf("got Y-DCLICK, sign %d\n", sign);
            }
            /* Z-Double */
            if (!click_src_reg.bit.X_AXIS && !click_src_reg.bit.Y_AXIS
                && click_src_reg.bit.Z_AXIS) {
                int8_t sign = click_src_reg.bit.Sign ? -1 : 1;
                printf("got Z-DCLICK, sign %d\n", sign);
            }
        }

        /* enable IRQ again */
        gpio_irq_enable(dev.p->int1_pin);
        gpio_irq_enable(dev.p->int2_pin);

        /* thread will sleep until next wokeup_lis */
        thread_sleep();
    }

    return NULL;
}
#endif /* MODULE_LIS2DH12_INT */

int shell_lis2dh12_cmd(int argc, char **argv) {

    printf("Command: lis %s %s\n", (argc > 1) ? argv[1] : "",
           (argc > 2) ? argv[2] : "");
#ifdef MODULE_LIS2DH12_INT
    const char * usage = "USAGE: lis <subcommand> [arg], with subcommand "
                         "in (enable, disable, read, read_fifo, clear_data, "
                         "set-click, set-thold-shock, set-thold-inter, "
                         "set-highpass, change_rate, change_power, change_scale).";
#else
    const char * usage = "USAGE: lis <subcommand> [arg], with subcommand "
                         "in (enable, disable, read, read_fifo, clear_data, "
                         "change_rate, change_power, change_scale).";
#endif /* MODULE_LIS2DH12_INT */

    /* MISSING command */
    if (argc < 2) {
        printf("Error: Missing sub-command. %s\n", usage);
        return -1;
    }

    /* enable disable device */
    else if (strncmp(argv[1], "enable", sizeof("enable")) == 0) {
        if (lis2dh12_poweron(&dev) != LIS2DH12_OK) {
            puts("unable to poweron device.");
        }
        return 1;
    }
    else if (strncmp(argv[1], "disable", sizeof("disable")) == 0) {
        if (lis2dh12_poweroff(&dev) != LIS2DH12_OK) {
            puts("unable to poweroff device.");
            return -1;
        }
        return 1;
    }

    /* read acceleration data */
    else if (strncmp(argv[1], "read", sizeof("read")) == 0) {
        uint8_t amount = (argc < 3) ? 1 : atoi(argv[2]);
        uint8_t amt = 0;

        /* read sensor data */
        for (amt = 0; amt < amount; amt++){
            if (lis2dh12_read(&dev, data_lis) != LIS2DH12_OK) {
                puts("error: no data from sensor");
                return -1;
            }
            /* format data */
            for (int i = 0; i < 3; i++) {
                size_t len = fmt_s16_dfp(str_out[i], data_lis[i], -3);
                str_out[i][len] = '\0';
            }

            /* print data to STDIO */
            printf("X: %6s  Y: %6s  Z: %6s\n", str_out[0], str_out[1], str_out[2]);

            xtimer_msleep(250);
        }
        return 1;
    }
    else if (strncmp(argv[1], "read_fifo", sizeof("read_fifo")) == 0) {
        uint8_t number = 0;
        if ((argc < 3) || (number = atoi(argv[2])) > 32) {
            puts("Error: Missing parameter.");
            puts("The command should contain number of FIFO values to read (max. 32)).");
            puts("USAGE: lis read_fifo [number]");
            return -1;
        }

        /* read raw data from FIFO */
        uint8_t number_read = lis2dh12_read_fifo_data(&dev, data_fifo, number);

        DEBUG("[lis_command]: fifo_read %d elements.\n", number_read);

        /* print data */
        for (int entry = 0; entry < number_read; entry++){

            /* format data */
            size_t len = fmt_s16_dfp(str_out[0], data_fifo[entry].X_AXIS, -3);
            str_out[0][len] = '\0';
            len = fmt_s16_dfp(str_out[1], data_fifo[entry].Y_AXIS, -3);
            str_out[1][len] = '\0';
            len = fmt_s16_dfp(str_out[2], data_fifo[entry].Z_AXIS, -3);
            str_out[2][len] = '\0';

            printf("[%2d] X: %6s  Y: %6s  Z: %6s\n", entry, str_out[0], str_out[1], str_out[2]);
        }
        return 1;
    }

    /* clear memory */
    else if (strncmp(argv[1], "clear_data", sizeof("clear_data")) == 0) {

        lis2dh12_clear_data(&dev);
        return 1;
    }

#ifdef MODULE_LIS2DH12_INT
    /* set commands */
    else if (strncmp(argv[1], "set-click", sizeof("set-click")) == 0) {
        uint8_t thold = 0;
        if ((argc < 3) || (thold = atoi(argv[2])) > 127) {
            puts("Error: Missing parameter.");
            puts("The command should contain a threshold value below 128. "
                 "The LSB changes according to selected SCALE "
                 "(@2G LSB=16mg; @4G LSB=32mg; @8G LSB=62mg: @16G LSB=186mg).");
            puts("USAGE: lis set-click [thold]");
            return -1;
        }

        click_cfg.CLICK_thold = thold;
        lis2dh12_set_click(&dev, &click_cfg);

        /* enable click interrupt */
        params_int1.int_type = LIS2DH12_INT_TYPE_I1_CLICK;
        lis2dh12_set_int(&dev, &params_int1, LIS2DH12_INT1);

        return 1;
    }
    else if (strncmp(argv[1], "set-thold-shock", sizeof("set-thold-shock")) == 0) {
        uint16_t thold = 0;
        if ((argc < 3) || !(thold = atoi(argv[2]))) {
            puts("Error: Missing parameter.");
            puts("The command should contain a threshold value in [mg] below the max SCALE. "
                 "(@2G below 2000; @4G below 4000; and so on)");
            puts("USAGE: lis set-thold-shock [thold]");
            return -1;
        }
        shock_thold = thold;
        return 1;
    }
    else if (strncmp(argv[1], "set-thold-inter", sizeof("set-thold-inter")) == 0) {
        uint8_t line = 0;
        if ((argc < 4) || (line = atoi(argv[3])) > 2) {
            puts("Error: Missing parameter.");
            puts("The command should contain threshold value and interrupt line (1 or 2). "
                 "The threshold LSB changes according to selected SCALE "
                 "(@2G LSB=16mg; @4G LSB=32mg; @8G LSB=62mg: @16G LSB=186mg).");
            puts("To disable interrupt set thold to 0.");
            puts("USAGE: lis set-thold-inter [thold] [line]");
            return -1;
        }
        uint8_t thold = atoi(argv[2]);
        if (line == 1) {
            if (!thold) {
                params_int1.int_config = 0;
            }
            else {
                /* enables all axes for acceleration above threshold */
                params_int1.int_config = LIS2DH12_INT_CFG_XHIE
                                       | LIS2DH12_INT_CFG_YHIE
                                       | LIS2DH12_INT_CFG_ZHIE;
            }
            params_int1.int_type = LIS2DH12_INT_TYPE_I1_IA1;
            params_int1.int_threshold = thold;

            lis2dh12_set_int(&dev, &params_int1, LIS2DH12_INT1);
        }
        else if (line == 2){
            if (!thold) {
                params_int2.int_config = 0;
            }
            else {
                /* enables Y-axis for acceleration under the threshold */
                params_int2.int_config = LIS2DH12_INT_CFG_YLIE;
            }
            params_int2.int_type = LIS2DH12_INT_TYPE_I2_IA2;
            params_int2.int_threshold = thold;
            lis2dh12_set_int(&dev, &params_int2, LIS2DH12_INT2);
        }
        if (thold) {
            printf("Info: Interrupt thold = %d on line %d.\n", thold, line);
        }
        else {
            printf("Info: Interrupt disabled.\n");
        }

        return 1;
    }

    else if (strncmp(argv[1], "set-highpass", sizeof("set-highpass")) == 0) {
        uint8_t out = 0;
        uint8_t reference = atoi(argv[2]);
        if ((argc < 4) || (out = atoi(argv[3])) > 3) {
            puts("Error: Missing parameter.");
            puts("The command should contains the number of an output which gets filtered "
                 "and the reference value less than 255. "
                 "Possible outputs are IA1 (1) or IA2 (2) or CLICK (3) "
                 "or (0) to disable the filter.");
            puts("USAGE: lis set-highpass [reference] [out_number]");
            return -1;
        }

        if (out) {
            /* enable filter for output */
            highpass_cfg.DATA_OUT_enable = 0;

            /* enable HP for interrupt */
            if (out == 1) {
                highpass_cfg.INT1_enable = 1;
            }
            else if (out == 2) {
                highpass_cfg.INT2_enable = 1;
            }
            else {
                /* enable filter for click function */
                highpass_cfg.CLICK_enable = 1;
            }
            printf("Info: Filter set to %d on output %d.\n", reference, out);
        }
        else {
            printf("Info: Filter disabled.\n");
        }

        lis2dh12_set_reference(&dev, reference);
        lis2dh12_set_highpass(&dev, &highpass_cfg);
        return 1;
    }
#endif /* MODULE_LIS2DH12_INT */

    /* change sampling rate */
    else if (strncmp(argv[1], "change_rate", sizeof("change_rate")) == 0) {
        uint8_t rate = 0;
        if ((argc < 3) || (rate = atoi(argv[2])) > 9) {
            puts("Error: Missing parameter.");
            puts("The command should contain a number for sampling rate. "
                 "Possible outputs are 1Hz (1), 10Hz (2), 25Hz (3), "
                 "50Hz (4), 100Hz (5), 200Hz (6) or 400Hz (7).");
            puts("USAGE: lis change_rate [samplingrate]");
            return -1;
        }

        lis2dh12_set_datarate(&dev, rate);
        return 1;
    }

    /* change power mode */
    else if (strncmp(argv[1], "change_power", sizeof("change_power")) == 0) {
        uint8_t power = 0;
        if ((argc < 3) || (power = atoi(argv[2])) > 9) {
            puts("Error: Missing parameter.");
            puts("The command should contain a number for power mode. "
                 "Possible outputs are POWER_DOWN (0), POWER_LOW (1), "
                 "POWER_NORMAL (2) or POWER_HIGH (3).");
            puts("USAGE: lis change_power [powermode]");
            return -1;
        }

        lis2dh12_set_powermode(&dev, power);
        return 1;
    }

    /* change scale value */
    else if (strncmp(argv[1], "change_scale", sizeof("change_scale")) == 0) {
        uint8_t scale = 0;
        if ((argc < 3) || (scale = atoi(argv[2])) > 3) {
            puts("Error: Missing parameter.");
            puts("The command should contain a number for scale value. "
                 "Possible values are SCALE_2G (0), SCALE_4G (1), "
                 "SCALE_8G (2) or SCALE_16G (3).");
            puts("USAGE: lis change_scale [scale]");
            return -1;
        }

        lis2dh12_set_scale(&dev, scale<<4);
        return 1;
    }

    /* UNKNOWN */
    else {
        printf("Error: Unknown sub-command. %s\n", usage);
        return -1;
    }
}

int main(void)
{

#ifdef MODULE_LIS2DH12_INT
    /* processing lis2dh12 acceleration data */
    lis2dh12_process = thread_create(lis2dh12_process_stack, sizeof(lis2dh12_process_stack),
                  THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_SLEEPING,
                  lis2dh12_test_process, NULL, "lis2dh12_process");
#endif /* MODULE_LIS2DH12_INT */

    /* init lis */
    lis2dh12_test_init();

    /* running shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
