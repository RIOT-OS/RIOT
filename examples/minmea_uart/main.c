/*
 * Copyright (C) 2015 Freie Universität Berlin
 * Copyright (C) 2021 Université Grenoble Alpes
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
 * @brief       Test application for reading and parse NMEA sentences for a GNSS module connected to one UART

 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Didier Donsez <didier.donsez@univ-grenoble-alpes.fr> (for parse_nmea())
 *
 * @}
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "board.h"
#include "shell.h"
#include "thread.h"
#include "msg.h"
#include "ringbuffer.h"
#include "periph/uart.h"
#include "xtimer.h"

#ifdef MODULE_STDIO_UART
#include "stdio_uart.h"
#endif

#ifndef SHELL_BUFSIZE
#define SHELL_BUFSIZE       (128U)
#endif
#ifndef UART_BUFSIZE
#define UART_BUFSIZE        (128U)
#endif

#define PRINTER_PRIO        (THREAD_PRIORITY_MAIN - 1)
#define PRINTER_TYPE        (0xabcd)

#define POWEROFF_DELAY      (250U * US_PER_MS)      /* quarter of a second */

#ifndef STDIO_UART_DEV
#define STDIO_UART_DEV      (UART_UNDEF)
#endif

#ifndef STX
#define STX 0x2
#endif

typedef struct {
    char rx_mem[UART_BUFSIZE];
    ringbuffer_t rx_buf;
} uart_ctx_t;

static uart_ctx_t ctx[UART_NUMOF];

static kernel_pid_t printer_pid;
static char printer_stack[THREAD_STACKSIZE_MAIN];

static bool test_mode;

#ifdef MODULE_PERIPH_UART_MODECFG
static uart_data_bits_t data_bits_lut[] = { UART_DATA_BITS_5, UART_DATA_BITS_6,
                                            UART_DATA_BITS_7, UART_DATA_BITS_8 };
static int data_bits_lut_len = ARRAY_SIZE(data_bits_lut);

static uart_stop_bits_t stop_bits_lut[] = { UART_STOP_BITS_1, UART_STOP_BITS_2 };
static int stop_bits_lut_len = ARRAY_SIZE(stop_bits_lut);
#endif

static int parse_dev(char *arg)
{
    unsigned dev = atoi(arg);

    if (dev >= UART_NUMOF) {
        printf("Error: Invalid UART_DEV device specified (%u).\n", dev);
        return -1;
    }
    else if (UART_DEV(dev) == STDIO_UART_DEV) {
        printf("Error: The selected UART_DEV(%u) is used for the shell!\n", dev);
        return -2;
    }
    return dev;
}

#ifdef MODULE_PERIPH_UART_RXSTART_IRQ
static void rxs_cb(void *arg)
{
    ringbuffer_add_one(arg, STX);
}
#endif



#include "fmt.h"
#include "minmea.h"

static void parse_nmea_gll(const char *sentence)
{
    struct minmea_sentence_gll frame;
    int res = minmea_parse_gll(&frame, sentence);

    if (!res) {
        print_str("FAILURE: error parsing GPS GLL sentence\n");
    }
    else {
        print_str("parsed coordinates: lat=");
        print_float(minmea_tocoord(&frame.latitude), 6);
        print_str(" lon=");
        print_float(minmea_tocoord(&frame.longitude), 6);
        print_str("\nSUCCESS\n");
    }
}


static void parse_nmea_zda(const char *sentence)
{
    struct minmea_sentence_zda frame;
    int res = minmea_parse_zda(&frame, sentence);

    if (!res) {
        print_str("FAILURE: error parsing GPS ZDA sentence\n");
    }
    else {
        printf("parsed UTC and local date/time data");
        printf(" date=%d-%d-%d %d:%d:%d.%d",
               frame.date.year,
               frame.date.month,
               frame.date.day,
               frame.time.hours,
               frame.time.minutes,
               frame.time.seconds,
               frame.time.microseconds
               );
        printf(" offset=%d:%d",
               frame.hour_offset,
               frame.minute_offset
               );
        print_str("\nSUCCESS\n");
    }
}

