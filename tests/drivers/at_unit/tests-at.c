/*
 * Copyright (C) 2024 ML!PA GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "embUnit.h"
#include <stdio.h>

#include "at.h"
#include "isrpipe/read_timeout.h"

#define UNIT_TEST_LONG_URC "+UNITTEST_LONG_URC_VEEERY_LONG"
#define UNIT_TEST_SHORT_URC "+U"
#define LONG_COMMAND "AT+COMMAND_AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"\
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
#define COMMAND_WITH_EOL "AT+COMMAND_"AT_RECV_EOL"BLABLA"AT_RECV_EOL"BLA"

at_dev_t at_dev;
static char buf[256];
static char rp_buf[256];
#ifdef MODULE_AT_URC
static unsigned urc_count = 0;

void unit_test_urc_long_handler(void *arg, char const *code)
{
    TEST_ASSERT(strncmp(UNIT_TEST_LONG_URC, code, strlen(UNIT_TEST_LONG_URC)) == 0);
    unsigned *urc_count = (unsigned *)arg;
    *urc_count += 1;
}

void unit_test_urc_short_handler(void *arg, char const *code)
{
    TEST_ASSERT(strncmp(UNIT_TEST_SHORT_URC, code, strlen(UNIT_TEST_SHORT_URC)) == 0);
    unsigned *urc_count = (unsigned *)arg;
    *urc_count += 1;
}

at_urc_t urc_long = {
    .arg = &urc_count,
    .code = UNIT_TEST_LONG_URC,
    .cb = unit_test_urc_long_handler
};
at_urc_t urc_short = {
    .arg = &urc_count,
    .code = UNIT_TEST_SHORT_URC,
    .cb = unit_test_urc_short_handler
};
#endif

#ifdef BOARD_NATIVE
#define AT_UNIT_UART_DEV 0
#else
/* Most non-native boards have stdout mapped to device 0 */
#define AT_UNIT_UART_DEV 1
#endif

static void set_up(void)
{
    at_dev_init_t at_init_params = {
        .baudrate = 115200,
        .rp_buf = rp_buf,
        .rp_buf_size = sizeof(rp_buf),
        .rx_buf = buf,
        .rx_buf_size = sizeof(buf),
        .uart = UART_DEV(AT_UNIT_UART_DEV),
    };
    int res = at_dev_init(&at_dev, &at_init_params);
    /* check the UART initialization return value and respond as needed */
    if (res == UART_NODEV) {
        TEST_FAIL("Invalid UART device given!");
    }
    if (res == UART_NOBAUD) {
        TEST_FAIL("Baudrate is not applicable!");
    }

    /* we don't use the serial device, make sure it doesn't clobber our rx buffer */
    at_dev_poweroff(&at_dev);
    at_drain(&at_dev);

#ifdef MODULE_AT_URC
    at_add_urc(&at_dev, &urc_long);
    at_add_urc(&at_dev, &urc_short);
#endif
}

static void tear_down(void)
{
#ifdef MODULE_AT_URC
    at_remove_urc(&at_dev, &urc_long);
    at_remove_urc(&at_dev, &urc_short);
#endif
}

static void assert_urc_count(unsigned expected)
{
#ifdef MODULE_AT_URC
    TEST_ASSERT_EQUAL_INT(expected, urc_count);
    urc_count = 0;
#endif
    (void)expected;
}

int _emb_read_line_or_echo(at_dev_t *dev, char const *cmd, char *resp_buf,
                        size_t len, uint32_t timeout);
ssize_t _emb_get_lines(at_dev_t *dev, char *resp_buf, size_t len, uint32_t timeout);
ssize_t _emb_get_resp_with_prefix(at_dev_t *dev, const char *resp_prefix,
                                    char *resp_buf, size_t len, uint32_t timeout);
int _emb_wait_echo(at_dev_t *dev, char const *command, uint32_t timeout);
int _emb_wait_prompt(at_dev_t *dev, uint32_t timeout);

static void inject_resp_str(at_dev_t *dev, char const *str)
{
    isrpipe_write(&dev->isrpipe, (unsigned char const *)str, strlen(str));
}