static void parse_nmea_rmc(const char *sentence)
{
    struct minmea_sentence_rmc frame;
    int res = minmea_parse_rmc(&frame, sentence);

    if (!res) {
        print_str("FAILURE: error parsing GPS RMC sentence\n");
    }
    else {
        printf("parsed Position and time: valid=%d",
               frame.valid
               );

        printf(" date=%d-%d-%d %d:%d:%d.%d",
               frame.date.year,
               frame.date.month,
               frame.date.day,
               frame.time.hours,
               frame.time.minutes,
               frame.time.seconds,
               frame.time.microseconds
               );
        printf("\n");

        print_str(" lat=");
        print_float(minmea_tocoord(&frame.latitude), 6);
        print_str(" lng=");
        print_float(minmea_tocoord(&frame.longitude), 6);
        print_str(" speed=");
        print_float(minmea_tocoord(&frame.speed), 6);
        print_str(" course=");
        print_float(minmea_tocoord(&frame.course), 6);
        print_str(" variation=");
        print_float(minmea_tocoord(&frame.variation), 6);

        print_str("\nSUCCESS\n");
    }
}


#define NMEA_BUFFER_SIZE 256
static uint8_t nema_buffer[NMEA_BUFFER_SIZE];
static int nema_buffer_idx = 0;

static void parse_nmea(uint8_t data)
{
    if (data == '\n') {

        // Determine sentence identifier.
        enum minmea_sentence_id s = minmea_sentence_id((const char *)nema_buffer, true);
        switch (s) {
        case MINMEA_SENTENCE_GLL: // $GPGLL Sentence (Position)
            parse_nmea_gll((const char *)nema_buffer);
            break;
        case MINMEA_SENTENCE_ZDA: // $GPZDA Sentence (UTC and local date/time data)
            parse_nmea_zda((const char *)nema_buffer);
            break;
        case MINMEA_SENTENCE_RMC: // $GPRMC Sentence (Position and time)
            parse_nmea_rmc((const char *)nema_buffer);
            break;
        case MINMEA_SENTENCE_GGA:   // $GPGGA Sentence (Fix data)
        case MINMEA_SENTENCE_GSA:   // $GPGSA Sentence (Active satellites)
        case MINMEA_SENTENCE_GST:   // $GPGST Sentence (pseudorange noise statistics)
        case MINMEA_SENTENCE_GSV:   // $GPGSV Sentence (Satellites in view)
        case MINMEA_SENTENCE_VTG:   // $GPVTG Sentence (Course over ground)
        {
            printf("TODO: parse NMEA sentence %d.\n", s);
        }
        break;
        case MINMEA_INVALID:
        case MINMEA_UNKNOWN:
            break;
        default:
            break;
        }
        nema_buffer_idx = 0;
    }
    else if (data == '\r') {
        // Skip
    }
    else {
        if (nema_buffer_idx < NMEA_BUFFER_SIZE) {
            nema_buffer[nema_buffer_idx] = data;
            nema_buffer_idx++;
            nema_buffer[nema_buffer_idx] = '\0';
        }
    }
}


static void rx_cb(void *arg, uint8_t data)
{
    uart_t dev = (uart_t)arg;

    ringbuffer_add_one(&ctx[dev].rx_buf, data);
    // Add the char for parsing the potential NMEA sentence
    parse_nmea(data);
    if (!test_mode && data == '\n') {
        msg_t msg;
        msg.content.value = (uint32_t)dev;
        msg_send(&msg, printer_pid);
    }
}