void test_readline_or_echo(void)
{
    int res;
    char resp_buf[64];
    at_dev_t *dev = &at_dev;
    at_drain(dev);

    res = _emb_read_line_or_echo(dev, "AT+COMMAND", resp_buf, sizeof(resp_buf), 1000);
    TEST_ASSERT(res == -ETIMEDOUT);

    inject_resp_str(dev,
                    "AT+COMMAND"
                    CONFIG_AT_SEND_EOL);
    res = _emb_read_line_or_echo(dev, "AT+COMMAND", resp_buf, sizeof(resp_buf), 1000);
    TEST_ASSERT(res == 0);
    res = isrpipe_read_timeout(&dev->isrpipe, (unsigned char *)resp_buf, 1, 1000);
    TEST_ASSERT(res -ETIMEDOUT);

    inject_resp_str(dev,
                    "AT+COMMAND"
                    CONFIG_AT_SEND_EOL
                    AT_RECV_EOL
                    "OK"
                    AT_RECV_EOL);
    /* Reading in a buffer <= 1 should not read any characters from the RX */
    res = _emb_read_line_or_echo(dev, "AT+COMMAND", resp_buf, 0, 1000);
    TEST_ASSERT(res == -EINVAL);
    res = _emb_read_line_or_echo(dev, "AT+COMMAND", resp_buf, 1, 1000);
    TEST_ASSERT(res == -ENOBUFS);
    res = _emb_read_line_or_echo(dev, "AT+COMMAND", resp_buf, sizeof(resp_buf), 1000);
    TEST_ASSERT(res == 0);
    res = at_readline_skip_empty(dev, resp_buf, sizeof(resp_buf), false, 1000);
    TEST_ASSERT(res > 0);
    res = strcmp("OK", resp_buf);
    TEST_ASSERT(res == 0);

    inject_resp_str(dev,
                    ""
                    CONFIG_AT_SEND_EOL);
    res = _emb_read_line_or_echo(dev, "", resp_buf, sizeof(resp_buf), 1000);
    TEST_ASSERT(res == -EINVAL);

    /* here we should have a rogue CONFIG_AT_SEND_EOL left in the buffer from before */
    inject_resp_str(dev,
                    LONG_COMMAND
                    CONFIG_AT_SEND_EOL
                    AT_RECV_EOL
                    "OK"
                    AT_RECV_EOL);
    res = _emb_read_line_or_echo(dev, LONG_COMMAND, resp_buf, sizeof(resp_buf), 1000);
    TEST_ASSERT(res == 0);
    res = at_readline_skip_empty(dev, resp_buf, sizeof(resp_buf), false, 1000);
    TEST_ASSERT(res > 0);
    res = strcmp("OK", resp_buf);
    TEST_ASSERT(res == 0);

    inject_resp_str(dev,
                    AT_RECV_EOL
                    "+R"
                    AT_RECV_EOL);
    res = _emb_read_line_or_echo(dev, "AT+COMMAND", resp_buf, sizeof(resp_buf), 1000);
    TEST_ASSERT(res > 0);
    res = strcmp("+R", resp_buf);
    TEST_ASSERT(res == 0);

    inject_resp_str(dev,
                    CONFIG_AT_SEND_EOL
                    AT_RECV_EOL
                    "+R"
                    AT_RECV_EOL);
    res = _emb_read_line_or_echo(dev, "AT+COMMAND", resp_buf, sizeof(resp_buf), 1000);
    TEST_ASSERT(res > 0);
    res = strcmp("+R", resp_buf);
    TEST_ASSERT(res == 0);

    inject_resp_str(dev,
                    "+R"
                    AT_RECV_EOL);
    res = _emb_read_line_or_echo(dev, "AT+COMMAND", resp_buf, sizeof(resp_buf), 1000);
    TEST_ASSERT(res > 0);
    res = strcmp("+R", resp_buf);
    TEST_ASSERT(res == 0);

    inject_resp_str(dev,
                    COMMAND_WITH_EOL
                    CONFIG_AT_SEND_EOL);
    res = _emb_read_line_or_echo(dev, COMMAND_WITH_EOL, resp_buf, sizeof(resp_buf), 1000);
    TEST_ASSERT(res == 0);
    res = isrpipe_read_timeout(&dev->isrpipe, (unsigned char *)resp_buf, 1, 1000);
    TEST_ASSERT(res -ETIMEDOUT);
}