static int _self_test(uart_t dev, unsigned baud)
{
    const char test_string[] = "Hello UART!";

    if (uart_init(UART_DEV(dev), baud, rx_cb, (void *)dev)) {
        printf("error configuring %u baud\n", baud);
        return -1;
    }

    test_mode = true;

    uart_write(dev, (uint8_t *)test_string, sizeof(test_string));
    for (unsigned i = 0; i < sizeof(test_string); ++i) {
        int c = ringbuffer_get_one(&ctx[dev].rx_buf);
        if (c != test_string[i]) {
            printf("mismatch at index %u: %x != %x\n", i, c, test_string[i]);
            return -1;
        }
    }

#ifdef MODULE_PERIPH_UART_RXSTART_IRQ
    /* test RX Start detection if available */
    uart_rxstart_irq_configure(dev, rxs_cb, &ctx[dev].rx_buf);
    uart_rxstart_irq_enable(dev);

    uart_write(dev, (uint8_t *)test_string, sizeof(test_string));
    for (unsigned i = 0; i < sizeof(test_string); ++i) {
        int c = ringbuffer_get_one(&ctx[dev].rx_buf);
        if (c != STX) {
            printf("expected start condition, got %x\n", c);
            return -1;
        }

        c = ringbuffer_get_one(&ctx[dev].rx_buf);
        if (c != test_string[i]) {
            printf("mismatch at index %u: %x != %x, start condition reported\n",
                   i, c, test_string[i]);
            return -1;
        }
    }
    uart_rxstart_irq_disable(dev);
#endif

    test_mode = false;
    return 0;
}

static void *printer(void *arg)
{
    (void)arg;
    msg_t msg;
    msg_t msg_queue[8];

    msg_init_queue(msg_queue, 8);

    while (1) {
        msg_receive(&msg);
        uart_t dev = (uart_t)msg.content.value;
        char c;

        printf("Success: UART_DEV(%i) RX: [", dev);
        do {
            c = (int)ringbuffer_get_one(&(ctx[dev].rx_buf));
            if (c == '\n') {
                puts("]\\n");
            }
            else if (c >= ' ' && c <= '~') {
                printf("%c", c);
            }
            else {
                printf("0x%02x", (unsigned char)c);
            }
        } while (c != '\n');
    }

    /* this should never be reached */
    return NULL;
}

static void sleep_test(int num, uart_t uart)
{
    printf("UARD_DEV(%i): test uart_poweron() and uart_poweroff()  ->  ", num);
    uart_poweroff(uart);
    xtimer_usleep(POWEROFF_DELAY);
    uart_poweron(uart);
    puts("[OK]");
}

static int cmd_init(int argc, char **argv)
{
    int dev, res;
    uint32_t baud;

    if (argc < 3) {
        printf("usage: %s <dev> <baudrate>\n", argv[0]);
        return 1;
    }
    /* parse parameters */
    dev = parse_dev(argv[1]);
    if (dev < 0) {
        return 1;
    }
    baud = strtol(argv[2], NULL, 0);

    /* initialize UART */
    res = uart_init(UART_DEV(dev), baud, rx_cb, (void *)dev);
    if (res == UART_NOBAUD) {
        printf("Error: Given baudrate (%u) not possible\n", (unsigned int)baud);
        return 1;
    }
    else if (res != UART_OK) {
        puts("Error: Unable to initialize UART device");
        return 1;
    }
    printf("Success: Initialized UART_DEV(%i) at BAUD %" PRIu32 "\n", dev, baud);

    /* also test if poweron() and poweroff() work (or at least don't break
     * anything) */
    sleep_test(dev, UART_DEV(dev));

    return 0;
}