void test_wait_echo(void)
{
    int res;
    char resp_buf[64];
    at_dev_t *dev = &at_dev;

    at_drain(dev);

    res = _emb_wait_echo(dev, "AT+COMMAND", 1000);
    TEST_ASSERT(res == -ETIMEDOUT);

    inject_resp_str(dev, "AT+COMMAND" CONFIG_AT_SEND_EOL);
    res = _emb_wait_echo(dev, "AT+COMMAND", 1000);
    TEST_ASSERT(res == 0);

    inject_resp_str(dev,
                    AT_RECV_EOL
                    UNIT_TEST_LONG_URC
                    AT_RECV_EOL
                    "AT+COMMAND"
                    CONFIG_AT_SEND_EOL);
    res = _emb_wait_echo(dev, "AT+COMMAND", 1000);
    TEST_ASSERT(res == 0);

    inject_resp_str(dev,
                    AT_RECV_EOL
                    UNIT_TEST_LONG_URC
                    AT_RECV_EOL
                    "AT+COMMAND"
                    CONFIG_AT_SEND_EOL
                    AT_RECV_EOL
                    "OK"
                    AT_RECV_EOL);
    res = _emb_wait_echo(dev, "AT+COMMAND", 1000);
    TEST_ASSERT(res == 0);
    res = at_readline_skip_empty(dev, resp_buf, sizeof(resp_buf), false, 1000);
    TEST_ASSERT(res > 0);
    res = at_parse_resp(dev, resp_buf);
    TEST_ASSERT(res == 0);

    inject_resp_str(dev,
                    ""
                    CONFIG_AT_SEND_EOL);
    res = _emb_wait_echo(dev, "", 1000);
    TEST_ASSERT(res == -EINVAL);

    inject_resp_str(dev,
                    AT_RECV_EOL
                    UNIT_TEST_SHORT_URC
                    AT_RECV_EOL
                    "AT+COMMAND"
                    CONFIG_AT_SEND_EOL
                    AT_RECV_EOL
                    "OK"
                    AT_RECV_EOL);
    res = _emb_wait_echo(dev, "AT+COMMAND", 1000);
    TEST_ASSERT(res == 0);
    res = at_readline_skip_empty(dev, resp_buf, sizeof(resp_buf), false, 1000);
    TEST_ASSERT(res > 0);
    res = at_parse_resp(dev, resp_buf);
    TEST_ASSERT(res == 0);

    inject_resp_str(dev,
                    AT_RECV_EOL
                    UNIT_TEST_SHORT_URC
                    AT_RECV_EOL
                    AT_RECV_EOL
                    UNIT_TEST_LONG_URC
                    AT_RECV_EOL
                    "AT+COMMAND"
                    CONFIG_AT_SEND_EOL
                    AT_RECV_EOL
                    "OK"
                    AT_RECV_EOL);
    res = _emb_wait_echo(dev, "AT+COMMAND", 1000);
    TEST_ASSERT(res == 0);
    res = at_readline_skip_empty(dev, resp_buf, sizeof(resp_buf), false, 1000);
    TEST_ASSERT(res > 0);
    res = at_parse_resp(dev, resp_buf);
    TEST_ASSERT(res == 0);

    inject_resp_str(dev,
                    AT_RECV_EOL
                    UNIT_TEST_SHORT_URC
                    AT_RECV_EOL
                    AT_RECV_EOL
                    UNIT_TEST_LONG_URC
                    AT_RECV_EOL
                    COMMAND_WITH_EOL
                    CONFIG_AT_SEND_EOL
                    AT_RECV_EOL
                    "OK"
                    AT_RECV_EOL);
    res = _emb_wait_echo(dev, COMMAND_WITH_EOL, 1000);
    TEST_ASSERT(res == 0);
    res = at_readline_skip_empty(dev, resp_buf, sizeof(resp_buf), false, 1000);
    TEST_ASSERT(res > 0);
    res = at_parse_resp(dev, resp_buf);
    TEST_ASSERT(res == 0);

    inject_resp_str(dev,
                    AT_RECV_EOL
                    UNIT_TEST_SHORT_URC
                    AT_RECV_EOL
                    AT_RECV_EOL
                    UNIT_TEST_LONG_URC
                    AT_RECV_EOL
                    COMMAND_WITH_EOL
                    CONFIG_AT_SEND_EOL
                    "OK"
                    AT_RECV_EOL);
    res = _emb_wait_echo(dev, COMMAND_WITH_EOL, 1000);
    TEST_ASSERT(res == 0);
    res = at_readline_skip_empty(dev, resp_buf, sizeof(resp_buf), false, 1000);
    TEST_ASSERT(res > 0);
    res = at_parse_resp(dev, resp_buf);
    TEST_ASSERT(res == 0);

    assert_urc_count(9);
}

void test_get_resp_with_prefix(void)
{
    int res;
    char resp_buf[64];
    at_dev_t *dev = &at_dev;

    at_drain(dev);

    inject_resp_str(dev,
                    AT_RECV_EOL
                    UNIT_TEST_SHORT_URC
                    AT_RECV_EOL
                    AT_RECV_EOL
                    "+RESPONSE: 123"
                    AT_RECV_EOL);
    res = _emb_get_resp_with_prefix(dev, "+RESPONSE: ", resp_buf, sizeof(resp_buf), 10000);
    TEST_ASSERT(res > 0);
    res = strcmp("123", resp_buf);
    TEST_ASSERT(res == 0);

    inject_resp_str(dev,
                    AT_RECV_EOL
                    UNIT_TEST_SHORT_URC
                    AT_RECV_EOL
                    AT_RECV_EOL
                    "OK"
                    AT_RECV_EOL);
    res = _emb_get_resp_with_prefix(dev, "+RESPONSE: ", resp_buf, sizeof(resp_buf), 10000);
    TEST_ASSERT(res == 0);

    inject_resp_str(dev,
                    AT_RECV_EOL
                    UNIT_TEST_SHORT_URC
                    AT_RECV_EOL
                    AT_RECV_EOL "+CME ERROR: 1"
                    AT_RECV_EOL);
    res = _emb_get_resp_with_prefix(dev, "+RESPONSE: ", resp_buf, sizeof(resp_buf), 10000);
    TEST_ASSERT(res == -AT_ERR_EXTENDED);
    res = strncmp("1", dev->rp_buf, 1);
    TEST_ASSERT(res == 0);

    inject_resp_str(dev,
                    AT_RECV_EOL
                    UNIT_TEST_SHORT_URC
                    AT_RECV_EOL
                    AT_RECV_EOL
                    "ERROR"
                    AT_RECV_EOL);
    res = _emb_get_resp_with_prefix(dev, "+RESPONSE: ", resp_buf, sizeof(resp_buf), 10000);
    TEST_ASSERT(res == -1);

    inject_resp_str(dev,
                    AT_RECV_EOL
                    UNIT_TEST_SHORT_URC
                    AT_RECV_EOL);
    res = _emb_get_resp_with_prefix(dev, "+RESPONSE: ", resp_buf, sizeof(resp_buf), 10000);
    TEST_ASSERT(res == -ETIMEDOUT);

    inject_resp_str(dev,
                    "trash"
                    AT_RECV_EOL
                    "+RESPONSE: 123"
                    AT_RECV_EOL);
    res = _emb_get_resp_with_prefix(dev, "+RESPONSE: ", resp_buf, sizeof(resp_buf), 10000);
    TEST_ASSERT(res > 0);
    res = strcmp("123", resp_buf);
    TEST_ASSERT(res == 0);

    assert_urc_count(5);
}