#ifdef MODULE_PERIPH_UART_MODECFG
static int cmd_mode(int argc, char **argv)
{
    int dev, data_bits_arg, stop_bits_arg;
    uart_data_bits_t data_bits;
    uart_parity_t parity;
    uart_stop_bits_t stop_bits;

    if (argc < 5) {
        printf("usage: %s <dev> <data bits> <parity> <stop bits>\n", argv[0]);
        return 1;
    }

    dev = parse_dev(argv[1]);
    if (dev < 0) {
        return 1;
    }

    data_bits_arg = atoi(argv[2]) - 5;
    if (data_bits_arg >= 0 && data_bits_arg < data_bits_lut_len) {
        data_bits = data_bits_lut[data_bits_arg];
    }
    else {
        printf("Error: Invalid number of data_bits (%i).\n", data_bits_arg + 5);
        return 1;
    }

    argv[3][0] &= ~0x20;
    switch (argv[3][0]) {
    case 'N':
        parity = UART_PARITY_NONE;
        break;
    case 'E':
        parity = UART_PARITY_EVEN;
        break;
    case 'O':
        parity = UART_PARITY_ODD;
        break;
    case 'M':
        parity = UART_PARITY_MARK;
        break;
    case 'S':
        parity = UART_PARITY_SPACE;
        break;
    default:
        printf("Error: Invalid parity (%c).\n", argv[3][0]);
        return 1;
    }

    stop_bits_arg = atoi(argv[4]) - 1;
    if (stop_bits_arg >= 0 && stop_bits_arg < stop_bits_lut_len) {
        stop_bits = stop_bits_lut[stop_bits_arg];
    }
    else {
        printf("Error: Invalid number of stop bits (%i).\n", stop_bits_arg + 1);
        return 1;
    }

    if (uart_mode(UART_DEV(dev), data_bits, parity, stop_bits) != UART_OK) {
        puts("Error: Unable to apply UART settings");
        return 1;
    }
    printf("Success: Successfully applied UART_DEV(%i) settings\n", dev);

    return 0;
}
#endif /* MODULE_PERIPH_UART_MODECFG */

static int cmd_send(int argc, char **argv)
{
    int dev;
    uint8_t endline = (uint8_t)'\n';

    if (argc < 3) {
        printf("usage: %s <dev> <data (string)>\n", argv[0]);
        return 1;
    }
    /* parse parameters */
    dev = parse_dev(argv[1]);
    if (dev < 0) {
        return 1;
    }

    printf("UART_DEV(%i) TX: %s\n", dev, argv[2]);
    uart_write(UART_DEV(dev), (uint8_t *)argv[2], strlen(argv[2]));
    uart_write(UART_DEV(dev), &endline, 1);
    return 0;
}

static int cmd_test(int argc, char **argv)
{
    int dev;

    if (argc < 2) {
        printf("usage: %s <dev>\n", argv[0]);
        return 1;
    }
    /* parse parameters */
    dev = parse_dev(argv[1]);
    if (dev < 0) {
        return 1;
    }

    puts("[START]");

    /* run self test with different baud rates */
    for (unsigned i = 1; i <= 12; ++i) {
        if (_self_test(dev, 9600 * i)) {
            puts("[FAILURE]");
            return -1;
        }
    }

    puts("[SUCCESS]");
    return 0;
}

static const shell_command_t shell_commands[] = {
    { "init", "Initialize a UART device with a given baudrate", cmd_init },
#ifdef MODULE_PERIPH_UART_MODECFG
    { "mode", "Setup data bits, stop bits and parity for a given UART device", cmd_mode },
#endif
    { "send", "Send a string through given UART device", cmd_send },
    { "test", "Run an automated test on a UART with RX and TX connected", cmd_test },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("\nReading and parse NMEA sentences for a GNSS module connected to one UART");

    /* do sleep test for UART used as STDIO. There is a possibility that the
     * value given in STDIO_UART_DEV is not a numeral (depends on the CPU
     * implementation), so we rather break the output by printing a
     * non-numerical value instead of breaking the UART device descriptor */
    if (STDIO_UART_DEV != UART_UNDEF) {
        sleep_test(STDIO_UART_DEV, STDIO_UART_DEV);
    }

    puts("\nUART INFO:");
    printf("Available devices:               %i\n", UART_NUMOF);
    if (STDIO_UART_DEV != UART_UNDEF) {
        printf("UART used for STDIO (the shell): UART_DEV(%i)\n\n", STDIO_UART_DEV);
    }

    /* initialize ringbuffers */
    for (unsigned i = 0; i < UART_NUMOF; i++) {
        ringbuffer_init(&(ctx[i].rx_buf), ctx[i].rx_mem, UART_BUFSIZE);
    }

    /* start the printer thread */
    printer_pid = thread_create(printer_stack, sizeof(printer_stack),
                                PRINTER_PRIO, 0, printer, NULL, "printer");

    /* run the shell */
    char line_buf[SHELL_BUFSIZE];

    shell_run(shell_commands, line_buf, SHELL_BUFSIZE);
    return 0;
}