void test_read_lines(void)
{
    int res;
    char resp_buf[62];
    char *p;
    at_dev_t *dev = &at_dev;

    at_drain(dev);

    inject_resp_str(dev,
                    AT_RECV_EOL
                    "+R1"
                    AT_RECV_EOL
                    "+R2"
                    AT_RECV_EOL
                    "OK"
                    AT_RECV_EOL);
    res = _emb_get_lines(dev, resp_buf, sizeof(resp_buf), 1000);
    TEST_ASSERT(res > 0);
    p = resp_buf;
    p = strstr(resp_buf, "+R1");
    TEST_ASSERT(p);
    p = strstr(resp_buf, "+R2");
    TEST_ASSERT(p);
    p = strstr(resp_buf, "OK");
    TEST_ASSERT(p);

    /* inconsistent EOL */
    inject_resp_str(dev,
                    "+R1"
                    AT_RECV_EOL
                    "+R2"
                    AT_RECV_EOL
                    "OK"
                    AT_RECV_EOL);
    res = _emb_get_lines(dev, resp_buf, sizeof(resp_buf), 1000);
    TEST_ASSERT(res > 0);
    p = resp_buf;
    p = strstr(resp_buf, "+R1");
    TEST_ASSERT(p);
    p = strstr(resp_buf, "+R2");
    TEST_ASSERT(p);
    p = strstr(resp_buf, "OK");
    TEST_ASSERT(p);

    /* URCs should get handled here */
    inject_resp_str(dev,
                    AT_RECV_EOL
                    UNIT_TEST_SHORT_URC
                    AT_RECV_EOL
                    AT_RECV_EOL
                    UNIT_TEST_SHORT_URC
                    AT_RECV_EOL
                    AT_RECV_EOL
                    "ERROR"
                    AT_RECV_EOL);
    res = _emb_get_lines(dev, resp_buf, sizeof(resp_buf), 1000);
    TEST_ASSERT(res == -1);

    /* URCs shouldn't get handled here. DCE answered neither OK nor error,
     * something went terribly wrong anyway, fine to just drop them. */
    inject_resp_str(dev,
                    AT_RECV_EOL
                    UNIT_TEST_SHORT_URC
                    AT_RECV_EOL
                    AT_RECV_EOL
                    UNIT_TEST_SHORT_URC
                    AT_RECV_EOL
                    AT_RECV_EOL);
    res = _emb_get_lines(dev, resp_buf, sizeof(resp_buf), 1000);
    TEST_ASSERT(res == -ETIMEDOUT);

    /* overflow the input buffer */
    inject_resp_str(dev,
                    AT_RECV_EOL
                    AT_RECV_EOL
                    "LONG_RESPONSE____________________________________________"
                    AT_RECV_EOL
                    "LONG_RESPONSE____________________________________________"
                    AT_RECV_EOL
                    "OK"
                    AT_RECV_EOL);
    res = _emb_get_lines(dev, resp_buf, sizeof(resp_buf), 1000);
    TEST_ASSERT(res == -ENOBUFS);

    assert_urc_count(2);
}

void test_wait_prompt(void)
{
    int res;
    char resp_buf[64];
    at_dev_t *dev = &at_dev;

    at_drain(dev);

    inject_resp_str(dev,
                    AT_RECV_EOL
                    ">"
                    "123");
    res = _emb_wait_prompt(dev, 1000);
    TEST_ASSERT(res == 0);
    res = isrpipe_read_timeout(&dev->isrpipe, (unsigned char *)resp_buf, sizeof(resp_buf), 1000);
    TEST_ASSERT(res == 3);
    res = strncmp("123", resp_buf, 3);

    inject_resp_str(dev,
                    UNIT_TEST_SHORT_URC
                    AT_RECV_EOL
                    AT_RECV_EOL
                    ">"
                    "456");
    res = _emb_wait_prompt(dev, 1000);
    TEST_ASSERT(res == 0);
    res = isrpipe_read_timeout(&dev->isrpipe, (unsigned char *)resp_buf, sizeof(resp_buf), 1000);
    TEST_ASSERT(res == 3);
    res = strncmp("456", resp_buf, 3);

    inject_resp_str(dev,
                    AT_RECV_EOL
                    UNIT_TEST_SHORT_URC
                    AT_RECV_EOL
                    AT_RECV_EOL
                    "ERROR"
                    AT_RECV_EOL);
    res = _emb_wait_prompt(dev, 1000);
    TEST_ASSERT(res == -1);

    inject_resp_str(dev,
                    ">"
                    "123");
    res = _emb_wait_prompt(dev, 1000);
    TEST_ASSERT(res == 0);
    res = isrpipe_read_timeout(&dev->isrpipe, (unsigned char *)resp_buf, sizeof(resp_buf), 1000);
    TEST_ASSERT(res == 3);
    res = strncmp("123", resp_buf, 3);
    TEST_ASSERT(res == 0);

    assert_urc_count(2);
}

void test_wait_ok(void)
{
    int res;
    unsigned urc_cnt = 5;
    at_dev_t *dev = &at_dev;

    at_drain(dev);

    inject_resp_str(dev,
                    UNIT_TEST_SHORT_URC
                    AT_RECV_EOL
                    AT_RECV_EOL
                    "OK"
                    AT_RECV_EOL);
    res = at_wait_ok(dev, 1000);
    TEST_ASSERT(res == 0);

    inject_resp_str(dev,
                    AT_RECV_EOL
                    AT_RECV_EOL
                    UNIT_TEST_SHORT_URC
                    AT_RECV_EOL
                    "ERROR"
                    AT_RECV_EOL);
    res = at_wait_ok(dev, 1000);
    TEST_ASSERT(res == -1);

    inject_resp_str(dev,
                    AT_RECV_EOL
                    AT_RECV_EOL
                    UNIT_TEST_SHORT_URC
                    AT_RECV_EOL
                    AT_RECV_EOL
                    UNIT_TEST_SHORT_URC
                    AT_RECV_EOL
                    AT_RECV_EOL
                    "+CME ERROR: 2"
                    AT_RECV_EOL);
    res = at_wait_ok(dev, 1000);
    TEST_ASSERT(res == -AT_ERR_EXTENDED);

    inject_resp_str(dev,
                    "trash"
                    AT_RECV_EOL
                    UNIT_TEST_SHORT_URC
                    AT_RECV_EOL
                    AT_RECV_EOL
                    "OK"
                    AT_RECV_EOL);
    res = at_wait_ok(dev, 1000);
    TEST_ASSERT(res == 0);
#ifdef CONFIG_AT_SEND_SKIP_ECHO
    if (strcmp(AT_RECV_EOL, CONFIG_AT_SEND_EOL) == 0) {
        /* Test echo handling when none expected */
        urc_cnt += 5;
        at_drain(dev);

        inject_resp_str(dev,
                        UNIT_TEST_SHORT_URC
                        AT_RECV_EOL
                        "AT+COMMAND"
                        CONFIG_AT_SEND_EOL
                        AT_RECV_EOL
                        "OK"
                        AT_RECV_EOL);
        res = at_wait_ok(dev, 1000);
        TEST_ASSERT(res == 0);

        inject_resp_str(dev,
                        AT_RECV_EOL
                        AT_RECV_EOL
                        UNIT_TEST_SHORT_URC
                        AT_RECV_EOL
                        "AT+COMMAND"
                        CONFIG_AT_SEND_EOL
                        "ERROR"
                        AT_RECV_EOL);
        res = at_wait_ok(dev, 1000);
        TEST_ASSERT(res == -1);

        inject_resp_str(dev,
                        AT_RECV_EOL
                        AT_RECV_EOL
                        UNIT_TEST_SHORT_URC
                        AT_RECV_EOL
                        AT_RECV_EOL
                        UNIT_TEST_SHORT_URC
                        AT_RECV_EOL
                        "AT+COMMAND"
                        CONFIG_AT_SEND_EOL
                        AT_RECV_EOL
                        "+CME ERROR: 2"
                        AT_RECV_EOL);
        res = at_wait_ok(dev, 1000);
        TEST_ASSERT(res == -AT_ERR_EXTENDED);

        inject_resp_str(dev,
                        "trash"
                        AT_RECV_EOL
                        UNIT_TEST_SHORT_URC
                        AT_RECV_EOL
                        "AT+COMMAND"
                        CONFIG_AT_SEND_EOL
                        AT_RECV_EOL
                        "OK"
                        AT_RECV_EOL);
        res = at_wait_ok(dev, 1000);
        TEST_ASSERT(res == 0);
    }
#endif /* CONFIG_AT_SEND_SKIP_ECHO */
    assert_urc_count(urc_cnt);
}

#ifdef MODULE_AT_URC
void test_process_urc(void)
{
    at_dev_t *dev = &at_dev;
    at_drain(dev);

    at_process_urc(dev, 1000);

    inject_resp_str(dev,
                    "trash"
                    AT_RECV_EOL
                    UNIT_TEST_SHORT_URC
                    AT_RECV_EOL
                    AT_RECV_EOL
                    UNIT_TEST_LONG_URC
                    AT_RECV_EOL);
    at_process_urc(dev, 1000);

    assert_urc_count(2);
}
#endif /* MODULE_AT_URC */

static Test *tests_at(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_readline_or_echo),
        #ifndef CONFIG_AT_SEND_SKIP_ECHO
        new_TestFixture(test_wait_echo),
        #endif
        new_TestFixture(test_get_resp_with_prefix),
        new_TestFixture(test_read_lines),
        new_TestFixture(test_wait_prompt),
        new_TestFixture(test_wait_ok),
        #ifdef MODULE_AT_URC
        new_TestFixture(test_process_urc),
        #endif
    };

    EMB_UNIT_TESTCALLER(at_tests, set_up, tear_down, fixtures);

    return (Test *)&at_tests;
}

int main(void)
{

    puts("AT unit-like test\n");

    TESTS_START();
    TESTS_RUN(tests_at());
    TESTS_END();

    return 0;
}
